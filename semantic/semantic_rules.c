#include "semantic/semantic_rules.h"
#include "string/string.h"


char* sem_rule_strings[] = {
	[sem_rule_none] = "none",
	[sem_rule_dec] = "dec",
	[sem_rule_stat] = "stat",
	[sem_rule_var] = "var",
	[sem_rule_assign] = "assign",
	[sem_rule_exp] = "exp",
	[sem_rule_func] = "func",
	[sem_rule_label] = "label",
	[sem_rule_jmp] = "jmp",
	[sem_rule_scope] = "scope",
	[sem_rule_param] = "param",
	[sem_rule_val] = "val",
	[sem_rule_op] = "op",
	[sem_rule_cond] = "cond",
	[sem_rule_call] = "call",
	[sem_rule_args] = "args",
	[sem_rule_for] = "for",
	[sem_rule_while] = "while",
	[sem_rule_dowhile] = "dowhile",
	[sem_rule_inc] = "inc",
	[sem_rule_struct] = "struct",
	[sem_rule_rules_count] = "rules_count",
	[sem_rule_ret] = "ret",
	[sem_rule_def] = "def",
	[sem_rule_interf] = "interf",
	[sem_rule_enum] = "enum",
	[sem_rule_enum_case] = "enum_case",
	[sem_rule_parent] = "parent",
	[sem_rule_else] = "else",
	[sem_rule_syn] = "syn",
	[sem_rule_deref] = "deref",
	[sem_rule_addr] = "addr",
	[sem_rule_switch] = "switch",
	[sem_rule_cases] = "cases",
	[sem_rule_struct_init] = "struct_init",
	[sem_rule_prop_init] = "prop_init",
	[sem_rule_cast] = "cast",
	[sem_rule_array_init] = "array_init",
	[sem_rule_array_entry] = "array_entry",
	[sem_rule_rule_sequence] = "rule_sequence",
	[sem_rule_rule_entry] = "rule_entry",
	[sem_rule_sexp] = "sexp",
	[sem_rule_atom] = "atom",
};
sem_rule sem_rule_from_string(char * val){
	if (!strcmp("none", val)){
		return sem_rule_none;
	} else if (!strcmp("dec", val)){
		return sem_rule_dec;
	} else if (!strcmp("stat", val)){
		return sem_rule_stat;
	} else if (!strcmp("var", val)){
		return sem_rule_var;
	} else if (!strcmp("assign", val)){
		return sem_rule_assign;
	} else if (!strcmp("exp", val)){
		return sem_rule_exp;
	} else if (!strcmp("func", val)){
		return sem_rule_func;
	} else if (!strcmp("label", val)){
		return sem_rule_label;
	} else if (!strcmp("jmp", val)){
		return sem_rule_jmp;
	} else if (!strcmp("scope", val)){
		return sem_rule_scope;
	} else if (!strcmp("param", val)){
		return sem_rule_param;
	} else if (!strcmp("val", val)){
		return sem_rule_val;
	} else if (!strcmp("op", val)){
		return sem_rule_op;
	} else if (!strcmp("cond", val)){
		return sem_rule_cond;
	} else if (!strcmp("call", val)){
		return sem_rule_call;
	} else if (!strcmp("args", val)){
		return sem_rule_args;
	} else if (!strcmp("for", val)){
		return sem_rule_for;
	} else if (!strcmp("while", val)){
		return sem_rule_while;
	} else if (!strcmp("dowhile", val)){
		return sem_rule_dowhile;
	} else if (!strcmp("inc", val)){
		return sem_rule_inc;
	} else if (!strcmp("struct", val)){
		return sem_rule_struct;
	} else if (!strcmp("rules_count", val)){
		return sem_rule_rules_count;
	} else if (!strcmp("ret", val)){
		return sem_rule_ret;
	} else if (!strcmp("def", val)){
		return sem_rule_def;
	} else if (!strcmp("interf", val)){
		return sem_rule_interf;
	} else if (!strcmp("enum", val)){
		return sem_rule_enum;
	} else if (!strcmp("enum_case", val)){
		return sem_rule_enum_case;
	} else if (!strcmp("parent", val)){
		return sem_rule_parent;
	} else if (!strcmp("else", val)){
		return sem_rule_else;
	} else if (!strcmp("syn", val)){
		return sem_rule_syn;
	} else if (!strcmp("deref", val)){
		return sem_rule_deref;
	} else if (!strcmp("addr", val)){
		return sem_rule_addr;
	} else if (!strcmp("switch", val)){
		return sem_rule_switch;
	} else if (!strcmp("cases", val)){
		return sem_rule_cases;
	} else if (!strcmp("struct_init", val)){
		return sem_rule_struct_init;
	} else if (!strcmp("prop_init", val)){
		return sem_rule_prop_init;
	} else if (!strcmp("cast", val)){
		return sem_rule_cast;
	} else if (!strcmp("array_init", val)){
		return sem_rule_array_init;
	} else if (!strcmp("array_entry", val)){
		return sem_rule_array_entry;
	} else if (!strcmp("rule_sequence", val)){
		return sem_rule_rule_sequence;
	} else if (!strcmp("rule_entry", val)){
		return sem_rule_rule_entry;
	} else if (!strcmp("sexp", val)){
		return sem_rule_sexp;
	} else if (!strcmp("atom", val)){
		return sem_rule_atom;
	}return 0;
}


char* sem_elem_strings[] = {
	[sem_elem_none] = "none",
	[sem_elem_type] = "type",
	[sem_elem_name] = "name",
	[sem_elem_parent] = "parent",
	[sem_elem_subtype] = "subtype",
	[sem_elem_ref] = "ref",
	[sem_elem_count] = "count",
};
sem_elem sem_elem_from_string(char * val){
	if (!strcmp("none", val)){
		return sem_elem_none;
	} else if (!strcmp("type", val)){
		return sem_elem_type;
	} else if (!strcmp("name", val)){
		return sem_elem_name;
	} else if (!strcmp("parent", val)){
		return sem_elem_parent;
	} else if (!strcmp("subtype", val)){
		return sem_elem_subtype;
	} else if (!strcmp("ref", val)){
		return sem_elem_ref;
	} else if (!strcmp("count", val)){
		return sem_elem_count;
	}return 0;
}


char* sem_action_strings[] = {
	[sem_action_none] = "none",
	[sem_action_declare] = "declare",
	[sem_action_check] = "check",
};
sem_action sem_action_from_string(char * val){
	if (!strcmp("none", val)){
		return sem_action_none;
	} else if (!strcmp("declare", val)){
		return sem_action_declare;
	} else if (!strcmp("check", val)){
		return sem_action_check;
	}return 0;
}


char* semantic_types_strings[] = {
	[semantic_types_none] = "none",
	[semantic_types_literal] = "literal",
	[semantic_types_int64] = "int64",
	[semantic_types_int32] = "int32",
	[semantic_types_passthrough] = "passthrough",
};
semantic_types semantic_types_from_string(char * val){
	if (!strcmp("none", val)){
		return semantic_types_none;
	} else if (!strcmp("literal", val)){
		return semantic_types_literal;
	} else if (!strcmp("int64", val)){
		return semantic_types_int64;
	} else if (!strcmp("int32", val)){
		return semantic_types_int32;
	} else if (!strcmp("passthrough", val)){
		return semantic_types_passthrough;
	}return 0;
}
