#pragma once

#include <string>

#include "NFA.h"
#include "DFA.h"

struct RegToDFA{

	int nodes;

	std::map<int, std::string> flag_map;

	int flagCnt = 10;

	NFANode * startNode;
	char reg[65536];
	
	void init() {
		for (int i = 9; i < 127; ++i) {
			if (true/*i != '#'*/) {

				allowed_chars.push_back(i);
			}
		}
		memset(FA_flag, 0, sizeof FA_flag);
	}

	void appendRegex(std::string regStr, std::string info) {
		strcpy(reg, regStr.c_str());
		if (flag_map.empty()) {
			parseFirstReg(reg, startNode, flagCnt);
		}else {
			parseReg(reg, startNode, flagCnt);
		}
		flag_map[flagCnt] = info;
		++flagCnt;
	}

	void generate() {
		int states = NFAtoDFA(startNode);
		DFAMinimizer.init(states);
		DFAMinimizer.deal();
		nodes = DFAMinimizer.generate();
	}

	void show() {
		for (int i = 0; i < nodes; ++i) {
			printf("%3d : ", i);
			printf(" [ %4d ] ", minDFA[i].flag);
			for (auto & p_ : minDFA[i].ptrs) {
				printf(" < %c , %3d >", p_.first, p_.second->id);
			}
			printf("\n\n");
		}
	}

};
