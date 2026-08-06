#pragma once

#include "codegen/codegen.h"
#include "string/slice.h"

typedef enum { var_none, var_deref, var_addr } ref_transform;

typedef struct {
    codegen stat;
    codegen chain;
} blk_code;

codegen blk_code_init();

typedef struct {
    string_slice type;
    string_slice name;
    codegen initial_value;
} dec_code;

codegen dec_code_init();

typedef struct {
    codegen var;
    codegen expression;
} ass_code;

codegen ass_code_init();

typedef struct {
    codegen exp;
    codegen chain;
} arg_code;

codegen arg_code_init();

typedef struct {
    string_slice val;
    codegen var;
    string_slice operand;
    codegen exp;
    codegen lambda;
    bool paren;
    bool invert;
} exp_code;

codegen exp_code_init();

typedef struct {
    string_slice name;
    codegen args;
    codegen cast;
    ref_transform transform;
} call_code;

codegen call_code_init();

typedef struct {
    codegen cond;
    codegen scope;
    codegen chain;
} cond_code;

codegen cond_code_init();

typedef struct {
    string_slice jump;
} jmp_code;

codegen jmp_code_init();

typedef struct {
    string_slice name;
} label_code;

codegen label_code_init();

typedef struct {
    string_slice type;
    string_slice name;
    codegen chain;
} param_code;

codegen param_code_init();

typedef struct {
    string_slice type;
    string_slice name;
    codegen signature;
    codegen args;
    codegen body;
} func_code;

codegen func_code_init();

typedef struct {
    codegen initial;
    codegen increment;
    codegen condition;
    codegen body;
} for_code;

codegen for_code_init();
typedef struct {
    codegen condition;
    codegen body;
} while_code;

codegen while_code_init();
typedef struct {
    codegen condition;
    codegen body;
} dowhile_code;

codegen dowhile_code_init();

typedef struct {
    string_slice name;
    codegen var;
    codegen expression;
    string_slice operation;
    codegen cast;
    ref_transform transform;
} var_code;

codegen var_code_init();

typedef struct {
    Token value;
} inc_code;

codegen inc_code_init();

typedef struct {
    string_slice name;
    codegen contents;
    Token parent;
} struct_code;

codegen struct_code_init();

typedef struct {
    codegen expression;
} ret_code;

codegen ret_code_init();

typedef struct {
    codegen expression;
} def_code;

codegen def_code_init();

typedef struct {
    string_slice name;
    codegen contents;
} int_code;

codegen int_code_init();

typedef struct {
    Token name;
    codegen contents;
} enum_code;

codegen enum_code_init();

typedef struct {
    Token name;
    codegen chain;
} enum_case_code;

codegen enum_case_code_init();

typedef struct {
    codegen block;
} else_code;

codegen else_code_init();

typedef struct {
    codegen condition;
    codegen cases;
} switch_code;

codegen switch_code_init();

typedef struct {
    codegen match;
    codegen body;
    codegen chain;
} case_code;

codegen case_code_init();

typedef struct {
    string_slice name;
    codegen expression;
    codegen chain;
} prop_init_code;

codegen prop_init_code_init();

typedef struct {
    string_slice name;
    codegen content;
} struct_init_code;

codegen struct_init_code_init();

typedef struct {
    string_slice cast;
    bool reference;
} cast_code;

codegen cast_code_init();

typedef struct {
    codegen entries;
} array_init_code;

codegen array_init_code_init();

typedef struct {
    string_slice index;
    codegen chain;  
    codegen exp;
} array_entry_code;

codegen array_entry_code_init();

// MARK: Rule code

typedef struct {
    string_slice name;
    string_slice tag;
    string_slice value;
    bool optional;
    string_slice type;
    bool declare;
    codegen chain;
} rule_sequence_code;

codegen rule_sequence_code_init();

typedef struct {
    string_slice name;
    codegen list;  
    string_slice tag;
    bool declaration;
    codegen chain;
} rule_entry_code;

codegen rule_entry_code_init();

// MARK: Lisp code

typedef struct {
    enum { car_none, car_identifier, car_string, car_num, car_true } type;
    string_slice val;
    int number;
} atom_code;

codegen atom_code_init();

typedef struct {
    codegen car;
    codegen cdr;
} s_exp_code;

codegen s_exp_code_init();