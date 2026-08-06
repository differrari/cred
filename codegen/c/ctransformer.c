#ifdef CTRANS

#include "ir/general.h"
#include "ir/arch_transformer.h"
#include "emit_context.h"
#include "string/slice.h"
#include "ir/manual_gen.h"
#include "semantic/sem_analysis.h"
#include "c_syms.h"

string_slice resolve_type(symbol_t *sym, bool subtype, string_slice fallback){
    if (!sym) return fallback;
    return slice_from_string(type_name(sym, subtype, true));
}

codegen blk_code_transform(codegen instance){
    blk_code *code = (blk_code*)instance.ptr;
    TRANSFORM(stat);
    if (ctx.lambdas.ptr && code->stat.type == sem_rule_func){
        blk_code *lcode = ctx.lambdas.ptr;
        while (lcode->chain.ptr){
            lcode = lcode->chain.ptr;
        }
        TRANSFORM(chain);
        lcode->chain = instance;
        return ctx.lambdas;
    }
    TRANSFORM(chain);
    return instance;
}

codegen dec_code_transform(codegen instance){
    dec_code *code = (dec_code*)instance.ptr;
    symbol_t *sym = find_symbol(sem_rule_dec, code->name);
    code->type = resolve_type(sym, false, code->type);
    if (code->initial_value.ptr){
        TRANSFORM(initial_value);
    } else {
        if (sym->reference_type)
            code->initial_value = make_literal_expression(slice_from_literal("0"));
        else 
            code->initial_value = make_struct_init(code->type, (codegen){});
    }
    return instance;
}

codegen ass_code_transform(codegen instance){
    ass_code *code = (ass_code*)instance.ptr;
    TRANSFORM(expression);
    return instance;
}

codegen lambda_to_func(codegen lambda, string_slice name){
    codegen func = func_code_init();
    func_code *code = func.ptr;
    func_code *lcode = lambda.ptr;
    code->name = name;
    code->args = lcode->args;
    code->body = lcode->body;
    code->type = lcode->type;
    return func;
}

codegen exp_code_transform(codegen instance){
    exp_code *code = (exp_code*)instance.ptr;
    TRANSFORM(lambda);
    if (code->lambda.ptr){
        string_slice lamname = make_temp_name(sem_rule_func);
        ctx.lambdas = wrap_in_block(lambda_to_func(code->lambda,lamname), ctx.lambdas, true);
        return make_literal_expression(lamname);
    }
    TRANSFORM(var);
    TRANSFORM(exp);
    return instance;
}

codegen call_code_transform(codegen instance){
    call_code *code = (call_code*)instance.ptr;
    TRANSFORM(args);
    return instance;
}

codegen arg_code_transform(codegen instance){
    arg_code *code = (arg_code*)instance.ptr;
    TRANSFORM(chain);
    TRANSFORM(exp);
    return instance;
}

codegen cond_code_transform(codegen instance){
    cond_code *code = (cond_code*)instance.ptr;
    TRANSFORM(cond);
    TRANSFORM(chain);
    TRANSFORM(scope);
    return instance;
}

codegen jmp_code_transform(codegen instance){
    jmp_code *code = (jmp_code*)instance.ptr;
    return instance;
}

codegen label_code_transform(codegen instance){
    label_code *code = (label_code*)instance.ptr;
    return instance;
}

codegen param_code_transform(codegen instance){
    param_code *code = (param_code*)instance.ptr;
    code->type = resolve_type(find_symbol(sem_rule_param, code->name), false, code->type);
    TRANSFORM(chain);
    return instance;
}

void replace_returns(codegen body, bool has_return){
    if (body.type != sem_rule_scope) return;
    blk_code *code = body.ptr;
    
    if (code->stat.type == sem_rule_ret){
        ret_code *ret = code->stat.ptr;
        
        codegen goto_code = jmp_code_init();
        jmp_code *go = goto_code.ptr;
        go->jump = slice_from_literal("defer");
        
        codegen b = wrap_in_block(goto_code, (codegen){}, false);
        
        if (has_return){
            codegen assign_code = ass_code_init();
            ass_code *ass = assign_code.ptr;
            ass->var = make_literal_var(slice_from_literal("__return_val"));
            ass->expression = ret->expression;
            b = wrap_in_block(assign_code, b, false);
        }
        
        code->stat = b;
        
    } else if (code->stat.ptr) {
        codegen subblock = codegen_get_subscope(code->stat);
        if (subblock.ptr){
            replace_returns(subblock, has_return);
        }
    }
    
    if (code->chain.ptr) replace_returns(code->chain, has_return);
    
}

