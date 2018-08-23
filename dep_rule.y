%{
#include <cstdio>
#include <iostream>
#include <string>
#include "DepRuleNode.h"
#include "ParseException.h"
#include <exception>
#include <vector>
#define YYDEBUG 1
using namespace std;

// stuff from flex that bison needs to know about:
extern "C" int yylex();
extern "C" int yyparse();
extern "C" void scan_string(char*);
extern int yylineno;
extern FILE *yyin;
extern string g_rule_file_name;
extern void yyrestart(FILE*);
int yylex_destroy (void );
DepRuleNodeSet* dpr_root;
 
void yyerror(const char *s);
struct RuleInfoType {
	int priority;
	string rule_class;
	string rule_label;
	string comments;
};
struct yyNodeConstraintType{	
	string rule_class;
	string rule_label;
	string not_rule_class;
	string not_rule_label;
};
struct RuleAux {
	RuleConstraintList* cons_list;
	vector<RuleProperty*>*   prop_list;
	RuleAux(): cons_list(NULL), prop_list(NULL) {}	
	~RuleAux() {
		if(cons_list){
			for(auto p: *cons_list) delete p;
		}

		if(prop_list){
			for(auto p: *prop_list) delete p;
		}
	}
};
void free_tree_pos(void* ptr){
	if(strlen((const char*)ptr) > 0) free(ptr);	
}
%}

%code requires {
#include "DepRuleNode.h"
struct RuleInfoType;
struct yyNodeConstraintType;
struct RuleAux;
}

// Bison fundamentally works by asking flex to get the next token, which it
// returns as an object of type "yystype".  But tokens could be of any
// arbitrary data type!  So we deal with that in Bison by defining a C union
// holding each of the types of tokens that Flex could return, and have Bison
// use that union instead of "int" for the definition of "yystype":
%union {
	int		ival;	
	char*	sval;	
	DepRuleNode*		node_ptr;
	DepRuleNodeList*	node_list;
	DepRuleNodeSet*		node_set;
	RuleInfoType*		rule_info;
	RuleConstraint*		rule_const_t;
	RuleConstraintList*	rc_list_t;
	RuleProperty*		   rule_prop_t;
	vector<RuleProperty*>* rp_list_t;
	yyNodeConstraintType*  node_const_t;
	RuleAux*               rule_aux_t;
}

// define the constant-string tokens:
%token END
%token COMMENT_HEAD
%token GROUP_START GROUP_END
%token LEFT_AB RIGHT_AB LEFT_SB RIGHT_SB
%token LEFT_P RIGHT_P
%token COLON ASTERISK VBAR HASH DOT AT FOLLOW
%token PLUS MINUS EQUAL TILDE PERCT DOLLAR CARET
%token TREE ROOT LEN SEQ TERM PTREE
%token EQUAL_OP NOT_EQUAL_OP LIKE
%token LEFT_ARROW RIGHT_ARROW UD_LINK SIBLINGS

// define the "terminal symbol" token types I'm going to use (in CAPS
// by convention), and associate each with a field of the union:
%token <ival> INT
%token <sval> STRING
%token <sval> QSTRING
%token <sval> COMMENTSTR
%type <node_ptr> rule
%type <node_list> rule_list
%type <node_set> rule_set
%type <node_ptr> node
%type <node_ptr> head_node
%type <node_list> node_list
%type <node_list> node_set
%type <node_ptr> node_content
%type <rule_info> rule_info rule_head
%type <ival> ref  valence rel_op node_conn_op tree_var
%type <sval> tree_pos comments
%type <rule_const_t> rule_constraint;
%type <rule_const_t> constraint_expr
%type <node_const_t> node_constraint;
%type <rc_list_t>    rule_cons_list;
%type <rule_prop_t>  prop_expr;
%type <rp_list_t>      prop_list;
%type <rule_aux_t>   rule_aux;

// there are 9 predictable shift/reduces in this grammar. 
// Among the incidences, 8 of them occurs when appending elements to list, 
// 1 of them is appending rel_op to constraint_expr
%expect 9

%%
// the first rule defined is the highest-level rule
rule_set: rule_list {$$ = new DepRuleNodeSet($1); dpr_root = $$; }		
		;

rule_list: rule_list rule {$$ = $1; $1->push_back($2);}		   
		   | rule { $$ = new DepRuleNodeList(); $$->push_back($1); }
		   ;

