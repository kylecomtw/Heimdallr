#include "RuleConstraint.h"
#include <sstream>

std::array<string, 5> RCRelOpStr{ { "==", "!=", "~", "<", ">" } };
RuleConstraint::RuleConstraint(int v, const char* cond_label, int idx, string _arg1, int w)
	:valence(v), apply_node(idx), arg(_arg1), weight(w)
{
	rel_op = RCRelOp::Equal;
	if (_strcmpi(cond_label, "branch") == 0){
		cond = RCCondition::Branching;
	} else if (_strcmpi(cond_label, "pos") == 0){
		cond = RCCondition::POS;
	} else if (_strcmpi(cond_label, "class") == 0){
		cond = RCCondition::RuleClass;
	} else if (_strcmpi(cond_label, "noparent") == 0){
		cond = RCCondition::NoParent;
	} else if (_strcmpi(cond_label, "simple") == 0){
		cond = RCCondition::Simple;
	} else if (_strcmpi(cond_label, "strategy") == 0){
		cond = RCCondition::Strategy;
	} else if (_strcmpi(cond_label, "parentclass") == 0){
		cond = RCCondition::ParentClass;
	} else if (_strcmpi(cond_label, "parentlabel") == 0) {
		cond = RCCondition::ParentLabel;	
	} else {
		printf("[ERROR] unrecognized condition label: %s\n", cond_label);
		cond = RCCondition::Invalid;
	}
}

void RuleConstraint::set_relational_operator(int opcode)
{
	switch (opcode){
	case(1) :
		rel_op = RCRelOp::Equal;
		break;
	case(2) :
		rel_op = RCRelOp::NotEqual;
		break;
	case(3) :
		rel_op = RCRelOp::Like;
		break;
	case(4) :
		rel_op = RCRelOp::Assignment;
		break;
	case(5) :
		rel_op = RCRelOp::LessThan;
		break;
	case(6) :
		rel_op = RCRelOp::GreaterThan;
		break;
	}
}

RuleConstraint::operator string() const
{
	std::stringstream ss;	
	if (valence == 1) ss << "[+" << weight << "] ";
	else if (valence == -1) ss << "[-" << weight << "] ";
	else { ss << "= ";  }

	if (cond == RCCondition::Branching){
		ss << "Branching {Node " << apply_node << "}";
	} else if (cond == RCCondition::RuleClass){
		ss << "RuleClass " << RCRelOpStr[(int)rel_op] << " " << arg;
	} else if (cond == RCCondition::ParentClass){
		ss << "ParentClass " << "==" << arg;
	} else if (cond == RCCondition::ParentLabel){
		ss << "ParentLabel " << "!=" << arg;
	} else {
		ss << "Not recognized or no supporting string() constraints";
	}
	
	return ss.str();
}

bool RCRelOpComp::operator()(const string& a, const string& b)
{
	int min_size = 0;
	switch (rel_op){
	case (RCRelOp::Equal) :
		return a == b;
		break;
	case(RCRelOp::NotEqual) :
		return a != b;
		break;
	case(RCRelOp::Like) :		
		min_size = std::min(a.size(), b.size());
		return a.substr(0, min_size) == b.substr(0, min_size) ||
			a.substr(a.size() - min_size, min_size) == b.substr(b.size() - min_size, min_size);		
		break;
	default:
		printf("[WARNING] invalid RCRelOp, or not supported for string comparison\n");
		return 0;
	}
}

bool RCRelOpComp::operator()(const int& a, const int& b)
{
	int min_size = 0;
	switch (rel_op){
	case(RCRelOp::LessThan) :
		return a < b;
		break;
	case(RCRelOp::GreaterThan) :
		return a > b;
		break;
	default:
		printf("[WARNING] invalid RCRelOp, or not supported for int comparison\n");
		return 0;
	}
}