codegen func_code_transform(codegen instance){
    func_code *code = (func_code*)instance.ptr;
    
    if (code->signature.ptr){
        func_code *sig = code->signature.ptr;
        if (sig->type.length) code->type = sig->type;
        if (sig->name.length) code->name = sig->name;
        if (sig->args.ptr) code->args = sig->args;
        release(sig);
        code->signature = (codegen){};
    }
    
    code->type = resolve_type(find_symbol(sem_rule_func, code->name),false,code->type);
    TRANSFORM(args);
    emit_context orig = save_and_push_context((emit_context){ .context_rule = sem_rule_func });
    TRANSFORM(body);
    if (ctx.defer_statements.ptr){
        bool has_return = code->type.length && !slice_lit_match(code->type, "void", false);
        if (has_return) code->body = wrap_in_block(make_declaration("__return_val", code->type, (codegen){}), code->body, false);
        code->body = wrap_in_block(make_label("defer"), code->body, true);
        code->body = wrap_in_block(ctx.defer_statements, code->body, true);
        replace_returns(code->body, has_return);
        if (has_return) code->body = wrap_in_block(make_return(make_const_exp(slice_from_literal("__return_val"))), code->body, true);
    }
    orig.lambdas = ctx.lambdas;
    pop_and_restore_context(orig);
    
    return instance;
}

codegen for_code_transform(codegen instance){
    for_code *code = (for_code*)instance.ptr;
    TRANSFORM(initial);
    TRANSFORM(increment);
    TRANSFORM(condition);
    TRANSFORM(body);
    return instance;
}

codegen while_code_transform(codegen instance){
    while_code *code = (while_code*)instance.ptr;
    TRANSFORM(condition);
    TRANSFORM(body);
    return instance;
}

codegen dowhile_code_transform(codegen instance){
    dowhile_code *code = (dowhile_code*)instance.ptr;
    TRANSFORM(condition);
    TRANSFORM(body);
    return instance;
}

codegen var_code_transform(codegen instance){
    var_code *code = (var_code*)instance.ptr;
    string_slice orig_var_name = code->var.ptr ? ((var_code*)code->var.ptr)->name : (string_slice){};
    TRANSFORM(var);
    TRANSFORM(expression);
    string_slice orig_name = code->name.length ? code->name : (string_slice){};
    if (code->name.length){
        symbol_t *sym = find_symbol(sem_rule_dec, code->name);
        if (sym && sym->table_type == sem_rule_struct){
            code->var = make_var_chain(make_literal_var(slice_from_literal("instance")), make_literal_var(code->name), true);
            code->name = (string_slice){};
        }
    }
    if (code->operation.length){
        if (*code->operation.data == '.' && code->expression.type == sem_rule_call){
            call_code *function = code->expression.ptr;
            function->args = make_argument(code->name.length ? make_literal_expression(code->name) : var_to_exp(code->var), function->args);
            symbol_t *sym = find_symbol(sem_rule_dec, orig_var_name);
            if (sym){
                string s = type_name(sym, false, false);
                function->name = slice_from_string(string_format("%S_%v", s, function->name));
            }
            return code->expression;
        }
        else if (*code->operation.data == '['){
            codegen call = call_code_init();
            call_code *function = call.ptr;
            string_slice symname = orig_name.length ? orig_name : orig_var_name;
            symbol_t *sym = find_symbol(sem_rule_dec, symname);
            if (sym){
                string s = type_name(sym, false, false);
                if (sym->resolved_subtype){
                    function->transform = var_deref;
                    function->cast = make_cast(slice_from_string(type_name(sym, true, false)), true);
                }
                function->name = slice_from_string(string_format("%S_get",s));
            }
            function->args = make_argument(code->name.length ? make_literal_expression(code->name) : var_to_exp(code->var), make_argument(code->expression,function->args));
            return call;
        }
    }
    return instance;
}

