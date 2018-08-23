#include <stack>
#include "PropNodeGraph.h"
#include "DepData.h"
#include <unordered_map>
#include <algorithm>
#include <queue>

PropNodeGraph::~PropNodeGraph()
{
	//! MatchedDepRule are stored in mrule_set and term_set in copy_mrule
	//! they are released by hashset destructor, do not release again here.
	//! term_aux_repo stored pointers to terminal auxiliary rules when importing mrule
	//! they must be released manually
	for (auto x : term_aux_repo) delete x;
	term_aux_repo.clear();
	for (auto eng : rule_engine_vec) {
		delete eng;
	}
	rule_engine_vec.clear();
	
}

int PropNodeGraph::import_mrule(const MatchedDepRule* mrule)
{		
	// copy MatchedDepRule to store in mrule_set for later matching
	const MatchedDepRule* mrule_ptr = copy_mrule(mrule);
	import_nodes(mrule_ptr);
	seq_mrule.push_back(mrule_ptr);
	current_base += mrule->end_pos();
	return 0;
}

int PropNodeGraph::import_pnprops(const vector<PNProp>& prop_list, DepRuleEngine* engine)
{
	std::unordered_map<string, MatchedDepRule*> edge_map;	
	
	//! find out the index of sequence end
	int max_seq_index = 0;
	for (const PNProp& prop : prop_list) {
		int pend = prop.args.back().end;
		if (max_seq_index < pend) {
			max_seq_index = pend;
		}
	}
	
	//! find the top prop and create MatchedDepRule's for each propositions and terminals
	const PNProp* top_prop = nullptr;
	const int RULE_NOT_FOUND = 1001;
	for (const PNProp& prop : prop_list) {
		if (prop.args[0].start == 0 && prop.args.back().end == max_seq_index) {
			top_prop = &prop;
		}

		const string& predicate = prop.predicate;
		try {
			string rclass = predicate.substr(0, predicate.find("."));
			string rlabel = predicate.substr(predicate.find(".") + 1);
			const DepRuleNode* rnode = engine->find_rule_dynamic(rclass, rlabel, prop.args.size());
			if (rnode == nullptr) throw RULE_NOT_FOUND;
			
			MatchedDepRule* mrule = new MatchedDepRule(*rnode, prop.args[0].start, prop.args.back().end);
			string code_key = prop.code;
			if (prop.args.size() > 0) {
				code_key += "@" + std::to_string(prop.args[0].start) + "-" + std::to_string(prop.args.back().end);
			}
			edge_map[code_key] = mrule;
			
			for (int i = 0; i < prop.args.size(); ++i) {
				const PropArg& arg = prop.args[i];

				if (arg.is_reference) continue;

				//! create terminal edges for later linking
				DepRuleNode* rule_term = DepRuleNode::make_terminal_rule(arg.text, arg.pos);

				//! set proper anchor flag
				if (arg.start == 0) {
					rule_term->set_anchor(AnchorType::AnchorStart);
				} else if (arg.end == max_seq_index) {
					rule_term->set_anchor(AnchorType::AnchorEnd);
				}

				//! store terminal rule in a container, so that they can be released
				//! or transfered later
				term_aux_repo.push_back(rule_term);
				MatchedDepRule* edge_term = MatchedDepRule::make_matched_terminal(rule_term, arg.start, arg.end);				
				string arg_key = arg.text + "@" + std::to_string(arg.start) + "-" + std::to_string(arg.end);
				edge_map[arg_key] = edge_term;				
			}
		} catch (std::out_of_range& ex){
			printf("out of range when extracting label\n");
			return 0;
		} catch (int& ex) {
			printf("Cannot find valid rule\n");
			return 0;
		}
	}

	if (top_prop == nullptr) {
		printf("ERROR: import_pnprop only handles prop_list with a top-level prop");
		return 0;
	}

	//! link child mrules with parent mrule
	for (const PNProp& prop : prop_list) {
		string code_key = prop.code;
		if (prop.args.size() > 0) {
			code_key += "@" + std::to_string(prop.args[0].start) + "-" + std::to_string(prop.args.back().end);
		}
		MatchedDepRule* mrule = edge_map[code_key];
		for (int i = 0; i < prop.args.size(); ++i) {
			const PropArg& arg = prop.args[i];
			MatchedDepRule* mrule_ch = nullptr;	
			string arg_key = arg.text + "@" + std::to_string(arg.start) + "-" + std::to_string(arg.end);
			mrule_ch = edge_map[arg_key];
			mrule->advance_dot_with(mrule_ch);			
		}
	}

	for (auto item : edge_map) {
		MatchedDepRule* mrule = item.second;
		mrule->update_pos();
		mrule->update_anchor();
	}

	string top_key = top_prop->code;
	if (top_prop->args.size() > 0) {
		top_key += "@" + std::to_string(top_prop->args[0].start) + "-" + std::to_string(top_prop->args.back().end);
	}
	import_mrule(edge_map[top_key]);
	//! clear the MatchedDepRule's created in this function
	//! import_mrules will make a copy of mrules
	for (auto item : edge_map) {
		delete item.second;
	}

	return edge_map.size();
}

