
#include <vector>
#include <set>
#include <map>

struct DFANode {
	int id;
	int flag = 0;
	std::map<char, DFANode*> ptrs;
} DFANodes[65536], minDFA[65536];

struct DFAMinimizerClass{
	std::vector< std::set<int> > DFASets;
	int DFANodeIdx[65536];
	
	void init(int stateCnt) {
		std::map<int, std::set<int> > initialSets;
		for (int i = 0; i < stateCnt; ++i) {
			int dfatype = DFANodes[i].flag;
			initialSets[dfatype].insert(i);
		}
		for (auto & p_ : initialSets) {
			DFASets.push_back(p_.second);
			for (int idx: p_.second){
				DFANodeIdx[idx] = DFASets.size() - 1;
			}
		}
	}

	void deal() {
		int last_size;
		int z = 0;
		do {
			last_size = DFASets.size();
			for (int i = 0; i < DFASets.size(); ++i) {
				auto & curSet = DFASets[i];
				std::set<int> splitedSet;
				int stde = *(curSet.begin());
				auto & stdtable = DFANodes[stde].ptrs;
				for (int e : curSet) {
					auto & table = DFANodes[e].ptrs;
					bool notSame = false;
					if (stdtable.size() == table.size()) {
						for (auto p_ : stdtable) {
							auto it = table.find(p_.first);
							if (it == table.end()) {
								notSame = true;
								break;
							}
							else {
								if (DFANodeIdx[p_.second->id] != DFANodeIdx[it->second->id]) {
									notSame = true;
									break;
								}
							}
						}
					}else {
						notSame = true;
					}
					if (notSame) splitedSet.insert(e);
				}
				if (!splitedSet.empty()) {

					//printf(" %d size\n", splitedSet.size());
					DFASets.push_back(splitedSet);                   // curSet no use again
					int newIdx = DFASets.size() - 1;
					for (int e : splitedSet) {
						DFASets[i].erase(e);
						DFANodeIdx[e] = newIdx;
					}
				}

			}

		} while (DFASets.size() != last_size);

	}

	void show() {
		for (int i = 0; i < DFASets.size(); ++i) {
			printf("%3d : ", i);
			for (int j : DFASets[i]) {
				printf("%5d", j);
			}
			printf("\n");
		}
	}

	int generate() {
		for (int i = 0; i < DFASets.size(); ++i) {
			minDFA[i].id = i;
			auto e = *(DFASets[i].begin());
			minDFA[i].flag = DFANodes[e].flag;
			for (auto & p_ : DFANodes[e].ptrs) {
				char c = p_.first;
				int idx = DFANodeIdx[p_.second->id];
				minDFA[i].ptrs[c] = &minDFA[idx];
			}
		}
		return DFASets.size();
	}

} DFAMinimizer;