#include "codeformat.h"
#include "common.h"
#include "alloc/allocate.h"
#include "files/buffer.h"
#include "math/math.h"
#include "files/helpers.h"

#define MAX_BUF 0x10000

static buffer code_buf;
static int current_indent = 0;

void reset_emit_buffer(){
    if (code_buf.buffer) buffer_destroy(&code_buf);
    code_buf = (buffer){
        .buffer = zalloc(0x10000),
        .limit = 0x10000,
        .options = buffer_can_grow,
        .buffer_size = 0,
        .cursor = 0,
    };
}

void emit_const(char *lit){
    if (!code_buf.buffer) reset_emit_buffer();
    buffer_write_const(&code_buf,lit);
}

void emit(char* fmt, ...){
    if (!code_buf.buffer) reset_emit_buffer();
    __attribute__((aligned(16))) va_list args;
    va_start(args, fmt); 
    buffer_write_va(&code_buf, fmt, args);
    va_end(args);
}

void emit_token(Token t){
    if (!code_buf.buffer) reset_emit_buffer();
    buffer_write_lim(&code_buf, (char*)t.start, t.length);
}

void emit_slice(string_slice slice){
    if (!code_buf.buffer) reset_emit_buffer();
    buffer_write_lim(&code_buf, (char*)slice.data, slice.length);
}

void emit_space(){
    if (!code_buf.buffer) reset_emit_buffer();
    emit_const(" ");
}

void emit_newline(){
    if (!code_buf.buffer) reset_emit_buffer();
    emit("\n\%s",indent_by(max(current_indent,0)));
}

void emit_newlines(int amount){
    if (!code_buf.buffer) reset_emit_buffer();
    for (int i = 0; i < amount; i++){
        emit_newline();
    }
}

void increase_indent(bool newline){
    if (!code_buf.buffer) reset_emit_buffer();
    current_indent++;
    if (newline) emit_newline();
}

void decrease_indent(bool newline){
    if (!code_buf.buffer) reset_emit_buffer();
    current_indent--;
    if (newline) emit_newline();
}

void output_code(const char *path, const char *extension){
    string s = strlen(extension) ? string_format("%s.%s",path,extension) : string_from_literal(path);
    
    write_full_file(s.data,code_buf.buffer,code_buf.buffer_size);
    
    string_free(s);
}