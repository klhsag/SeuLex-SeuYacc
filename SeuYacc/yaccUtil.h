#pragma once

#include <vector>
#include <queue>
#include <set>
#include <map>

#include "yyProducer.h"

extern std::map<int, std::set<int> > firsts;
void initFirsts();

struct LRItem {
	int position = 0;       //点的位置
	int gramarIdx = -1;     //产生式标号
	int predictiveSymbol;   //预测符
	bool operator < (const LRItem & t) const {
		if (gramarIdx == t.gramarIdx) {
			if (position == t.position) return predictiveSymbol < t.predictiveSymbol;
			return position < t.position;
		}
		return gramarIdx < t.gramarIdx;
	}
};

struct LRState {
	int idx = -1;                         //状态号
	std::map<int, int> edgesMap;         //<发出边上符号，状态号>
	std::set<LRItem> LRItemsSet;
	void extend() {
		using namespace std;

		auto & s = *this;
		queue<LRItem> que;
		for (auto & item : s.LRItemsSet) {
			que.push(item);
		}
		while (!que.empty()) {
			auto item = que.front();
			que.pop();
			auto & producer = producers[item.gramarIdx];
			int itemLen = producer.second.size();
			if (item.position >= itemLen) continue;  // or ==
			if (isTerminator(producer.second[item.position])) continue;
			int newLeft = producer.second[item.position];
			set<int> nexts;
			bool flag = true;
			for (int j = item.position + 1; j < itemLen; ++j) {
				bool noEpsilon = true;
				auto & fi = firsts[producer.second[j]];
				for (int c : fi) {
					if (c == 0) noEpsilon = false;
					else {
						nexts.insert(c);
					}
				}
				if (noEpsilon) {
					flag = false;
					break;
				}
			}
			if (flag) {
				nexts.insert(item.predictiveSymbol);
			}
			for (int i = 0; i < producers.size(); ++i) {
				if (producers[i].first != newLeft) continue;
				for (auto c : nexts) {
					LRItem newItem;
					newItem.gramarIdx = i;
					newItem.position = 0;
					newItem.predictiveSymbol = c;
					if (s.LRItemsSet.count(newItem) == 0) {
						s.LRItemsSet.insert(newItem);
						que.push(newItem);
					}
				}
			}

		}
	}

};

struct LRDFA {
	int startState = 0;
	std::vector< LRState > statesVec;     //store all LRState

	LRDFA() {
		using namespace std;
		auto & dfa = *this;

		LRItem item0;
		item0.gramarIdx = 0;
		item0.predictiveSymbol = '$';
		LRState state0;
		state0.LRItemsSet.insert(item0);

		initFirsts();

		state0.extend();

		state0.idx = 0;

		dfa.startState = 0;
		dfa.statesVec.push_back(state0);

		queue<int> que;
		que.push(0);
		while (!que.empty()) {
			int idx = que.front();
			que.pop();
			auto & start = dfa.statesVec[idx];
			map<int, LRState> newStates;
			for (const LRItem & item : start.LRItemsSet) {
				auto & producer = producers[item.gramarIdx];
				if (item.position >= producer.second.size()) continue;
				int c = producer.second[item.position];
				LRItem newItem = item;
				++newItem.position;
				newStates[c].LRItemsSet.insert(newItem);
			}
			for (auto & p_ : newStates) {
				p_.second.extend();
			}
			for (auto & p_ : newStates) {
				auto & start = dfa.statesVec[idx];                  // push changes the '&'. Must reget.
				bool flag = true;
				for (auto & other : dfa.statesVec) {
					if (p_.second.LRItemsSet.size() != other.LRItemsSet.size()) continue;
					bool same = true;
					for (auto & item : p_.second.LRItemsSet) {
						if (other.LRItemsSet.count(item) == 0) {
							same = false;
							break;
						}
					}
					if (same) {
						start.edgesMap[p_.first] = other.idx;
						flag = false;
						break;
					}
				}
				if (flag) {
					int c = p_.first;
					int idx = dfa.statesVec.size();
					start.edgesMap[c] = idx;
					p_.second.idx = idx;
					dfa.statesVec.push_back(p_.second);
					que.push(idx);
				}
			}

		}
	}

	void generateLALR() {
		int n = statesVec.size();
		int stateBelongs[65536];
		for (int i = 0; i < n; ++i) stateBelongs[i] = -1;
		for (int i = 0; i < n; ++i) {
			if (stateBelongs[i] != -1) continue;
			stateBelongs[i] = i;
			for (int j = i + 1; j < n; ++j) {
				if (stateBelongs[j] != -1) continue;
				bool isSame = true;
				for (auto & item1 : statesVec[i].LRItemsSet) {
					bool flag = false;
					for (auto & item2 : statesVec[j].LRItemsSet) {
						if (item1.gramarIdx == item2.gramarIdx && item1.position == item2.position) {
							flag = true;
							break;
						}
					}
					if (!flag) {
						isSame = false;
						break;
					}
				}
				for (auto & item1 : statesVec[j].LRItemsSet) {
					bool flag = false;
					for (auto & item2 : statesVec[i].LRItemsSet) {
						if (item1.gramarIdx == item2.gramarIdx && item1.position == item2.position) {
							flag = true;
							break;
						}
					}
					if (!flag) {
						isSame = false;
						break;
					}
				}
				if (isSame) {
					stateBelongs[j] = i;
				}
			}
		}
		int cnt = 0;
		int stateCnts[65536];
		for (int i = 0; i < n; ++i) {
			if (stateBelongs[i] == i) {
				stateCnts[i] = cnt;
				++cnt;
			}
		}
		//printf("%d\n!\n", cnt);

		for (int i = 0; i < n; ++i) {
			int t = stateBelongs[i];
			for (auto & item : statesVec[i].LRItemsSet) {
				statesVec[t].LRItemsSet.insert(item);
			}
			/*for (auto & p_ : statesVec[i].edgesMap) {
				p_.second = stateBelongs[p_.second];
			}*/
		}

		std::vector< LRState > newVec;
		for (int i = 0; i < n; ++i) {
			if (i != stateBelongs[i]) continue;
			LRState newState = statesVec[i];
			for (auto & p_ : newState.edgesMap) {
				//p_.second = stateCnts[p_.second];
				p_.second = stateCnts[stateBelongs[p_.second]];
			}
			newVec.push_back(std::move(newState));
		}

		statesVec = std::move(newVec);
	}


};
