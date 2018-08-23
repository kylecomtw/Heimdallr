#include "DepData.h"
#include "common.h"
#include <unordered_map>
#include <unordered_set>
#include <stack>
#include <algorithm>
#include <cstdio>
#include <stdarg.h>
#include <set>
#include <map>
#include <fstream>
#include <sstream>


vector<DepInfo> DepData::dep_convert(const vector<EdgeInfoType*> edges, int nTerm)
{
	std::stack<const EdgeInfoType*> work_stack;
	for (auto& edge_x: edges){
		work_stack.push(edge_x);
	}
	vector<DepInfo>	dep_data;
	dep_data.resize(nTerm);

	while (!work_stack.empty()){
		const EdgeInfoType* edge_ptr = work_stack.top();
		work_stack.pop();
		if (edge_ptr->children.size() > 0){
			//! not terminal, add all children to work_stack
			for (auto& edge_c : edge_ptr->children){
				work_stack.push(edge_c);
			}
		} else {
			//! edge is terminal
			int head_idx = -1;
			

			if (edge_ptr->is_head){

				const EdgeInfoType* buf_ptr = edge_ptr;
				//! go up to the 'not-head' parent				
				while (buf_ptr->parent != nullptr){
					buf_ptr = buf_ptr->parent;					
					if (!buf_ptr->is_head) break;
				}

				if (buf_ptr->parent != nullptr){
					buf_ptr = buf_ptr->parent;
				}

				//! go down to the 'terminal' head
				while (buf_ptr->head_ptr != nullptr){
					buf_ptr = buf_ptr->head_ptr;
				}

				if (buf_ptr == edge_ptr || buf_ptr->children.size() > 0){
					//! the head is the node itself, or the parent does not have a head
					//! then assign the head to the root
					head_idx = -1;
				} else {
					head_idx = buf_ptr->start;
				}
				
			} else {
				//! the node is not head
				if (edge_ptr->parent != nullptr && edge_ptr->parent->head_ptr == nullptr){
					//! the parent does not have a head, assign the head to root
					head_idx = -1;
				} else{
					//! go down to the 'terminal' head
					const EdgeInfoType* buf_ptr = edge_ptr->parent->head_ptr;
					while (buf_ptr->head_ptr != nullptr){
						buf_ptr = buf_ptr->head_ptr;
					}
					head_idx = buf_ptr->start;
				}
				
			}

			DepInfo& di = dep_data[edge_ptr->start];
			di.word = edge_ptr->word;
			di.pos = edge_ptr->pos;
			di.head_index = head_idx;			
		}
	}

	return dep_data;

}

