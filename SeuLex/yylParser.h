#pragma once

#include<iostream>
#include<fstream>
#include<cstdlib>
#include<cstdio>
#include<string>
#include<map>
#include<vector>

struct YylParser{
	std::string program1;
	std::map<std::string, std::string> define_rules;
	std::vector<std::pair<std::string, std::string> > regex_rules;
	std::string program2;

	std::pair<std::string, std::string> lnToPair(std::string ln) {
		std::string left = "", right = "";
		int len = ln.length();
		int i = 0;
		while (i<len && (ln[i] == ' ' || ln[i] == '\t')) ++i;
		while (i<len && !(ln[i] == ' ' || ln[i] == '\t')) {
			left += ln[i];
			++i;
		}
		while (i<len && (ln[i] == ' ' || ln[i] == '\t')) ++i;
		while (i < len) right += ln[i++];
		return{ left, right };
	}

	void readFromStream(std::istream & ifile) {
		using namespace std;

		string ln;                // ln <==> line
		int step = 0;
		bool inDef = false;
		while (getline(ifile, ln)) {
			//cout << ln << endl;
			if (inDef) {
				if (ln == "%}") inDef = false;
				else {
					program1 += ln + "\n";
				}
				continue;
			}
			if (ln == "%{") {
				inDef = true;
				continue;
			}
			if (ln == "%%") {
				++step;
				continue;
			}
			if (ln == "") continue;
			switch (step) {
			case 0: {
				auto p_ = lnToPair(ln);
				define_rules[p_.first] = p_.second;
				break;
			}
			case 1: {
				auto p_ = lnToPair(ln);
				regex_rules.push_back(p_);
				break;
			}
			case 2:
				program2 += ln;
				break;
			default:
				throw "";
			}
		}

	}

	void initAll(std::string filename) {
		std::ifstream ifile;
		ifile.open(filename, std::ios::in);
		readFromStream(ifile);
		ifile.close();
	}

	YylParser() = default;
	YylParser(std::string filename) {
		initAll(filename);
	}

};
