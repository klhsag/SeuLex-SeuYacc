#pragma once

#include <string>
#include <iostream>

#include "yyProducer.h"

struct  YyWrite{

	static std::string make_init() {
		using namespace std;
		std::string content = "";
		for (auto p_ : map_idx_idname) {
			int idx = p_.first;
			string name = p_.second;
			content += "strcpy(item_name_table[" + to_string(idx) + "], \"" + name + "\");";
		}
		for (int t = 0; t < producers.size(); ++t) {
			auto & p_ = producers[t];
			int idx = p_.first;
			auto & vec = p_.second;
			int l = vec.size();
			vector<int> v(2);
			v[0] = l;
			v[1] = idx;
			for (int i = 0; i < l; ++i) {
				v.push_back(vec[i]);
			}
			for (int i = 0; i < l + 2; ++i) {
				content += "producers[" + to_string(t) + "][" + to_string(i) + "]=" + to_string(v[i]) + ";";
			}
		}
		return content;
	}

};