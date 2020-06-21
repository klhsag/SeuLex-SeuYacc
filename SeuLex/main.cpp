#define _CRT_SECURE_NO_WARNINGS 

#include <cstdio>
#include <cstring>

#include "yylParser.h"

#include "RegFilter.h"

#include "RegToDFA.h"

#include "SwitchDuilder.h"

#include "lexSource.h"

using namespace std;

RegToDFA ctrl;

int main(){
	
	YylParser yyl("littlec.l");


	std::map<std::string, std::string> ruleMap = yyl.define_rules;
	for (auto & p_ : ruleMap) {
		p_.second = RegFilter::spcharFilter(p_.second);
	}

	ctrl.init();

	auto regs = yyl.regex_rules;
	for (auto & p_ : regs) {
		if (*p_.first.begin() == '\"' && *p_.first.rbegin() == '\"') {
			p_.first = RegFilter::quoteFilter(p_.first);
		} else {
			p_.first = RegFilter::totalFilter(p_.first, ruleMap);
		}
	}

	for (auto & p_ : regs) {
		ctrl.appendRegex(p_.first, p_.second);
	}
	
	//system("pause");

	ctrl.generate();

	//ctrl.show();

	//system("pause");

	int cnt = ctrl.nodes;
	std::map<std::string, std::set<int > > case_list;
	for (int i = 0; i < cnt; ++i) {
		int f = minDFA[i].flag;
		if (f != 0) {
			string s = ctrl.flag_map[f];
			case_list[s].insert(i);
		}
	}
	string DFAExecuter = SwitchBuilder::stdBuild("DFAState", case_list, "error(\"unexpected word\");") + " return WHITESPACE;";    // whitespace : 0
	string Func_DFAExec = "int DFAExec(void){ " + DFAExecuter + "} ";

	//system("pause");
	case_list.clear();
	for (int i = 0; i < cnt; ++i) {
		int f = minDFA[i].flag;
		if (f != 0) {
			case_list["{return 1;}"].insert(i);
		}
	}
	string Func_DFATry = "int DFATry(void){ " + SwitchBuilder::stdBuild("DFAState", case_list, "return 0;") + "} ";

	case_list.clear();
	for (int i = 0; i < cnt; ++i) {
		auto & table = minDFA[i].ptrs;
		std::map<std::string, std::set<int > > local_list;
		for (auto p_ : table) {
			int next = p_.second->id;
			string order = "{ DFAState = " + to_string(next) + "; }";
			local_list[order].insert(p_.first);
		}
		string content = "{ " + SwitchBuilder::stdBuild("c", local_list, "{return 1;}") + " }";
		case_list[content].insert(i);
	}

	string DFA_pusher = SwitchBuilder::stdBuild("DFAState", case_list, "return 0;") +" return 0; ";
	string Func_DFAPush = "int DFAPush(char c){ " + DFA_pusher + "} ";

	//printf("%s\n%s\n", Func_DFAExec.c_str(), Func_DFAPush.c_str());

	//system("pause");

	lexSource::program1 = yyl.program1;
	lexSource::program2 = yyl.program2;
	lexSource::auto_program = Func_DFAExec + "\n" + Func_DFATry + "\n" + Func_DFAPush + "\n";
	
	yieldLexYyC("lex.yy.c");
	
    return 0;
}

