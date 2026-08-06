#include "manual_gen.h"
#include "general.h"

codegen make_argument(codegen expression, codegen chain){
    codegen arg = arg_code_init();
    arg_code *code = arg.ptr;
    code->exp = expression;
    code->chain = chain;
    return arg;
}

codegen make_param(string_slice type, string_slice name, codegen chain){
    codegen param = param_code_init();
    param_code *code = param.ptr;
    code->name = name;
    code->type = type;
    code->chain = chain;
    return param;
}

codegen make_literal_expression(string_slice slice){
    codegen exp = exp_code_init();
    exp_code *code = exp.ptr;
    codegen var = var_code_init();
    ((var_code*)var.ptr)->name = slice;
    code->var = var;
    return exp;
}

codegen make_literal_var(string_slice slice){
    codegen var = var_code_init();
    ((var_code*)var.ptr)->name = slice;
    return var;
}

codegen var_to_exp(codegen var){
    codegen exp = exp_code_init();
    exp_code *code = exp.ptr;
    code->var = var;
    return exp;
}

codegen wrap_in_block(codegen statement, codegen chain, bool tail){
    codegen blk_ret_value = blk_code_init();
    blk_code *code = (blk_code*)blk_ret_value.ptr;
    code->stat = statement;
    if (tail && chain.ptr){
        blk_code *block = chain.ptr;
        while (block->chain.ptr){
            block = block->chain.ptr;
        }
        block->chain = blk_ret_value;
        return chain;
    } else code->chain = chain;
    return blk_ret_value;
}

codegen wrap_in_expression(codegen statement){
    codegen exp = exp_code_init();
    exp_code *code = exp.ptr;
    code->exp = statement;
    return exp;
}

codegen make_const_exp(string_slice exp){
    codegen exp_codegen = exp_code_init();
    exp_code *code = exp_codegen.ptr;
    code->val = exp;
    return exp_codegen;
}

codegen make_math(codegen lhs, string_slice op, codegen rhs){
    codegen exp_codegen = exp_code_init();
    exp_code *code = exp_codegen.ptr;
    code->var = lhs;
    code->operand = op;
    code->exp = rhs;
    return exp_codegen;
}

codegen make_label(literal name){
    codegen def_label = label_code_init();
    label_code *label = (label_code*)def_label.ptr;
    label->name = slice_from_literal(name);
    return def_label;
}

codegen make_declaration(literal name, string_slice type, codegen initial){
    codegen def_ret_value = dec_code_init();
    dec_code *declaration = (dec_code*)def_ret_value.ptr;
    declaration->name = slice_from_literal(name);
    declaration->type = type;
    declaration->initial_value = initial;
    return def_ret_value;
}

codegen make_return(codegen expression){
    codegen ret_value = ret_code_init();
    ret_code *retstat = (ret_code*)ret_value.ptr;
    retstat->expression = expression;
    return ret_value;
}

codegen make_if(codegen condition, codegen body, codegen chain){
    codegen cond = cond_code_init();
    cond_code *code = cond.ptr;
    code->cond = condition;
    code->scope = body;
    code->chain = chain;
    return cond;
}

codegen make_else(codegen els){
    codegen el = else_code_init();
    else_code *code = el.ptr;
    code->block = els;
    return el;
}

codegen invert_exp(codegen exp){
    exp_code *orig = exp.ptr;
    orig->invert = !orig->invert;
    return exp;
}

codegen make_var_chain(codegen lh, codegen rh, bool reference){
    codegen var = var_code_init();
    var_code *code = var.ptr;
    code->var = lh;
    code->expression = rh;
    code->operation = slice_from_literal(reference ? "->" : ".");
    return var;
}

codegen param_to_arg(codegen param){
    if (!param.ptr) return (codegen){};
    codegen arg = arg_code_init();
    arg_code *code = arg.ptr;
    param_code *pcode = param.ptr;
    code->exp = make_literal_expression(pcode->name);
    if (pcode->chain.ptr) code->chain = param_to_arg(pcode->chain);
    return arg;
}

codegen make_func_call(string_slice name, codegen args){
    codegen func = call_code_init();
    call_code *code = func.ptr;
    code->name = name;
    code->args = args;
    return func;
}

codegen make_function(string_slice type, string_slice name, codegen params, codegen body){
    codegen fun = func_code_init();
    func_code *code = fun.ptr;
    code->name = name;
    code->body = body;
    code->args = params;
    code->type = type;
    return fun;
}

codegen make_struct_init(string_slice type, codegen prop){
    codegen ini = struct_init_code_init();
    struct_init_code *code = ini.ptr;
    code->name = type;
    code->content = prop;
    return ini;
}

codegen make_prop_init(string_slice name, codegen exp, codegen chain){
    codegen ini = prop_init_code_init();
    prop_init_code *code = ini.ptr;
    code->name = name;
    code->expression = exp;
    code->chain = chain;
    return ini;
}

codegen make_cast(string_slice type, bool reference){
    codegen cast = cast_code_init();
    cast_code *code = cast.ptr;
    code->cast = type;
    code->reference = reference;
    return cast;
}

codegen make_array(codegen value){
    codegen ret = array_init_code_init();
    array_init_code *code = ret.ptr;
    code->entries = value;
    return ret;
}

codegen make_indexed_array_entry(string_slice index, codegen exp, codegen chain){
    codegen ret = array_entry_code_init();
    array_entry_code *code = ret.ptr;
    code->exp = exp;
    code->index = index;
    code->chain = chain;
    return ret;
}

codegen make_rule(string_slice name, string_slice tag, bool declaration, codegen options, codegen existing){
    codegen rule = rule_entry_code_init();
    rule_entry_code *code = rule.ptr;
    code->name = name;
    code->tag = tag;
    code->declaration = declaration;
    code->list = options;
    if (existing.ptr) {
        rule_entry_code *old = existing.ptr;
        while (old->chain.ptr) old = old->chain.ptr;
        old->chain = rule;
        return existing;
    }
    return rule;
}

codegen make_rule_sequence(string_slice name, string_slice value, string_slice type, string_slice tag, bool optional, codegen existing){
    codegen rule = rule_sequence_code_init();
    rule_sequence_code *code = rule.ptr;
    code->name = name;
    code->value = value;
    code->type = type;
    code->tag = tag;
    code->optional = optional;
    if (existing.ptr) {
        rule_sequence_code *old = existing.ptr;
        while (old->chain.ptr) old = old->chain.ptr;
        old->chain = rule;
        return existing;
    }
    return rule;
}

codegen make_id_atom(string_slice sl){
    codegen s = atom_code_init();
    atom_code *code = s.ptr;
    code->val = sl;
    code->type = car_identifier;
    return s;
}

codegen make_int_atom(i64 a){
    codegen s = atom_code_init();
    atom_code *code = s.ptr;
    code->number = a;
    code->type = car_num;
    return s;
}

codegen make_true_atom(){
    codegen s = atom_code_init();
    atom_code *code = s.ptr;
    code->type = car_true;
    return s;
}

codegen make_list(codegen single){
    codegen s = s_exp_code_init();
    s_exp_code *code = s.ptr;
    code->car = single;
    return s;
}

codegen make_lambda(codegen contents){
    codegen s = s_exp_code_init();
    s_exp_code *code = s.ptr;
    code->car = make_id_atom(SLICE("lambda"));
    code->cdr = contents;
    return s;
}