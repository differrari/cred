#include "general.h"
#include "semantic/semantic_rules.h"
#include "alloc/allocate.h"
#include "syscalls/syscalls.h"
#include "common.h"

void *codegen_page;

#define CODEGEN_DEC(name,t,subscope,printer) \
extern bool name##_emit_code(codegen instance);\
extern codegen name##_transform(codegen instance);\
codegen name##_init(){\
    if (!codegen_page) codegen_page = page_alloc(PAGE_SIZE);\
    return (codegen){\
        .ptr = allocate(codegen_page, sizeof(name), page_alloc),\
        .register_elem = name##_register_elem,\
        .register_subrule = name##_register_subrule,\
        .emit_code = name##_emit_code,\
        .transform = name##_transform,\
        .debug_print = printer,\
        .get_subscope = subscope,\
        .type = t,\
    };\
}

void blk_code_register_elem(codegen instance, int type, Token elem){
    
}

void blk_code_register_subrule(codegen instance, int type, codegen child){
    blk_code *code = (blk_code*)instance.ptr;
    if (type == sem_rule_scope){
        code->chain = child;
    } else
        code->stat = child;
}

CODEGEN_DEC(blk_code, sem_rule_scope, 0, 0)

void dec_code_register_elem(codegen instance, int type, Token elem){
    dec_code *code = (dec_code*)instance.ptr;
    switch (type) {
        case sem_elem_type: code->type = token_to_slice(elem); break; 
        case sem_elem_name: code->name = token_to_slice(elem); break;
    }
}

void dec_code_register_subrule(codegen instance, int type, codegen child){
    dec_code *code = (dec_code*)instance.ptr;
    code->initial_value = child;
}

CODEGEN_DEC(dec_code, sem_rule_dec, 0, 0)

void ass_code_register_elem(codegen instance, int type, Token elem){

}

void ass_code_register_subrule(codegen instance, int type, codegen child){
    ass_code *code = (ass_code*)instance.ptr;
    if (type == sem_rule_var)
        code->var = child;
    if (type == sem_rule_exp)
        code->expression = child;
}

CODEGEN_DEC(ass_code, sem_rule_assign, 0, 0)

void call_code_register_elem(codegen instance, int type, Token elem){
    call_code *code = (call_code*)instance.ptr;
    switch (type) {
        case sem_rule_deref: code->transform = var_deref; break;
        case sem_rule_addr: code->transform = var_addr; break;
        case sem_rule_func: code->name = token_to_slice(elem); break;
    }
}

void call_code_register_subrule(codegen instance, int type, codegen child){
    call_code *code = (call_code*)instance.ptr;
    code->args = child;
}

CODEGEN_DEC(call_code, sem_rule_call, 0, 0)

void cond_code_register_elem(codegen instance, int type, Token elem){
    
}

void cond_code_register_subrule(codegen instance, int type, codegen child){
    cond_code *code = (cond_code*)instance.ptr;
    switch (type){
        case sem_rule_cond: code->cond = child; break;
        case sem_rule_scope: code->scope = child; break;
        case sem_rule_else: code->chain = child; break;
    }
}

codegen cond_code_get_subscope(codegen instance){
    cond_code *code = instance.ptr;
    return code->scope;
}

CODEGEN_DEC(cond_code, sem_rule_cond, cond_code_get_subscope,0)

void jmp_code_register_elem(codegen instance, int type, Token elem){
    jmp_code *code = (jmp_code*)instance.ptr;
    code->jump = token_to_slice(elem);
}

void jmp_code_register_subrule(codegen instance, int type, codegen child){
    
}

CODEGEN_DEC(jmp_code, sem_rule_jmp, 0,0)

void label_code_register_elem(codegen instance, int type, Token elem){
    label_code *code = (label_code*)instance.ptr;
    code->name = token_to_slice(elem);
}

void label_code_register_subrule(codegen instance, int type, codegen child){
    
}

CODEGEN_DEC(label_code, sem_rule_label, 0,0)

void exp_code_register_elem(codegen instance, int type, Token elem){
    exp_code *code = (exp_code*)instance.ptr;
    if (type == sem_rule_val)
        code->val = token_to_slice(elem);
    else if (type == sem_rule_syn)
        code->paren = true;
    else if (type == sem_rule_op && slice_lit_match(token_to_slice(elem), "not", false))
        code->invert = true;
    else code->operand = token_to_slice(elem);
}

