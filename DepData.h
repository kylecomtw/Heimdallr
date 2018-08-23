#ifndef SD_CONVERTER_H
#define SD_CONVERTER_H

#include "EdgeInfoType.h"
#include <string>
#include <vector>
using std::string;
using std::vector;

struct DepFormCheck
{
	bool acyclic;
	bool single_head;
	bool projective;
	bool connected;

	DepFormCheck(){ acyclic = true; single_head = true; projective = true; connected = true; }
	bool pass(){
		return acyclic && single_head && projective && connected;
	}
};

struct DepInfo
{
	int head_index;
	string label;
	string word;
	string pos;
	DepInfo() {
		head_index = -1;
		label = "";
		word = "";
		pos = "";
	}

	bool operator == (const DepInfo& other) const{
		return this->head_index == other.head_index &&
			this->label == other.label && this->word == other.word &&
			this->pos == other.pos;
	}

	bool operator != (const DepInfo& other) const{
		return !(*this == other);
	}
};

class DepData{
	
private:
	bool is_sentence_delim(const DepInfo& dd);
	bool contains_quot_end(const string& str);
	void load_pos_text(std::istream& ins, vector<vector<DepInfo>>& dep_info_vec);

public:

	vector<DepInfo> dep_convert(const vector<EdgeInfoType*> edges, int nTerm);
	DepFormCheck dep_test_well_formed(const vector<DepInfo>& dep_data);
	void write_dep_data(std::ofstream& out_fs, const vector<DepInfo>&);
	void convert_pos_text(const vector<string>& word_vec, const vector<string>& pos_vec,
		vector<vector<DepInfo>>& dep_doc);
	void load_dep_data(std::ifstream& in_fs, vector<vector<DepInfo>>& dep_data_vec);	
	void load_pos_text_file(const string&, vector<vector<DepInfo>>& dep_info_vec);
	void load_pos_text_string(const string&, vector<vector<DepInfo>>& dep_info_vec);
};

#endif //SD_CONVERTER_H