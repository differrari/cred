#include "imaginal.h"
#include "files/buffer.h"
#include "syscalls/syscalls.h"

extern buffer imaginal_buf;

void atom_code_debug_print(codegen instance, int depth){
    atom_code *car = instance.ptr;
    switch (car->type) {
        case car_identifier:
            buffer_write(&imaginal_buf,"%v ",car->val);
            break;
        case car_string:
            buffer_write(&imaginal_buf,"%v ",car->val);
            break;
        case car_num:
            buffer_write(&imaginal_buf,"%i ",car->number);
            break;
        case car_true:
            buffer_write(&imaginal_buf,"t ");
            break;
        default: print("{err wrong type %i}",car->type);
    }
}

void s_exp_code_debug_print(codegen instance, int depth){
    if (!instance.ptr){
        buffer_write(&imaginal_buf,"nil");
        return;
    }
    if (instance.type != sem_rule_sexp){
        buffer_write(&imaginal_buf,"{err wrong rule %s}",sem_rule_strings[instance.type]);
        return;
    }
    s_exp_code *code = instance.ptr;
    buffer_write(&imaginal_buf,"(");
    codegen_debug_print(code->car, depth+1);
    buffer_write(&imaginal_buf," . ");
    codegen_debug_print(code->cdr, depth+1);
    buffer_write(&imaginal_buf,")");
}