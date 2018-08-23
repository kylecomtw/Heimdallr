#include "PNGraphLayout.h"
#include <cstdlib>
PNGraphLayout::~PNGraphLayout()
{
	
}

#include <fstream>
void PNGraphLayout::toDot(const string& fpath)
{
	std::ofstream ofs(fpath);
	if (!ofs.good()){
		printf("[ERROR] cannot open %s for writing\n", fpath.c_str());
		return;
	}

	string tab = "    ";

	ofs << "strict graph{" <<std::endl;
	ofs << "node[fontname = \"MingLiU\"]" << std::endl;

	int seq_counter = 0;
	for (const PropNode& pn: G.all_nodes()){
		if (pn.node_type == PropNodeType::Sequence){
			ofs << tab << "S" << pn.id << "[ label = \"" << ++seq_counter << "\", ";
			ofs << "style = filled, width = 0.25, fillcolor = red, fontcolor = white]" << std::endl;
		} else if (pn.node_type == PropNodeType::UniqueTerminal){
			ofs << tab << pn.id << "[ label = \"" << pn.word << "\"]" << std::endl;
		} else if (pn.node_type == PropNodeType::Terminal){
			ofs << tab << pn.id << "[ color=blue, label = \"" << pn.word << "\"]" << std::endl;
		} else {

		}
	}
	
	for (const PropEdge& pe : G.edges()){
		
		const PropEdgeInfo& peinfo = G.get_edge_info(&pe);
		if (peinfo.has_type(PropEdgeType::Sequence)){
			//! top-level nodes
			ofs << tab << "S" << pe.a->id << " -- " << pe.b->id << std::endl;
		} else {
			//! terminal nodes
			ofs << tab << pe.a->id << " -- " << pe.b->id;
			ofs << "[ ";			
			if (peinfo.has_type(PropEdgeType::Rule)) {				
				ofs << "color = blue";
			}
			ofs << " ];" << std::endl;
		}
		
	}

	ofs << "}"; 

	
	return;
}

void PNGraphLayout::layout_graph()
{


	for (const PropNode* pn : G.term_nodes()){

	}

	for (const PropEdge& pe : G.edges()){

	}

}

void PNGraphLayout::toPNG(const string& dot_path, const string& png_path)
{
	char buf[255];
	sprintf_s(buf, "sfdp -Goverlap=prism -Tpng %s > %s", dot_path.c_str(), png_path.c_str());
	printf("gv command: %s\n", buf);
	std::system(buf);
	printf("gv command complete\n");

}

