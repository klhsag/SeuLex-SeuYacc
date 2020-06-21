#pragma once

#include <map>

#include "yaccUtil.h"

struct LRTableSide{
	int type;                      // 0-reduce               1-shift / GOTO
	int data;                      // reduce: producerId     shift: stateId
};

using LRTableRow = std::map<int, LRTableSide>;

struct  LRTable{

	std::vector< LRTableRow > table;

	LRTable() = default;
	LRTable(const LRDFA & dfa) {
		for (const LRState & src : dfa.statesVec) {
			LRTableRow row;
			for (auto & p_ : src.edgesMap) {
				int c = p_.first;
				LRTableSide e = { 1, p_.second };
				row.insert({ c, e });
			}
			for (auto & item : src.LRItemsSet) {
				if (item.position != producers[item.gramarIdx].second.size()) continue;
				int gid = item.gramarIdx;
				auto it = row.find(item.predictiveSymbol);
				if (it == row.end() || (it->second.type == 0 && gid < it->second.data)) {
					row[item.predictiveSymbol] = { 0, gid } ;
				}
			}
			table.push_back(std::move(row));
		}
	}

	void dump(int(* print_)(const char *, ...)) {
		for (int i = 0; i < table.size(); ++i) {
			print_("%3d :  ", i);
			for (auto p_ : table[i]) {
				if (p_.second.type == 0) {
					print_(" %c->r[%3d ] ", p_.first, p_.second.data);
				} else {
					print_(" %c ==> %3d> ", p_.first, p_.second.data);
				}
			}
			print_("\n");
		}
	}

	std::string code_dump(std::string state_name, std::string symbol_name) {
		using std::to_string;
		std::string outer_before = "switch (" + state_name + ")	{";
		std::string outer_after = "default: return 65536; }";
		std::string inner = "";
		for (int i = 0; i < table.size(); ++i) {
			std::string inner_before = "case " + to_string(i) + " : switch (" + symbol_name + ")	{";
			std::string inner_after = "default: return 65536; }";
			std::string tmp = "";
			for (auto p_ : table[i]) {
				int to;
				if (p_.second.type == 0) {
					to = -1 - p_.second.data;
				}else {
					to = p_.second.data;
				}
				tmp += "case " + to_string(p_.first) + " :{return " + to_string(to) + ";}";
			}
			inner += inner_before + tmp + inner_after;
		}
		return outer_before + inner + outer_after;
	}

};