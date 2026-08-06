#ifdef RULECODEGEN

#include "ir/general.h"
#include "codegen/codeformat.h"

void generate_code(const char *name, codegen cg){
    emit_const("import hardcoded_rules from './hardcoded.js';");
    emit_newlines(2);
    emit_const("export default grammar({");
    increase_indent(true);
    emit_const("name: \"cred\",");
    emit_newline();
    emit_const("extras: ($) => [\n\t\t// NEWLINE,\n\t\t/\\s/,\n\t\t$.comment,\n\t],");
    emit_const("rules: {");
    increase_indent(true);
    codegen_emit_code(cg);
    decrease_indent(false);
    emit_const("...hardcoded_rules");
    emit_newline();
    emit_const("}");
    decrease_indent(true);
    emit_const("});");
    output_code(name,"js");
}


bool dec_code_emit_code(codegen instance){
    return false;
}

bool ass_code_emit_code(codegen instance){
    return false;
}

bool call_code_emit_code(codegen instance){
    return false;
}

bool cond_code_emit_code(codegen instance){
    return false;
}

bool jmp_code_emit_code(codegen instance){
    return false;
}

bool label_code_emit_code(codegen instance){
    return false;
}

bool exp_code_emit_code(codegen instance){
    return false;
}

bool arg_code_emit_code(codegen instance){
    return false;
}

bool param_code_emit_code(codegen instance){
    return false;
}

bool func_code_emit_code(codegen instance){
    return false;
}

bool for_code_emit_code(codegen instance){
    return false;
}

bool while_code_emit_code(codegen instance){
    return false;
}

bool dowhile_code_emit_code(codegen instance){
    return false;
}

bool var_code_emit_code(codegen instance){
    return false;
}

bool inc_code_emit_code(codegen instance){
    return false;
}

bool struct_code_emit_code(codegen instance){
    return false;
}

bool ret_code_emit_code(codegen instance){
    return false;
}

bool def_code_emit_code(codegen instance){
    return false;
}

bool int_code_emit_code(codegen instance){
    return false;
}

bool enum_code_emit_code(codegen instance){
    return false;
}

bool enum_case_code_emit_code(codegen instance){
    return false;
}

bool else_code_emit_code(codegen instance){
    return false;
}

bool switch_code_emit_code(codegen instance){
    return false;
}

bool case_code_emit_code(codegen instance){
    return false;
}

bool prop_init_code_emit_code(codegen instance){
    return false;
}

bool struct_init_code_emit_code(codegen instance){
    return false;
}

bool cast_code_emit_code(codegen instance){
    return false;
}

bool array_init_code_emit_code(codegen instance){
    return false;
}

bool array_entry_code_emit_code(codegen instance){
    return false;
}

bool blk_code_emit_code(codegen instance){
    blk_code *code = instance.ptr;
    emit_const("seq(");
    codegen_emit_code(code->stat);
    emit_const(")");
    if (code->chain.ptr){
        emit_const(",");
        codegen_emit_code(code->chain);
    }
    return true;
}

bool rule_sequence_code_emit_code(codegen instance){
    rule_sequence_code *code = instance.ptr;
    if (code->optional){
        emit_const("optional(");
    }
    if (code->value.length){
        emit_slice(code->value);
    } else {
        if (code->name.length && *code->name.data != '"') emit_const("$.");
        emit_slice(code->name);
    }
    if (code->optional){
        emit_const(")");
    }
    if (code->chain.ptr){
        emit_const(", ");  
        codegen_emit_code(code->chain);
    }
    return false;
}

int pref = 100;

bool rule_entry_code_emit_code(codegen instance){
    rule_entry_code *code = instance.ptr;
    emit_slice(code->name);
    emit_const(": $ => ");
    emit("prec(%i,",pref--);
    emit_const("choice(");
    codegen_emit_code(code->list);
    emit_const(")),");
    emit_newline();
    codegen_emit_code(code->chain);
    return false;
}

bool s_exp_code_emit_code(codegen instance, codegen this){
    return false;
}

bool atom_code_emit_code(codegen instance, codegen this){
    return false;
}

#endif