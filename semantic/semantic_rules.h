#pragma once

typedef enum {
	sem_rule_none,
	sem_rule_dec,
	sem_rule_stat,
	sem_rule_var,
	sem_rule_assign,
	sem_rule_exp,
	sem_rule_func,
	sem_rule_label,
	sem_rule_jmp,
	sem_rule_scope,
	sem_rule_param,
	sem_rule_val,
	sem_rule_op,
	sem_rule_cond,
	sem_rule_call,
	sem_rule_args,
	sem_rule_for,
	sem_rule_while,
	sem_rule_dowhile,
	sem_rule_inc,
	sem_rule_struct,
	sem_rule_rules_count,
	sem_rule_ret,
	sem_rule_def,
	sem_rule_interf,
	sem_rule_enum,
	sem_rule_enum_case,
	sem_rule_parent,
	sem_rule_else,
	sem_rule_syn,
	sem_rule_deref,
	sem_rule_addr,
	sem_rule_switch,
	sem_rule_cases,
	sem_rule_struct_init,
	sem_rule_prop_init,
	sem_rule_cast,
	sem_rule_array_init,
	sem_rule_array_entry,
	sem_rule_rule_sequence,
	sem_rule_rule_entry,
	sem_rule_sexp,
	sem_rule_atom,
} sem_rule;

extern char* sem_rule_strings[];
sem_rule sem_rule_from_string(char * val);
typedef enum {
	sem_elem_none,
	sem_elem_type,
	sem_elem_name,
	sem_elem_parent,
	sem_elem_subtype,
	sem_elem_ref,
	sem_elem_count,
} sem_elem;

extern char* sem_elem_strings[];
sem_elem sem_elem_from_string(char * val);
typedef enum {
	sem_action_none,
	sem_action_declare,
	sem_action_check,
} sem_action;

extern char* sem_action_strings[];
sem_action sem_action_from_string(char * val);
typedef enum {
	semantic_types_none,
	semantic_types_literal,
	semantic_types_int64,
	semantic_types_int32,
	semantic_types_passthrough,
} semantic_types;

extern char* semantic_types_strings[];
semantic_types semantic_types_from_string(char * val);