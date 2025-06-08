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


executable::executable(uint8_t* dram_bin, int length) 
    : bin_size(length) 
{
    // null checks
    if(!dram_bin || length <= 0) {
        ESP_LOGE(TAG, "received invalid execuatble binary");
        return;
    }

    // copy buffer to internal Iram buffer
    ESP_LOGD(TAG, "allocating %d available %d", bin_size,
        heap_caps_get_free_size(MALLOC_CAP_EXEC));

    raw_data = (uint8_t*) heap_caps_malloc(bin_size, MALLOC_CAP_EXEC);
    if(!raw_data){
        ESP_LOGE(TAG, "insufficient IRAM : malloc failed");
        return;
    }

    memcpy(raw_data, dram_bin, bin_size);


    // assign header value
    header = (Elf32_Ehdr*) raw_data;

    // check identity
    if(!strcmp((char*) &header->e_ident, ELF_IDENTITY)){
        ESP_LOGE(TAG, "invalid ELF_IDENTITY or corrupt ELF");
        return;
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