codegen inc_code_transform(codegen instance){
    inc_code *code = (inc_code*)instance.ptr;
    return instance;
}

codegen struct_code_transform(codegen instance){
    struct_code *code = (struct_code*)instance.ptr;
    blk_code *dec = code->contents.ptr;
    codegen extracted = {};
    codegen maintained = {};
    codegen props = {};
    emit_context ctx = save_and_push_context((emit_context){.context_rule = sem_rule_struct });
    while (dec){
        if (dec->stat.ptr){
            if (dec->stat.type == sem_rule_dec){
                maintained = wrap_in_block(codegen_transform(dec->stat),maintained,true);
            } else if (dec->stat.type == sem_rule_func){
                func_code *function = dec->stat.ptr;
                function->body = wrap_in_block(make_declaration("*instance", code->name, make_const_exp(SLICE("instance.ptr"))),codegen_transform(function->body),false);
                string_slice parent_name = function->name;
                function->name = slice_from_string(string_format("%v_%v",code->name,function->name));
                function->args = make_param(token_to_slice(code->parent),slice_from_literal("interf"), function->args);
                extracted = wrap_in_block(dec->stat, extracted, true);
                if (code->parent.length){
                    symbol_t *parent_sym = find_symbol(sem_rule_interf, token_to_slice(code->parent));
                    if (!parent_sym || !parent_sym->child){
                        print("Error: interface %v not found. Should've done better semantic analysis. smh",token_to_slice(code->parent));
                        return instance;
                    }
                    symbol_table *table = parent_sym->child;
                    for (int i = 0; i < table->symbol_count; i++){
                        symbol_t *sym = &table->symbol_table[i];
                        if (slices_equal(sym->name, parent_name, false) && sym->sym_type == sem_rule_func){
                            props = make_prop_init(parent_name, make_literal_expression(function->name), props);
                            break;
                        }
                    }
                }
            }
        }
        dec = dec->chain.ptr;
    }
    code->contents = maintained;
    if (code->parent.length){
        props = make_prop_init(slice_from_literal("ptr"), make_func_call(slice_from_literal("zalloc"), make_func_call(slice_from_literal("sizeof"), make_literal_expression(code->name))), props);
        extracted = wrap_in_block(
            make_function(token_to_slice(code->parent), slice_from_string(string_format("%v_init",code->name)), (codegen){}, 
                make_return(make_struct_init(token_to_slice(code->parent), props))
            ), extracted, true);
    }
    return wrap_in_block(instance,extracted, false);
}

codegen ret_code_transform(codegen instance){
    ret_code *code = (ret_code*)instance.ptr;
    TRANSFORM(expression);
    return instance;
}

codegen def_code_transform(codegen instance){
    def_code *code = (def_code*)instance.ptr;
    TRANSFORM(expression);
    ctx.defer_statements = wrap_in_block(code->expression, ctx.defer_statements, true);
    return (codegen){};
}

codegen int_code_transform(codegen instance){
    int_code *code = (int_code*)instance.ptr;
    blk_code *dec = code->contents.ptr;
    codegen extracted = {};
    while (dec){
        if (dec->stat.ptr){
            if (dec->stat.type == sem_rule_dec){
                dec->stat = codegen_transform(dec->stat);
            } else if (dec->stat.type == sem_rule_func){
                codegen funcptr = func_code_init();
                func_code *stub = funcptr.ptr;
                func_code *function = dec->stat.ptr;
                stub->type = function->type;
                stub->name = function->name;
                function->name = slice_from_string(string_format("%v_%v",code->name,function->name));
                stub->args = make_param(code->name,slice_from_literal("instance"), function->args);
                codegen call = make_argument(make_literal_expression(slice_from_literal("instance")), param_to_arg(function->args));
                function->args = make_param(code->name,slice_from_literal("instance"), function->args);
                codegen def = !function->type.length || slice_lit_match(function->type, "void", true) ? (codegen){} : make_return(make_literal_expression(slice_from_string(string_format("(%v){}",function->type))));
                function->body = 
                    wrap_in_block(
                        make_if(make_var_chain(make_literal_expression(slice_from_literal("instance")), make_literal_expression(stub->name),false), 
                            make_return(make_var_chain(make_literal_expression(slice_from_literal("instance")),make_func_call(stub->name, call), false)), 
                        (codegen){}), 
                    def, false); //TODO: proper default handling once we have a better type system
                extracted = wrap_in_block(dec->stat, extracted, true);
                dec->stat = funcptr;
            }
        }
        dec = dec->chain.ptr;
    }
    code->contents = wrap_in_block(make_declaration("ptr", slice_from_literal("void*"), (codegen){}), code->contents, false);
    return wrap_in_block(instance,extracted, false);
}

