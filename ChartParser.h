#ifndef ChartParser_H
#define ChartParser_H

#include <string>
#include <vector>
#include <unordered_set>
using std::string;
using std::vector;

#include "common.h"
#include "DepRuleEngine.h"
#include "ChartParserAgenda.h"
#include "ChartparserChart.h"
#include "MatchedDepRule.h"
#include "DepData.h"
#include "EdgeInfoType.h"
#include "IParserInitializable.h"

typedef MatchedDepRule EdgeType;

class ChartParserException : public std::exception
{
private:
	const char* msg;
public:
	ChartParserException(const char* _msg){ msg = new char[strlen(_msg)]; }
	~ChartParserException(){ delete msg; }
	virtual const char* what() const{
		return msg;
	}
};

typedef std::unordered_set<EdgeType> EdgeSetType;
typedef std::unordered_set<EdgeType*> EdgePtrSetType;
typedef std::function<void(const ChartParserChart&)> OnInitFunc_t;
class ChartParser: public IParserInitializable
{
private:
	int			dot_pos;
	int			to_complete_count;
	int			full_edge_index;
	int         iter_max;
	vector<EdgeType*>       delim_edges;
	vector<EdgeInfoType*>	edge_info;
	
	void fwd_fundamental(const EdgeType* edge);
	void bck_fundamental(const EdgeType* edge);		
	void rep_fundamental(const EdgeType* edge);
	void uni_fundamental(const EdgeType* edge);
	void predict(const EdgeType* edge);
	void build_edge_info(int full_idx);
	CompleteEdgePred get_complete_pred() const;
	

protected:
	ChartParserChart	chart;
	ChartParserAgenda	agenda;
	vector<EdgeType*>		edge_buf;
	vector<EdgeType*>		term_vec;
	vector<DepRuleNode*>	aux_rule;
	vector<DepToken>		dep_vec;
	DepRuleEngine*			engine;
	const StateTree*		current_state;
	OnInitFunc_t			init_callback;
	
	void			reset();
	void			init();
	void			process_edge(const EdgeType* edge);
	virtual void	move_to_agenda(EdgeType* edge) override;	
	virtual void    move_to_chart(EdgeType* edge) override;
	const EdgeType* add_to_chart(const EdgeType* edge) override;
	void			move_to_agenda(EdgeType* edge, AgendaInsert_t insert_data);	
	vector<DepToken> trim_sequence_marker(const vector<DepToken>& _dep_vec, DepToken& seq_marker);
	vector<DepToken> trim_quotation(const vector<DepToken>& dp_vec, bool& quot_end);
	void            attach_seq_delimited_mrule(const DepToken& seq_marker, bool quot_end);
	EdgeType*	    create_seq_container_mrule(const vector<const EdgeType*>& mrule_vec);
	virtual void	remove_edge_in_chart(const EdgeType* edge) override;

public:
	ChartParser();
	~ChartParser();
	void set_engine(DepRuleEngine* _eng) { engine = _eng; };
	const DepRuleEngine* get_const_rule_engine() const { return engine; };
	const ChartParserChart&		get_chart() { return chart; }
	const ChartParserAgenda&	get_agenda() { return agenda; }
	virtual DepRuleEngine* get_rule_engine() override { return engine; };	
	virtual const vector<DepToken>& get_term_tokens() override { return dep_vec; }
	virtual const vector<EdgeType*>& get_term_edges() override { return term_vec; }

	virtual int parse(const vector<DepToken>& dep_vec);
	int			parse(const vector<DepInfo>& dep_info_vec);
	int			select_parse_result(int idx);
	void		set_iter_max(int v) { iter_max = v; }
	void		set_to_complete_count(int v){ to_complete_count = v; }	
	void		build_full_edge_info();	

	const EdgeType*			get_full_edge() const;
	vector<const EdgeType*> get_partial_completed_edges() const;
	vector<DepInfo>			get_dep_info() const;
	vector<EdgeInfoTypeUP>	get_edge_info() const;		
	const vector<CPStateTree>	get_state_roots() const { return agenda.get_state_roots(); }
	EdgeInfoTypeUP find_edge(int idx) const;
	EdgeInfoTypeUP create_edge_snapshot(int state_idx) const;
	virtual void		   move_aux_rule(vector<DepRuleNode*>& aux_repo);

	void on_init(OnInitFunc_t init_callback);

};

#endif // ChartParser_H