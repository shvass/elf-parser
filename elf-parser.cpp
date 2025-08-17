// This file is a part of elf-parser
// Copyright (C) 2025  akshay bansod <akbansd@gmail.com>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// elf-parser to load and execute elf objects

#include <cstring>
#include <esp_heap_caps.h>
#include <esp_log.h>

#include "elf-parser.hpp"

#define TAG          "elf-parser"
#define ELF_IDENTITY ".ELF"

// entry point function pointer
typedef void (*entry_point_t)(void);
typedef void (*rela_sym_t)(void);


// global symbol table for symbol mapping
std::unordered_map<std::string, void*> executable::symbol_table;


executable::executable(uint8_t* dram_bin, int length) 
    : bin_size(length) 
{
    // null checks
    if(!dram_bin || length <= 0) {
        ESP_LOGE(TAG, "received invalid executable binary");
        return;
    }

    // keep a copy of elf header
    header = new Elf32_Ehdr;
    memcpy(header, dram_bin, sizeof(Elf32_Ehdr));

    // @todo debug log elf header

    // copy section header
    shdr = new Elf32_Shdr[header->e_shnum];
    memcpy(shdr, dram_bin + header->e_shoff, 
           sizeof(Elf32_Shdr) * header->e_shnum);
        
    ESP_LOGD(TAG, "section: found %d header", header->e_shnum);
    ESP_LOG_BUFFER_HEX_LEVEL(TAG, shdr, sizeof(Elf32_Shdr) * header->e_shnum, 
                             ESP_LOG_VERBOSE);

    // find elf string section and copy string array
    Elf32_Word str_arr_size = shdr[header->e_shstrndx].sh_size;
    elf_str_addr = new char[str_arr_size];
    memcpy(elf_str_addr, dram_bin + shdr[header->e_shstrndx].sh_offset, 
        str_arr_size);


    Elf32_Sym* dyn_sym_table = 0;
    Elf32_Shdr *dyn_sym_hdr = 0, *rela = 0, *dyn_str_hdr = 0;

    // enumerate over section header table and extract relevant sections
    Elf32_Shdr* ptr = shdr;
    for(int i = 0; i < header->e_shnum; i++){
        ESP_LOGD(TAG, "section %s ", elf_str_addr + ptr->sh_name);

        // extract relevant sections from the object
        if(ptr->sh_type == SHT_DYNSYM) dyn_sym_hdr = ptr;
        else if(ptr->sh_type == SHT_RELA) rela = ptr;

        else if(ptr->sh_type == SHT_STRTAB &&
            !strcmp(&elf_str_addr[ptr->sh_name], ".dynstr")) dyn_str_hdr = ptr;

        ptr++;
    };


    // resolve dynamic symbols
    if(dyn_sym_hdr)
        dyn_sym_table = (Elf32_Sym*) (dram_bin + dyn_sym_hdr->sh_offset);


    char* elf_dyn_str_addr = 0;
    if(dyn_str_hdr){
        ESP_LOGD(TAG, "found dynamic string table %s off %4lx", 
            &elf_str_addr[dyn_str_hdr->sh_name], dyn_str_hdr->sh_offset);

        elf_dyn_str_addr = (char*) (dram_bin + dyn_str_hdr->sh_offset);
    };
        
        
    // resolve relocations of imported symbols
    if(rela)
    {   
        uint32_t rela_count = rela->sh_size / rela->sh_entsize;
        
        ESP_LOGD(TAG, "found rela table with %ld entries", rela_count);
        
        rela_sym_t* rela_ptr = 0;
        Elf32_Rela* rela_arr = (Elf32_Rela*) (dram_bin + rela->sh_offset);
        
        for(int i = 0; i < rela_count; i++){

            ESP_LOGD(TAG, "rela @ %04lx type %04lx addend %04lx",
            rela_arr[i].r_offset,
            rela_arr[i].r_info,
            rela_arr[i].r_addend
            );

            // get function name from symbol table from symbol
            uint8_t symbol_index = rela_arr[i].r_info >> 8;
            Elf32_Sym* sym = dyn_sym_table + symbol_index;
            
            char* sym_name_str = (char*) (elf_dyn_str_addr + sym->st_name);

            ESP_LOGD(TAG,"symbol offset %ld %s",
                dyn_sym_table[symbol_index].st_name,
                (char*)(elf_dyn_str_addr + dyn_sym_table[symbol_index].st_name)
            );

            rela_ptr = (rela_sym_t*) (dram_bin + rela_arr[i].r_offset);

            // @todo implement symbol import
            void* sym_addr = symbol_table[sym_name_str];
            if(!sym_addr) ESP_LOGE(TAG, "symbol import error %s", sym_name_str);
            else *rela_ptr = (rela_sym_t) sym_addr;

        };

    }


    // check identity
    if(!strcmp((char*) &header->e_ident, ELF_IDENTITY)){
        ESP_LOGE(TAG, "invalid ELF_IDENTITY or corrupt ELF");
        return;
    }



    // copy elf obj to internal Iram buffer
    ESP_LOGD(TAG, "allocating %d available %d", bin_size,
        heap_caps_get_free_size(MALLOC_CAP_EXEC));

    raw_data = (uint8_t*) heap_caps_malloc(bin_size, MALLOC_CAP_EXEC);
    if(!raw_data){
        ESP_LOGE(TAG, "insufficient IRAM : malloc failed");
        return;
    }

    memcpy(raw_data, dram_bin, bin_size);

    // resolve symbols exports
    if(dyn_sym_hdr){
        uint32_t count = dyn_sym_hdr->sh_size / dyn_sym_hdr->sh_entsize;
        ESP_LOGD(TAG, "found dynamic symbol table with %ld entries", count);

        Elf32_Sym* dyn_sym = dyn_sym_table;
        for(int i = 0; i < count; i++){
            char* dyn_sym_name = (char*)(elf_dyn_str_addr + dyn_sym->st_name);

            if(ELF32_ST_TYPE(dyn_sym->st_info) == STT_FUNC){
                ESP_LOGD(TAG, "exporting function %s", dyn_sym_name);
                    
                // @todo check if offset is valid
                symbol_table[dyn_sym_name] = (raw_data + dyn_sym->st_value);
            };

            dyn_sym++;
        };
    }
};



void executable::printElfHeaderInfo(){
    if(!header) return;
    ESP_LOGI(TAG, "elf type %d", header->e_type);
    // @todo: print more relevant header info
};


void executable::operator()(){
    ESP_LOGD(TAG, "exec entry point offset %ld raw_addr %p", 
        header->e_entry, (void*) raw_data);

    entry_point_t entry = (entry_point_t) (raw_data + header->e_entry); 

    entry();
};