#ifdef CCODEGEN

#include "ir/general.h"
#include "../codeformat.h"
#include "semantic/sem_analysis.h"
#include "emit_context.h"
#include "c_syms.h"

tern is_header = false;

void generate_code(const char *name, codegen cg){
    
    int32_t ext = str_has_char(name, 0, '.');
    if (ext >= 0){
        is_header = -1;
        codegen_emit_code(cg);
        output_code(name,"");
    } else {
        is_header = true;
        emit("#pragma once\n\n",name);
        codegen_emit_code(cg);
        output_code(name,"h");
        
        reset_emit_buffer();
        
        is_header = false;
        emit("#include \"%s.h\"\n",name);
        emit_const("#include \"string/string.h\"\n\n");
        codegen_emit_code(cg);
        output_code(name,"c");
    }
}

bool blk_code_emit_code(codegen instance){
    blk_code *code = (blk_code*)instance.ptr;
    bool emitted = codegen_emit_code(code->stat);
    if (code->chain.ptr){ 
        if (emitted) emit_newline(); 
        codegen_emit_code(code->chain); 
    }
    return emitted;
}

void emit_type(symbol_t *sym, bool extra){
    string s = type_name(sym, false, extra);
    emit_slice(slice_from_string(s));
    string_free(s);
}

bool dec_code_emit_code(codegen instance){
    dec_code *code = (dec_code*)instance.ptr;
    if (is_header == false && (ctx.context_rule == sem_rule_struct || ctx.context_rule == sem_rule_interf)) return false;
    if (is_header == true && ctx.context_rule != sem_rule_struct && ctx.context_rule != sem_rule_interf)
        emit_const("extern ");
    emit_slice(code->type);
    emit_space();
    emit_slice(code->name);
    if (is_header != true && code->initial_value.ptr){
        emit_context orig = save_and_push_context((emit_context){ .context_rule = sem_rule_dec, .ignore_semicolon = true });
        emit_const(" = ");
        codegen_emit_code(code->initial_value);
        pop_and_restore_context(orig);
    }
    if (!ctx.ignore_semicolon) emit_const(";");
    return true;
}

bool ass_code_emit_code(codegen instance){
    if (is_header == true) return false;
    ass_code *code = (ass_code*)instance.ptr;
    emit_context orig = save_and_push_context((emit_context){ .context_rule = sem_rule_assign, .ignore_semicolon = true });
    codegen_emit_code(code->var);
    emit_const(" = ");
    codegen_emit_code(code->expression);
    pop_and_restore_context(orig);
    if (!ctx.ignore_semicolon) emit_const(";");
    return true;
}

bool call_code_emit_code(codegen instance){
    if (is_header == true) return false;
    call_code *code = (call_code*)instance.ptr;
    if (code->transform == var_deref){
        emit_const("*");
    }
    if (code->transform == var_addr){
        emit_const("&");
    }
    if (code->cast.ptr)
        codegen_emit_code(code->cast);
    emit_context orig = save_and_push_context((emit_context){ .context_rule = sem_rule_call, .ignore_semicolon = true });
    emit_slice(code->name);
    emit_const("(");
    codegen_emit_code(code->args);
    emit_const(")");
    pop_and_restore_context(orig);
    if (!ctx.ignore_semicolon) emit_const(";");
    return true;
}

bool cond_code_emit_code(codegen instance){
    if (is_header == true) return false;
    cond_code *code = (cond_code*)instance.ptr;
    // print("Condition code emit being %i",(*(codegen_t*)code->cond).ptr);
    emit_const("if (");
    emit_context orig = save_and_push_context((emit_context){ .ignore_semicolon = true });
    codegen_emit_code(code->cond);
    pop_and_restore_context(orig);
    emit_const("){");
    increase_indent(true);
        codegen_emit_code(code->scope);
    decrease_indent(true);
    emit_const("}");
    if (code->chain.ptr) codegen_emit_code(code->chain);
    else emit_newline();
    return true;
}

bool jmp_code_emit_code(codegen instance){
    if (is_header == true) return false;
    jmp_code *code = (jmp_code*)instance.ptr;
    emit_const("goto ");
    emit_slice(code->jump);
    emit_const(";");
    return true;
}

bool label_code_emit_code(codegen instance){
    if (is_header == true) return false;
    label_code *code = (label_code*)instance.ptr;
    emit_const("\r");
    emit_slice(code->name);
    emit_const(":");
    return true;
}

bool exp_code_emit_code(codegen instance){
    if (is_header == true) return false;
    exp_code *code = (exp_code*)instance.ptr;
    if (code->paren)
        emit_const("(");
    if (code->invert)
        emit_const("!");
    emit_context orig = save_and_push_context((emit_context){ .ignore_semicolon = true });
    if (code->var.ptr) codegen_emit_code(code->var);
    else emit_slice(code->val);
    if (code->exp.ptr){
        if (code->val.length || code->var.ptr) emit_space();
        if (code->operand.length){
            emit_slice(code->operand);
            emit_space();
        }
        codegen_emit_code(code->exp);
    }
    if (code->paren)
        emit_const(")");
    pop_and_restore_context(orig);
    return true;
}

