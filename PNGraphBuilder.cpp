#include "PNGraphBuilder.h"
#include "DepData.h"
#include "ChartParser.h"
#include "ChartParserAgenda.h"
#include "ChartParserChart.h"
#include "PNChart.h"
#include "PNAgenda.h"
#include "PNParser.h"
#include <unordered_map>
#include <boost/algorithm/string.hpp>

void PNGraphBuilder::build_edges()
{	
	parse_graph();	

	index_rule_class();
	G->build_terminal_dependencies();
	G->build_ports();
}

void PNGraphBuilder::parse_graph()
{
	PNAgenda pn_agenda(G->matched_rules());
	PNChart pn_chart(G->matched_rules());
	PNParser pn_parser(pn_agenda, pn_chart, *engine);
	pn_parser.parse();
	pn_agenda.clear_active_rules();

}

void PNGraphBuilder::add_port(const MRule* mrule, const MRule* head, bool is_top, int head_shift, PortType pt)
{
	if (mrule) {
		PortInfo pinfo = init_PortInfo();
		pinfo.mrule_start = mrule->start_pos();
		pinfo.mrule_end = mrule->end_pos();		
		pinfo.is_top = is_top; pinfo.head_shift = head_shift;
		pinfo.type = pt;

		if (head) {
			pinfo.head_start = head->start_pos();
			pinfo.head_end = head->end_pos();
		}

		add_port(pinfo);
	}
}

void PNGraphBuilder::add_port(const PortInfo& pinfo)
{	
	if (pinfo.mrule_start >= 0 && pinfo.mrule_end >= 0) {				
		SegTuple seg(pinfo.mrule_start, pinfo.mrule_end);
		PortMap& port_map = G->ports();
		if (port_map.count(seg) > 0) {
			auto pinfo_old = port_map.at(seg);
			auto pinfo_new = merge_port_info(pinfo_old, pinfo);
			port_map[seg] = pinfo_new;				
		} else {
			port_map.insert({ seg, pinfo });
		}		
	}
}

void PNGraphBuilder::remove_port(const SegTuple& seg)
{
	PortMap& port_map = G->ports();
	if (port_map.count(seg) > 0) {
		port_map.erase(seg);
	}
}

PortInfo PNGraphBuilder::merge_port_info(const PortInfo& oldp, const PortInfo& newp)
{
	PortInfo merged(oldp);
	if (!oldp.head_start >= 0) {
		merged.head_start = newp.head_start;
		merged.head_end = newp.head_end;
		merged.head_shift = newp.head_shift;
	}

	if (!oldp.has_det) merged.has_det = newp.has_det;
	if (!oldp.is_pronoun) merged.is_pronoun = newp.is_pronoun;
	if (oldp.type == PortType::Generic) merged.type = newp.type;

	return merged;
}


void PNGraphBuilder::exclude_ports(const MRule* seq)
{
	auto term_vec = find_terminals(seq);
	for (const MRule* term : term_vec) {
		bool is_not_port = check_mrule_class(term->start_pos(), term->end_pos(), "NotPort");
		if (!is_not_port) continue;
		remove_port(SegTuple(term->start_pos(), term->end_pos()));
	}
}

void PNGraphBuilder::sbj_tagging_default_V(const MRule* mrule, const MRule* mrule_head, 
	int ch_idx, bool is_top, TagResult& res)
{
	const string& pos = mrule->matched_pos();

	//! it is just a ad-hoc solution, what about Obj-Sbj-Vt construct?				
	if (ch_idx - 1 >= 0 &&
		mrule_head->get_matched_children(ch_idx - 1)->matched_pos() == "N") {
		//! if there is a noun tree before pn_top

		auto mrule_ch = mrule_head->get_matched_children(ch_idx - 1);
		auto ch_head = find_bottom_heading(mrule_ch);

		add_port(ch_head, mrule, is_top, -1, PortType::LowSbj);
		res.sbj_found = true;
	} else {
		//! if there is not a noun tree before pn_top, pn_top might having zero anaphora				
		// add_port(mrule, nullptr, is_top, -1, PortType::LowSbjZA);
	}

	if (pos != "Vi") {
		if (ch_idx + 1 < mrule_head->get_children_count() &&
			mrule_head->get_matched_children(ch_idx + 1)->matched_pos() == "N") {
			//! if there is a noun tree before pn_top
			auto mrule_ch = mrule_head->get_matched_children(ch_idx + 1);
			auto ch_head = find_bottom_heading(mrule_ch);
			add_port(ch_head, mrule, is_top, 1, PortType::LowObj);
			res.obj_found = true;
		} else {			
			// add_port(mrule, nullptr, is_top, 1, PortType::LowObjZA);
		}
	}
}