void exp_code_register_subrule(codegen instance, int type, codegen child){
    exp_code *code = (exp_code*)instance.ptr;
    switch (type){
        case sem_rule_var: code->var = child; break;
        case sem_rule_exp: code->exp = child; break;
        case sem_rule_func: code->lambda = child; break;
    }
}

CODEGEN_DEC(exp_code, sem_rule_exp, 0, 0)

void arg_code_register_elem(codegen instance, int type, Token elem){
   
}

void arg_code_register_subrule(codegen instance, int type, codegen child){
    arg_code *code = (arg_code*)instance.ptr;
    if (type == sem_rule_args)
        code->chain = child;
    if (type == sem_rule_exp)
        code->exp = child;
}

CODEGEN_DEC(arg_code, sem_rule_args, 0, 0)

void param_code_register_elem(codegen instance, int type, Token elem){
    param_code *code = (param_code*)instance.ptr;
    switch (type) {
        case sem_elem_type: code->type = token_to_slice(elem); break; 
        case sem_elem_name: code->name = token_to_slice(elem); break;
    }
}

void param_code_register_subrule(codegen instance, int type, codegen child){
    param_code *code = (param_code*)instance.ptr;
    if (type == sem_rule_param)
        code->chain = child;
}

CODEGEN_DEC(param_code, sem_rule_param, 0, 0)

void func_code_register_elem(codegen instance, int type, Token elem){
    func_code *code = (func_code*)instance.ptr;
    switch (type) {
        case sem_elem_type: code->type = token_to_slice(elem); break; 
        case sem_elem_name: code->name = token_to_slice(elem); break;
    }
}

void func_code_register_subrule(codegen instance, int type, codegen child){
    func_code *code = (func_code*)instance.ptr;
    if (type == sem_rule_func) code->signature = child;
    if (type == sem_rule_param) code->args = child;
    if (type == sem_rule_scope) code->body = child;
}

codegen func_code_get_subscope(codegen instance){
    func_code *code = instance.ptr;
    return code->body;
}


CODEGEN_DEC(func_code, sem_rule_func, func_code_get_subscope, 0)

void for_code_register_elem(codegen instance, int type, Token elem){

}
    
void for_code_register_subrule(codegen instance, int type, codegen child){
    for_code *code = (for_code*)instance.ptr;
    switch (type) {
        case sem_rule_assign: code->increment = child; break;
        case sem_rule_dec: code->initial = child; break;
        case sem_rule_cond: code->condition = child; break;
        case sem_rule_scope: code->body = child; break;
    }
}

codegen for_code_get_subscope(codegen instance){
    for_code *code = instance.ptr;
    return code->body;
}


CODEGEN_DEC(for_code, sem_rule_for, for_code_get_subscope, 0)

void while_code_register_elem(codegen instance, int type, Token elem){

}

void while_code_register_subrule(codegen instance, int type, codegen child){
    while_code *code = (while_code*)instance.ptr;
    switch (type) {
        case sem_rule_cond: code->condition = child; break;
        case sem_rule_scope: code->body = child; break;
    }
}

codegen while_code_get_subscope(codegen instance){
    while_code *code = instance.ptr;
    return code->body;
}

CODEGEN_DEC(while_code, sem_rule_while, while_code_get_subscope, 0)

void dowhile_code_register_elem(codegen instance, int type, Token elem){

}

void dowhile_code_register_subrule(codegen instance, int type, codegen child){
    dowhile_code *code = (dowhile_code*)instance.ptr;
    switch (type) {
        case sem_rule_cond: code->condition = child; break;
        case sem_rule_scope: code->body = child; break;
    }
}

codegen dowhile_code_get_subscope(codegen instance){
    dowhile_code *code = instance.ptr;
    return code->body;
}

CODEGEN_DEC(dowhile_code, sem_rule_dowhile, dowhile_code_get_subscope, 0)

