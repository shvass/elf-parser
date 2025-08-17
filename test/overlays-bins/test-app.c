
#define FROS_IMPORT extern
#define FROS_EXPORT __attribute__ ((visibility("default")))


FROS_IMPORT void external_binding();

FROS_EXPORT void export_binding(){
    external_binding();
};



typedef void (*external_binding_typ)();


int main(){

    external_binding_typ func_ptr = external_binding;
    
    if(func_ptr) external_binding();

    return 10;
};

void _start() {
    main();
}
