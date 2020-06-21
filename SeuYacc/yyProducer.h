#pragma once

#include <string>
#include <vector>
#include <map>

extern std::map<int, std::string> map_idx_idname;

int idnameToIdx(std::string producerItemName);
std::string idxToIdname(int producerItemIdx);

void appendNewSymbol(int idx, std::string name, bool isTerminator);
bool isTerminator(int);
bool isTerminator(std::string);

using Producer = std::pair< int, std::vector<int> >;

extern std::vector<Producer> producers;

void dumpProducers(int(*print_)(const char *, ...));

void writeTab(FILE * file, int(*print_)(FILE *, const char *, ...));
