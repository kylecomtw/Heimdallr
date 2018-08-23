#ifndef RULEPROPERTY_H
#define RULEPROPERTY_H

#include <string>
using std::string;

class RuleProperty
{
private:
	string key;
	string value;
public:
	RuleProperty(const char* k, const char* v) : key(k), value(v) {}
	const string& get_key() const { return key; }
	const string& get_value() const { return value; }
	void  set_value(const string& s) { value = s; }
};

#endif //RULEPROPERTY_H
