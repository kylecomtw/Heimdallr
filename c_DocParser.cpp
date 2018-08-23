#include "c_DocParser.h"

//! DocParser
//! ------------
DocParser* init_doc_parser()
{	
	return new DocParser();
}

void set_doc_parser_debug()
{
	debug_out = fopen("h:/heimdallr_debug.txt", "w");
}

void unset_doc_parser_debug()
{
	if (debug_out) {		
		fflush(debug_out);
		fclose(debug_out);
		debug_out = 0;
	}
}

PropNodeGraph* init_pn_graph()
{
	return new PropNodeGraph();
}

void free_doc_parser(DocParser* p)
{	
	delete p;
}

void free_pn_graph(PropNodeGraph* p)
{
	delete p;
}

void dp_set_iter_max(DocParser *dp, int v)
{
	dp->set_iter_max(v);
}

void dp_import_rule_file(DocParser* dp, const char* rule_path)
{
	dp->import_rule_file(rule_path);
}

void dp_parse_text(DocParser* dp, const char* txt, PropNodeGraph* pn_graph)
{
	dp->parse_text(txt, *pn_graph);
}

void dp_build_graph(DocParser* dp, PropNodeGraph* pn_graph)
{
	dp->build_graph(*pn_graph);
}

void dp_add_lexical_marker_target(DocParser* dp, const char* lm)
{
	dp->add_lexical_marker_target(lm);
}

//! PropNodeGraph
int png_sequence_mrules_count(PropNodeGraph* png)
{
	return png->sequence_mrules().size();
}

void png_import_mrule(PropNodeGraph* png, const MatchedDepRule* mrule)
{
	png->import_mrule(mrule);
}

void png_import_pnprops(PropNodeGraph* png, int n_prop, PNProp** prop_arr_ptr, DepRuleEngine* engine)
{
	vector<PNProp> prop_vec;
	for (int i = 0; i < n_prop; ++i) {
		prop_vec.push_back(*prop_arr_ptr[i]);
	}
	png->import_pnprops(prop_vec, engine);
}

const MatchedDepRule* png_sequence_mrules(PropNodeGraph* png, int idx)
{
	const CPMRuleVec& seq_mrules = png->sequence_mrules();
	if (idx < seq_mrules.size()) {
		return seq_mrules[idx];
	} else {
		return NULL;
	}
}

void png_terminal_text(PropNodeGraph* png, int start, int end, char* text_buf, int buf_len)
{
	string term_txt = png->get_terminal_text(start, end);
	if (term_txt.size() < buf_len) {		
		strcpy_s(text_buf, buf_len, term_txt.c_str());
	}

	return;
}

int png_port_count(PropNodeGraph* pn_graph)
{
	return pn_graph->ports().size();
}

void png_ports(PropNodeGraph* pn_graph, int n, const PortInfo** p_array)
{
	auto port_vec = pn_graph->port_vector();
	std::fill(p_array, p_array + n, nullptr);
	if (port_vec.size() != n) {
		return;
	}

	for (int i = 0; i < n; ++i) {
		p_array[i] = port_vec[i];
	}

	return;
}

int png_port_class_count(PropNodeGraph* pn_graph, int starti, int endi)
{
	ClassRuleMap& rclass_map = pn_graph->get_class_rule_map();
	SegTuple seg(starti, endi);
	if (rclass_map.count(seg) > 0) {
		return rclass_map.at(seg).size();
	} else {
		return 0;
	}
}

int png_port_class(PropNodeGraph* pn_graph, int starti, int endi, int arrlen, const char** arrbuf)
{
	ClassRuleMap& rclass_map = pn_graph->get_class_rule_map();
	SegTuple seg(starti, endi);
	if (rclass_map.count(seg) == 0 || rclass_map.at(seg).size() != arrlen) {
		return -1;
	}

	const vector<string>& class_vec = rclass_map.at(seg);
	for (int i = 0; i < class_vec.size(); ++i) {
		const string& class_str = class_vec.at(i);
		arrbuf[i] = class_str.c_str();
	}

	return arrlen;
}

//! PNPropLayout
PNPropLayout* init_prop_layout()
{
	return new PNPropLayout();
}

PNPropVector* init_prop_vector()
{
	return new PNPropVector();
}

void free_prop_layout(PNPropLayout* p)
{
	delete p;
}

void free_prop_vector(PNPropVector* p)
{
	delete p;
}

void pnpl_transform(PNPropLayout* prop_layout, PropNodeGraph* pn_graph, PNPropVector* pnp_vec)
{
	prop_layout->transform(*pn_graph, *pnp_vec);
}

//! PNPropVector
int pnpv_prop_count(PNPropVector* pn_vec)
{
	return pn_vec->size();
}

void pnpv_prop_vector(PNPropVector* pn_vec, int n, PNProp** pnp_arr)
{	
	int sz = pn_vec->size();	
	for (int i = 0; i < n; ++i) {
		if (i >= sz) continue;
		pnp_arr[i] = &(*pn_vec)[i];
		
	}
}

const char* pnp_predicate(PNProp* p)
{
	return p->predicate.c_str();
}

const char* pnp_code(PNProp* p)
{
	return p->code.c_str();
}

int pnp_arg_count(PNProp* p)
{
	return p->args.size();
}

void pnp_arg_vector(PNProp* p, int n, PropArg** pa_arr)
{
	int sz = p->args.size();
	for (int i = 0; i < n; ++i) {
		if (i >= sz) break;
		pa_arr[i] = &(p->args[i]);
	}
}

void pnp_to_string(PNProp* p, int n, char* buf)
{
	string pstr = p->to_string();
	strcpy_s(buf, n, pstr.c_str());
}

PNProp* pnp_create(const char* pred, const char* code, int n_arg, PropArg** arg_arr)
{
	PNProp* prop = new PNProp();
	prop->code = code;
	prop->predicate = pred;
	for (int i = 0; i < n_arg; ++i) {
		prop->args.push_back(*arg_arr[i]);
	}

	return prop;
}

void pnp_delete(PNProp* ptr)
{
	delete ptr;
}

//! PropArg
int pa_start(PropArg* p)
{
	return p->start;
}

int pa_end(PropArg* p) 
{
	return p->end;
}

bool pa_is_ref(PropArg* p)
{
	return p->is_reference;
}

const char* pa_text(PropArg* p)
{
	return p->text.c_str();
}

const char* pa_pos(PropArg* p)
{
	return p->pos.c_str();
}

const char* pa_decorator(PropArg* p)
{
	return p->decorator.c_str();
}

void pa_to_string(PropArg* p, int n, char* buf)
{
	string pstr = p->to_string();
	strcpy_s(buf, n, pstr.c_str());
}

PropArg* pa_create(int start, int end, const char* text, bool isref, const char* pos, const char* deco)
{
	PropArg* pa = new PropArg();
	pa->start = start; pa->end = end;
	pa->text = text;
	pa->pos = pos;
	pa->decorator = deco;
	pa->is_reference = isref;

	return pa;
}

void pa_delete(PropArg* ptr)
{
	delete ptr;
}