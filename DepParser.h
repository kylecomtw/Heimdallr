#ifndef DEPPARSER_H
#define DEPPARSER_H

#include <vector>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <functional>
#include "common.h"
#include "DepData.h"
#include "DepRuleEngine.h"

using std::vector;
using std::string;

typedef int ArcHeadType;
typedef int ArcDepType;
typedef std::unordered_map<ArcDepType, ArcHeadType> ArcMapType;
enum class DepRuleType{ RIGHT_DEP, LEFT_DEP, NO_RULE, WAITING };

enum class DPAction{LA, RA, Reduce, Shift};

class DepParser
{
private:
	vector<DepToken> input;
	int input_cursor;
	std::stack<DepToken> work_stack;
	ArcMapType arc_map;
	DepRuleEngine rule_engine;

	// parser action
	bool init();
	bool left_arc();
	bool right_arc();
	bool reduce();
	bool shift();
	bool advance_input();
	bool is_head_of_others(int _idx, ArcMapType& _arc_map);
	vector<DepInfo> DepParser::get_dep_info_vec();
	void emit_sync();

public:
	DepParser();
	~DepParser();	
	ArcMapType& get_arc_map();
	const std::stack<DepToken>& get_parser_stack() const;
	int							get_input_cursor() const;
	const vector<DepToken>&		get_input() const;

	void set_input(const vector<PosTaggedType>& _input);
	void next(DPAction act);
	void reset();

	int decode(const vector<DepInfo>& dep_data, vector<DPAction>& act_vec);
	vector<DepInfo> parse(const vector<PosTaggedType>& _input);
	vector<DepInfo> do_actions(const vector<DepInfo>& _input, const vector<DPAction>& act_vec);
	vector<DepInfo> do_actions(const vector<PosTaggedType>& _input, const vector<DPAction>& act_vec);

	int save_rule_set(const string& fpath);
	int load_rule_set(const string& fpath);
	DepRuleEngine& get_rule_engine();

};

#endif //DEPPARSER_H