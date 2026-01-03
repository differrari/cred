#include "syscalls/syscalls.h" 
#include "data/scanner/scanner.h"
#include "data/tokenizer/tokenizer.h"
#include "data/helpers/token_stream.h"
#include "std/string.h"
#include "std/stringview.h"
#include <stdio.h>

typedef enum { rule_statement, rule_declaration, rule_assignment, rule_value, rule_funccall, rule_argument, num_grammar_rules } grammar_rules;

typedef struct {
    bool rule;
    int value;
} grammar_elem;

typedef struct {
    grammar_elem rules[16];
    uint8_t num_elements;
} grammar_rule_opt;

typedef struct {
    grammar_rule_opt options[8];
    uint8_t num_elements;
} grammar_rule;

#define TOKEN(name) { false, TOK_##name}
#define RULE(name) { true, rule_##name }

grammar_rule language_rules[num_grammar_rules] = {
    [rule_statement] = {
        {
            {{RULE(declaration)}, 1},
            {{RULE(assignment)}, 1},
            {{RULE(funccall)}, 1},
        },
        3
    },
    [rule_declaration] = {
        {
            (grammar_rule_opt){ { TOKEN(OPERATOR), TOKEN(IDENTIFIER), TOKEN(OPERATOR), TOKEN(CONST), TOKEN(SEMICOLON) }, 5 },
            (grammar_rule_opt){ { TOKEN(IDENTIFIER), TOKEN(IDENTIFIER), TOKEN(OPERATOR), TOKEN(CONST), TOKEN(SEMICOLON) }, 5 }
        }, 
        2
    }
};

typedef struct {
    grammar_rules current_rule;
    int option;
    int sequence;
} parser_sm;

#define MAX_PARSE_DEPTH 16

parser_sm parser_stack[MAX_PARSE_DEPTH];
int parser_depth;

#define current_parser_rule(parser) language_rules[parser->current_rule].options[parser->option].rules[parser->sequence]

bool parser_advance_to_token(parser_sm *parser){
    while (current_parser_rule(parser).rule){
        print("Rule %i has subrule %i",parser->current_rule, current_parser_rule(parser).value);
        if (parser_depth == MAX_PARSE_DEPTH - 1){
            print("Maximum depth reached, too many nested statements, shame on you");
            return false;
        }
        printf("Push state");
        parser_stack[parser_depth++] = *parser;
        parser->current_rule = current_parser_rule(parser).value;
        parser->sequence = 0;
        parser->option = 0;
    }
    print("Current rule %i. Option %i. Sequence %i",parser->current_rule, parser->option, parser->sequence);
    return true;
}

bool pop_parser_stack(parser_sm *parser){
    if (parser_depth == 0){
        print("Cannot pop root state");
        return false;
    }
    printf("Pop state");
    *parser = parser_stack[parser_depth-1];
    parser_depth--;
    return true;
}

bool parser_advance_option_sm(parser_sm *parser){
    if (parser->option + 1 == language_rules[parser->current_rule].num_elements){
        printf("Ran out of options to parse rule %i. Failed",parser->current_rule);
        if (!pop_parser_stack(parser)) return false;
        return parser_advance_option_sm(parser);
    } else {
        parser->option++;
        parser->sequence = 0;
        return true;
    }
}

bool parser_advance_sequence(parser_sm *parser){
    if (parser->sequence + 1 == language_rules[parser->current_rule].options[parser->option].num_elements){
        print("Successfully parsed rule %i with option %i",parser->current_rule, parser->option);
        if (!pop_parser_stack(parser)) return true;
        return parser_advance_sequence(parser);
    }
    else parser->sequence++;
    return true;
}

bool parse_token(char *content, Token t, parser_sm *parser){
    parser_advance_to_token(parser);
    grammar_elem element = current_parser_rule(parser);
    if (t.kind == element.value){
        print("Token %v [%i] = %i", delimited_stringview(content, t.pos, t.length), t.kind, element.value);
        return parser_advance_sequence(parser);
    } else {
        print("Failed to match token %i, found %i. Skipping. There are %i options", element.value, t.kind,language_rules[parser->current_rule].num_elements);
        if (!parser_advance_option_sm(parser)) return false;
        return parse_token(content, t, parser);
    }
}

int main(int argc, char *argv[]){
    
    char *content = read_full_file("street.cred");
    
    Scanner scan = scanner_make(content,strlen(content));
    
    Tokenizer tk = tokenizer_make(&scan);
    tk.skip_type_check = true;
    TokenStream ts;
    ts_init(&ts,&tk);
    
    parser_sm parser = (parser_sm){
        .current_rule = rule_statement,
        .option = 0,
        .sequence = 0,
    };
    
    
    Token t;
    while (ts_next(&ts, &t)) {
        if (!t.kind) break;
        parse_token(content, t, &parser);
    }
    
    printf("String scanned");
    
    return 0;
}
