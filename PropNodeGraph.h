#ifndef PROPNODEGRAPH_H
#define PROPNODEGRAPH_H

#include <unordered_set>
#include "PropNode.h"
#include "PropEdge.h"
#include "DepRuleEngine.h"
#include "PNPropArg.h"

typedef std::vector<const PropNode*> CPPropNodeVec_t;
typedef std::unordered_set<PropNode> PropNodeSet_t;
typedef std::unordered_set<PropEdge> PropEdgeSet_t;
typedef std::unordered_map<const PropEdge*, PropEdgeInfo> PropEdgeInfoMap_t;
typedef std::unordered_set<MatchedDepRule> MRuleSet;
typedef std::vector<const MatchedDepRule*> CPMRuleVec;
typedef MatchedDepRule MRule;

enum class PortType {
	TopSbjZA,  //Zero Anaphora        
	LowSbjZA,
	TopObjZA,
	LowObjZA,
	TopSbj, // Top-Level Subject
	LowSbj, // Low-Level Subject
	TopObj, // Top-Level Object
	LowObj, // Low-Level Object        
	Generic
};

struct PortInfo {	
	int mrule_start; int mrule_end;
	int head_start;  int head_end;	
	bool	 is_top;
	bool     is_pronoun;
	bool	 has_det;
	int		 head_shift;
	PortType type;
};

inline PortInfo init_PortInfo() {
	PortInfo pinfo;
	pinfo.mrule_start = -1; pinfo.mrule_end = -1;
	pinfo.head_start = -1;  pinfo.head_end = -1;
	pinfo.is_top = false;   pinfo.is_pronoun = false;
	pinfo.has_det = false;  pinfo.head_shift = INT_MIN;
	pinfo.type = PortType::Generic;
	return pinfo;
}

using SegTuple = std::tuple<int, int>;
using ClassRuleMap = std::unordered_map<SegTuple, vector<string>>;

namespace std {
	template <>
	struct hash<SegTuple> {
		std::size_t operator()(const SegTuple& t) const {
			int t0, t1;
			std::tie(t0, t1) = t;
			return std::hash<int>()(t0) >> 1 ^ std::hash<int>()(t1);
		}
	};
}

class PropNodeGraph
{	
private:

	using PortMap = std::unordered_map<SegTuple, PortInfo>;
	// PropNodeVec_t root_nodes;
	// PropNodeVec_t flat_nodes;		
	// PropNodeVec_t collapsed_nodes;
	PropNodeSet_t prop_node_set;
	PropEdgeSet_t prop_edge_set;
	PropEdgeSet_t term_edge_set;
	PropEdgeInfoMap_t edge_info_map;
	MRuleSet				mrule_set;
	CPMRuleVec				seq_mrule;
	vector<DepRuleEngine*>  rule_engine_vec;
	vector<DepRuleNode*>    term_aux_repo;
	PortMap					port_map;
	ClassRuleMap			rclass_map;

	const MatchedDepRule* copy_mrule(const MatchedDepRule* mrule);
	const PropNode*		  import_nodes(const MatchedDepRule* mrule);
	const CPPropNodeVec_t filter_prop_node(std::function<bool(const PropNode&)> predicate) const;	

	int			  current_base;
	unsigned int  mrule_id_counter;

public:	
	PropNodeGraph() : current_base(0), mrule_id_counter(0){};
	~PropNodeGraph();	
	int			import_mrule(const MatchedDepRule*);
	int		    import_pnprops(const vector<PNProp>& prop_list, DepRuleEngine* engine);
	MRuleSet&	matched_rules() { return mrule_set; }
	PortMap&    ports() { return port_map; }
	ClassRuleMap&    get_class_rule_map() { return rclass_map; }
	const MRuleSet&  cmatched_rules() const { return mrule_set; }

	const PropNode* add_node(const PropNode& pn);
	const PropNode* add_node(const MatchedDepRule* mrule, const PropNode* parent, bool head_flag);
	const PropNode* find_node(int start, int end);
	const PropNode* find_node(const MatchedDepRule*);
	
	void		    push_rule_engine(DepRuleEngine* ptr) { rule_engine_vec.push_back(ptr); }
	const PropEdgeInfo& get_edge_info(const PropEdge* pe) const { return edge_info_map.at(pe); }

	void		    collapse_identical_terminal();
	void                    build_terminal_dependencies();
	void					build_ports();
	const CPPropNodeVec_t	term_nodes() const;	
	const CPPropNodeVec_t   children_nodes(const PropNode* parent) const;
	const CPPropNodeVec_t	unique_terms() const;
	const PropNodeSet_t&	all_nodes() const;
	const CPMRuleVec&		sequence_mrules() const { return seq_mrule; }
	const PropEdgeSet_t&	edges() const { return prop_edge_set; }
	vector<DepRuleNode*>&   get_term_aux_repo() { return term_aux_repo; }
	const vector<PropNode>         get_terminals(int start, int end);
	string       				   get_terminal_text(int start, int end);
	const vector<const PortInfo*>  port_vector();
	
	
	
	

};

#endif //PROPNODEGRAPH_H