rule: rule_head node_list {
		$$ = new DepRuleNode(NodeType::NdRule);
		$$->set_children($2);
		$$->set_info($1->priority, $1->rule_class, $1->rule_label);
		$$->set_src_line(yylineno);
		$$->set_src_file(g_rule_file_name);
		$$->set_comment($1->comments);
		delete $1;
		delete $2;
		}
	| rule_head rule_aux node_list {
		$$ = new DepRuleNode(NodeType::NdRule);
		$$->set_children($3);
		$$->set_info($1->priority, $1->rule_class, $1->rule_label);		
		$$->set_rule_constraints($2->cons_list);
		$$->set_rule_properties($2->prop_list);
		$$->set_src_line(yylineno);
		$$->set_comment($1->comments);
		delete $1;		
		delete $2;
		delete $3;
		}
	| comments node_list {
		$$ = new DepRuleNode(NodeType::NdRule);
		$$->set_children($2);
		$$->set_comment($1);		
		$$->set_src_line(yylineno);
		delete $1;		
		delete $2;
		}	
	;
	
rule_aux: prop_list { $$ = new RuleAux(); $$->prop_list = $1; } 		
		| rule_cons_list { $$ = new RuleAux(); $$->cons_list = $1; }
		| rule_aux prop_list { $$ = $1; $$->prop_list = $2; }
		| rule_aux rule_cons_list { $$ = $1; $$->cons_list = $2; }
		;

node_list: node_list node {$$ = $1; $1->push_back($2); }
		 | node { $$ = new DepRuleNodeList(); $$->push_back($1); }
		 | node_list node_conn_op node { 
				$$ = $1; 
				$3->set_node_connection($2); 
				$1->push_back($3); 
			}
         ;

node: 
	  GROUP_START node_content GROUP_END  { $$ = $2; }	
    | LEFT_P node_list RIGHT_P {
			$$ = new DepRuleNode();			
			$$->set_children($2);
		}
	| LEFT_P node_set RIGHT_P {
			$$ = new DepRuleNode(NodeType::NdOrNode);			
			$$->set_children($2);
		}
	| LEFT_SB node RIGHT_SB	{
			$$ = $2;
			$2->set_optional();
		}
	| head_node { $$ = $1; }	
	
    ;

node_set: node_set VBAR node { $$ = $1; $1->push_back($3); }
		| node VBAR node { $$ = new DepRuleNodeList(); $$->push_back($1); $$->push_back($3); }
	
head_node: 
		 LEFT_AB node RIGHT_AB {
				$2->set_head_flag();
				$$ = $2;
			}
		 | LEFT_AB ROOT RIGHT_AB {
				// printf("root head\n");
				DepRuleNode* nptr = new DepRuleNode(NodeType::NdRoot, "", "", -1);
				nptr->set_head_flag();
				$$ = nptr;
			}
         ;

