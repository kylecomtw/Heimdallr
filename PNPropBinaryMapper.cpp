#include "PNPropBinaryMapper.h"

PNPropBinaryMapper::PNPropBinaryMapper()
{

}

PNPropVector PNPropBinaryMapper::mapToBinary(const PNPropVector& prop_vec)
{
	PNPropVector propbin_vec;
	Map<string, string> code_map;
	
	rebuild_binary_pnprop(prop_vec, propbin_vec);
	create_code_map(propbin_vec, code_map);
	update_code_in_propvec(code_map, propbin_vec);

	return propbin_vec;
}

void PNPropBinaryMapper::rebuild_binary_pnprop(const PNPropVector& prop_vec, PNPropVector& propbin_vec)
{
	//! reindex original propositions into binary propositions.
	//! Original proposition list, prop_vec, are processed one by one. The process results are stored in propbin_vec.
	//! If the original proposition has two or less than two arguments, 
	//! it directly copied itself into propbin_vec. If it has more than two arguments,
	//! The first two arguments become a new proposition with a new temporary code,
	//! and stored into probin_vec. Each of remaining arguments become the second argument of each new proposition,
	//! where the first argument is referenced to the previous proposition.
	//! The new proposition has predicate _CONCAT_, and is given a new temp code, then store into probin_vec.
	
	for (const PNProp& prop: prop_vec) {
		PNProp base_prop(prop);
		if (prop.args.size() > 2) {			
			base_prop.args.erase(base_prop.args.begin(), base_prop.args.begin() + 2);
			base_prop.code = "T_" + base_prop.code + "_0";			
			propbin_vec.push_back(base_prop);

			PNProp* prev_prop = &base_prop;
			for (int i = 2; i < prop.args.size(); ++i) {
				PropArg arg_x = prop.args[i];
				PNProp new_prop;
				new_prop.predicate = "CONCAT";
				new_prop.code = "T_" + base_prop.code + "_" + std::to_string(i);

				PropArg embed_arg;
				embed_arg.is_reference = true;
				embed_arg.start = prev_prop->args[0].start;
				embed_arg.end = prev_prop->args.back().end;
				embed_arg.text = prev_prop->code;
				new_prop.args.push_back(embed_arg);

				PropArg next_arg(arg_x);
				new_prop.args.push_back(next_arg);
				
				propbin_vec.push_back(new_prop);
				prev_prop = &propbin_vec.back();
			}
		} else {	
			base_prop.code = "T_" + base_prop.code;
			propbin_vec.push_back(prop);
		}
	}
		
}

void PNPropBinaryMapper::create_code_map(const PNPropVector& prop_vec, Map<string, string>& code_map)
{
	//! build a map table between string codes and proposition
	Map<string, const PNProp*> code_prop_map;
	auto code_prop_mapper = [&code_prop_map](const PNProp& p) {
		code_prop_map[p.code] = &p;
	};

	std::for_each(prop_vec.cbegin(), prop_vec.cend(), code_prop_mapper);

	//! sort the string codes by their (1) literal words presenting order or 
	//! (2) prosition starting position in the text.
	auto find_prop_pos = [](const PNProp* p) {
		for (auto& arg : p->args) {
			if (!arg.is_reference) {
				return arg.start;
			}
		}

		return p->args.back().end;
	};

	auto code_sort_func = [&code_prop_map, find_prop_pos](const string& a, const string& b) {
		const PNProp* pa = code_prop_map[a];
		const PNProp* pb = code_prop_map[b];
		if (pa == nullptr || pb == nullptr) return 0;
		return find_prop_pos(pa) - find_prop_pos(pb);
	};

	vector<string> tmp_code_vec;
	std::transform(prop_vec.cbegin(), prop_vec.cend(), std::back_inserter(tmp_code_vec),
		[](const PNProp& p) { return p.code; });
	std::sort(tmp_code_vec.begin(), tmp_code_vec.end(), code_sort_func);

	//! assign final code and save to code_map
	code_map.clear();
	int chunk_counter = 0;
	int seq_counter = 0;
	for (const string& tmp_code : tmp_code_vec) {
		if (tmp_code.size() > 3 && tmp_code.substr(0, 3) == "T_S") {
			code_map[tmp_code] = "S" + std::to_string(++seq_counter);
		} else {
			code_map[tmp_code] = "C" + std::to_string(++chunk_counter);
		}
	}
}

void PNPropBinaryMapper::update_code_in_propvec(const Map<string, string>& code_map, PNPropVector& propbin_vec)
{
	for (PNProp& prop : propbin_vec) {
		if (!code_map.count(prop.code)) {
			prop.code = code_map.at(prop.code);
		} else {
			printf("[WARNING] code update failed in PNPropBinaryMapper\n");
		}
	}

	std::sort(propbin_vec.begin(), propbin_vec.end(), [](const PNProp& a, const PNProp& b) {
		return std::greater<string>()(a.code, b.code);
	});
}
