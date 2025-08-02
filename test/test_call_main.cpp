
#include <esp_log.h>
#include <unity.h>
// #include <test-app/test-app.hh>
// #include <elf-parser.hpp>


// extern "C"{
//     void external_binding(void){
//         ESP_LOGI("syscall", "called external binding");
//     };

//     void external_binding_2(void){
//         ESP_LOGI("syscall", "called external binding");
//     };

//     typedef void (export_binding_fn_t)(void);
// };


TEST_CASE("load a elf module and call entrypoint", "[elf-parser]")
{

    // executable::symbol_table["external_binding"] = (void*) external_binding;
    // executable exec(test_app, test_app_len);

    // exec.printElfHeaderInfo();
    // exec();

    // export_binding_fn_t* fn = 
    //     (export_binding_fn_t*) executable::symbol_table["export_binding"];
    
    // if(fn) fn();

    // return;
    TEST_ASSERT(false);
}