#define _CRT_SECURE_NO_WARNINGS

#include <cstdio>
#include <string>

using namespace std;

namespace lexSource {

string program1;
string program2;
string auto_program;

string prepared_program1 = R""(
#define ECHO echo_str(yytext)
#define error(x) printf("Line<%d>, Col<%d> : Lexical analysis fail: %s.\n", word_line, word_column+1, x)

#include <string.h>

FILE * yyin;
FILE * yyout;

char yytext[65536];

int yyleng;

int yylineno;
int column;

int yylex();

int yywrap();

char allText[3000000];
int textPtr = 0;

char input() {
	char ch = allText[textPtr];
	++textPtr;
	if (ch == '\n') ++yylineno;
	return ch;
}
void unput() {
	if (textPtr <= 0) return;
	if (allText[textPtr - 1] == '\n') {
		--yylineno;
	}
	--textPtr;
}
void echo_str(char * yytext);

int word_line;
int word_column;
)"";

string prepared_program2 = R""(

int DFAState;

int DFAPush(char c);

int DFAExec(void);

int DFATry(void);

int yylex() {
	word_line = yylineno;
	word_column = column;

	yyleng = 1;
	yytext[0] = input();

	if (yytext[0] == 0) {
		if (yywrap()) return -1;
	}

	DFAState = 0;
	int lastTerState = 0;
	int toUnput = 0;
	int state = DFAPush(yytext[0]);
	while (state == 0) {
		if (DFATry()) {
			lastTerState = DFAState;
			toUnput = 0;
		}
		else {
			++toUnput;
		}
		++yyleng;
		yytext[yyleng - 1] = input();
		if (yytext[yyleng - 1] == 0) break;
		state = DFAPush(yytext[yyleng - 1]);
	}
	++toUnput;
	for (int i = 0; i < toUnput; ++i) {
		unput();
		--yyleng;
	}
	if (yyleng == 0) {
		yyleng = 1;
		yytext[0] = input();
	}
	yytext[yyleng] = '\0';

	int label = DFAExec();

	return label;
}

char out_str[65536];

void echo_str(char * yytext) {
	sprintf(out_str, " < %d , %d > %s\x06\n", word_line, word_column, yytext);
}
)"";


string executer = R""(
char inputFilename[65536];
char outputFilename[65536];
int main(int argc, char * argv[]) {
	yyin = stdin;
	yyout = stdout;

	char inputFilename[] = "test.c";
	char outputFilename[] = "test.lo";

	if (argc == 3) {
		strcpy(inputFilename, argv[1]);
		strcpy(outputFilename, argv[2]);
	}

	FILE * outFile = NULL;
	if (freopen(inputFilename, "r", stdin) == NULL ||
		(outFile = fopen(outputFilename, "w")) == NULL) {
		printf("fail\n");
		return 1;
	};

	memset(allText, 0, sizeof allText);

	char c = 0;
	int p = 0;
	while (scanf("%c", &c) > 0) {
		allText[p++] = c;
	}

	yylineno = 1;
	column = 0;
	while (1) {
		out_str[0] = '\0';
		int x = yylex();
		if (x == -1) break;
		if (x!=0){
			char echoed_str[65536] = "";
			sprintf(echoed_str, "%d", x);
			strcat(echoed_str, out_str);
			fprintf(outFile, "%s", echoed_str);
		}
	}

	return 0;
}
)"";

}

void yieldLexYyC(std::string filename, bool hasExec = true) {
	using namespace lexSource;
	FILE * f = fopen(filename.c_str(), "w");
	string out = program1 + prepared_program1 + program2 + prepared_program2 + auto_program;
	if (hasExec) out += executer;
	fprintf(f, "#define _CRT_SECURE_NO_WARNINGS\n%s", out.c_str());
}