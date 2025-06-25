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


class executable
{
public:

    executable(uint8_t* rawdata, int length);

    void printElfHeaderInfo();

    void operator()();

    Elf32_Ehdr* header;

    uint8_t* raw_data = 0;
    int bin_size = -1;

    static std::unordered_map<std::string, void*> symbol_table;

private:

    Elf32_Shdr* shdr;
    Elf32_Phdr* phdr;
    char* elf_str_addr;
    
    
    void parse_section_header();
    void print_string_table(char* ptr, size_t size);
};



#endif //  ELF_PARSER_HPP