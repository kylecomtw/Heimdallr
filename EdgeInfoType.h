#ifndef EDGEINFOTYPE_H
#define EDGEINFOTYPE_H

#include <string>
#include <vector>
#include "MatchedDepRule.h"
#include <memory>
using std::string;
using std::vector;

class EdgeInfoType;
typedef std::unique_ptr<EdgeInfoType> EdgeInfoTypeUP;

class EdgeInfoType
{
public:	
	string word;
	string pos;
	string rule_class;
	string rule_label;	
	int    rule_id;
	int	   id;
	bool   is_head;
	int	   dot_pos;
	int    start;
	int    end;
	int	   cost;
	EdgeInfoType*		 parent;
	EdgeInfoType*		 head_ptr;
	vector<EdgeInfoType*> children;	

	EdgeInfoType(){ 
		rule_id = 0; id = 0; is_head = false; dot_pos = 0;
		start = 0; end = 0; cost = 0;
		parent = nullptr; head_ptr = nullptr; 
	}
	~EdgeInfoType() { for (auto x : children){ delete x; } }
	EdgeInfoType(const EdgeInfoType&) = delete;
	const EdgeInfoType& operator = (EdgeInfoType) = delete;

	EdgeInfoType* CopyTree() const
	{
		EdgeInfoType* new_obj = new EdgeInfoType();
		new_obj->dot_pos = dot_pos;
		new_obj->is_head = is_head;
		new_obj->rule_id = rule_id;			new_obj->id = id;
		new_obj->word = word;				new_obj->pos = pos;
		new_obj->rule_class = rule_class;	new_obj->rule_label = rule_label;		
		new_obj->start = start;				new_obj->end = end;
		new_obj->cost = cost;			
		
		for (int i = 0; i < children.size(); ++i){
			EdgeInfoType* copy_ch = children.at(i)->CopyTree();
			copy_ch->parent = new_obj;
			if (copy_ch->is_head){
				new_obj->head_ptr = copy_ch;
			}			
			new_obj->children.push_back(copy_ch);
		}

		return new_obj;
	}

	static EdgeInfoType* from_matched_dep_rule(const MatchedDepRule* m_rule)
	{
		EdgeInfoType* einfo = new EdgeInfoType();
		einfo->id = m_rule->id();
		einfo->dot_pos = m_rule->dot_pos();
		einfo->rule_id = m_rule->rule_id();
		einfo->word = m_rule->matched_word();
		einfo->pos = m_rule->matched_pos();
		einfo->rule_class = m_rule->matched_rule_class();
		einfo->rule_label = m_rule->matched_rule_label();				
		einfo->start = m_rule->start_pos();
		einfo->end = m_rule->end_pos();
		einfo->cost = m_rule->get_tree_match_cost();		

		//! build children node
		int head_index = m_rule->head_index();
		for (int i = 0; i < m_rule->get_children_count(); ++i){
			const MatchedDepRule* m_child = m_rule->get_matched_children(i);
			EdgeInfoType* edge_c = nullptr;
			if (m_child != nullptr){				
				if (m_child->matched_node_type() == NodeType::NdTermRule){
					const MatchedDepRule* m_cchild = m_child->get_matched_children(0);
					edge_c = EdgeInfoType::from_matched_dep_rule(m_cchild);
				} else {
					edge_c = EdgeInfoType::from_matched_dep_rule(m_child);
				}

				edge_c->parent = einfo;
				if (i == head_index){
					einfo->head_ptr = edge_c;
					edge_c->is_head = true;
				}
			} else {
				edge_c = new EdgeInfoType();
			}
			
			einfo->children.push_back(edge_c);			
		}
		return einfo;
	}
	
};

#endif //EDGEINFOTYPE_H