const MatchedDepRule* PropNodeGraph::copy_mrule(const MatchedDepRule* mrule)
{
	MatchedDepRule mrule_copy = MatchedDepRule(*mrule);
	mrule_copy.reassign_id(++mrule_id_counter);
	mrule_copy.rebase(current_base);

	if (mrule_copy.get_children_count() > 0){		
		const vector<const MatchedDepRule*>& mrule_chvec = mrule_copy.get_matched_children();
		for (int i = 0; i < mrule_chvec.size(); ++i) {			
			const MatchedDepRule* mrule_chx = mrule_chvec.at(i);
			const MatchedDepRule* copy_chx = copy_mrule(mrule_chx);
			mrule_copy.replace_matched_children(i, copy_chx);
		}
	}

	auto ret = mrule_set.insert(std::move(mrule_copy));
	const MatchedDepRule* mrule_ptr = &(*ret.first);	

	return mrule_ptr;
}

const PropNode*	PropNodeGraph::import_nodes(const MatchedDepRule* mrule)
{
	const PropNode* pn = nullptr;

	pn = add_node(mrule, nullptr, false);	
	
	return pn;
}

const PropNode* PropNodeGraph::add_node(const MatchedDepRule* mrule, const PropNode* pn_parent, bool head_flag)
{
	const PropNode* node_ptr = nullptr;
	const string& rclass = mrule->matched_rule_class();
	//! it's a mess, a PropGraph should not care node type were it really a graph
	if (rclass == "Terminal") {		
		PropNode pn = PropNode::create_instance(mrule, PropNodeType::Terminal);
		pn.parent = pn_parent;
		pn.head_flag = head_flag;
		node_ptr = add_node(pn);		
	} else if (rclass == "TermRule") {		
		PropNode pn = PropNode::create_instance(mrule, PropNodeType::TermRule);
		pn.parent = pn_parent;
		pn.head_flag = head_flag;
		node_ptr = add_node(pn);
	} else { //! parse node			
		PropNode pn = PropNode::create_instance(mrule, PropNodeType::Generic);
		pn.parent = pn_parent;
		pn.head_flag = head_flag;
		node_ptr = add_node(pn);
	}

	for (int i = 0; i < mrule->get_children_count(); ++i) {
		const MRule* mrule_ch = mrule->get_matched_children(i);
		bool is_head = false;
		try {
			is_head = mrule->get_rule_node().get_children(i)->get_head_flag();
		} catch (std::exception& ex) {
			//! pass
		}

		add_node(mrule_ch, node_ptr, is_head);
	}
		
	return node_ptr;
}

const PropNode* PropNodeGraph::add_node(const PropNode& pn)
{
	auto ret = prop_node_set.insert(pn);
	return &(*ret.first); 
}

const PropNode* PropNodeGraph::find_node(int start, int end)
{
	for (auto& pn_x : prop_node_set) {		
		if (pn_x.start == start && pn_x.end == end){
			return &pn_x;
		}
	}

	return nullptr;
}

const PropNode* PropNodeGraph::find_node(const MatchedDepRule* mptr)
{
	for (auto& pn_x : prop_node_set) {
		if (pn_x.mrule_ptr != mptr) continue;
		return &pn_x;
	}

	return nullptr;
}


