# SeuLex-SeuYacc
Course design of compiling

See the report for details

----------------------------------------------

+ SeuLex接受littelc.l生成lex.yy.c
+ SeuYacc接受littelc.y生成y.tab.h和y.tab.c
+ lex.yy.c+y.tab.h生成词法分析程序
+ y.tab.c生成语法分析程序
+ 词法分析程序接受源代码(默认test.c)，生成词法分析结果(默认test.lo)
+ 语法分析程序接受词法分析结果(默认test.o)，输出语法分析结果（默认控制台输出）
