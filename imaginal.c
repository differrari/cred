#include "syscalls/syscalls.h"
#include "files/buffer.h"
#include "files/helpers.h"
#include "alloc/allocate.h"
#include "interpreter/imaginal.h"
#include "ir/irgen.h"
#include "ir/arch_transformer.h"
#include "interpreter/repl.h"
#define _GNU_SOURCE
#include "memory/memory.h"

static buffer buf;

bool parse_arguments(int argc, char *argv[]){
    buf = (buffer){
        .buffer = zalloc(0x10000),
        .limit = 0x10000,
        .buffer_size = 0,
        .options = buffer_can_grow,
        .cursor = 0,
    };
    for (int i = 1; i < argc; i++){
        if (*argv[i] != '-'){
            char *content = read_full_file(argv[i],0);
            if (!content){
                print("Failed to open file %s",argv[i]);
                return false;
            }
            buffer_write_const(&buf, content);
        }
    }
    if (!buf.buffer_size){
        char *content = read_full_file("test.lisp",0);
        buffer_write_const(&buf, content);
    }
    return true;
}

extern buffer get_input_line();

int main(int argc, char *argv[]) {

    if (!parse_arguments(argc, argv)) return -1;

    imaginal_repl_ctx *im = zalloc(sizeof(imaginal_repl_ctx));
    im->should_print = true;
    im->fallback = 0;
    
    bool should_loop = false;

    do {
        should_loop = false;
        repl_run(slice_from_buffer(&buf), im);
        if (buf.buffer) buffer_destroy(&buf);
        buf = get_input_line();
        if (!buf.buffer || !buf.buffer_size || slice_lit_match(slice_from_buffer(&buf), "quit", true) || slice_lit_match(slice_from_buffer(&buf), "q", true) || slice_lit_match(slice_from_buffer(&buf), "exit", true)){
            print("Imaginal close");
            break;
        } else should_loop = false;
    } while (should_loop);
    
}
