#include "DepParser.h"
#include "DepData.h"

DepParser::DepParser(){};

DepParser::~DepParser(){};

ArcMapType& DepParser::get_arc_map()
{
	return arc_map;
}

const std::stack<DepToken>& DepParser::get_parser_stack() const
{
	return work_stack;
}

int DepParser::get_input_cursor() const 
{
	return input_cursor;
}

const vector<DepToken>& DepParser::get_input() const
{
	return input;
}

vector<DepInfo> DepParser::parse(const vector<PosTaggedType>& _input)
{
	reset();
	set_input(_input);

	shift();
	while (input_cursor < input.size()){
		const DepToken& stack0 = work_stack.top();
		const DepToken& input0 = input.at(input_cursor);
		DepRuleType rule_found = DepRuleType::WAITING;
		if (rule_found == DepRuleType::RIGHT_DEP){
			right_arc();
		} else if (rule_found == DepRuleType::LEFT_DEP){
			left_arc();
		} else if (rule_found == DepRuleType::NO_RULE){
			reduce();
		} else {
			shift();
		}
	}

	return get_dep_info_vec();
}

int DepParser::decode(const vector<DepInfo>& dep_data, vector<DPAction>& act_vec)
{
	
	reset();

	// convert dep_data into input and arc_ans;
	ArcMapType arc_ans;
	for (int i = 0; i < dep_data.size(); ++i){
		DepToken dt;
		dt.index = i;
		dt.word = dep_data.at(i).word;
		dt.pos = dep_data.at(i).pos;
		input.push_back(dt);

		arc_ans[i] = dep_data.at(i).head_index;
	}

	shift();
	act_vec.push_back(DPAction::Shift);

	while (input_cursor < input.size()){
		// check elements in stack
		if (work_stack.size() == 0){
			act_vec.push_back(DPAction::Shift);
			shift();
		}

		const DepToken& stack0 = work_stack.top();
		const DepToken& input0 = input.at(input_cursor);
		if (arc_ans.count(stack0.index) > 0 && 
			arc_ans.at(stack0.index) == input0.index){
			// input0 is the head of stack0
			// it is a [LeftArc]
			act_vec.push_back(DPAction::LA);
			left_arc();
			arc_ans.erase(stack0.index);
		} else if (arc_ans.count(input0.index) > 0 &&
			arc_ans.at(input0.index) == stack0.index){
			// stack0 is the head of input 0
			// there is a [RightArc]
			act_vec.push_back(DPAction::RA);
			right_arc();
			arc_ans.erase(input0.index);
		} else if (arc_ans.count(stack0.index) == 0 &&			
			!is_head_of_others(stack0.index, arc_ans)){
			// stack0 already has a head and not used as a head, reduce it
			act_vec.push_back(DPAction::Reduce);
			reduce();
		} else {
			// shift to next
			act_vec.push_back(DPAction::Shift);
			shift();
		}

		if (act_vec.size() > input.size() * 5){
			printf("ERROR: too much iteration: %d\n", act_vec.size());
			break;
		}
	}

	return 0;
}

vector<DepInfo> DepParser::do_actions(const vector<DepInfo>& _input, const vector<DPAction>& act_vec)
{
	vector<PosTaggedType> input_pos;
	for (const auto& di_x : _input){
		PosTaggedType pos_x;
		pos_x.pos = di_x.pos;
		pos_x.word = di_x.word;
		input_pos.push_back(pos_x);
	}

	return do_actions(input_pos, act_vec);
}

vector<DepInfo> DepParser::do_actions(const vector<PosTaggedType>& _input, const vector<DPAction>& act_vec)
{	
	reset();
	set_input(_input);
	
	dprintf("-- Do Actions --\n");
	for (const auto& act : act_vec){
		next(act);
	}

	return get_dep_info_vec();
}

void DepParser::set_input(const vector<PosTaggedType>& _input)
{
	// convert _input into input
	for (int i = 0; i < _input.size(); ++i){
		DepToken dt;
		dt.index = i;
		dt.word = _input.at(i).word;
		dt.pos = _input.at(i).pos;
		input.push_back(dt);
	}
}

