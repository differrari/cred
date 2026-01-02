#include "syscalls/syscalls.h" 
#include "data/scanner/scanner.h"
#include "data/tokenizer/tokenizer.h"
#include "data/helpers/token_stream.h"
#include "std/string.h"
#include "std/stringview.h"
#include <stdio.h>

int main(int argc, char *argv[]){
    
    char *content = read_full_file("street.cred");
    
    Scanner scan = scanner_make(content,strlen(content));
    
    Tokenizer tk = tokenizer_make(&scan);
    tk.skip_type_check = true;
    TokenStream ts;
    ts_init(&ts,&tk);
    
    Token t;
    while (ts_next(&ts, &t)) {
        if (!t.kind) break;
        print("Token [%i{%i,%i}] = %v",t.kind, t.pos, t.length, delimited_stringview(content, t.pos, t.length));
    }
    
    printf("String scanned");
}
