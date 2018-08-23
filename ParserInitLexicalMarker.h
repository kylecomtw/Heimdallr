#ifndef PARSERINITLEXICALMARKER_H
#define PARSERINITLEXICALMARKER_H

#include "ParserInitializer.h"
#include <unordered_map>
#include <unordered_set>

typedef std::unordered_map<int, int> IntIntMap;
typedef std::unordered_set<int>		 IntSet;
typedef std::pair<int, DepRuleNode*> IntRulePtrPair;
typedef std::pair<int, int>			 IntIntPair;

class ParserInitLexicalMarker : public ParserInitializer
{
private:
	int	seq_end_pos;
	IntSet				frame_leaders;
	IntSet				matched_literals;
	// IntIntMap			matched_literals;	
	vector<EdgeType*>	lexical_marker_edges;	
	

	void	remove_key_terminals();	
	vector<int>				find_literals(const vector<IntRulePtrPair>&);
	MatchedDepRule*			get_parser_terminals(int matched_idx);
	void					designate_frame_leader(int matched_idx);
	vector<IntRulePtrPair>	extract_literal_terminals(const DepRuleNode*);	
	vector<MatchedDepRule*>	backward_match_inactive_edge(EdgeType* edge);
	vector<MatchedDepRule*>	create_LM_mrule(
							const DepRuleNode* rule,
							const vector<int>& m_idx, const vector<const MatchedDepRule*>& m_vec);
	void					add_lexical_marker_edge();
	void					initialize_agenda();
	bool					check_mrule_valid(MatchedDepRule* mrule);
	

public :
	ParserInitLexicalMarker(IParserInitializable* p) : ParserInitializer(*p){}
	virtual void initialize_parser() override;	
	const IntSet&		get_matched_literals() { return matched_literals; }
	const vector<EdgeType*>& get_lexical_marker_edges(){ return lexical_marker_edges; }

};

#endif //PARSERINITLEXICALMARKER_H