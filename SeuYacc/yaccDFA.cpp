
#include <set>
#include <map>

#include "yyProducer.h"

using namespace std;

map<int, set<int> > firsts;


/*
bool isTerminator(char c) {
	return (c<'A' || c>'Z');
}*/


void initFirsts() {
	for (auto & p_ : producers) {
		for (int s : p_.second) {
			if (isTerminator(s)) firsts[s].insert(s);
		}
	}
	bool flag = true;
	while (flag) {
		flag = false;
		for (auto & p_ : producers) {
			int s = p_.first;
			if (isTerminator(s)) continue;
			bool hasEpsilon = true;
			for (int item : p_.second) {
				auto & thisSet = firsts[item];
				for (int c : thisSet) {
					if (c == 0) continue;
					if (firsts[s].count(c) == 0) {
						flag = true;
						firsts[s].insert(c);
					}
				}
				if (thisSet.count(0) == 0) {
					hasEpsilon = false;
					break;
				}
			}
			if (hasEpsilon && firsts[s].count(0) == 0) {
				flag = true;
				firsts[s].insert(0);
			}
		}
	}
}
