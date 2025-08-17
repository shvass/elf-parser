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

#include <esp_log.h>
#include <unity.h>
#include <elf-parser.hpp>

#include <overlays-bins/test-app.hh>


extern "C"{
    void external_binding(void){
        ESP_LOGI("syscall", "called external binding");
    };

    typedef void (export_binding_fn_t)(void);
};


TEST_CASE("load a elf module and call entrypoint", "[elf-parser]")
{
    executable::symbol_table["external_binding"] = (void*) external_binding;
    executable exec(test_app_fo, test_app_fo_len);

    exec.printElfHeaderInfo();
    exec();

    export_binding_fn_t* fn = 
        (export_binding_fn_t*) executable::symbol_table["export_binding"];
    
    if(fn) fn();
}