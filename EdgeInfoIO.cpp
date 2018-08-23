#include "EdgeinfoIO.h"
#include "EdgeInfoParser.h"
EdgeInfoIO::EdgeInfoIO()
{

}

EdgeInfoIO::EdgeInfoIO(const EdgeInfoType* einfo)
{
	add_edge_info(einfo);
}

EdgeInfoIO::~EdgeInfoIO()
{
	for (auto einfo_ptr : edge_info_vec){
		delete einfo_ptr;
	}
	edge_info_vec.clear();
}

void EdgeInfoIO::add_edge_info(const EdgeInfoType* einfo)
{
	edge_info_vec.push_back(einfo->CopyTree());
}

string EdgeInfoIO::serialize()
{
	std::ostringstream oss;
	for (int i = 0; i < edge_info_vec.size(); ++i){
		einfo_to_string(oss, edge_info_vec.at(i));
	}
	return oss.str();
}

void EdgeInfoIO::deserialize(const string& edge_str, vector<EdgeInfoType*>& ret)
{	
	EdgeInfoParser parser;
	int error = parser.parse(edge_str, ret);
	if (error){
		for (auto x : ret){ delete x; }
		ret.clear();
	}
}

void EdgeInfoIO::write_to_file(const string& fpath)
{
	std::ofstream ofs(fpath, std::ofstream::out);
	const string& str = serialize();
	ofs << str;
	ofs.close();
}

void EdgeInfoIO::read_from_file(const string& fpath, vector<EdgeInfoType*>& ret)
{
	std::ifstream ifs(fpath, std::ifstream::in);
	std::ostringstream oss;
	oss << ifs.rdbuf();
	ifs.close();
	return deserialize(oss.str(), ret);
}

void EdgeInfoIO::einfo_to_string(std::ostringstream& oss, const EdgeInfoType* einfo)
{
	if (einfo->children.size() == 0){
		//! terminal
		oss << "\"" << einfo->word << "\"" << ":" << einfo->pos;
		if (einfo->is_head) oss << "*";
	} else {
		oss << einfo->rule_label;
		if (einfo->is_head) oss << "*";
		oss << "(";
		for (int i = 0; i < einfo->children.size(); ++i){
			if (i > 0) oss << "|";
			einfo_to_string(oss, einfo->children.at(i));			
		}
		oss << ")";
	}
}