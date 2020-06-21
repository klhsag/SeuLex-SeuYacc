
#include <cstdlib>

#include <cstdio>

#include <stack>
#include <utility>

#include <map>
#include <set>
#include <queue>

#include <unordered_map>

#include <vector>

#include "DFA.h"

#define CHAR_NULL 127

using namespace std;

std::vector<char> allowed_chars;

struct NFANode {
	int id;
	char input1 = CHAR_NULL;
	NFANode * next1 = NULL;
	char input2 = CHAR_NULL;
	NFANode * next2 = NULL;
};

int FA_flag[65536];

NFANode NFANodes[65536];

int NFANodesPtr = 0;

NFANode * newNFANode() {
	NFANodes[NFANodesPtr].id = NFANodesPtr;
	return &NFANodes[NFANodesPtr++];
}

stack<char> op_stack;
stack<pair<NFANode*, NFANode*> > node_stack;

void NFA_pop_node_by_op(char op) {
	auto pair_ = node_stack.top();
	NFANode * start = pair_.first, *end = pair_.second;
	switch (op) {
	case '|': {
		node_stack.pop();
		auto pair2 = node_stack.top();
		NFANode * start2 = pair2.first, *end2 = pair2.second;
		NFANode * newStart = newNFANode();
		NFANode * newEnd = newNFANode();
		newStart->input1 = newStart->input2 = '\0';
		newStart->next1 = start;
		newStart->next2 = start2;
		end->input1 = end2->input1 = '\0';
		end->next1 = end2->next1 = newEnd;
		node_stack.pop();
		node_stack.push({ newStart,newEnd });
		break;
	}
	case '\0': {
		node_stack.pop();
		auto pair2 = node_stack.top();
		NFANode * start2 = pair2.first, *end2 = pair2.second;
		end2->input1 = start->input1;
		end2->next1 = start->next1;
		end2->input2 = start->input2;
		end2->next2 = start->next2;
		node_stack.pop();
		node_stack.push({ start2, end });
		break;
	}
	case '?':
	case '+':
	case '*': {
		NFANode * newStart = newNFANode();
		NFANode * newEnd = newNFANode();
		newStart->input1 = '\0';
		newStart->next1 = start;
		end->input1 = '\0';
		end->next1 = newEnd;
		if (op != '?') {
			end->input2 = '\0';
			end->next2 = start;
		}
		if (op != '+') {
			newStart->input2 = '\0';
			newStart->next2 = newEnd;
		}
		node_stack.pop();
		node_stack.push({ newStart,newEnd });
		break;
	}
			  // case '(': pass
			  /*case '(': {
			  //if (node_stack.size() <= 1) break;
			  node_stack.pop();
			  auto pair2 = node_stack.top();
			  NFANode * start2 = pair2.first, *end2 = pair2.second;
			  end2->input1 = start->input1;
			  end2->next1 = start->next1;
			  end2->input2 = start->input2;
			  end2->next2 = start->next2;
			  node_stack.pop();
			  node_stack.push({ start2, end });
			  break;
			  }*/
	default:
		fprintf(stderr, "error op");
		break;
	}
}

pair<NFANode*, NFANode*> newRegInput(char * str) {
	while (!(op_stack.empty())) op_stack.pop();
	while (!(node_stack.empty()))node_stack.pop();
	int ptr = 0;
	char cur = str[ptr];

	NFANode * initNode = newNFANode();
	node_stack.push({ initNode, initNode });

	while (cur != '\0') {
		if (cur != '\\') {
			switch (cur) {
			case '(':
			case '|': {
				if (cur == '|') {
					// deal ops (connect & or) before 
					// which makes '\0' prior to '|'
					char op = op_stack.top();
					while (op != '(') {
						NFA_pop_node_by_op(op);
						op_stack.pop();
						if (op_stack.empty()) break;
						else op = op_stack.top();
					}
				}
				op_stack.push(cur);
				NFANode * newInit = newNFANode();
				node_stack.push({ newInit, newInit });
				break;
			}
			case ')':
			case '\0': {
				char op = op_stack.top();
				while (op != '(') {
					NFA_pop_node_by_op(op);
					op_stack.pop();
					if (op_stack.empty()) break;
					else op = op_stack.top();
				}
				if (op == '(') {
					//NFA_pop_node_by_op(op);
					op_stack.pop();
					op_stack.push('\0');
				}
				break;
			}
			case '+':
			case '?':
			case '*': {
				op_stack.push(cur);
				NFA_pop_node_by_op(cur);
				op_stack.pop();
				break;
			}
			default: {
				op_stack.push('\0');
				NFANode * newStart = newNFANode();
				NFANode * newEnd = newNFANode();
				newStart->input1 = cur;
				newStart->next1 = newEnd;
				node_stack.push({ newStart, newEnd });
				break;
			}
			}
		}
		else {
			++ptr;
			cur = str[ptr];
			if (cur != '\0') {     // the same as default
				op_stack.push('\0');
				NFANode * newStart = newNFANode();
				NFANode * newEnd = newNFANode();
				newStart->input1 = cur;
				newStart->next1 = newEnd;
				node_stack.push({ newStart, newEnd });
			}
		}

		++ptr;
		cur = str[ptr];
	}

	while (!(op_stack.empty())) {
		char op = op_stack.top();
		NFA_pop_node_by_op(op);
		op_stack.pop();
	}

	auto pair_ = node_stack.top();
	NFANode * start = pair_.first, *end = pair_.second;

	return{ start, end };
}

