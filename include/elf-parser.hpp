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

#ifndef ELF_PARSER_HPP
#define ELF_PARSER_HPP

extern "C"{
    #include <elf.h>
}

#include <string>
#include <unordered_map>


/**
 * @brief executable class to load and execute elf objects
 * manages the elf header, section header, program header and symbol table
 * provides methods to print elf header info and string table.
 */
class executable
{
public:

    /**
     * @brief constructor to initialize the executable object with raw data and length
     */
    executable(uint8_t* rawdata, int length);

    void printElfHeaderInfo();

    // handover execution to entry_point of the elf object
    void operator()();

    Elf32_Ehdr* header;

    uint8_t* raw_data = 0;
    int bin_size = -1;

    // global symbol table to store the symbols and their corresponding addresses
    static std::unordered_map<std::string, void*> symbol_table;

private:

    Elf32_Shdr* shdr;
    Elf32_Phdr* phdr;
    char* elf_str_addr;
    
    
    void parse_section_header();
    void print_string_table(char* ptr, size_t size);
};



#endif //  ELF_PARSER_HPP