node_content: 
		  CARET  { $$ = new DepRuleNode(NodeType::NdSeqStart, "", "^", -1); }
		| DOLLAR { $$ = new DepRuleNode(NodeType::NdSeqEnd, "", "$", -1); }
		| AT INT {
				$$ = new DepRuleNode(NodeType::NdSeqMiddle, "", "@", -1);
				$$->set_start_pos_hint($2);
			}
		| TERM AT INT { 
			$$ = new DepRuleNode(NodeType::NdTerminal, "", "", -1); 
			$$->set_start_pos_hint($3);
			}
		| tree_pos  {
				// printf("node_content: {%s}\n", $1);					
				$$ = new DepRuleNode(NodeType::NdTerminal, "", $1, -1);
				free_tree_pos($1);
			}		
        | QSTRING {
				// printf("node_content: {QSTRING[%d]}\n", strlen($1));
				$$ = new DepRuleNode(NodeType::NdTerminal, $1, "", -1);
				delete $1;
			}	
		| DOT DOT DOT QSTRING { 
				DepRuleNodeList* nlist = new DepRuleNodeList();
				DepRuleNode* ntree = new DepRuleNode(NodeType::NdTree, "", "", -1);
				DepRuleNode* treex = new DepRuleNode(NodeType::NdTerminal, $4, "", -1);
				nlist->push_back(ntree);
				nlist->push_back(treex);
				$$ = new DepRuleNode(); 
				$$->set_children(nlist);
				delete $4;
			}
		| DOT DOT DOT QSTRING DOT DOT DOT  { 
				DepRuleNodeList* nlist = new DepRuleNodeList();
				DepRuleNode* ntree_1 = new DepRuleNode(NodeType::NdTree, "", "", -1);
				DepRuleNode* ntree_2 = new DepRuleNode(NodeType::NdTree, "", "", -1);
				DepRuleNode* treex = new DepRuleNode(NodeType::NdTerminal, $4, "", -1);
				nlist->push_back(ntree_1);
				nlist->push_back(treex);
				nlist->push_back(ntree_2);
				$$ = new DepRuleNode(); 
				$$->set_children(nlist);
				delete $4;
			}
		| QSTRING DOT DOT DOT { 
				DepRuleNodeList* nlist = new DepRuleNodeList();			
				DepRuleNode* ntree_2 = new DepRuleNode(NodeType::NdTree, "", "", -1);			
				DepRuleNode* treex = new DepRuleNode(NodeType::NdTerminal, $1, "", -1);
				nlist->push_back(treex);
				nlist->push_back(ntree_2);
				$$ = new DepRuleNode(); 
				$$->set_children(nlist);
				delete $1;
			}	
		| SEQ  COLON tree_pos {
				$$ = new DepRuleNode(NodeType::NdSeqRule, "", $3, -1);				
				free_tree_pos($3);		
			}
        | tree_var COLON tree_pos {
				$$ = new DepRuleNode(NodeType::NdTree, "", $3, -1);
				if($1 == 1) $$->set_passive(true);
				free_tree_pos($3);	
			}
		| tree_var COLON tree_pos COLON QSTRING {
				$$ = new DepRuleNode(NodeType::NdTree, $5, $3, -1);
				if($1 == 1) $$->set_passive(true);
				delete $5;
				free_tree_pos($3);	
			}
		| tree_var COLON QSTRING {
				$$ = new DepRuleNode(NodeType::NdTree, $3, "", -1);					
				if($1 == 1) $$->set_passive(true);
				delete $3;
			}
		| tree_var node_constraint {
				$$ = new DepRuleNode(NodeType::NdTree, "", "", -1);
				if($1 == 1) $$->set_passive(true);
				$$->set_node_constraint(NodeConstraint::RuleClass, $2->rule_class);
				$$->set_node_constraint(NodeConstraint::RuleLabel, $2->rule_label);
				$$->set_node_constraint(NodeConstraint::NotRuleClass, $2->not_rule_class);
				$$->set_node_constraint(NodeConstraint::NotRuleLabel, $2->not_rule_label);
				delete $2;
			}
		| tree_var node_constraint COLON tree_pos {
				$$ = new DepRuleNode(NodeType::NdTree, "", $4, -1);
				if($1 == 1) $$->set_passive(true);				
				$$->set_node_constraint(NodeConstraint::RuleClass, $2->rule_class);
				$$->set_node_constraint(NodeConstraint::RuleLabel, $2->rule_label);
				$$->set_node_constraint(NodeConstraint::NotRuleClass, $2->not_rule_class);
				$$->set_node_constraint(NodeConstraint::NotRuleLabel, $2->not_rule_label);							
				delete $2;
				free_tree_pos($4);					
			}		
		| tree_var node_constraint COLON QSTRING {
				$$ = new DepRuleNode(NodeType::NdTree, $4, "", -1);
				if($1 == 1) $$->set_passive(true);
				$$->set_node_constraint(NodeConstraint::RuleClass, $2->rule_class);
				$$->set_node_constraint(NodeConstraint::RuleLabel, $2->rule_label);
				$$->set_node_constraint(NodeConstraint::NotRuleClass, $2->not_rule_class);
				$$->set_node_constraint(NodeConstraint::NotRuleLabel, $2->not_rule_label);	
				delete $2;
			}	
		| tree_pos COLON QSTRING {
				$$ = new DepRuleNode(NodeType::NdTerminal, $3, $1, -1);
				free_tree_pos($1);
				delete $3;
			}
        | tree_pos COLON ref {
			$$ = new DepRuleNode(NodeType::NdTerminal, "", $1, $3);
			free_tree_pos($1);
			// printf("node_content: {%s:%d}", $1, $3);
			}		
        ;

ref: INT
   ;

tree_var: TREE { $$ = 0; }
		| PTREE { $$ = 1; }

