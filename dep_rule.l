%option noyywrap
%option yylineno
%s LITERAL COMMENT

%{
#include <cstdio>
#include <io.h>
#include <iostream>
using namespace std;
#define YY_DECL extern "C" int yylex()
#include "dep_rule.tab.h"  // to get the token types that we return
static const int buflen = 1024;
static char strbuf[buflen];
static int strbuf_cur = 0;


void yy_reset_buffer(){
	YY_FLUSH_BUFFER;
}

%}
%%
^#.*$			    {}
<INITIAL>[ \t\r\n]  {}  ;
<INITIAL>"{"		{ return GROUP_START; }
<INITIAL>"}"		{ return GROUP_END; }
<INITIAL>"["		{ return LEFT_SB; }
<INITIAL>"]"		{ return RIGHT_SB; }
<INITIAL>"<"		{ return LEFT_AB; }
<INITIAL>">"		{ return RIGHT_AB; }
<INITIAL>"("		{ return LEFT_P; }
<INITIAL>")"		{ return RIGHT_P; }
<INITIAL>":"		{ return COLON; }
<INITIAL>"*"		{ return ASTERISK; }
<INITIAL>"|"		{ return VBAR; }
<INITIAL>"."		{ return DOT; }
<INITIAL>"+"		{ return PLUS; }
<INITIAL>"-"			{ return MINUS; }
<INITIAL>"="		{ return EQUAL; }
<INITIAL>"%"		{ return PERCT; }
<INITIAL>"~"		{ return TILDE; }
<INITIAL>"$"		{ return DOLLAR; }
<INITIAL>"^"		{ return CARET; }
<INITIAL>"@"		{ return AT; }
<INITIAL>"=="		{ return EQUAL_OP; }
<INITIAL>"!="		{ return NOT_EQUAL_OP; }
<INITIAL>"<-"		{ return LEFT_ARROW; }
<INITIAL>"->"		{ return RIGHT_ARROW; }
<INITIAL>"--"		{ return UD_LINK; }
<INITIAL>">>"		{ return FOLLOW; }
<INITIAL>".."		{ return SIBLINGS; }
<INITIAL>"LIKE"		{ return LIKE; }
<INITIAL>"Tree"		       { return TREE; }
<INITIAL>"PTree"		       { return PTREE; }
<INITIAL>"Term"		       { return TERM; }
<INITIAL>"Seq"		       { return SEQ; }
<INITIAL>"len"			   { return LEN;  }
<INITIAL>"ROOT"		       { return ROOT; }
<INITIAL>[\-0-9]+            { yylval.ival = atoi(yytext); return INT; }
<INITIAL>[a-zA-Z0-9\_]+      {	
	yylval.sval = _strdup(yytext);
	// printf("STRING: %s\n", yylval.sval);
	return STRING;
}



\\\"			      {}
<LITERAL>[^\"]		  { 
	strbuf[strbuf_cur] = *yytext; 
	if(strbuf_cur < buflen-1){strbuf_cur++;}	
	// printf("@");
}

<LITERAL>\"		  { 
	BEGIN(INITIAL); 
	// printf("end LITERAL. size: %d, strlen: %d\n", strbuf_cur+1, strlen(strbuf));
	yylval.sval = new char[strbuf_cur+1];
	strcpy_s(yylval.sval, strbuf_cur+1, strbuf);
	for(int i = 0; i < buflen; ++i){strbuf[i] = 0;}
	return QSTRING;
}
\"				  { BEGIN(LITERAL); strbuf_cur = 0; }
<INITIAL>"RULE:"  { BEGIN(COMMENT); strbuf_cur = 0;}
<COMMENT>[^\n\r]    { 
	strbuf[strbuf_cur] = *yytext;
	if(strbuf_cur< buflen - 1){strbuf_cur++;}
}

<COMMENT>\n      {
	// printf("comments end\n");
	BEGIN(INITIAL);
	yylval.sval = new char[strbuf_cur+1]; 
	strcpy_s(yylval.sval, strbuf_cur+1, strbuf);
	for(int i = 0; i < buflen; ++i){strbuf[i] = 0;}
	return COMMENTSTR;
}

<<EOF>> {
	BEGIN(INITIAL);
	yyterminate();
}

.                 { printf(".");}


%%

