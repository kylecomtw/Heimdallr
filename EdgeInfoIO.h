#ifndef EDGEINFOIO_H
#define EDGEINFOIO_H

#include "EdgeInfoType.h"
#include <string>
#include <vector>
#include <sstream>
#include <fstream>

using std::string;
using std::vector;
class EdgeInfoIO
{
private:
	void einfo_to_string(std::ostringstream& oss, const EdgeInfoType* einfo);

public:
	vector<EdgeInfoType*> edge_info_vec;
	EdgeInfoIO();
	EdgeInfoIO(const EdgeInfoType* einfo);
	EdgeInfoIO(vector<const EdgeInfoType*> einfo_vec);
	~EdgeInfoIO();

	void add_edge_info(const EdgeInfoType* einfo);
	string serialize();	
	void write_to_file(const string& fpath);
	void deserialize(const string& edge_str, vector<EdgeInfoType*>& ret);
	void read_from_file(const string& fpath, vector<EdgeInfoType*>& ret);
};

#endif //EDGEINFOIO_H