void PropNodeGraph::collapse_identical_terminal()
{	
	std::unordered_map<string, const PropNode*> str_map;
	std::unordered_map<const PropNode*, const PropNode*> node_map;
	
	//! collpase identity node
	const int PN_ID_BASE = 1e4;
	for (const PropNode& pn : prop_node_set){
		if (pn.node_type != PropNodeType::Terminal) continue;
		if (pn.word.size() > 0 && str_map.count(pn.word) == 0){
			PropNode new_pn(pn);
			new_pn.node_type = PropNodeType::UniqueTerminal;
			new_pn.id = PN_ID_BASE + str_map.size();
			new_pn.start = -1; new_pn.end = -1;
			const PropNode* new_pn_ptr = add_node(new_pn);
			str_map[pn.word] = new_pn_ptr;
		}
		node_map[&pn] = str_map.at(pn.word);			
	}

	//! update entries in edges	
	for (const PropEdge& pe : prop_edge_set){
		if (node_map.count(pe.a) == 0 && node_map.count(pe.b) == 0){
			prop_edge_set.insert(pe);
		} else {			
			const PropNode* new_a = node_map.count(pe.a) > 0? node_map.at(pe.a): pe.a;
			const PropNode* new_b = node_map.count(pe.b) > 0 ? node_map.at(pe.b) : pe.b;

			//! remove edge that come from identity link
			if (new_a == new_b) continue;

			PropEdge new_pe;			
			PropEdgeInfo pe_info(edge_info_map.at(&pe));
			new_pe.a = new_a; new_pe.b = new_b;
			auto ret = prop_edge_set.insert(new_pe);
			
			edge_info_map[&(*ret.first)] = pe_info;

		}
	}

	//! remove all edges ends with node type PropNodeType::Terminal
	auto pe_iter_end = prop_edge_set.end();
	for (auto pe_iter = prop_edge_set.begin();
		pe_iter != pe_iter_end;){
		const PropEdge& pe = *pe_iter;
		if (pe.a->node_type == PropNodeType::Terminal ||
			pe.b->node_type == PropNodeType::Terminal){
			edge_info_map.erase(&(*pe_iter));
			pe_iter = prop_edge_set.erase(pe_iter);
		} else {
			++pe_iter;
		}
	}

	//! remove all nodes having type PropNodeType::Terminal
	auto pn_iter_end = prop_node_set.end();
	for (auto pn_iter = prop_node_set.begin(); 
		 pn_iter != pn_iter_end; ){
		if ((*pn_iter).node_type == PropNodeType::Terminal){			
			pn_iter = prop_node_set.erase(pn_iter);
		} else {
			++pn_iter;
		}
	}

}

void PropNodeGraph::build_terminal_dependencies()
{
	CPPropNodeVec_t terms = term_nodes();
	for (const PropNode* term_node : terms) {
		std::unordered_set<const PropNode*> visited;
		std::vector<const PropNode*> buf;
		buf.push_back(term_node);
		visited.insert(term_node);
		// printf("-- Terminal %d --\n", term_node->start);

		bool in_head_path = true;
		int n_non_head = term_node->head_flag? 0: 1;
		bool to_ascend = true;
		//! find the heading terminal of the parent of [the first non-head ancestor/ self(if-not-a-head)]
		while (buf.size() > 0) {
			const PropNode* node_x = buf.back();
			buf.pop_back();
			CPPropNodeVec_t ch_nodes = children_nodes(node_x);			

			//! find next nodes:
			//! 1. parent node
			if (to_ascend && node_x->parent != nullptr && visited.count(node_x->parent) == 0) {								
				if (n_non_head > 0) to_ascend = false;
				const PropNode* panode = node_x->parent;
				buf.push_back(node_x->parent);
				visited.insert(node_x->parent);
				// printf("Parent: %d-%d, %s\n", node_x->parent->start, node_x->parent->end, node_x->parent->rule_label.c_str());

				n_non_head += panode->head_flag ? 0 : 1;
			}

			//! 2. heading child node(s)
			if (!to_ascend) {				
				for (const PropNode* ch_node : ch_nodes) {
					if (ch_node == nullptr) continue;
					if (visited.count(ch_node) > 0) continue;
					if (!ch_node->head_flag && ch_nodes.size() > 1) continue;
					buf.push_back(ch_node);
					visited.insert(ch_node);
					// printf("Child: %d-%d, %s\n", ch_node->start, ch_node->end, ch_node->rule_label.c_str());
				}
			}

			//! another terminal found, register as a new term edge
			if (node_x->node_type == PropNodeType::Terminal && node_x != term_node) {
				PropEdge edge;
				edge.a = term_node;
				edge.b = node_x;
				term_edge_set.insert(edge);
				printf("Edge: Term %d -> Term %d\n", edge.a->start, edge.b->start);
			}
		}
	}
	
}

