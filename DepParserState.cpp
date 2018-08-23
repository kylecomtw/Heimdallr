#include "DepParserState.h"
#include <functional>

DepParserState::DepParserState(const DepParser& _parser)
	: parser(_parser)
{

}

const vector<DepToken>* DepParserState::get_input() const
{
	return &(parser.get_input());
}

int DepParserState::get_input_cursor() const
{
	return parser.get_input_cursor();
}

const std::stack<DepToken> DepParserState::get_stack() const
{
	return parser.get_parser_stack();
}

const vector<int>& DepParserState::get_head_vec() const{ return head_vec; }
const vector<int>& DepParserState::get_dep_vec() const{ return dep_vec; }
const vector<int>& DepParserState::get_lc_vec() const{ return lc_vec; }
const vector<int>& DepParserState::get_rc_vec() const{ return rc_vec; }
const vector<int>& DepParserState::get_ls_vec() const{ return ls_vec; }
const vector<int>& DepParserState::get_rs_vec() const{ return rs_vec; }

void DepParserState::sync_callback()
{

}