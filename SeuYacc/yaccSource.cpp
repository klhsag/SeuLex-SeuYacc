#include <string>

std::string yacc_program = R"(
#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char item_name_table[2000][300];

int producers[2000][100];

void init_producers();

int GoTo(int state, int c);

int yylineno, column;

void yyerror(char const *s);

typedef struct yyword {
	char * text;
	int line;
	int column;
	int id;
} yyword;

yyword * newYyword(char * text, int line, int column, int id) {
	yyword * word = (yyword *)malloc(sizeof(yyword));
	int len = strlen(text);
	word->text = (char *)malloc(len * (sizeof(char) + 2));
	strcpy(word->text, text);
	word->line = line;
	word->column = column;
	word->id = id;
	return word;
}

yyword * nextYyword(int(*scan_)(const char *, ...)) {
	static char text[65536];
	static int ln;
	static int col;
	static int id;
	if (scan_("%d < %d , %d > %[^\x06]\x06\n", &id, &ln, &col, text) < 4) {
		return NULL;
	}
	return newYyword(text, ln, col, id);
}

typedef struct yyGNode yyGNode;
typedef struct yyGNode {
	int isTer;
	int idx;
	int line;
	int col;
	int childCnt;
	yyGNode ** childs;
	char * caption;
	char * content;
} yyGNode;


yyGNode * newYyGNode(int isTer, int idx, int line, int col, int childCnt, yyGNode ** childs, char * caption, char * content) {
	yyGNode * node = (yyGNode *)malloc(sizeof(yyGNode));
	node->isTer = isTer;
	node->idx = idx;
	node->line = line;
	node->col = col;
	node->childCnt = childCnt;
	node->childs = (yyGNode **)malloc(childCnt * sizeof(yyGNode*));
	for (int i = 0; i < childCnt; ++i) {
		node->childs[i] = childs[i];
	}
	int len = strlen(caption);
	node->caption = (char *)malloc(len * (sizeof(char) + 2));
	strcpy(node->caption, caption);
	len = strlen(content);
	node->content = (char *)malloc(len * (sizeof(char) + 2));
	strcpy(node->content, content);
	return node;
}

yyGNode * gNodeStack[65536];
int gNodeStackPtr = 0;

yyGNode * gNodeTop() {
	if (gNodeStackPtr == 0) return NULL;
	return gNodeStack[gNodeStackPtr - 1];
}

void gNodePush(int isTer, int idx, char * caption, char * content, int childCnt, int line, int col) {
	static yyGNode * emptyNodes[1024] = { NULL };
	yyGNode * newNode = newYyGNode(isTer, idx, line, col, childCnt, emptyNodes, caption, content);
	gNodeStack[gNodeStackPtr] = newNode;
	++gNodeStackPtr;
}

void gNodeGenerate(int producerId) {
	static char content[65536];
	static yyGNode * childNodes[1024];
	int len = producers[producerId][0];
	int left = producers[producerId][1];
	int line;
	int col;
	for (int i = len - 1; i >= 0; --i) {
		childNodes[i] = gNodeTop();
		line = gNodeTop()->line;
		col = gNodeTop()->col;
		--gNodeStackPtr;
	}
	content[0] = '\0';
	for (int i = 0; i < len; ++i) {
		strcat(content, childNodes[i]->content);
	}
	gNodePush(0, producerId, item_name_table[left], content, len, line, col);
	for (int i = len - 1; i >= 0; --i) {
		gNodeTop()->childs[i] = childNodes[i];
	}
}

void gNodeDump(yyGNode * root, int tabs) {
	int cnt = root->childCnt;
	for (int i = 0; i < tabs; ++i) printf(" ");
	if (root->isTer) {
		printf("%s <%d, %d> : %s\n", root->caption, root->line, root->col, root->content);
	}
	else {
		int idx = root->idx;
		int len = producers[idx][0];
		printf("%s : =>", item_name_table[producers[idx][1]]);
		for (int i = 2; i < len + 2; ++i) {
			printf(" %s ", item_name_table[producers[idx][i]]);
		}
		printf(" <%d, %d> : %s\n", root->line, root->col, root->content);
	}
	for (int i = 0; i < cnt; ++i) {
		gNodeDump(root->childs[i], tabs + 1);
	}
}


char item_name_table[2000][300];

int producers[2000][100];

char * itemName(int idx) {
	return item_name_table[idx];
}

int producerLen(int idx) {
	return producers[idx][0];
}

int producerLeft(int idx) {
	return producers[idx][1];
}

int state_stack[3000000];
int state_stack_ptr = 0;

void state_stack_push(int x) {
	state_stack[state_stack_ptr] = x;
	++state_stack_ptr;
}

int state_stack_top() {
	return state_stack[state_stack_ptr - 1];
}

void state_stack_pop() {
	--state_stack_ptr;
}

void gramarParse(yyword * (*nextItem)()) {
	static char curItemName[1024];
	state_stack_push(0);
	yyword * w = nextItem();
	int c = w->id;
	while (state_stack_ptr>0) {
		int state = state_stack_top();
		int goTo = GoTo(state, c);
		if (goTo == 65536) {
			yyerror("grammar parsing error!");
			do {
				if (strcmp(w->text, "$")==0) goto BAD_STATE;
				w = nextItem();
				c = w->id;
				goTo = GoTo(state, c);
			} while (goTo < 0 || goTo == 65536);
		}
		if (goTo >= 0) {
			state_stack_push(goTo);
			strcpy(curItemName, w->text);
			strcat(curItemName, " ");
			gNodePush(1, w->id, itemName(w->id), curItemName, 0, w->line, w->column);
			w = nextItem();
			c = w->id;
		}
		else {
			int producerId = -1 - goTo;
			int h = producerLen(producerId);
			int R = producerLeft(producerId);
			gNodeGenerate(producerId);
			while (h--) state_stack_pop();
			state = state_stack_top();
			goTo = GoTo(state, R);
			if (goTo == 65536) {
				goto BAD_STATE;      // impossible except End
			}
			state_stack_push(goTo);
		}

	}
	return;
BAD_STATE:  // just for incident
	if (gNodeStackPtr == 1) return;
	yyerror("grammar parsing error!");
}

yyword * curWord;
yyword * nextWord() {
	curWord = nextYyword(scanf);
	if (curWord == NULL) {
		return newYyword("$", -1, -1, '$');
	}
	yylineno = curWord->line;
	column = curWord->column;
	return curWord;

}

void yyerror(char const *s) {
	fflush(stdout);
	printf("\nline<%d>, col<%d>: %s\n", yylineno, column, s);
	for (int i = 0; i < 900; ++i) {
		if (GoTo(state_stack_top(), i) != 65536) {
			printf("\tExpected - %s\n", itemName(i));
		}
	}
	if (curWord != NULL) printf("\tUnexpected : %s\n", itemName(curWord->id));
	else printf("\tUnexpected : program termination tag.\n");
}

int main(int argc, char * argv[]) {
	init_producers();

	int success = 1;
	if (argc == 1) success = freopen("test.lo", "r", stdin);
	else success = freopen(argv[1], "r", stdin);
	if (!success) {
		printf("Fail to open input file.\n");
		return -1;
	}

	gramarParse(nextWord);

	if (argc == 3) success = freopen(argv[2], "w", stdout);
	if (!success) {
		printf("Fail to create output file.\n");
		return -1;
	}

	for (int i = 0; i < gNodeStackPtr; ++i) {
		printf("\n");
		gNodeDump(gNodeStack[i], 0);
	}

	return 0;
}

)";