void PropNodeGraph::build_ports()
{
	vector<const PropNode*> top_nodes;
	for (const PropNode* term : term_nodes()) {
		bool is_top_node = true;
		int n_as_src = 0;
		int n_as_targ = 0;
		for (const PropEdge& e : term_edge_set) {
			//! check if there is any edge originates from this term
			//! if none, it is a top node
			if (e.a == term) n_as_src += 1;
			if (e.b == term) n_as_targ += 1;
		}

		if (n_as_src == 0 && n_as_targ > 0) {
			top_nodes.push_back(term);
		}
	}

	std::queue<const PropNode*> buf;
	for(auto n: top_nodes) buf.push(n);

	while(buf.size() > 0){

		const PropNode* node_x = buf.front();
		buf.pop();

		bool is_top_node = std::find(top_nodes.begin(), top_nodes.end(), node_x) != top_nodes.end();
		bool has_sbj = false;
		bool has_obj = false;
		if (node_x->pos != "N" && node_x->pos != "Vi" && node_x->pos != "Vt") continue;


		for (const PropEdge& e : term_edge_set) {			
			//! find those term edges that targeting node_x
			if (e.b != node_x) continue;
			const PropNode* src = e.a;
			if (src->pos == "CATEGORY") continue;
			if (src->pos == "ASP" || src->pos == "T" || src->pos == "POST" ||
				src->pos == "ADV") continue;

			buf.push(src);

			PortInfo pinfo = init_PortInfo();
			if (node_x->pos != "N") {				
				if (e.a->start < e.b->start) {
					//! a right arc
					if (is_top_node) pinfo.type = PortType::TopSbj;
					else pinfo.type = PortType::LowSbj;
					has_sbj = true;
				} else {
					//! a left arc				
					if (is_top_node) pinfo.type = PortType::TopObj;
					else pinfo.type = PortType::LowObj;
					has_obj = true;
				}
			} else {				
				if (e.a->start < e.b->start) {
					//! a right arc
					pinfo.type = PortType::Generic;
				}
			}

			pinfo.mrule_start = src->start;
			pinfo.mrule_end = src->end;
			SegTuple seg(src->start, src->end);
			port_map[seg] = pinfo;
		}

		if (node_x->pos != "N" && (!has_obj || !has_sbj)) {
			PortInfo pinfo = init_PortInfo();
			if (!has_sbj) {
				if (is_top_node) pinfo.type = PortType::TopSbjZA;
				else pinfo.type = PortType::LowSbjZA;
			} else {
				//! !has_obj
				if (node_x->pos == "Vi") continue;
				if (is_top_node) pinfo.type = PortType::TopObjZA;
				else pinfo.type = PortType::LowObjZA;
			}

			pinfo.mrule_start = node_x->start;
			pinfo.mrule_end = node_x->end;
			SegTuple seg(node_x->start, node_x->end);
			port_map[seg] = pinfo;
		}
	}

}

const CPPropNodeVec_t PropNodeGraph::term_nodes() const
{
	const CPPropNodeVec_t ret = 
		filter_prop_node([](const PropNode& x)->bool{
			return x.node_type == PropNodeType::Terminal;
		});
	return ret;
}

const CPPropNodeVec_t PropNodeGraph::children_nodes(const PropNode* parent) const
{
	const CPPropNodeVec_t ret =
		filter_prop_node([parent](const PropNode& x)->bool {
		return x.parent == parent;
	});
	return ret;
}

const PropNodeSet_t& PropNodeGraph::all_nodes() const
{
	return prop_node_set;
}

const CPPropNodeVec_t PropNodeGraph::unique_terms() const 
{ 	
	return filter_prop_node([](const PropNode& x)->bool{
		return x.node_type == PropNodeType::UniqueTerminal; 
	});
}

const CPPropNodeVec_t PropNodeGraph::filter_prop_node(std::function<bool(const PropNode&)> predicate) const
{
	vector<const PropNode*> buf;
	for (auto& pn_x : prop_node_set){
		if (!predicate(pn_x)) continue;
		buf.push_back(&pn_x);
	}
	return buf;
}

const vector<PropNode> PropNodeGraph::get_terminals(int start, int end)
{
	vector<PropNode> pn_vec;	

	for (auto& n : prop_node_set) {
		if (n.mrule_ptr->matched_node_type() != NodeType::NdTerminal) continue;
		if (n.mrule_ptr->start_pos() >= start &&
			n.mrule_ptr->end_pos() <= end) {
			pn_vec.push_back(n);
		}
	}

	std::sort(pn_vec.begin(), pn_vec.end(), [](PropNode& a, PropNode& b) {
		return a.mrule_ptr->start_pos() < b.mrule_ptr->start_pos();
	});

	return pn_vec;
}

string PropNodeGraph::get_terminal_text(int start, int end)
{
	const vector<PropNode> term_nodes = get_terminals(start, end);
	string buf;
	std::for_each(term_nodes.begin(), term_nodes.end(), [&buf](const PropNode& pn_x) {
		buf.append(pn_x.mrule_ptr->matched_word());
	});

	return buf;
}

const vector<const PortInfo*> PropNodeGraph::port_vector()
{
	
	vector<SegTuple> seg_vec;
	for (auto item : port_map) {
		seg_vec.push_back(item.first);		
	}

	struct IndexGen_t{ 
		int n; IndexGen_t() : n(0) {} 
		int operator()() { return n++; }
	} IndexGen;

	vector<int> idx_vec(seg_vec.size());
	std::generate(idx_vec.begin(), idx_vec.end(), IndexGen);
	std::sort(idx_vec.begin(), idx_vec.end(), 
		[&seg_vec](int a, int b) {return std::get<0>(seg_vec[a]) < std::get<0>(seg_vec[b]); });

	vector<const PortInfo*> ret_vec;
	for (auto idx : idx_vec) {
		ret_vec.push_back(&port_map[seg_vec[idx]]);
	}

	return ret_vec;
}