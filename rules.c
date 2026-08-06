#include "rules.h"
typedef enum { rule_chain,rule_exp,rule_sexp,rule_args,rule_atom, num_grammar_rules } grammar_rules;

grammar_rule language_rules[num_grammar_rules] = {
	[rule_chain] = {{
		{{ 
			RULE(exp), RULE(chain), 
		 },2},
		{{ 
			RULE(exp), 
		 },1},
	},2, 0},
	[rule_exp] = {{
		{{ 
			RULE(sexp), 
		 },1},
		{{ 
			RULE(atom), 
		 },1},
	},2, 0},
	[rule_sexp] = {{
		{{ 
			TOKEN(LPAREN), RULE(args), TOKEN(RPAREN), 
		 },3},
		{{ 
			TOKEN(LPAREN), TOKEN(RPAREN), 
		 },2},
	},2, sem_rule_sexp, sem_action_none},
	[rule_args] = {{
		{{ 
			RULE(exp), RULE(args), 
		 },2},
		{{ 
			RULE(exp), 
		 },1},
	},2, 0},
	[rule_atom] = {{
		{{ 
			SYMCHECK(IDENTIFIER,val), 
		 },1},
		{{ 
			SYMCHECK(STRING,val), 
		 },1},
		{{ 
			SYMCHECK(NUMBER,val), 
		 },1},
	},3, sem_rule_atom, sem_action_none},
};

char* rule_names[num_grammar_rules] = {
		[rule_chain] = "chain",
		[rule_exp] = "exp",
		[rule_sexp] = "sexp",
		[rule_args] = "args",
		[rule_atom] = "atom",
	
};