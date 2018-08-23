/* A Bison parser, made by GNU Bison 2.7.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2012 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_E_STUDY_14_LDA_30_CODES_COEUS_HEIMDALLR_DEP_RULE_TAB_H_INCLUDED
# define YY_YY_E_STUDY_14_LDA_30_CODES_COEUS_HEIMDALLR_DEP_RULE_TAB_H_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
/* Line 2058 of yacc.c  */
#line 55 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"

#include "DepRuleNode.h"
struct RuleInfoType;
struct yyNodeConstraintType;
struct RuleAux;


/* Line 2058 of yacc.c  */
#line 54 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.tab.h"

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     END = 258,
     COMMENT_HEAD = 259,
     GROUP_START = 260,
     GROUP_END = 261,
     LEFT_AB = 262,
     RIGHT_AB = 263,
     LEFT_SB = 264,
     RIGHT_SB = 265,
     LEFT_P = 266,
     RIGHT_P = 267,
     COLON = 268,
     ASTERISK = 269,
     VBAR = 270,
     HASH = 271,
     DOT = 272,
     AT = 273,
     FOLLOW = 274,
     PLUS = 275,
     MINUS = 276,
     EQUAL = 277,
     TILDE = 278,
     PERCT = 279,
     DOLLAR = 280,
     CARET = 281,
     TREE = 282,
     ROOT = 283,
     LEN = 284,
     SEQ = 285,
     TERM = 286,
     PTREE = 287,
     EQUAL_OP = 288,
     NOT_EQUAL_OP = 289,
     LIKE = 290,
     LEFT_ARROW = 291,
     RIGHT_ARROW = 292,
     UD_LINK = 293,
     SIBLINGS = 294,
     INT = 295,
     STRING = 296,
     QSTRING = 297,
     COMMENTSTR = 298
   };
#endif


#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 2058 of yacc.c  */
#line 67 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"

	int		ival;	
	char*	sval;	
	DepRuleNode*		node_ptr;
	DepRuleNodeList*	node_list;
	DepRuleNodeSet*		node_set;
	RuleInfoType*		rule_info;
	RuleConstraint*		rule_const_t;
	RuleConstraintList*	rc_list_t;
	RuleProperty*		   rule_prop_t;
	vector<RuleProperty*>* rp_list_t;
	yyNodeConstraintType*  node_const_t;
	RuleAux*               rule_aux_t;


/* Line 2058 of yacc.c  */
#line 128 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_YY_E_STUDY_14_LDA_30_CODES_COEUS_HEIMDALLR_DEP_RULE_TAB_H_INCLUDED  */