void parseReg(char * reg, NFANode * startNode, int flag) {
	auto pair_ = newRegInput(reg);
	NFANode * nextStart = pair_.first;
	NFANode * pastStart = newNFANode();
	pastStart->input1 = startNode->input1;
	pastStart->next1 = startNode->next1;
	pastStart->input2 = startNode->input2;
	pastStart->next2 = startNode->next2;
	startNode->input1 = startNode->input2 = '\0';
	startNode->next1 = pastStart;
	startNode->next2 = nextStart;
	FA_flag[pair_.second->id] = flag;
}

void parseFirstReg(char * reg, NFANode * & startNode, int flag) {
	auto pair_ = newRegInput(reg);
	startNode = pair_.first;
	FA_flag[pair_.second->id] = flag;
}

set<int> state_set[65536];
map<char, set<int> > state_table[65536];

void extend_closure(set<int> &set) {
	queue<int> que;
	for (int id : set) {
		que.push(id);
	}
	while (!que.empty()) {
		int id = que.front();
		que.pop();
		NFANode &p = NFANodes[id];
		if (p.input1 != '\0') continue;
		if (set.find(p.next1->id) == set.end()) {
			set.insert(p.next1->id);
			que.push(p.next1->id);
		}
		if (p.input2 != '\0') continue;
		if (set.find(p.next2->id) == set.end()) {
			set.insert(p.next2->id);
			que.push(p.next2->id);
		}
	}
}


void init_state_set(NFANode * startNode) {
	state_set[0].insert(startNode->id);
	extend_closure(state_set[0]);
}

int NFA_to_DFA_table(NFANode * startNode) {
	init_state_set(startNode);
	int last_state_idx = 0;
	for (int i = 0; i <= last_state_idx; ++i) {
		auto & srcState = state_set[i];
		auto & tgtTable = state_table[i];
		for (char c : allowed_chars) {
			auto & table = tgtTable[c];
			for (int id : srcState) {
				auto & node = NFANodes[id];
				if (node.input1 == c) {
					table.insert(node.next1->id);
				}
				if (node.input2 == c) {
					table.insert(node.next2->id);
				}
			}
			extend_closure(table);
			if (!table.empty()) {
				bool newOne = true;
				for (int j = 0; j <= last_state_idx; ++j) {
					if (table == state_set[j]) {
						newOne = false;
						break;
					}
				}
				if (newOne) {
					state_set[++last_state_idx] = table;
				}
			}
		}

	}
	return last_state_idx + 1;
}


void constructDFA(int stateCnt) {
	for (int i = 0; i < stateCnt; ++i) {
		DFANodes[i].id = i;
		DFANodes[i].flag = 0;
		for (int j : state_set[i]) {
			if (FA_flag[j] != 0) {
				if (DFANodes[i].flag == 0 || FA_flag[j] < DFANodes[i].flag)	DFANodes[i].flag = FA_flag[j];
			}
		}
		map<char, set<int> > thisTable = state_table[i];
		for (char c : allowed_chars) {
			if (thisTable[c].empty()) continue;
			auto & tgtState = thisTable[c];
			for (int j = 0; j < stateCnt; ++j) {
				if (tgtState == state_set[j]) {
					DFANodes[i].ptrs[c] = &DFANodes[j];
					break;
				}
			}
		}
	}
}

int NFAtoDFA(NFANode * startNode) {
	init_state_set(startNode);
	int states = NFA_to_DFA_table(startNode);
	constructDFA(states);
	return states;
}