tree_pos: ASTERISK { $$ = ""; }
         | STRING

node_constraint: AT STRING { $$ = new yyNodeConstraintType{"", $2, "", ""}; free($2); }
			   | DOT STRING { $$ = new yyNodeConstraintType{$2, "", "", ""}; free($2); }
			   | DOT STRING DOT TILDE STRING { $$ = new yyNodeConstraintType{$2, $5, "", ""}; free($2); free($5)}
			   | AT STRING AT TILDE STRING { $$ = new yyNodeConstraintType{"", "", $2, $5};	free($2);free($5);}

rule_head: comments rule_info { $$ = $2; $$->comments = string($1); delete $1; }
		 | rule_info { $$ = $1; }

rule_info: INT COLON { $$ = new RuleInfoType{$1, "", "", ""}; }
		 | INT COLON STRING COLON { $$ = new RuleInfoType{$1, $3, "", ""}; free($3);}
		 | INT COLON STRING COLON STRING COLON { $$ = new RuleInfoType{$1, $3, $5, ""}; free($3); free($5);}
		 
rule_cons_list: rule_constraint { $$ = new RuleConstraintList(); $$->push_back($1); }			  
			  | rule_cons_list rule_constraint { $$ = $1; $$->push_back($2); }

rule_constraint: valence constraint_expr{ $$ = $2; $$->valence = $1;	}
			   | valence constraint_expr COLON INT{
					$$ = $2;
					$$->valence = $1;
					$$->weight = $4;					
					}
			   
constraint_expr: STRING LEFT_P INT RIGHT_P rel_op STRING {
					$$ = new RuleConstraint(0, $1, $3, $6, 0);
					$$->set_relational_operator($5);
					free($1); free($6);
					}
			   | STRING LEFT_P INT RIGHT_P {
					$$ = new RuleConstraint(0, $1, $3, "", 0);		
					free($1);			
					}
				| STRING LEFT_P RIGHT_P {
					$$ = new RuleConstraint(0, $1, -1, "", 0);					
					free($1);
					}
			   | STRING rel_op STRING {
					$$ = new RuleConstraint(0, $1, -1, $3, 0);
					$$->set_relational_operator($2);
					free($1); free($3);
					}

prop_list: prop_expr { $$ = new vector<RuleProperty*>(); $$->push_back($1); }
		 | prop_list prop_expr { $$ = $1; $$->push_back($2); }

prop_expr: PERCT STRING COLON QSTRING {
				$$ = new RuleProperty($2, $4);
				free($2); free($4);
			}

comments: COMMENTSTR { $$ = $1; }
		| comments COMMENTSTR { 
			int buflen = strlen($1) + strlen($2) + 1;
			$$ = new char[buflen](); 
			strcat_s($$, buflen, $1);
			strcat_s($$, buflen, $2);
			delete $1; delete $2;
			}

valence: PLUS { $$=1; }
	   | MINUS { $$=-1; }
	   | EQUAL { $$=0; }

rel_op: EQUAL_OP {$$ = 1;}
	  | NOT_EQUAL_OP {$$ = 2;}	  	  
	  | LIKE {$$ = 3;}	  
	  | EQUAL {$$ = 4;}

node_conn_op: LEFT_ARROW {$$ = 1;}
			| RIGHT_ARROW {$$ = 2;}
			| UD_LINK {$$ = 3; }
			| FOLLOW { $$ = 4; }
			| SIBLINGS { $$ = 5; }

%%

int bison_parse_dep_rule_file(const char* fpath) {
	// open a file handle to a particular file:	
	yydebug = 0;
	yylineno = 1;
	FILE *myfile = fopen(fpath, "r");
	// make sure it's valid:
	if (!myfile) {
		cout << "can't open " << fpath << endl;
		return -1;
	}

	// set lex to read from it instead of defaulting to STDIN:
	yyin = myfile;
	yyrestart(myfile);
	
	// parse through the input until there is no more:	
	do {		
		yyparse();
	} while (!feof(yyin));
	fclose(myfile);
	
	yylex_destroy();
	return 0;	
}


void yyerror(const char *s) {
	cout << "parse error @ Line:" << yylineno << "\n";
	cout << "Message: " << s << endl;
	// might as well halt now:
	// getchar();	
	if(yyin > 0) fclose(yyin);

	throw ParseException(yylineno);
}

