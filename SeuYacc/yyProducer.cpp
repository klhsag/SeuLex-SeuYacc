
#include <cassert>

#include <vector>
#include <string>
#include <map>
#include <set>


using Producer = std::pair< int, std::vector<int> >;
std::vector<Producer> producers;/* = {
{ 'S',{ 'A' } },
{ 'A',{ 'C', 'C' } },
{ 'C',{ 'c', 'C' } },
{ 'C',{ 'd', 'D' } },
{ 'D',{ 'e' } },
{ 'D',{} }
};
*/

std::map<std::string, int> map_idname_idx;
std::map<int, std::string> map_idx_idname;

std::set<int> terminators;

int idnameToIdx(std::string name) {
	assert(map_idname_idx.find(name)!= map_idname_idx.end());
	return map_idname_idx[name];
}
std::string idxToIdname(int idx) {
	assert(map_idx_idname.find(idx) != map_idx_idname.end());
	return map_idx_idname[idx];
}

void appendNewSymbol(int idx, std::string name, bool isTerminator) {
	map_idname_idx.insert({ name, idx });
	map_idx_idname.insert({ idx, name });
	if (isTerminator) {
		terminators.insert(idx);
	}
}

bool isTerminator(int idx) {
	return terminators.count(idx);
}
bool isTerminator(std::string str) {
	int idx = idnameToIdx(str);
	return isTerminator(idx);
}

void dumpProducers(int(*print_)(const char *, ...)) {
	using namespace std;

	for (auto & producer: producers){
		string left = idxToIdname(producer.first);
		print_("%s : => ", left.c_str());
		for (int item : producer.second) {
			print_(" %s ", idxToIdname(item).c_str());
		}
		print_("\n");
	}
}

void writeTab(FILE * file, int(*print_)(FILE *, const char *, ...)) {
	print_(file, "#define WHITESPACE 0  \n /*sp */ \n\n");
	for (auto & p_ : map_idname_idx) {
		if (!isTerminator(p_.second)) continue;
		if ((p_.first[0] <'a' || p_.first[0]>'z') &&
			(p_.first[0] <'A' || p_.first[0]>'Z')) continue;
		print_(file, "#define %s %s\n", p_.first.c_str(), std::to_string(p_.second).c_str());
	}
}