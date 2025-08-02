idf_component_register(SRCS "elf-parser.cpp"
                    INCLUDE_DIRS "include")


  
execute_process(COMMAND make -C test-app all
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
)