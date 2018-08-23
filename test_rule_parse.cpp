

#include <string>
#include "DepRuleNode.h"
#include <iostream>

using std::string;

// int bison_parse_dep_rule(const char* buf);
extern DepRuleNodeSet* dpr_root;
typedef struct yy_buffer_state *YY_BUFFER_STATE;
YY_BUFFER_STATE yy_scan_string(const char *yy_str);
void yy_delete_buffer(YY_BUFFER_STATE b);
extern "C" int yyparse();
extern int yydebug;

void test_rule_parse()
{
	string test_rule_str;
	test_rule_str.append("RULE: \n");
	test_rule_str.append("50:BogusCat:FakeKit:\n");
	test_rule_str.append("+ parentClass == PUNC\n");
	test_rule_str.append("% Prop: \"Rule Prop testing\"\n");
	test_rule_str.append("{Tree:*}[{...\"777\"...}]{M}\n");
	const char* buf_ptr = test_rule_str.c_str();
	
	yydebug = 0;
	YY_BUFFER_STATE ybuf = yy_scan_string(buf_ptr);
	yyparse();
	yy_delete_buffer(ybuf);

	DepRuleNodeList* node = dpr_root->vec;
	if (node->size() > 0){
		std::cout << (*node)[0]->to_detail_str() << std::endl;
	}
	
}