void PNGraphBuilder::sbj_tagging_rule_prop(const MRule* mrule, const MRule* mrule_head, 
											int ch_idx, bool is_top, const SOTag& so_tag, TagResult& res)
{
	if (so_tag.sbj_idx >= 0) {
		auto mrule_ch = mrule_head->get_matched_children(so_tag.sbj_idx);
		auto ch_head = find_bottom_heading(mrule_ch);
		add_port(ch_head, mrule, is_top, 1, so_tag.sbj_type);
		res.sbj_found = true;
	}

	if (so_tag.obj_idx >= 0) {
		auto mrule_ch = mrule_head->get_matched_children(so_tag.obj_idx);
		auto ch_head = find_bottom_heading(mrule_ch);
		add_port(ch_head, mrule, is_top, 1, so_tag.obj_type);
		res.obj_found = true;
	}
}

//! find the highest parent that is not headed by pn.
//! for example, 
//! A -> X1 <pn>
//! B -> Y1 <A>
//! C -> <Z1> B
//! D -> W1 <C>
//! find_top_headed(pn, idx) return C, and ch_idx equals to 1
const PropNode* PNGraphBuilder::find_top_headed(const PropNode* pn, int& ch_idx)
{
	//! check if it is the top-level verb
	auto parent_x = pn->parent;
	auto local_x = pn;
	int cur_idx = -1;
	const string& ori_pos = pn->mrule_ptr->matched_pos();

	while (parent_x) {
		//! pptr is the parent mrule, mptr is the local mrule.
		//! cur_idx is defined between these two mrules
		auto pptr = parent_x->mrule_ptr;
		auto mptr = local_x->mrule_ptr;
		cur_idx = find_mrule_child_index(pptr, mptr);		

		//! local_x is the top headed if:
		//! 1) parent have more than one child AND
		//! 2) head of the parent mrule is not the current local rule
		if (pptr->get_children_count() > 1 && pptr->head_index() != cur_idx) {					
			break;
		}
		
		//! move higher to the top, update values of local_x and parent_x
		ch_idx = cur_idx;
		local_x = parent_x;
		parent_x = parent_x->parent;			
	}
	
	return local_x;
}

const MRule* PNGraphBuilder::find_bottom_heading(const MRule* mrule)
{
	//! check if it is the top-level verb	
	auto local_x = mrule;

	auto head_x = find_head(mrule);
	int cur_idx = -1;
	while (head_x) {			    
		local_x = head_x;
		head_x = find_head(local_x);
	}

	return local_x;
}

const MRule* PNGraphBuilder::find_tree_parent(const PropNode* pn)
{
	auto parent_x = pn->parent;
	while (parent_x) {
		if (parent_x->mrule_ptr->matched_node_type() == NodeType::NdTree &&
			parent_x->mrule_ptr->get_children_count() > 1) {
			break;
		}

		parent_x = parent_x->parent;
	}

	return parent_x->mrule_ptr;
}

const MRule* PNGraphBuilder::find_head(const MRule* mrule)
{
	if (mrule->get_children_count() == 1) {
		return mrule->get_matched_children(0);
	}

	int hindex = mrule->head_index();
	if (hindex < 0) return nullptr;

	if (mrule->get_children_count() == 0) {
		return nullptr;
	}

	if (hindex < mrule->get_children_count()) {
		return mrule->get_matched_children(hindex);
	} else {
		return nullptr;
	}
}

const MRule* PNGraphBuilder::find_prev_sibling(const MRule* mrule)
{
	const PropNode* pn = G->find_node(mrule);
	if (!pn->parent) return nullptr;

	int this_idx = find_mrule_child_index(pn->mrule_ptr, mrule);
	if (this_idx - 1 >= 0) {
		return pn->mrule_ptr->get_matched_children(this_idx - 1);
	} else {
		return nullptr;
	}
}

const MRule* PNGraphBuilder::find_next_sibling(const MRule* mrule)
{
	const PropNode* pn = G->find_node(mrule);
	if (!pn->parent) return nullptr;

	int this_idx = find_mrule_child_index(pn->mrule_ptr, mrule);
	if (this_idx + 1 < pn->mrule_ptr->get_children_count()) {
		return pn->mrule_ptr->get_matched_children(this_idx + 1);
	} else {
		return nullptr;
	}
}

