#include "DocParser.h"
#include "DepData.h"
#include "DepRuleNode.h"
#include "DepRuleEngine.h"
#include "PropnodeGraph.h"
#include "ChartParser.h"
#include "PNGraphBuilder.h"

void DocParser::parse_file(const string& seg_path, PropNodeGraph& pn_graph)
{
	DepData dep;
	vector<vector<DepInfo>> dep_data;

	dep.load_pos_text_file(seg_path, dep_data);
	parse_data(dep_data, pn_graph);
}

void DocParser::parse_text(const string& text, PropNodeGraph& pn_graph)
{
	DepData dep;
	vector<vector<DepInfo>> dep_data;

	dep.load_pos_text_string(text, dep_data);
	if (dep_data.size() > 0) {
		parse_seq(dep_data[0], pn_graph);
	}
}

void DocParser::build_graph(PropNodeGraph& pn_graph)
{
	DepRuleEngine* dep_rule_eng = new DepRuleEngine();
	for (const auto& lex_cat : lex_marker_strvec) {
		dep_rule_eng->add_lexical_marker_category(lex_cat);
	}

	for (const string& fpath : rule_path_vec) {
		dep_rule_eng->import_dep_rules(fpath);
	}
	dep_rule_eng->post_process_rules();

	PNGraphBuilder gbuilder;
	gbuilder.set_engine(dep_rule_eng);
	gbuilder.set_graph(&pn_graph);
	gbuilder.build_edges();
}


void DocParser::parse_seq(vector<DepInfo>& dep_seq, PropNodeGraph& pn_graph)
{
	DepRuleEngine* dep_rule_eng = new DepRuleEngine();
	for (const auto& lex_cat : lex_marker_strvec) {
		dep_rule_eng->add_lexical_marker_category(lex_cat);
	}

	for (const string& fpath : rule_path_vec) {
		dep_rule_eng->import_dep_rules(fpath);
	}
	dep_rule_eng->post_process_rules();


	//! rule engine keeps all the rule node used in parsing
	//! it cannot be deleted before pn_graph, 
	//! therefore take the dep_rule_eng with pn_graph, 
	//! dep_rule_eng will be deleted along with pn_graph
	pn_graph.push_rule_engine(dep_rule_eng);

	ChartParser chart_parser;
	chart_parser.set_engine(dep_rule_eng);
	chart_parser.set_iter_max(parse_iter_max);

	int n_loop = chart_parser.parse(dep_seq);
	const MatchedDepRule* full_edge = chart_parser.get_full_edge();

	dprintf("\n= Sequence ");
	printf("\n= Sequence ");
	if (!full_edge) {
		dprintf(" failed");
		printf(" failed");
	} else {
		dprintf(" successfully parsed in %d loops", n_loop);
		printf(" successfully parsed in %d loops", n_loop);
		pn_graph.import_mrule(full_edge);
		chart_parser.move_aux_rule(pn_graph.get_term_aux_repo());
		pn_graph.build_terminal_dependencies();
		pn_graph.build_ports();
	}
	dprintf("= \n\n");
	printf("= \n\n");

	return;
}

void DocParser::parse_data(vector<vector<DepInfo>>& dep_data, PropNodeGraph& pn_graph)
{
	DepRuleEngine* dep_rule_eng = new DepRuleEngine();
	for (const auto& lex_cat : lex_marker_strvec) {
		dep_rule_eng->add_lexical_marker_category(lex_cat);
	}

	for (const string& fpath : rule_path_vec) {
		dep_rule_eng->import_dep_rules(fpath);
	}
	dep_rule_eng->post_process_rules();	
	

	//! rule engine keeps all the rule node used in parsing
	//! it cannot be deleted before pn_graph, 
	//! therefore take the dep_rule_eng with pn_graph, 
	//! dep_rule_eng will be deleted along with pn_graph
	pn_graph.push_rule_engine(dep_rule_eng);

	ChartParser chart_parser;
	chart_parser.set_iter_max(parse_iter_max);
	chart_parser.set_engine(dep_rule_eng);

	for (int i = 0; i < dep_data.size(); ++i) {
		int n_loop = chart_parser.parse(dep_data.at(i));
		const MatchedDepRule* full_edge = chart_parser.get_full_edge();

		dprintf("\n= Sequence %d ", i);
		printf("\n= Sequence %d ", i);
		if (!full_edge) {
			dprintf(" failed");
			printf(" failed");
		} else {
			dprintf(" successfully parsed in %d loops", n_loop);
			printf(" successfully parsed in %d loops", n_loop);
			pn_graph.import_mrule(full_edge);
			chart_parser.move_aux_rule(pn_graph.get_term_aux_repo());
		}		
		dprintf("= \n\n");
		printf("= \n\n");

	}

	PNGraphBuilder gbuilder;	
	gbuilder.set_engine(dep_rule_eng);
	gbuilder.set_graph(&pn_graph);
	gbuilder.build_edges();	

	return;
}

void DocParser::import_rule_file(const string& rule_path)
{
	rule_path_vec.push_back(rule_path);
}

void DocParser::add_lexical_marker_target(const string& s)
{
	lex_marker_strvec.push_back(s);
}