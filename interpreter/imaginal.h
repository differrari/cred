#pragma once

#include "ir/general.h"
#include "data/struct/hashmap.h"
#include "data/struct/stack.h"

#define nil_exp (codegen){}
typedef arr_stack_t *imaginal_env;

typedef codegen (*imaginal_fallback)(codegen fn_exp, codegen a, codegen *env);

extern imaginal_fallback imaginal_fallback_fncall;

static inline bool is_atom(codegen exp){
    return exp.type == sem_rule_atom;
} 

static inline codegen car(codegen a){
    if (a.type != sem_rule_sexp) return nil_exp;
    if (is_atom(a)) return nil_exp;
    s_exp_code *arg = a.ptr;
    return arg->car;
}

static inline codegen cdr(codegen a){
    if (a.type != sem_rule_sexp) return nil_exp;
    if (is_atom(a)) return nil_exp;
    s_exp_code *arg = a.ptr;
    return arg->cdr;
}

static inline codegen cons(codegen a, codegen b){
    codegen news = s_exp_code_init();
    s_exp_code *code = news.ptr;
    code->car = a;
    code->cdr = b;
    return news;
}

static inline string_slice car_id(codegen car){
    if (car.type != sem_rule_atom) return (string_slice){};
    atom_code *code = car.ptr;
    if (code->type != car_identifier) return (string_slice){};
    return code->val;
}

static inline string_slice car_get_string(codegen car){
    if (car.type != sem_rule_atom) return (string_slice){};
    atom_code *code = car.ptr;
    if (code->type != car_string) return (string_slice){};
    string_slice sl = code->val;
    if (sl.length > 2) return (string_slice){sl.data+1,sl.length-2};
    return sl;
}

static inline i64 car_integer(codegen exp){
    codegen c = car(exp);
    if (c.type != sem_rule_atom) return 0;
    atom_code *atom = c.ptr;
    if (atom->type == car_flo) return (i64)atom->floating;
    if (atom->type != car_int) return 0;
    return atom->integer;
}

static inline double car_double(codegen exp){
    codegen c = car(exp);
    if (c.type != sem_rule_atom) return 0;
    atom_code *atom = c.ptr;
    if (atom->type == car_int) return (double)atom->integer;
    if (atom->type != car_flo) return 0;
    return atom->floating;
}

codegen apply(codegen fn_exp, codegen a, codegen *env);
codegen eval(codegen exp, codegen *env);
codegen evlis(codegen l, codegen *env);
bool is_atom(codegen exp);

void imaginal_print(codegen exp);
