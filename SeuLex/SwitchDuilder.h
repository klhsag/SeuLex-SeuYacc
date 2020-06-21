#pragma once

#include <string>

#include <vector>

#include <set>

struct SwitchBuilder {
	
	static std::string stdBuild(std::string var_name, std::map<std::string, std::set<int> > case_list, std::string defaultCase = "error(\"\");") {
		using std::string;
		using std::to_string;

		string begin = "switch(" + var_name + ") { ";
		string end = "default: {" + defaultCase + "} }";

		string middle = "";

		for (auto & p_ : case_list) {
			string pbegin = "";
			for (int caseIdx : p_.second) {
				pbegin += "case " + to_string(caseIdx) + " : ";
			}
			pbegin += " { ";
			string pend = " break; }";
			middle += pbegin + p_.first + pend;
		}

		return begin + middle + end;
	}

};