void var_code_register_elem(codegen instance, int type, Token elem){
    var_code *code = (var_code*)instance.ptr;
    switch (type) {
        case sem_rule_var: code->name = token_to_slice(elem); break;
        case sem_rule_op: code->operation = token_to_slice(elem); break;
        case sem_rule_deref: code->transform = var_deref; break;
        case sem_rule_addr: code->transform = var_addr; break;
    }
}

void var_code_register_subrule(codegen instance, int type, codegen child){
    var_code *code = (var_code*)instance.ptr;
    if (type == sem_rule_exp)
        code->expression = child;
    else if (type == sem_rule_var)
        code->var = child;
}

CODEGEN_DEC(var_code, sem_rule_var, 0, 0)

void inc_code_register_elem(codegen instance, int type, Token elem){
    inc_code *code = (inc_code*)instance.ptr;
    if (type == sem_rule_inc && *elem.start == '"') code->value = elem;
}

void inc_code_register_subrule(codegen instance, int type, codegen child){
    
}

CODEGEN_DEC(inc_code, sem_rule_inc, 0, 0)

void struct_code_register_elem(codegen instance, int type, Token elem){
    struct_code *code = (struct_code*)instance.ptr;
    if (type == sem_elem_parent) code->parent = elem;
    if (type == sem_elem_name) code->name = token_to_slice(elem);
}

void struct_code_register_subrule(codegen instance, int type, codegen child){
    struct_code *code = (struct_code*)instance.ptr;
    code->contents = child;
}

CODEGEN_DEC(struct_code, sem_rule_struct, 0, 0)

void ret_code_register_elem(codegen instance, int type, Token elem){
    
}

void ret_code_register_subrule(codegen instance, int type, codegen child){
    ret_code *code = (ret_code*)instance.ptr;
    code->expression = child;
}

CODEGEN_DEC(ret_code, sem_rule_ret, 0, 0)

void def_code_register_elem(codegen instance, int type, Token elem){
    
}

void def_code_register_subrule(codegen instance, int type, codegen child){
    def_code *code = (def_code*)instance.ptr;
    code->expression = child;
}

CODEGEN_DEC(def_code, sem_rule_def, 0, 0)

void int_code_register_elem(codegen instance, int type, Token elem){
    int_code *code = (int_code*)instance.ptr;
    if (type == sem_elem_name) code->name = token_to_slice(elem);
}

void int_code_register_subrule(codegen instance, int type, codegen child){
    int_code *code = (int_code*)instance.ptr;
    code->contents = child;
}

CODEGEN_DEC(int_code, sem_rule_interf, 0, 0)

void enum_code_register_elem(codegen instance, int type, Token elem){
    enum_code * code = (enum_code*)instance.ptr;
    if (type == sem_elem_name)
        code->name = elem;
}

void enum_code_register_subrule(codegen instance, int type, codegen child){
    enum_code * code = (enum_code*)instance.ptr;
    if (type == sem_rule_enum_case)
        code->contents = child;
}

CODEGEN_DEC(enum_code, sem_rule_enum, 0, 0)

void enum_case_code_register_elem(codegen instance, int type, Token elem){
    enum_case_code * code = (enum_case_code*)instance.ptr;
    if (type == sem_elem_name)
        code->name = elem;
}

void enum_case_code_register_subrule(codegen instance, int type, codegen child){
    enum_case_code * code = (enum_case_code*)instance.ptr;
    if (type == sem_rule_enum_case)
        code->chain = child;
}

CODEGEN_DEC(enum_case_code, sem_rule_enum_case, 0, 0)

void else_code_register_elem(codegen instance, int type, Token elem){
    
}

void else_code_register_subrule(codegen instance, int type, codegen child){
    else_code *code = (else_code*)instance.ptr;
    code->block = child;
}

codegen else_code_get_subscope(codegen instance){
    else_code *code = instance.ptr;
    return code->block;
}

CODEGEN_DEC(else_code, sem_rule_else, else_code_get_subscope, 0)

void switch_code_register_elem(codegen instance, int type, Token elem){
    
}

void switch_code_register_subrule(codegen instance, int type, codegen child){
    switch_code *code = (switch_code*)instance.ptr;
    switch (type){
        case sem_rule_exp: code->condition = child; break;
        case sem_rule_scope: code->cases = child; break;    
    }
}
CODEGEN_DEC(switch_code, sem_rule_switch, 0, 0)

