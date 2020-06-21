#define _CRT_SECURE_NO_WARNINGS

#include "yaccUtil.h"

#include "yaccParser.h"

#include "LRTable.h"

#include "yyWrite.h"

#include "yaccSource.h"

using namespace std;

int main() {

	YaccParser yyy("littlec.y");

	dumpProducers(printf);

	//printf("%s\n", YyWrite::make_init().c_str());

	yyy.writeTabs("y.tab.h");

	LRDFA dfa;
	dfa.generateLALR();

	LRTable lrt(dfa);

	//lrt.dump(printf);
	//printf("%s", lrt.code_dump("state", "c").c_str());

	freopen("y.tab.c", "w", stdout);

	printf("%s\n", yacc_program.c_str());
	printf("void init_producers() { %s }\n", YyWrite::make_init().c_str());
	printf("int GoTo(int state, int c) { %s }\n", lrt.code_dump("state", "c").c_str());

	 
	return 0;
}