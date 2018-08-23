#ifndef PNGRAPHLAYOUT_H
#define PNGRAPHLAYOUT_H

#include "PropNodeGraph.h"

class PNGraphLayout
{
private:
	const PropNodeGraph& G;	
	void layout_graph();

public:
	PNGraphLayout(const PropNodeGraph& _g) :
		G(_g){}
	~PNGraphLayout();
	void toDot(const string& fpath);
	void toPNG(const string& dot_path, const string& png_path);
};

#endif //PNGRAPHLAYOUT_H