#ifndef C_DOCPARSER_H
#define C_DOCPARSER_H

#include "DocParser.h"
#include "PNTextLayout.h"
#include "PropNodeGraph.h"
#include "PNPropLayout.h"
#define EXPORT extern"C" __declspec(dllexport)

struct PortInfo;

//! DocParser
//! ------------
EXPORT DocParser* init_doc_parser();
EXPORT PropNodeGraph* init_pn_graph();
EXPORT void set_doc_parser_debug();
EXPORT void unset_doc_parser_debug();
EXPORT void free_doc_parser(DocParser*);
EXPORT void free_pn_graph(PropNodeGraph*);
EXPORT void dp_import_rule_file(DocParser*, const char* rule_path);
EXPORT void dp_set_iter_max(DocParser *dp, int v);
EXPORT void dp_parse_text(DocParser* dp, const char* txt, PropNodeGraph* pn_graph);
EXPORT void dp_build_graph(DocParser* dp, PropNodeGraph* pn_graph);
EXPORT void dp_add_lexical_marker_target(DocParser*, const char* lm);

//! PropNodeGraph
EXPORT int	png_sequence_mrules_count(PropNodeGraph* png);
EXPORT void png_import_mrule(PropNodeGraph* png, const MatchedDepRule* mrule);
EXPORT void png_import_pnprops(PropNodeGraph* png, int n_prop, PNProp** prop_arr_ptr, DepRuleEngine* engine);
EXPORT void png_terminal_text(PropNodeGraph* png, int start, int end, char* text_buf, int buf_len);
EXPORT int png_port_count(PropNodeGraph* pn_graph);
EXPORT void png_ports(PropNodeGraph* pn_graph, int n, const PortInfo** p_array);
EXPORT int png_port_class_count(PropNodeGraph* pn_graph, int starti, int endi);
EXPORT int png_port_class(PropNodeGraph* pn_graph, int starti, int endi, int arrlen, const char** arrbuf);
EXPORT const MatchedDepRule* png_sequence_mrules(PropNodeGraph* png, int idx);

//! PNPropLayout
EXPORT PNPropLayout* init_prop_layout();
EXPORT PNPropVector* init_prop_vector();
EXPORT void		free_prop_layout(PNPropLayout* p);
EXPORT void		free_prop_vector(PNPropVector* p);
EXPORT void		pnpl_transform(PNPropLayout*, PropNodeGraph*, PNPropVector*);

//! PNPropVector
EXPORT int pnpv_prop_count(PNPropVector*);
EXPORT void pnpv_prop_vector(PNPropVector*, int n, PNProp**);

//! PNProp
EXPORT const char* pnp_predicate(PNProp*);
EXPORT const char* pnp_code(PNProp*);
EXPORT int pnp_arg_count(PNProp*);
EXPORT void pnp_arg_vector(PNProp*, int n, PropArg**);
EXPORT void pnp_to_string(PNProp*, int n, char* buf);
EXPORT PNProp* pnp_create(const char* pred, const char* code, int n_arg, PropArg** arg_arr);
EXPORT void pnp_delete(PNProp* ptr);

//! PropArg
EXPORT int pa_start(PropArg*);
EXPORT int pa_end(PropArg*);
EXPORT bool pa_is_ref(PropArg*);
EXPORT const char* pa_text(PropArg*);
EXPORT const char* pa_pos(PropArg*);
EXPORT const char* pa_decorator(PropArg*);
EXPORT void pa_to_string(PropArg*, int n, char* buf);
EXPORT PropArg* pa_create(int start, int end, const char* text, bool isref, const char* pos, const char* deco);
EXPORT void pa_delete(PropArg* ptr);

#endif //C_DOCPARSER_H