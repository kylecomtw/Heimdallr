#ifndef PNPROPARG_H
#define PNPROPARG_H
#include <string>
#include <vector>
#include "DepRuleNode.h"
#include <sstream>

using std::string;
using std::vector;

struct PropArg {
	int     start;
	int		end;
	bool	is_reference;
	string	text;
	string  pos;
	string	decorator;
	string	to_string()	const {
		std::ostringstream oss;
		if (this->decorator.size() > 0) {
			oss << this->decorator << "(";
			oss << this->text << ")";
		} else {
			oss << this->text << "-" << this->start;
		}

		return oss.str();
	}
};

struct PNProp {		
	string			predicate;
	string			code;
	vector<PropArg> args;
	string			to_string() const {
		std::ostringstream oss;
		oss << this->code << " (";
		oss << this->predicate << ", ";

		for (int i = 0; i < this->args.size(); ++i) {
			if (i > 0) oss << ", ";
			oss << args[i].to_string();
		}

		oss << ")";
		return oss.str();
	}
};

#endif //PNPROPARG_H
