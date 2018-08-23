#ifndef PNPROPBINARYMAPPER_H
#define PNPROPBINARYMAPPER_H
#include "PNPropArg.h"
#include <unordered_map>
typedef vector<PNProp> PNPropVector;
template<typename K, typename V>
using Map = std::unordered_map<K, V>;

class PNPropBinaryMapper
{
private:

public:
	PNPropBinaryMapper();
	PNPropVector mapToBinary(const PNPropVector& prop_vec);
	void rebuild_binary_pnprop(const PNPropVector& prop_vec, PNPropVector& propbin_vec);
	void create_code_map(const PNPropVector& prop_vec, Map<string, string>& code_map);
	void update_code_in_propvec(const Map<string, string>& code_map, PNPropVector& propbin_vec);
};

#endif //PNPROPBINARYMAPPER_H