bool arg_code_emit_code(codegen instance){
    if (is_header == true) return false;
    arg_code *code = (arg_code*)instance.ptr;
    codegen_emit_code(code->exp);
    if (code->chain.ptr){
        emit_const(", ");
        codegen_emit_code(code->chain);
    }
    return true;
}

bool param_code_emit_code(codegen instance){
    param_code *code = (param_code*)instance.ptr;
    emit_slice(code->type);
    emit_space();
    emit_slice(code->name);
    if (code->chain.ptr){
        emit_const(", ");
        codegen_emit_code(code->chain);
    }
    return true;
}

bool func_code_emit_code(codegen instance){
    func_code *code = (func_code*)instance.ptr;
    if (code->type.length){
        emit_slice(code->type);
        emit_const(" ");
    } else 
        emit_const("void ");
    if (ctx.context_rule == sem_rule_struct){
        emit_slice(ctx.context_prefix);
        emit_const("_");
    }
    if (ctx.context_rule == sem_rule_interf){
        emit_const("(*");
    }
    emit_slice(code->name);
    if (ctx.context_rule == sem_rule_interf){
        emit_const(")");
    }
    emit_const("(");
    codegen_emit_code(code->args);
    if (ctx.context_rule == sem_rule_interf || is_header == true){
        emit_const(");");
    } else {
        emit_const("){");
        emit_context orig = save_and_push_context((emit_context){ .context_prefix = code->name, .ignore_semicolon = false });
        increase_indent(true);
            codegen_emit_code(code->body);
        decrease_indent(true);
        emit_const("}");
        emit_newline();
        pop_and_restore_context(orig);
    }
    return true;
}

bool for_code_emit_code(codegen instance){
    if (is_header == true) return false;
    for_code *code = (for_code*)instance.ptr;
    emit_context orig = save_and_push_context((emit_context){ .ignore_semicolon = true });
    emit_const("for (");
    codegen_emit_code(code->initial);
    emit_const("; ");
    codegen_emit_code(code->condition);
    emit_const("; ");
    codegen_emit_code(code->increment);
    emit_const(")");
    pop_and_restore_context(orig);
    emit_const("{");
    increase_indent(true);
        if (code->body.ptr) codegen_emit_code(code->body);
    decrease_indent(true);
    emit_const("}");
    return true;
}

bool while_code_emit_code(codegen instance){
    if (is_header == true) return false;
    while_code *code = (while_code*)instance.ptr;
    emit_const("while (");
    codegen_emit_code(code->condition);
    emit_const("){");
    increase_indent(true);
        if (code->body.ptr) codegen_emit_code(code->body);
    decrease_indent(true);
    emit_const("}");
    return true;
}

bool dowhile_code_emit_code(codegen instance){
    if (is_header == true) return false;
    dowhile_code *code = (dowhile_code*)instance.ptr;
    emit_const("do {");
    increase_indent(true);
        if (code->body.ptr) codegen_emit_code(code->body);
    decrease_indent(true);
    emit_const("} while (");
        codegen_emit_code(code->condition);
    emit_const(");");
    return true;
}

bool var_code_emit_code(codegen instance){
    if (is_header == true) return false;
    var_code *code = (var_code*)instance.ptr;
    if (code->var.ptr) codegen_emit_code(code->var); 
    else { 
        
        if (code->transform == var_deref){
            emit_const("*");
        }
        if (code->transform == var_addr){
            emit_const("&");
        }
        if (code->cast.ptr)
            codegen_emit_code(code->cast);
        emit_slice(code->name);
    }
    if (code->operation.length) emit_slice(code->operation);
    if (code->expression.ptr){
        emit_context orig = save_and_push_context((emit_context){.ignore_semicolon = true});
        codegen_emit_code(code->expression);
        pop_and_restore_context(orig);
    } 
    if (!ctx.ignore_semicolon) emit_const(";");
    return true;
}

bool inc_code_emit_code(codegen instance){
    if (is_header == false) return false;
    inc_code *code = (inc_code*)instance.ptr;
    emit_const("#include ");
    emit_token(code->value);
    return true;
}

bool struct_code_emit_code(codegen instance){
    struct_code *code = (struct_code*)instance.ptr;
    emit_context orig = save_and_push_context((emit_context){ .context_rule = sem_rule_struct, .ignore_semicolon = false, .context_prefix = code->name, .context_parent = token_to_slice(code->parent) });

    if (is_header != false){
        emit_const("typedef struct ");
        emit_slice(code->name);
        emit_const(" { ");
        increase_indent(true);
            codegen_emit_code(code->contents);
        decrease_indent(true);
        emit(" } %v;",code->name);
    } else codegen_emit_code(code->contents);
    pop_and_restore_context(orig);
    return true;
}