void DepParser::next(DPAction act)
{
	switch (act){
	case DPAction::RA:
		right_arc();
		break;
	case DPAction::LA:
		left_arc();
		break;
	case DPAction::Reduce:
		reduce();
		break;
	case DPAction::Shift:
		shift();
		break;
	default:
		break;
	}
}

void DepParser::reset()
{
	init();
}

bool DepParser::init()
{
	// initialize
	work_stack.empty();
	arc_map.clear();
	input.clear();
	DepToken dt_root;
	dt_root.index = -1; dt_root.pos = "ROOT"; dt_root.word = "ROOT";
	input.push_back(dt_root);
	input_cursor = 0;
	return 0;
}

bool DepParser::left_arc()
{	
	if (work_stack.size() == 0){
		dprintf("[LeftArc] failed: empty stack\n");
		return false;
	}

	if (input_cursor >= input.size()){
		dprintf("[LeftArc] failed: cursor out of bound\n");
		return false;
	}

	const DepToken& stack0 = work_stack.top();
	const DepToken& input0 = input.at(input_cursor);
	dprintf("[LeftArc] %d -> %d: ", input0.index, stack0.index);

	if (arc_map.count(stack0.index) == 0){
		// no element is the head of current element
		arc_map[stack0.index] = input0.index;
		work_stack.pop();
		dprintf("Success\n");
		return true;
	}

	dprintf("Failed\n");
	return false;
}

bool DepParser::right_arc()
{
	if (work_stack.size() == 0){
		dprintf("[RightArc] failed: empty stack\n");
		return false;
	}

	if (input_cursor >= input.size()){
		dprintf("[RightArc] failed: cursor out of bound\n");
		return false;
	}

	const DepToken& stack0 = work_stack.top();
	const DepToken& input0 = input.at(input_cursor);
	dprintf("[RightArc] %d -> %d: ", input0.index, stack0.index);

	if (arc_map.count(input0.index) == 0){
		// no element is the head of input0
		arc_map[input0.index] = stack0.index;		
		dprintf("Success\n");
		work_stack.push(input0);
		return advance_input();
	}
	dprintf("Failed\n");
	return false;
}

bool DepParser::reduce()
{
	if (work_stack.size() == 0){
		dprintf("[Reduce] failed: empty stack\n");
		return false;
	}

	const DepToken& stack0 = work_stack.top();
	dprintf("[Reduce] %d: ", stack0.index);
	if (arc_map.count(stack0.index) > 0){
		// stack0 already has a head
		work_stack.pop();

		dprintf("Success\n");
		return true;
	}

	dprintf("Failed\n");
	return false;
}

bool DepParser::shift()
{
	dprintf("[Shift]\n");
	if (input_cursor >= input.size()){
		dprintf("[Shift] failed: cursor out of bound\n");
		return false;
	}
	const DepToken& input0 = input.at(input_cursor);
	work_stack.push(input0);	
	return advance_input();
}

bool DepParser::advance_input()
{
	++input_cursor;
	if (input_cursor < input.size()){		
		return true;
	} else {
		return false;
	}
}

vector<DepInfo> DepParser::get_dep_info_vec()
{
	vector<DepInfo> di_vec(input.size()-1);
	// input in DepParser includes the ROOT element.
	// vector<DepInfo> does not contain ROOT, 
	// therefore starts with i=1
	for (int i = 1; i < input.size(); ++i){
		DepInfo& di_x = di_vec[i-1];
		const DepToken& dt = input.at(i);
		di_x.word = dt.word;
		di_x.pos = dt.pos;
		if (arc_map.count(dt.index) > 0){
			di_x.head_index = arc_map.at(dt.index);
		}
	}

	return di_vec;
}

bool DepParser::is_head_of_others(int _idx, ArcMapType& _arc_map)
{
	for (const auto& item_x : _arc_map){
		if (item_x.second == _idx){
			return true;
		}
	}
	return false;
}

int DepParser::save_rule_set(const string& fpath)
{
	return rule_engine.save(fpath);
}

int DepParser::load_rule_set(const string& fpath)
{
	return -1;
}

DepRuleEngine& DepParser::get_rule_engine()
{
	return rule_engine;
}