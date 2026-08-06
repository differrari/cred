#ifdef RULETRANSFORM

#include "ir/general.h"
#include "ir/arch_transformer.h"
#include "ir/manual_gen.h"
#include "syscalls/syscalls.h"

codegen additional_rules = {};
codegen literals = {};

codegen perform_transformations(codegen root){
    root = codegen_transform(root);
    rule_entry_code *code = root.ptr;
    while (code->chain.ptr) code = code->chain.ptr;
    if (additional_rules.ptr) code->chain = additional_rules;
    while (code->chain.ptr) code = code->chain.ptr;
    code->chain = literals;
    return root;
}

bool is_token(string_slice slice){
    for (int i = 0; i < slice.length; i++)
        if (slice.data[i] <= 'A' || slice.data[i] >= 'Z') return false;
    return true;
}

bool is_string_lit_alphanum(string_slice slice){
    for (int i = 1; i < slice.length - 1; i++)
        if (!is_alnum(slice.data[i])) return false;
    return true;
}

codegen blk_code_transform(codegen instance){
    blk_code *code = instance.ptr;
    TRANSFORM(stat);
    TRANSFORM(chain);
    return instance;
}

codegen get_inserted_rule(codegen list, string_slice name){
    codegen rule = list;
    do {
        rule_entry_code *code = rule.ptr;
        if (!code) return (codegen){};
        if (slices_equal(code->name, name, true)){
            return rule;
        }
        rule = code->chain;
    } while(rule.ptr);
    return (codegen){};
}

codegen get_inserted_option(codegen options, string_slice name){
    codegen rule = options;
    do {
        blk_code *code = rule.ptr;
        if (!code) return (codegen){};
        if (code->stat.ptr){
            rule_sequence_code *seq = code->stat.ptr;
            if (slices_equal(seq->name, name, true)){
                return rule;
            }
        }
        rule = code->chain;
    } while(rule.ptr);
    return (codegen){};
}

codegen insert_new_rule(codegen list, string_slice name, string_slice tag, bool declaration, string_slice opt_value){
    codegen rule = get_inserted_rule(list, name);
    codegen seq = make_rule_sequence(opt_value, (string_slice){}, (string_slice){}, (string_slice){}, false, (codegen){});
    if (!rule.ptr) list = make_rule(name, tag, declaration, wrap_in_block(seq,(codegen){},false), list);
    else {
        rule_entry_code *code = rule.ptr;
        codegen list = get_inserted_option(code->list, opt_value);
        if (!list.ptr) code->list = wrap_in_block(seq, code->list, true);
    }
    return list;
}

codegen rule_sequence_code_transform(codegen instance){
    rule_sequence_code *code = instance.ptr;
    if (code->tag.length && *code->tag.data != '"' && is_token(code->name)){
        additional_rules = insert_new_rule(additional_rules, code->tag, (string_slice){}, false, code->name);
        code->name = code->tag;
    } else if (code->tag.length && *code->tag.data == '"' && is_string_lit_alphanum(code->tag))
        literals = insert_new_rule(literals, slice_from_literal("literals"), (string_slice){}, false, code->tag);
    else if (code->name.length && *code->name.data == '"' && is_string_lit_alphanum(code->name))
        literals = insert_new_rule(literals, slice_from_literal("literals"), (string_slice){}, false, code->name);
    TRANSFORM(chain);
    return instance;
}

codegen rule_entry_code_transform(codegen instance){
    rule_entry_code *code = instance.ptr;
    TRANSFORM(list);
    TRANSFORM(chain);
    return instance;
}

codegen s_exp_code_transform(codegen instance){
    return instance;
}

codegen atom_code_transform(codegen instance){
    return instance;
}


#endif