#pragma once

#include "codegen/codegen.h"

codegen wrap_in_block(codegen statement, codegen chain, bool tail);
codegen wrap_in_expression(codegen statement);
codegen make_const_exp(string_slice exp);
codegen make_label(literal name);
codegen make_declaration(literal name, string_slice type, codegen initial);
codegen make_return(codegen expression);
codegen make_argument(codegen expression, codegen chain);
codegen make_param(string_slice type, string_slice name, codegen chain);
codegen make_literal_expression(string_slice slice);
codegen make_literal_var(string_slice slice);
codegen make_if(codegen condition, codegen body, codegen chain);
codegen invert_exp(codegen exp);
codegen make_else(codegen els);
codegen make_var_chain(codegen lh, codegen rh, bool reference);
codegen param_to_arg(codegen param);
codegen make_func_call(string_slice name, codegen args);
codegen lambda_to_func(codegen lambda, string_slice name);
codegen var_to_exp(codegen var);
codegen make_function(string_slice type, string_slice name, codegen params, codegen body);
codegen make_struct_init(string_slice type, codegen prop);
codegen make_prop_init(string_slice type, codegen exp, codegen chain);
codegen make_cast(string_slice type, bool reference);
codegen make_array(codegen value);
codegen make_indexed_array_entry(string_slice index, codegen exp, codegen chain);
codegen make_math(codegen lhs, string_slice op, codegen rhs);
codegen make_rule(string_slice name,  string_slice tag, bool declaration, codegen options, codegen existing);
codegen make_rule_sequence(string_slice name, string_slice value, string_slice type, string_slice tag, bool optional, codegen existing);
codegen make_id_atom(string_slice sl);
codegen make_int_atom(i64 a);
codegen make_flo_atom(double a);
codegen make_true_atom();
codegen make_list(codegen single);
codegen make_lambda(codegen contents);

static inline codegen make_array_entry(codegen exp, codegen chain){
    return make_indexed_array_entry((string_slice){}, exp, chain);
}