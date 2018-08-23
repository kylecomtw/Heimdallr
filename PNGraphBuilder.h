#ifndef PNGRAPHBUILDER_H
#define PNGRAPHBUILDER_H

#include "PropNodeGraph.h"
#include "DepRuleEngine.h"
#include "MatchedDepRule.h"

typedef MatchedDepRule MRule;


class PNGraphBuilder
{
private:
	struct TagResult { bool sbj_found; bool obj_found; };	
	using PortMap = std::unordered_map<SegTuple, PortInfo>;
	DepRuleEngine*		engine;
	PropNodeGraph*		G;
	vector<PropNode*>   ref_candid_vec;	
	struct TopLevelInfo { bool is_period; };
	struct SOTag { PortType sbj_type; int sbj_idx; PortType obj_type; int obj_idx; };

	void parse_graph();	
	void add_port(const MRule* mrule, const MRule* head, bool is_top, int head_shift, PortType pt);
	void add_port(const PortInfo& port);
	void remove_port(const SegTuple& seg);
	PortInfo merge_port_info(const PortInfo& oldp, const PortInfo& newp);
	void find_references();
	void find_matched_rule_class(const string& rclass, vector<const PropNode*>& out_vec);				
	void find_ports(const MRule* seq, TopLevelInfo& info);
	void find_ports_recursive_V(const MRule* mrule, TopLevelInfo& info);
	void find_ports_VHead(const MRule* mrule, TopLevelInfo& info);
	void find_ports_generic(const MRule* mrule, TopLevelInfo& info);
	void find_ports_by_terminals(const MRule* seq);
	void exclude_ports(const MRule* seq);
	void sbj_tagging_default_V(const MRule* mrule, const MRule* mrule_head, int ch_idx, bool is_top, TagResult& res);
	void sbj_tagging_rule_prop(const MRule* mrule, const MRule* mrule_head, int ch_idx, bool is_top, const SOTag& so_tag, TagResult& res);
	void index_rule_class();
	bool check_mrule_class(int istart, int iend, const string& rclass);	
	const PropNode* find_top_headed(const PropNode* mrule, int& ch_idx);
	const MRule* find_bottom_heading(const MRule* mrule);
	const MRule* find_tree_parent(const PropNode* mrule);
	const MRule* find_head(const MRule* mrule);
	const MRule* find_prev_sibling(const MRule* mrule);
	const MRule* find_next_sibling(const MRule* mrule);
	const vector<const MRule*> find_terminals(const MRule* mrule);
	int find_mrule_child_index(const MRule* parent, const MRule* child);
	PropNode* match_rule(const DepRuleNode* rule_x, int start_pos);
	PropNode* match_rules(const RuleList& rule_vec_x, int start_pos);
	MatchedDepRule match_by_ffr(const DepRuleNode* rule_x, int start_pos);
	SOTag parse_so_tag(const string* so_prop_str, bool is_top);
	PortType port_type_from_string(const string& tag, bool is_top);

public:
	void set_engine(DepRuleEngine* eng) { engine = eng; }
	void set_graph(PropNodeGraph* _g) { G = _g; }
	void build_edges();	
	void find_ports();

	
};

#endif //PNGRAPHBUILDER_H