int PNGraphBuilder::find_mrule_child_index(const MRule* parent, const MRule* child)
{
	for (int i = 0; i < parent->get_children_count(); ++i) {
		auto parent_ch = parent->get_matched_children(i);
		if (parent_ch->start_pos() == child->start_pos() &&
			parent_ch->end_pos() == child->end_pos()) return i;
	}
	return -1;
}

void PNGraphBuilder::index_rule_class()
{
	ClassRuleMap& rclass_map = G->get_class_rule_map();
	for (auto mrule : G->matched_rules()) {
		const string& rclass = mrule.matched_rule_class();
		SegTuple seg(mrule.start_pos(), mrule.end_pos());
		if (rclass_map.count(seg) > 0) {			
			rclass_map[seg].push_back(rclass);
		} else {
			rclass_map[seg] = vector<string>{ rclass };
		}
		
	}

	return;
}

bool PNGraphBuilder::check_mrule_class(int istart, int iend, const string& rclass)
{
	ClassRuleMap& rclass_map = G->get_class_rule_map();
	SegTuple seg(istart, iend);
	if (rclass_map.count(seg) > 0) {
		auto rc_vec = rclass_map[seg];
		auto find_iter = std::find(rc_vec.begin(), rc_vec.end(), rclass);
		return find_iter != rc_vec.end();
	} else {
		return false;
	}
}


void PNGraphBuilder::find_matched_rule_class(const string& rclass, vector<const PropNode*>& out_vec)
{
	for (const MRule& mrule_x : G->matched_rules()){
		if (mrule_x.matched_rule_class() != rclass) continue;
		const PropNode* pn = G->find_node(&mrule_x);
		if (pn != nullptr){
			out_vec.push_back(pn);
		}
	}	
}

const vector<const MRule*> PNGraphBuilder::find_terminals(const MRule* mrule_in)
{
	vector<const MRule*> term_vec;
	vector<const MRule*> work_buf;

	work_buf.push_back(mrule_in);
	while (work_buf.size() > 0) {
		const MRule* mrule = work_buf.back();
		work_buf.pop_back();
		
		if (mrule->matched_rule_class() == "TermRule") {
			term_vec.push_back(mrule->get_matched_children(0));
		} else {
			auto& ch_vec = mrule->get_matched_children();
			for (int i = ch_vec.size() - 1; i >= 0; --i) {
				auto mrule_ch = ch_vec.at(i);
				work_buf.push_back(mrule_ch);
			}
		}		
	}

	return term_vec;
}

PNGraphBuilder::SOTag PNGraphBuilder::parse_so_tag(const string* prop_str, bool is_top)
{
	//! parsing text: SbjZA(0); Obj(2);
	vector<string> colon_parts;
	boost::split(colon_parts, *prop_str, boost::is_any_of(";"), boost::token_compress_on);
	SOTag so_tag;

	for (const string& cpart: colon_parts) {
		auto istart = cpart.find("(");
		auto iend = cpart.find(")");
		if (istart == std::string::npos || iend == std::string::npos) {
			printf("[WARNING] invalid SOTag format: %s\n", cpart.c_str());
			continue;
		}

		if (boost::starts_with(cpart, "Sbj")) {
			try {
				so_tag.sbj_type = port_type_from_string(cpart.substr(0, istart), is_top);
				so_tag.sbj_idx = std::stoi(cpart.substr(istart + 1, iend));
			}
			catch (std::exception& ex) {
				printf("[WARNING] invalid SOTag format: %s\n", ex.what());
			}
		} else {
			try {
				so_tag.obj_type = port_type_from_string(cpart.substr(0, istart), is_top);
				so_tag.obj_idx = std::stoi(cpart.substr(istart + 1, iend));
			}
			catch (std::exception& ex) {
				printf("[WARNING] invalid SOTag format: %s\n", ex.what());
			}
		}
	}

	return so_tag;

}

PortType PNGraphBuilder::port_type_from_string(const string& tag, bool is_top)
{
	if (tag == "SbjZA") {
		if (is_top) return PortType::TopSbjZA; else PortType::LowSbjZA;
	} else if (tag == "ObjZA") {
		if (is_top) return PortType::TopObjZA; else PortType::LowObjZA;
	} else if (tag == "Sbj") {
		if (is_top) return PortType::TopSbj; else PortType::LowSbj;
	} else if (tag == "Obj") {
		if (is_top) return PortType::TopObj; else PortType::LowObj;
	}

	return PortType::Generic;
}