codegen enum_code_transform(codegen instance){
    enum_code *code = (enum_code*)instance.ptr;
    TRANSFORM(contents);
    
    emit_context orig = save_and_push_context((emit_context){ .convenience = convenience_type_to_string, .context_prefix = token_to_slice(code->name) });
    codegen to_str = make_declaration(string_format("%v_strings[]",token_to_slice(code->name)).data, slice_from_literal("char*"), make_array(codegen_transform(code->contents)));
    pop_and_restore_context(orig);
    
    orig = save_and_push_context((emit_context){ .convenience = convenience_type_from_string, .context_prefix = token_to_slice(code->name) });
    codegen from_str = codegen_transform(code->contents);
    pop_and_restore_context(orig);
    
    return wrap_in_block(instance, 
        wrap_in_block(to_str,
           make_function(token_to_slice(code->name), slice_from_string(string_format("%v_from_string",token_to_slice(code->name))), make_param(slice_from_literal("char *"), slice_from_literal("val"), (codegen){}), from_str),
          false), 
        false);
}

codegen enum_case_code_transform(codegen instance){
    enum_case_code *code = (enum_case_code*)instance.ptr;
    if (ctx.convenience == convenience_type_to_string){
        return make_indexed_array_entry(slice_from_string(string_format("%v_%v",ctx.context_prefix, token_to_slice(code->name))), make_const_exp(slice_from_string(string_format("\"%v\"",token_to_slice(code->name)))), codegen_transform(code->chain));
    } if (ctx.convenience == convenience_type_from_string){
        codegen chain = code->chain.ptr ? make_else(codegen_transform(code->chain)) : make_return(make_const_exp(SLICE("0")));
        codegen i = make_if(
            invert_exp(wrap_in_expression(make_func_call(slice_from_literal("strcmp"), 
                make_argument(make_const_exp(slice_from_string(string_format("\"%v\"",token_to_slice(code->name)))), 
                make_argument(make_literal_var(slice_from_literal("val")), (codegen){})))))
            , 
            make_return(make_const_exp(slice_from_string(string_format("%v_%v",ctx.context_prefix,token_to_slice(code->name))))), chain);
        return i;
        
    }
    TRANSFORM(chain);
    return instance;
}

codegen else_code_transform(codegen instance){
    else_code *code = (else_code*)instance.ptr;
    TRANSFORM(block);
    return instance;
}

codegen switch_code_transform(codegen instance){
    switch_code *code = (switch_code*)instance.ptr;
    TRANSFORM(condition);
    TRANSFORM(cases);
    return instance;
}

codegen case_code_transform(codegen instance){
    case_code *code = (case_code*)instance.ptr;
    TRANSFORM(match);
    TRANSFORM(body);
    TRANSFORM(chain);
    return instance;
}

codegen prop_init_code_transform(codegen instance){
    return instance;
}

codegen struct_init_code_transform(codegen instance){
    return instance;
}

codegen cast_code_transform(codegen instance){
    return instance;
}

codegen array_init_code_transform(codegen instance){
    return instance;
}

codegen array_entry_code_transform(codegen instance){
    return instance;
}

codegen atom_code_transform(codegen instance){
    return instance;
}

codegen perform_transformations(codegen root){
    return codegen_transform(root);
}

#endif