DepFormCheck DepData::dep_test_well_formed(const vector<DepInfo>& dep_data)
{
	// single-head
	typedef int HeadIndex;
	typedef int TokenIndex;
	std::unordered_map<HeadIndex, TokenIndex> head_table;
	DepFormCheck form_flags;

	// single-head
	// DepInfo only has a scalar field of head_index
	// it is internally a single-head structure
	form_flags.single_head = true;
	
	// -----------------
	//      acyclic
	// -----------------
	for (int i = 0; i < dep_data.size(); ++i){
		int node_idx = i;
		DepInfo dep_x = dep_data.at(i);
		while (dep_x.head_index != -1){
			if (i == dep_x.head_index){
				printf("cyclic dependency\n");
				form_flags.acyclic = false;
			}
			dep_x = dep_data.at(dep_x.head_index);
		}
	}

	// -----------------
	//     connected
	// -----------------
	std::stack<int> con_stack;
	std::unordered_set<int> visited;
	std::unordered_map<int, vector<int>> rev_link;
	
	// prepare reverse link data structure
	for (int i = 0; i < dep_data.size(); ++i){
		rev_link[dep_data.at(i).head_index].push_back(i);
	}

	// push the head and reverse links of first node into the stack
	const DepInfo& dep_x = dep_data[0];
	con_stack.push(dep_x.head_index);
	if (rev_link.count(0) > 0){
		for (auto x : rev_link.at(0)){
			con_stack.push(x);
		}
	}
	visited.insert(0);

	while (con_stack.size() > 0){	
		int idx = con_stack.top();		
		con_stack.pop();
		if (idx < 0) continue;
		visited.insert(idx);

		// add the head to the stack
		int head_idx = dep_data.at(idx).head_index;
		if (visited.count(head_idx) == 0){
			con_stack.push(head_idx);
		}

		// also add the reverse link (head of other nodes) to the stack
		if (rev_link.count(idx) > 0){
			vector<int>& idx_vec = rev_link.at(idx);
			for (int rev_idx : idx_vec){
				if (visited.count(rev_idx) == 0){
					con_stack.push(rev_idx);
				}
			}
		}
	}

	if (visited.size() != dep_data.size()){
		printf("not connected\n");
		form_flags.connected = false;;
	}

	// -----------------
	//     projective
	// -----------------
	for (int i = 0; i < dep_data.size(); ++i){
		int head_idx = dep_data.at(i).head_index;
		if (head_idx < 0) continue;

		int left_idx = std::min(head_idx, i);
		int right_idx = std::max(head_idx, i);
		for (int j = left_idx+1; j <= right_idx-1; ++j){
			int head_j = dep_data.at(j).head_index;
			if (head_j < left_idx || head_j > right_idx){
				printf("non-projective detected at pos %d\n", i);
				form_flags.projective = false;
			}
		}		 
	}

	if (form_flags.pass()){
		dprintf("Dep-Well-Form: pass\n");
	}
	return form_flags;
}

void DepData::write_dep_data(std::ofstream& out_fs, const vector<DepInfo>& dep_data)
{
	out_fs << "<line>\n";
	for (int i = 0; i < dep_data.size(); ++i){
		const DepInfo& di = dep_data.at(i);
		out_fs << di.word << ", ";
		out_fs << di.pos << ",";
		out_fs << di.head_index << "\n";
	}
	out_fs << "</line>\n";
}

void DepData::load_pos_text_file(const string& fpath, vector<vector<DepInfo>>& dep_info_vec)
{
	std::ifstream in_fs(fpath);
	if (!in_fs.is_open()) {
		printf("Cannot open file stream");
		return;
	}

	load_pos_text(in_fs, dep_info_vec);
}

void DepData::load_pos_text_string(const string& txt, vector<vector<DepInfo>>& dep_info_vec)
{
	std::istringstream iss;
	iss.str(txt);

	load_pos_text(iss, dep_info_vec);
}

void DepData::load_pos_text(std::istream& ins, vector<vector<DepInfo>>& dep_info_vec)
{
	string line, part, word, pos;
	vector<DepInfo> dep_data;
	std::vector<string> fields(2);	

	vector<string> split_buf;
	bool is_quot_sen = false;

	while (std::getline(ins, line)){				
		if (line.size() == 0) continue;
		std::stringstream ss(line);			
		const string delim = "\xe3\x80\x80";  // full-width space \u3000
		size_t cur_i = 0;		
		if (line.substr(0, 1) == "#") continue;
		while ((cur_i = line.find(delim)) != std::string::npos || line.size() > 0){
			//! if there is no further delimiter, but cur_i is not at the end of line,
			//! set cur_i at the end of line and loop the last time
			if (cur_i == std::string::npos) cur_i = line.size();

			split_buf.push_back(line.substr(0, cur_i));
			line.erase(0, cur_i + delim.length());
		}

	}


	for (int i = 0; i < split_buf.size(); ++i) {
		part = split_buf[i];
		int istart = part.find_first_not_of(' ');
		int iend = part.find_last_not_of(' ');

		//! handle chunk breaking by manual mark ($$)
		if (istart != std::string::npos &&
			iend != std::string::npos &&
			part.substr(istart, iend - istart + 1) == "$$") {
			dep_info_vec.push_back(std::move(dep_data));
			dep_data.clear();
			continue;
		}

		int pos_istart = part.find_first_of('(');
		int pos_iend = part.find_last_of(')');
		if (pos_istart == std::string::npos || pos_iend == std::string::npos) {
			printf("cannot find POS tag\n");
			continue;
		}

		pos = part.substr(pos_istart + 1, pos_iend - pos_istart - 1);
		word = part.substr(0, pos_istart);
		fields[0] = word;
		fields[1] = pos;

		if (fields.size() < 2) {
			printf("parse failed\n");
			continue;
		}

		DepInfo di;
		if (fields.size() == 2) {
			di.word = fields[0];
			di.pos = fields[1];
			di.label = "";
		}

		dep_data.push_back(di);

		//! peek ahead to check for an ending quotation
		if (is_sentence_delim(di) &&
			i < split_buf.size() - 1 && 
			contains_quot_end(split_buf[i+1])) {
			is_quot_sen = true;
			continue;
		}

		//! chunk breaking by punctuations
		if (is_quot_sen || is_sentence_delim(di)) {
			dep_info_vec.push_back(std::move(dep_data));
			dep_data.clear();
			is_quot_sen = false;
		}
	}

	if (dep_data.size() > 0){
		dep_info_vec.push_back(std::move(dep_data));
	}
}

