#ifndef RULECONSTRAINT_H
#define RULECONSTRAINT_H

#include <vector>
#include <string>
#include <array>
using std::vector;
using std::string;

struct RuleConstraint;
struct ConstraintCheck{ int index; int result; bool matcher; };
typedef vector<RuleConstraint*> RuleConstraintList;
enum class RCCondition { 
	Branching, RuleClass, POS,	
	NoParent, ParentClass, ParentLabel, Strategy,
	Invalid, Simple };
enum class RCRelOp { 
	Equal, NotEqual, Like, Assignment,
	LessThan, GreaterThan
};
struct RCRelOpComp{
	RCRelOp rel_op;
	RCRelOpComp(RCRelOp _op) : rel_op(_op){}
	bool operator()(const string& a, const string& b);
	bool operator()(const int& a, const int& b);
};
extern std::array<string, 5> RCRelOpStr;

struct RuleConstraint
{
public:
	RuleConstraint(int v, const char* cond_label, int idx, string _arg1, int w);

	int			valence;
	RCCondition cond;
	RCRelOp		rel_op;
	int         apply_node;
	string      arg;
	int			weight;
		
	void set_relational_operator(int opcode);
	operator string() const;
};

#endif // RULECONSTRAINT_H