
#include <boost/filesystem.hpp>
#include "DepRuleEngine.h"
#include "DepRuleNodeFactory.h"

void test_rule_factory()
{
	string fpath = "../etc/dep_rules_factory.txt";
	if (boost::filesystem::exists(fpath)) {
		DepRuleEngine dep_rule_eng;
		dep_rule_eng.import_dep_rules(fpath);
		dep_rule_eng.post_process_rules();
		
		DepRuleNodeFactory factory(&dep_rule_eng);
		DepRuleNode* r2 = dep_rule_eng.get_rules_vec()[0];

		DepRuleNodeUP ruptr = factory.generalize(r2);
		dprintf(((string)*ruptr).c_str());

	} else {
		printf("Cannot find %s\n", fpath.c_str());
	}

	
}