bool ret_code_emit_code(codegen instance){
    if (is_header == true) return false;
    ret_code *code = (ret_code*)instance.ptr;
    emit_context orig = save_and_push_context((emit_context){.context_rule = sem_rule_ret, .ignore_semicolon = true });
    emit_const("return");
    if (code->expression.ptr){
        emit_const(" ");
        codegen_emit_code(code->expression);
    }
    pop_and_restore_context(orig);
    emit_const(";");
    return true;
}

bool def_code_emit_code(codegen instance){
    return true;
}

bool int_code_emit_code(codegen instance){
    int_code *code = (int_code*)instance.ptr;
    emit_context orig = save_and_push_context((emit_context){ .context_rule = sem_rule_interf, .ignore_semicolon = false, .context_prefix = code->name });
    if (is_header != false){
        emit_const("typedef struct ");
        emit_slice(code->name);
        emit_const(" ");
        emit_slice(code->name);
        emit_const(";");
        emit_newline();
        emit_const("struct ");
        emit_slice(code->name);
        emit_const(" { ");
        increase_indent(true);
            codegen_emit_code(code->contents);
        decrease_indent(true);
        emit_const(" };");
        emit_newline();
    }
    pop_and_restore_context(orig);
    return is_header != false;
}

bool enum_code_emit_code(codegen instance){
    enum_code *code = (enum_code*)instance.ptr;
    if (is_header != false){
        emit_const("typedef enum {");
        increase_indent(true);
            emit_context orig = save_and_push_context((emit_context){ .convenience = convenience_type_none, .context_prefix = token_to_slice(code->name) });
            codegen_emit_code(code->contents);
            pop_and_restore_context(orig);
        decrease_indent(true);
        emit_const("} ");
        emit_token(code->name);
        emit_const(";");
        emit_newline();
    }
    return true;
}

bool enum_case_code_emit_code(codegen instance){
    enum_case_code *code = (enum_case_code*)instance.ptr;
    
    emit_slice(ctx.context_prefix);
    emit_const("_");
    emit_token(code->name);
    emit_const(",");
    if (code->chain.ptr) {
        emit_newline();
        codegen_emit_code(code->chain);
    }
    return true;
}

bool else_code_emit_code(codegen instance){
    else_code *code = (else_code*)instance.ptr;
    emit_const(" else ");
    if (code->block.type == sem_rule_cond){
        codegen_emit_code(code->block);
    } else {
        emit_const("{");
        increase_indent(true);
            codegen_emit_code(code->block);
        decrease_indent(true);
        emit_const("}");
    }
    return true;
}

bool switch_code_emit_code(codegen instance){
    switch_code *code = (switch_code*)instance.ptr;
    emit_const("switch (");
    codegen_emit_code(code->condition);
    emit_const("){");
    increase_indent(true);
        codegen_emit_code(code->cases);
    decrease_indent(true);
    emit_const("}");
    return true;
}

bool case_code_emit_code(codegen instance){
    case_code *code = (case_code*)instance.ptr;
    
    emit_const("case ");
    codegen_emit_code(code->match);
    emit_const(": {");
    codegen_emit_code(code->body);
    emit_const("} break;");
    if (code->chain.ptr){
        emit_newline();
        codegen_emit_code(code->chain);
    }
    return true;
}

bool prop_init_code_emit_code(codegen instance){
    prop_init_code *code = instance.ptr;
    emit(".%v = ",code->name);
    codegen_emit_code(code->expression);
    emit_const(",");
    if (!code->chain.ptr) return true;
    emit_newline();
    codegen_emit_code(code->chain);
    return true;
}

bool struct_init_code_emit_code(codegen instance){
    struct_init_code *code = instance.ptr;
    emit("(%v){",code->name);
    if (code->content.ptr){
        increase_indent(true);
            codegen_emit_code(code->content);
        decrease_indent(true);
    }
    emit_const("}");
    return true;
}

bool cast_code_emit_code(codegen instance){
    if (is_header == true) return false;
    cast_code *code = instance.ptr;
    emit("(%v%s)",code->cast,code->reference ? "*" : "");
    return true;
}

bool array_init_code_emit_code(codegen instance){
    array_init_code *code = instance.ptr;
    emit_const("{");
    increase_indent(true);
    codegen_emit_code(code->entries);
    decrease_indent(true);
    emit_const("}");
    return true;
}

bool array_entry_code_emit_code(codegen instance){
    array_entry_code *code = instance.ptr;
    bool newl = false;
    if (code->index.length){
        newl = true;
        emit("[%v] = ",code->index);
    }
    codegen_emit_code(code->exp);
    emit_const(",");
    if (code->chain.ptr){
        if (newl) emit_newline();
        codegen_emit_code(code->chain);
    }
    return true;
}

bool rule_sequence_code_emit_code(codegen instance){
    return false;
}

bool rule_entry_code_emit_code(codegen instance){
    return false;
}

bool s_exp_code_emit_code(codegen instance){
    return false;
}

bool atom_code_emit_code(codegen instance, codegen this){
    return false;
}

#endif