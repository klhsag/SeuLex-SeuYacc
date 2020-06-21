#pragma once

#include<iostream>
#include<fstream>
#include<cstdlib>
#include<cstdio>
#include<string>
#include<map>
#include<vector>

#include "yyProducer.h"

using namespace std;

struct YaccParser {
	vector<string> terminal;
	string start;
	using Producer = pair < string, vector<string> >;
	vector< Producer > producer_list;
	std::string program2;

	void define_rules(std::string ln) {
		std::string left = "", right = "";
		int len = ln.length();
		int i = 0;
		bool flag = 1;
		if (ln[i] == '%'&& ln[i + 1] != '%'){
			++i;
			while (i < len && ln[i] == ' ') ++i;// || ln[i] == '\t')
			while (i < len && !(ln[i] == ' ')) {
				left += ln[i];
				++i;
			}

			while (i < len && ln[i] == ' ')	i++;

			if (left == "token") {
				for (; i < len; i++){
					if (ln[i] != ' ') {
						right += ln[i];
						if (i == len - 1)
							terminal.push_back(right);
					}
					else {
						terminal.push_back(right);
						right = "";
					}
				}
			}else if (left=="start"){
				start = "";
				while (i < len) start.push_back(ln[i++]);
			}else {
				throw "";
			}

		}
	}

	void readProducerRight(std::istream & ifile, std::string left) {
		std::string str;
		do {
			ifile >> str;
			vector<std::string> curRight;
			while (str != "|" && str != ";") {
				curRight.push_back(str);
				ifile >> str;
			}
			producer_list.push_back({ left, curRight });
		} while (str != ";");
	}

	void producerParsing(std::istream & ifile) {
		std::string left;
		do { ifile >> left; } while (left == "");
		while (left != "%%") {
			std::string str;
			ifile >> str;
			if (str != ":" && str != "|") throw "";
			readProducerRight(ifile, left);
			do { ifile >> left; } while (left == "");
		}
	}


	void readFromStream(std::istream & ifile)
	{
		using namespace std;
		string ln;
		int step = 0;
		bool inDef = false;
		while (getline(ifile, ln)) {
			if (ln == "%%") {
				++step;
				if (step == 1) {
					producerParsing(ifile);
					++step;
				}
				continue;
			}
			if (ln == "") continue;

			switch (step) {
			case 0: {
				define_rules(ln);
				break;
			}
			case 2:
				program2 += ln + '\n';
				break;
			default:
				throw "";
			}
		}

	}


	static bool strlist_contain(vector<string> str, string stt) {
		for (int i = 0; i < str.size(); i++) {
			if (str[i] == stt) return true;
		}
		return false;
	}

	void last_deal() {
		vector<string> allTer;
		vector<string> allNotTer;
		vector<int> terIds;
		vector<int> gIds;
		int terId = 300;
		int gId = 999;
		for (int i = 0; i < terminal.size(); ++i) {
			allTer.push_back(terminal[i]);
			terIds.push_back(terId++);
		}
		allNotTer.push_back("__PROGRAM__");
		gIds.push_back(gId++);
		allNotTer.push_back(start);
		gIds.push_back(gId++);
		for (auto iter = producer_list.begin(); iter != producer_list.end(); iter++) {
			for (auto k : iter->second) {
				if (k[0] == '\'') {
					if (strlist_contain(allTer, k)) continue;
					else {
						allTer.push_back(k);
						terIds.push_back(k[1]);
					}
				}
				else {
					if (strlist_contain(allNotTer, k) || strlist_contain(allTer, k)) continue;
					else {
						allNotTer.push_back(k);
						gIds.push_back(gId++);
					}
				}
			}
		}

		for (int i = 0; i < terIds.size(); ++i) {
			appendNewSymbol(terIds[i], allTer[i], true);
		}
		for (int i = 0; i < gIds.size(); ++i) {
			appendNewSymbol(gIds[i], allNotTer[i], false);
		}
		producers.push_back({ idnameToIdx("__PROGRAM__"), {idnameToIdx(start)} });
		for (auto & pro : producer_list) {
			int left = idnameToIdx(pro.first);
			vector<int> v;
			for (auto & str : pro.second) {
				v.push_back(idnameToIdx(str));
			}
			producers.push_back({ left, v });
		}

	}

	void initAll(std::string filename) {
		std::ifstream ifile;
		ifile.open(filename, std::ios::in);
		readFromStream(ifile);
		ifile.close();
		last_deal();
	}

	YaccParser() = default;
	YaccParser(std::string filename) {
		initAll(filename);
	}

	void writeTabs(std::string filename) {
		FILE * tabFile = fopen(filename.c_str(), "w");
		writeTab(tabFile, fprintf);
		fclose(tabFile);
	}

};