codegen switch_code_init();

void case_code_register_elem(codegen instance, int type, Token elem){
    
}

void case_code_register_subrule(codegen instance, int type, codegen child){
    case_code *code = (case_code*)instance.ptr;
    switch (type){
        case sem_rule_exp: code->match = child; break;
        case sem_rule_scope: code->body = child; break;   
        case sem_rule_cases: code->chain = child; break;    
    }
}

CODEGEN_DEC(case_code, sem_rule_cases, 0, 0)

void prop_init_code_register_elem(codegen instance, int type, Token elem){

}

void prop_init_code_register_subrule(codegen instance, int type, codegen child){

}

CODEGEN_DEC(prop_init_code, sem_rule_prop_init, 0, 0)

void struct_init_code_register_elem(codegen instance, int type, Token elem){

}

void struct_init_code_register_subrule(codegen instance, int type, codegen child){

}

CODEGEN_DEC(struct_init_code, sem_rule_struct_init, 0, 0)

void cast_code_register_elem(codegen instance, int type, Token elem){

}

void cast_code_register_subrule(codegen instance, int type, codegen child){

}

CODEGEN_DEC(cast_code, sem_rule_cast, 0, 0);

void array_init_code_register_elem(codegen instance, int type, Token elem){
    
}

void array_init_code_register_subrule(codegen instance, int type, codegen child){
    
}

CODEGEN_DEC(array_init_code, sem_rule_array_init, 0, 0);

void array_entry_code_register_elem(codegen instance, int type, Token elem){
    
}

void array_entry_code_register_subrule(codegen instance, int type, codegen child){
    
}

CODEGEN_DEC(array_entry_code, sem_rule_array_entry, 0, 0);

void rule_sequence_code_register_elem(codegen instance, int type, Token elem){
    
}

void rule_sequence_code_register_subrule(codegen instance, int type, codegen child){
    
}

CODEGEN_DEC(rule_sequence_code, sem_rule_rule_sequence, 0, 0);

void rule_entry_code_register_elem(codegen instance, int type, Token elem){
    
}

void rule_entry_code_register_subrule(codegen instance, int type, codegen child){
    
}

CODEGEN_DEC(rule_entry_code, sem_rule_rule_entry, 0, 0);

#include "syscalls/syscalls.h"

void atom_code_register_elem(codegen instance, int type, Token elem){
    atom_code *code = (atom_code*)instance.ptr;
    switch (elem.kind){
        case TOK_NUMBER: 
            code->number = parse_int64(elem.start, elem.length);
            code->type = car_num;
        break;
        case TOK_IDENTIFIER: 
            if (slice_lit_match(token_to_slice(elem), "t", true))
                code->type = car_true;
            else if (slice_lit_match(token_to_slice(elem), "nil", true))
                code->type = car_none;
            else {
                code->val = token_to_slice(elem);
                code->type = car_identifier;
            }
        break;
        case TOK_STRING: 
            code->type = car_string;    
            code->val = token_to_slice(elem);
        break;
        default: print("[ERROR] unknown token type %s",token_name(elem.kind)); return;
    }
}

void atom_code_register_subrule(codegen instance, int type, codegen child){
    
}

#ifdef RULECODEGEN
CODEGEN_DEC(atom_code, sem_rule_atom, 0, 0);
#else
extern void atom_code_debug_print(codegen instance, int depth);
CODEGEN_DEC(atom_code, sem_rule_atom, 0, atom_code_debug_print);
#endif

void s_exp_code_register_elem(codegen instance, int type, Token elem){
    
}

void s_exp_code_register_subrule(codegen instance, int type, codegen child){
    s_exp_code *code = (s_exp_code*)instance.ptr;
    if (!code->car.ptr){
        code->car = child;
    } else {
        if (!code->cdr.ptr) code->cdr = s_exp_code_init();
        s_exp_code_register_subrule(code->cdr, type, child);
    }
}

#ifdef RULECODEGEN
CODEGEN_DEC(s_exp_code, sem_rule_sexp, 0, 0);
#else
extern void s_exp_code_debug_print(codegen instance, int depth);
CODEGEN_DEC(s_exp_code, sem_rule_sexp, 0, s_exp_code_debug_print);
#endif