void DepData::load_dep_data(std::ifstream& in_fs, vector<vector<DepInfo>>& dep_data_vec)
{
	if (!in_fs.is_open()){
		printf("Cannot open file stream");
		return;
	}

	string line;
	vector<DepInfo> dep_data;
	std::vector<string> fields;
	fields.reserve(4);
	while (std::getline(in_fs, line)){
		if (_stricmp(line.c_str(), "<line>") == 0){
			dep_data.clear();
		} else if (_stricmp(line.c_str(), "</line>") == 0){
			dep_data_vec.push_back(std::move(dep_data));
		} else {
			string part;						
			std::stringstream ss(line);
			DepInfo di;
			fields.clear();
			while (std::getline(ss, part, ',')){				
				int start = part.find_first_not_of(' ');
				int end = part.find_last_not_of(' ');
				part = part.substr(start, end - start + 1);
				fields.push_back(part);
			}
			if (fields.size() == 3){
				di.word = fields[0];
				di.pos = fields[1];
				di.head_index = stoi(fields[2]);
				di.label = "";
				dep_data.push_back(di);
			} else {
				printf("parse failed\n");
			}
			
		}
	}
}

bool DepData::is_sentence_delim(const DepInfo& dd)
{
	if (dd.word == "\xe3\x80\x82" ||  // ¡C
		dd.word == "\xef\xbc\x8c" ||  // ¡A
		dd.word == "\xef\xbc\x81" ||  // ¡I
		dd.word == "\xef\xbc\x9f" ||  // ¡H
		dd.word == "\xef\xbc\x9a" ||  // ¡G
		dd.word == "\xef\xbc\x9b"   // ¡F
		)
		return true;
	else
		return false;
}

bool DepData::contains_quot_end(const string& str)
{	
	std::string QUOTATION_END = "\xE3\x80\x8D";	
	auto ret = str.find(QUOTATION_END);
	
	if (ret == string::npos) {
		return false;
	} else {
		return true;
	}	
}

void DepData::convert_pos_text(const vector<string>& word_vec, const vector<string>& pos_vec,
	vector<vector<DepInfo>>& dep_doc)
{
	vector<DepInfo> dep_data;
	DepInfo di;

	if (word_vec.size() != pos_vec.size()){
		printf("[ERROR] word_vec.size() != pos_vec.size()\n");
		return;
	}

	for (int i = 0; i < word_vec.size(); ++i){
		di.word = word_vec.at(i);
		di.pos = pos_vec.at(i);
		dep_data.push_back(di);

		if (is_sentence_delim(di)){
			dep_doc.push_back(std::move(dep_data));
			dep_data.clear();
		}
	}	

	if (dep_data.size() > 0){
		dep_doc.push_back(std::move(dep_data));
	}

	return;
}