#include "ast.h"
#include "syscalls/syscalls.h"
#include "rules.h"
#include "common.h"

void push_node(chunk_array_t *stack, ast_node node){
    chunk_array_push(stack, &node);
}

bool pop_stack(stack_navigator *sn, ast_node* node){
    if (sn->cursor >= sn->size){
        return false;
    }
    *node = *(ast_node*)chunk_array_get(sn->stack, sn->cursor++);
    return true;
}

tern switch_rule(stack_navigator *sn, int *current_rule, int *curr_option){
    ast_node node;
    if (!pop_stack(sn, &node)) return -1;
    if (node.t.kind){
        print("Found token %s instead of rule %v",token_name(node.t.kind),token_to_slice(node.t));
        return false;
    }
    *current_rule = node.rule;
    *curr_option = node.option;
    return true;
}

chunk_array_t* init_ast(){
    return chunk_array_create(sizeof(ast_node), 1024);
}

size_t tree_count(chunk_array_t *stack){
    return chunk_array_count(stack);
}

void tree_reset(chunk_array_t *tree, size_t to){
    if (tree->chunk_capacity <= to){
        if (tree->next){
            tree_reset(tree->next,to-tree->chunk_capacity);
        }
    } else {
        tree->count = to;
        if (tree->next)
            tree_reset(tree->next,0);
    }
}

void print_stack(chunk_array_t *stack){
    size_t count = chunk_array_count(stack);
    for (uint32_t i = 0; i < count; i++){
        ast_node *node = chunk_array_get(stack, i);
        if (!node->t.kind)
            print("%c[%i,%i] %s -> (%s)",*rule_names[node->rule],node->option,node->sequence,indent_by(node->depth),sem_rule_strings[node->sem_value]);
        else
            print("%c[%i,%i] %s %v (%s)",*rule_names[node->rule],node->option,node->sequence,indent_by(node->depth),token_to_slice(node->t), sem_rule_strings[node->sem_value]);
    }
}