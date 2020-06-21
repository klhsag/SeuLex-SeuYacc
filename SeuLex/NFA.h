#pragma once

#include <cstdlib>

#include <cstdio>

#include <stack>
#include <utility>

#include <map>
#include <set>
#include <queue>

#include <unordered_map>

#include <vector>

#define CHAR_NULL 127

extern std::vector<char> allowed_chars;

struct NFANode {
	int id;
	char input1 = CHAR_NULL;
	NFANode * next1 = NULL;
	char input2 = CHAR_NULL;
	NFANode * next2 = NULL;
};

extern int FA_flag[65536];


void parseReg(char * reg, NFANode * startNode, int flag);

void parseFirstReg(char * reg, NFANode * & startNode, int flag);

int NFAtoDFA(NFANode * startNode);