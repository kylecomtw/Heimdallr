/* A Bison parser, made by GNU Bison 2.7.  */

/* Bison implementation for Yacc-like parsers in C
   
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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.7"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
/* Line 371 of yacc.c  */
#line 1 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"

#include <cstdio>
#include <iostream>
#include <string>
#include "DepRuleNode.h"
#include "ParseException.h"
#include <exception>
#include <vector>
#define YYDEBUG 1
using namespace std;

// stuff from flex that bison needs to know about:
extern "C" int yylex();
extern "C" int yyparse();
extern "C" void scan_string(char*);
extern int yylineno;
extern FILE *yyin;
extern string g_rule_file_name;
extern void yyrestart(FILE*);
int yylex_destroy (void );
DepRuleNodeSet* dpr_root;
 
void yyerror(const char *s);
struct RuleInfoType {
	int priority;
	string rule_class;
	string rule_label;
	string comments;
};
struct yyNodeConstraintType{	
	string rule_class;
	string rule_label;
	string not_rule_class;
	string not_rule_label;
};
struct RuleAux {
	RuleConstraintList* cons_list;
	vector<RuleProperty*>*   prop_list;
	RuleAux(): cons_list(NULL), prop_list(NULL) {}	
	~RuleAux() {
		if(cons_list){
			for(auto p: *cons_list) delete p;
		}

		if(prop_list){
			for(auto p: *prop_list) delete p;
		}
	}
};
void free_tree_pos(void* ptr){
	if(strlen((const char*)ptr) > 0) free(ptr);	
}

/* Line 371 of yacc.c  */
#line 122 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.tab.cpp"

# ifndef YY_NULL
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULL nullptr
#  else
#   define YY_NULL 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "dep_rule.tab.h".  */
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
/* Line 387 of yacc.c  */
#line 55 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"

#include "DepRuleNode.h"
struct RuleInfoType;
struct yyNodeConstraintType;
struct RuleAux;


/* Line 387 of yacc.c  */
#line 162 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.tab.cpp"

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
/* Line 387 of yacc.c  */
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


/* Line 387 of yacc.c  */
#line 236 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.tab.cpp"
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

/* Copy the second part of user declarations.  */

/* Line 390 of yacc.c  */
#line 264 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.tab.cpp"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(N) (N)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (YYID (0))
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  10
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   157

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  44
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  25
/* YYNRULES -- Number of rules.  */
#define YYNRULES  80
/* YYNRULES -- Number of states.  */
#define YYNSTATES  135

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   298

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     8,    10,    13,    17,    20,    22,
      24,    27,    30,    33,    35,    39,    43,    47,    51,    55,
      57,    61,    65,    69,    73,    75,    77,    80,    84,    86,
      88,    93,   101,   106,   110,   114,   120,   124,   127,   132,
     137,   141,   145,   147,   149,   151,   153,   155,   158,   161,
     167,   173,   176,   178,   181,   186,   193,   195,   198,   201,
     206,   213,   218,   222,   226,   228,   231,   236,   238,   241,
     243,   245,   247,   249,   251,   253,   255,   257,   259,   261,
     263
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      45,     0,    -1,    46,    -1,    46,    47,    -1,    47,    -1,
      58,    49,    -1,    58,    48,    49,    -1,    65,    49,    -1,
      63,    -1,    60,    -1,    48,    63,    -1,    48,    60,    -1,
      49,    50,    -1,    50,    -1,    49,    68,    50,    -1,     5,
      53,     6,    -1,    11,    49,    12,    -1,    11,    51,    12,
      -1,     9,    50,    10,    -1,    52,    -1,    51,    15,    50,
      -1,    50,    15,    50,    -1,     7,    50,     8,    -1,     7,
      28,     8,    -1,    26,    -1,    25,    -1,    18,    40,    -1,
      31,    18,    40,    -1,    56,    -1,    42,    -1,    17,    17,
      17,    42,    -1,    17,    17,    17,    42,    17,    17,    17,
      -1,    42,    17,    17,    17,    -1,    30,    13,    56,    -1,
      55,    13,    56,    -1,    55,    13,    56,    13,    42,    -1,
      55,    13,    42,    -1,    55,    57,    -1,    55,    57,    13,
      56,    -1,    55,    57,    13,    42,    -1,    56,    13,    42,
      -1,    56,    13,    54,    -1,    40,    -1,    27,    -1,    32,
      -1,    14,    -1,    41,    -1,    18,    41,    -1,    17,    41,
      -1,    17,    41,    17,    23,    41,    -1,    18,    41,    18,
      23,    41,    -1,    65,    59,    -1,    59,    -1,    40,    13,
      -1,    40,    13,    41,    13,    -1,    40,    13,    41,    13,
      41,    13,    -1,    61,    -1,    60,    61,    -1,    66,    62,
      -1,    66,    62,    13,    40,    -1,    41,    11,    40,    12,
      67,    41,    -1,    41,    11,    40,    12,    -1,    41,    11,
      12,    -1,    41,    67,    41,    -1,    64,    -1,    63,    64,
      -1,    24,    41,    13,    42,    -1,    43,    -1,    65,    43,
      -1,    20,    -1,    21,    -1,    22,    -1,    33,    -1,    34,
      -1,    35,    -1,    22,    -1,    36,    -1,    37,    -1,    38,
      -1,    19,    -1,    39,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   126,   126,   129,   130,   133,   143,   155,   165,   166,
     167,   168,   171,   172,   173,   181,   182,   186,   190,   194,
     198,   199,   202,   206,   215,   216,   217,   221,   225,   230,
     235,   245,   257,   267,   271,   276,   282,   287,   296,   306,
     315,   320,   327,   330,   331,   333,   334,   336,   337,   338,
     339,   341,   342,   344,   345,   346,   348,   349,   351,   352,
     358,   363,   367,   371,   377,   378,   380,   385,   386,   394,
     395,   396,   398,   399,   400,   401,   403,   404,   405,   406,
     407
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "END", "COMMENT_HEAD", "GROUP_START",
  "GROUP_END", "LEFT_AB", "RIGHT_AB", "LEFT_SB", "RIGHT_SB", "LEFT_P",
  "RIGHT_P", "COLON", "ASTERISK", "VBAR", "HASH", "DOT", "AT", "FOLLOW",
  "PLUS", "MINUS", "EQUAL", "TILDE", "PERCT", "DOLLAR", "CARET", "TREE",
  "ROOT", "LEN", "SEQ", "TERM", "PTREE", "EQUAL_OP", "NOT_EQUAL_OP",
  "LIKE", "LEFT_ARROW", "RIGHT_ARROW", "UD_LINK", "SIBLINGS", "INT",
  "STRING", "QSTRING", "COMMENTSTR", "$accept", "rule_set", "rule_list",
  "rule", "rule_aux", "node_list", "node", "node_set", "head_node",
  "node_content", "ref", "tree_var", "tree_pos", "node_constraint",
  "rule_head", "rule_info", "rule_cons_list", "rule_constraint",
  "constraint_expr", "prop_list", "prop_expr", "comments", "valence",
  "rel_op", "node_conn_op", YY_NULL
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    44,    45,    46,    46,    47,    47,    47,    48,    48,
      48,    48,    49,    49,    49,    50,    50,    50,    50,    50,
      51,    51,    52,    52,    53,    53,    53,    53,    53,    53,
      53,    53,    53,    53,    53,    53,    53,    53,    53,    53,
      53,    53,    54,    55,    55,    56,    56,    57,    57,    57,
      57,    58,    58,    59,    59,    59,    60,    60,    61,    61,
      62,    62,    62,    62,    63,    63,    64,    65,    65,    66,
      66,    66,    67,    67,    67,    67,    68,    68,    68,    68,
      68
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     1,     2,     3,     2,     1,     1,
       2,     2,     2,     1,     3,     3,     3,     3,     3,     1,
       3,     3,     3,     3,     1,     1,     2,     3,     1,     1,
       4,     7,     4,     3,     3,     5,     3,     2,     4,     4,
       3,     3,     1,     1,     1,     1,     1,     2,     2,     5,
       5,     2,     1,     2,     4,     6,     1,     2,     2,     4,
       6,     4,     3,     3,     1,     2,     4,     1,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,    67,     0,     2,     4,     0,    52,     0,    53,
       1,     3,     0,     0,     0,     0,    69,    70,    71,     0,
       0,     5,    13,    19,     9,    56,     8,    64,     0,    68,
       7,    51,     0,    45,     0,     0,    25,    24,    43,     0,
       0,    44,    46,    29,     0,     0,    28,     0,     0,     0,
       0,    13,     0,     0,     6,    11,    10,    79,    76,    77,
      78,    80,    12,     0,    57,    65,     0,    58,    54,     0,
      26,     0,     0,     0,    15,     0,     0,     0,    37,     0,
      23,    22,    18,    16,     0,    17,     0,     0,    14,     0,
      75,    72,    73,    74,     0,     0,     0,     0,    33,    27,
       0,    36,    34,    48,    47,     0,    42,    40,    41,    21,
      20,    66,    62,     0,    63,    59,    55,    30,    32,     0,
       0,     0,    39,    38,    61,     0,    35,     0,     0,     0,
       0,    49,    50,    60,    31
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     3,     4,     5,    20,    21,    62,    52,    23,    44,
     108,    45,    46,    78,     6,     7,    24,    25,    67,    26,
      27,     8,    28,    94,    63
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -71
static const yytype_int16 yypact[] =
{
      76,     9,   -71,     4,    76,   -71,    85,   -71,     6,   -17,
     -71,   -71,    56,    84,   106,   106,   -71,   -71,   -71,    -3,
      85,    23,   -71,   -71,    80,   -71,    24,   -71,    17,   -71,
      23,   -71,    52,   -71,    30,    31,   -71,   -71,   -71,    59,
      61,   -71,   -71,    67,    79,    86,    95,   102,   110,   104,
      14,   105,    51,   108,    23,    80,    24,   -71,   -71,   -71,
     -71,   -71,   -71,   106,   -71,   -71,    34,   109,    82,   107,
     -71,    -2,    87,   111,   -71,     2,    88,    89,   112,   -22,
     -71,   -71,   -71,   -71,   106,   -71,   106,    90,   -71,    -9,
     -71,   -71,   -71,   -71,    92,    91,   113,    93,   -71,   -71,
     117,   -71,   123,   120,   121,    13,   -71,   -71,   -71,   -71,
     -71,   -71,   -71,   126,   -71,   -71,   -71,   124,   -71,    98,
     119,   122,   -71,   -71,    42,   127,   -71,   114,   115,   116,
     129,   -71,   -71,   -71,   -71
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -71,   -71,   -71,   139,   -71,    21,    -6,   -71,   -71,   -71,
     -71,   -71,   -70,   -71,   -71,   140,   130,   -18,   -71,   131,
     -16,   -71,   -71,    25,   -71
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      22,    98,    22,   112,    10,   102,    64,    48,    49,    51,
      65,    12,    33,    13,    22,    14,    33,    15,   106,    12,
     107,    13,     9,    14,    32,    15,    83,    33,    12,    30,
      13,   113,    14,    57,    15,   123,    50,    64,    53,    42,
      65,    54,    57,    42,   101,    89,     1,    69,    19,    29,
      58,    59,    60,    61,    42,   122,    90,    88,    66,    58,
      59,    60,    61,    85,    90,    68,    86,    91,    92,    93,
      33,    70,    71,    34,    35,    91,    92,    93,   109,    72,
     110,    36,    37,    38,    73,    74,    39,    40,    41,    12,
      12,    13,    13,    14,    14,    15,    15,    42,    43,    75,
      16,    17,    18,    76,    77,    16,    17,    18,    79,    19,
      80,    12,    47,    13,    82,    14,     1,    15,    81,     2,
      84,    87,    95,    96,    97,   105,   116,    99,   100,   103,
     104,   115,   111,   114,   118,   117,   119,   120,   124,   121,
     126,   125,   127,    11,   130,   128,   134,     0,    31,   129,
      55,    56,     0,     0,     0,   131,   132,   133
};

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-71)))

#define yytable_value_is_error(Yytable_value) \
  YYID (0)

static const yytype_int8 yycheck[] =
{
       6,    71,     8,    12,     0,    75,    24,    13,    14,    15,
      26,     5,    14,     7,    20,     9,    14,    11,    40,     5,
      42,     7,    13,     9,    41,    11,    12,    14,     5,     8,
       7,    40,     9,    19,    11,   105,    15,    55,    41,    41,
      56,    20,    19,    41,    42,    11,    40,    17,    24,    43,
      36,    37,    38,    39,    41,    42,    22,    63,    41,    36,
      37,    38,    39,    12,    22,    13,    15,    33,    34,    35,
      14,    40,    13,    17,    18,    33,    34,    35,    84,    18,
      86,    25,    26,    27,    17,     6,    30,    31,    32,     5,
       5,     7,     7,     9,     9,    11,    11,    41,    42,    13,
      20,    21,    22,    17,    18,    20,    21,    22,    13,    24,
       8,     5,    28,     7,    10,     9,    40,    11,     8,    43,
      15,    13,    13,    41,    17,    13,    13,    40,    17,    41,
      41,    40,    42,    41,    17,    42,    13,    17,    12,    18,
      42,    17,    23,     4,    17,    23,    17,    -1,     8,   124,
      20,    20,    -1,    -1,    -1,    41,    41,    41
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    40,    43,    45,    46,    47,    58,    59,    65,    13,
       0,    47,     5,     7,     9,    11,    20,    21,    22,    24,
      48,    49,    50,    52,    60,    61,    63,    64,    66,    43,
      49,    59,    41,    14,    17,    18,    25,    26,    27,    30,
      31,    32,    41,    42,    53,    55,    56,    28,    50,    50,
      49,    50,    51,    41,    49,    60,    63,    19,    36,    37,
      38,    39,    50,    68,    61,    64,    41,    62,    13,    17,
      40,    13,    18,    17,     6,    13,    17,    18,    57,    13,
       8,     8,    10,    12,    15,    12,    15,    13,    50,    11,
      22,    33,    34,    35,    67,    13,    41,    17,    56,    40,
      17,    42,    56,    41,    41,    13,    40,    42,    54,    50,
      50,    42,    12,    40,    41,    40,    13,    42,    17,    13,
      17,    18,    42,    56,    12,    17,    42,    23,    23,    67,
      17,    41,    41,    41,    17
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))

/* Error token number */
#define YYTERROR	1
#define YYERRCODE	256


/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */
#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
        break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULL, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULL;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - Assume YYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  YYERROR is fine as it does not invoke this
       function.
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULL, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
        break;
    }
}




/* The lookahead symbol.  */
int yychar;


#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval YY_INITIAL_VALUE(yyval_default);

/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
/* Line 1792 of yacc.c  */
#line 126 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    {(yyval.node_set) = new DepRuleNodeSet((yyvsp[(1) - (1)].node_list)); dpr_root = (yyval.node_set); }
    break;

  case 3:
/* Line 1792 of yacc.c  */
#line 129 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    {(yyval.node_list) = (yyvsp[(1) - (2)].node_list); (yyvsp[(1) - (2)].node_list)->push_back((yyvsp[(2) - (2)].node_ptr));}
    break;

  case 4:
/* Line 1792 of yacc.c  */
#line 130 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    { (yyval.node_list) = new DepRuleNodeList(); (yyval.node_list)->push_back((yyvsp[(1) - (1)].node_ptr)); }
    break;

  case 5:
/* Line 1792 of yacc.c  */
#line 133 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    {
		(yyval.node_ptr) = new DepRuleNode(NodeType::NdRule);
		(yyval.node_ptr)->set_children((yyvsp[(2) - (2)].node_list));
		(yyval.node_ptr)->set_info((yyvsp[(1) - (2)].rule_info)->priority, (yyvsp[(1) - (2)].rule_info)->rule_class, (yyvsp[(1) - (2)].rule_info)->rule_label);
		(yyval.node_ptr)->set_src_line(yylineno);
		(yyval.node_ptr)->set_src_file(g_rule_file_name);
		(yyval.node_ptr)->set_comment((yyvsp[(1) - (2)].rule_info)->comments);
		delete (yyvsp[(1) - (2)].rule_info);
		delete (yyvsp[(2) - (2)].node_list);
		}
    break;

  case 6:
/* Line 1792 of yacc.c  */
#line 143 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    {
		(yyval.node_ptr) = new DepRuleNode(NodeType::NdRule);
		(yyval.node_ptr)->set_children((yyvsp[(3) - (3)].node_list));
		(yyval.node_ptr)->set_info((yyvsp[(1) - (3)].rule_info)->priority, (yyvsp[(1) - (3)].rule_info)->rule_class, (yyvsp[(1) - (3)].rule_info)->rule_label);		
		(yyval.node_ptr)->set_rule_constraints((yyvsp[(2) - (3)].rule_aux_t)->cons_list);
		(yyval.node_ptr)->set_rule_properties((yyvsp[(2) - (3)].rule_aux_t)->prop_list);
		(yyval.node_ptr)->set_src_line(yylineno);
		(yyval.node_ptr)->set_comment((yyvsp[(1) - (3)].rule_info)->comments);
		delete (yyvsp[(1) - (3)].rule_info);		
		delete (yyvsp[(2) - (3)].rule_aux_t);
		delete (yyvsp[(3) - (3)].node_list);
		}
    break;

  case 7:
/* Line 1792 of yacc.c  */
#line 155 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    {
		(yyval.node_ptr) = new DepRuleNode(NodeType::NdRule);
		(yyval.node_ptr)->set_children((yyvsp[(2) - (2)].node_list));
		(yyval.node_ptr)->set_comment((yyvsp[(1) - (2)].sval));		
		(yyval.node_ptr)->set_src_line(yylineno);
		delete (yyvsp[(1) - (2)].sval);		
		delete (yyvsp[(2) - (2)].node_list);
		}
    break;

  case 8:
/* Line 1792 of yacc.c  */
#line 165 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    { (yyval.rule_aux_t) = new RuleAux(); (yyval.rule_aux_t)->prop_list = (yyvsp[(1) - (1)].rp_list_t); }
    break;

  case 9:
/* Line 1792 of yacc.c  */
#line 166 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    { (yyval.rule_aux_t) = new RuleAux(); (yyval.rule_aux_t)->cons_list = (yyvsp[(1) - (1)].rc_list_t); }
    break;

  case 10:
/* Line 1792 of yacc.c  */
#line 167 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    { (yyval.rule_aux_t) = (yyvsp[(1) - (2)].rule_aux_t); (yyval.rule_aux_t)->prop_list = (yyvsp[(2) - (2)].rp_list_t); }
    break;

  case 11:
/* Line 1792 of yacc.c  */
#line 168 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    { (yyval.rule_aux_t) = (yyvsp[(1) - (2)].rule_aux_t); (yyval.rule_aux_t)->cons_list = (yyvsp[(2) - (2)].rc_list_t); }
    break;

  case 12:
/* Line 1792 of yacc.c  */
#line 171 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    {(yyval.node_list) = (yyvsp[(1) - (2)].node_list); (yyvsp[(1) - (2)].node_list)->push_back((yyvsp[(2) - (2)].node_ptr)); }
    break;

  case 13:
/* Line 1792 of yacc.c  */
#line 172 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    { (yyval.node_list) = new DepRuleNodeList(); (yyval.node_list)->push_back((yyvsp[(1) - (1)].node_ptr)); }
    break;

  case 14:
/* Line 1792 of yacc.c  */
#line 173 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    { 
				(yyval.node_list) = (yyvsp[(1) - (3)].node_list); 
				(yyvsp[(3) - (3)].node_ptr)->set_node_connection((yyvsp[(2) - (3)].ival)); 
				(yyvsp[(1) - (3)].node_list)->push_back((yyvsp[(3) - (3)].node_ptr)); 
			}
    break;

  case 15:
/* Line 1792 of yacc.c  */
#line 181 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    { (yyval.node_ptr) = (yyvsp[(2) - (3)].node_ptr); }
    break;

  case 16:
/* Line 1792 of yacc.c  */
#line 182 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    {
			(yyval.node_ptr) = new DepRuleNode();			
			(yyval.node_ptr)->set_children((yyvsp[(2) - (3)].node_list));
		}
    break;

  case 17:
/* Line 1792 of yacc.c  */
#line 186 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    {
			(yyval.node_ptr) = new DepRuleNode(NodeType::NdOrNode);			
			(yyval.node_ptr)->set_children((yyvsp[(2) - (3)].node_list));
		}
    break;

  case 18:
/* Line 1792 of yacc.c  */
#line 190 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    {
			(yyval.node_ptr) = (yyvsp[(2) - (3)].node_ptr);
			(yyvsp[(2) - (3)].node_ptr)->set_optional();
		}
    break;

  case 19:
/* Line 1792 of yacc.c  */
#line 194 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    { (yyval.node_ptr) = (yyvsp[(1) - (1)].node_ptr); }
    break;

  case 20:
/* Line 1792 of yacc.c  */
#line 198 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    { (yyval.node_list) = (yyvsp[(1) - (3)].node_list); (yyvsp[(1) - (3)].node_list)->push_back((yyvsp[(3) - (3)].node_ptr)); }
    break;

  case 21:
/* Line 1792 of yacc.c  */
#line 199 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    { (yyval.node_list) = new DepRuleNodeList(); (yyval.node_list)->push_back((yyvsp[(1) - (3)].node_ptr)); (yyval.node_list)->push_back((yyvsp[(3) - (3)].node_ptr)); }
    break;

  case 22:
/* Line 1792 of yacc.c  */
#line 202 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    {
				(yyvsp[(2) - (3)].node_ptr)->set_head_flag();
				(yyval.node_ptr) = (yyvsp[(2) - (3)].node_ptr);
			}
    break;

  case 23:
/* Line 1792 of yacc.c  */
#line 206 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    {
				// printf("root head\n");
				DepRuleNode* nptr = new DepRuleNode(NodeType::NdRoot, "", "", -1);
				nptr->set_head_flag();
				(yyval.node_ptr) = nptr;
			}
    break;

  case 24:
/* Line 1792 of yacc.c  */
#line 215 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    { (yyval.node_ptr) = new DepRuleNode(NodeType::NdSeqStart, "", "^", -1); }
    break;

  case 25:
/* Line 1792 of yacc.c  */
#line 216 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    { (yyval.node_ptr) = new DepRuleNode(NodeType::NdSeqEnd, "", "$", -1); }
    break;

  case 26:
/* Line 1792 of yacc.c  */
#line 217 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    {
				(yyval.node_ptr) = new DepRuleNode(NodeType::NdSeqMiddle, "", "@", -1);
				(yyval.node_ptr)->set_start_pos_hint((yyvsp[(2) - (2)].ival));
			}
    break;

  case 27:
/* Line 1792 of yacc.c  */
#line 221 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    { 
			(yyval.node_ptr) = new DepRuleNode(NodeType::NdTerminal, "", "", -1); 
			(yyval.node_ptr)->set_start_pos_hint((yyvsp[(3) - (3)].ival));
			}
    break;

  case 28:
/* Line 1792 of yacc.c  */
#line 225 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    {
				// printf("node_content: {%s}\n", $1);					
				(yyval.node_ptr) = new DepRuleNode(NodeType::NdTerminal, "", (yyvsp[(1) - (1)].sval), -1);
				free_tree_pos((yyvsp[(1) - (1)].sval));
			}
    break;

  case 29:
/* Line 1792 of yacc.c  */
#line 230 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    {
				// printf("node_content: {QSTRING[%d]}\n", strlen($1));
				(yyval.node_ptr) = new DepRuleNode(NodeType::NdTerminal, (yyvsp[(1) - (1)].sval), "", -1);
				delete (yyvsp[(1) - (1)].sval);
			}
    break;

  case 30:
/* Line 1792 of yacc.c  */
#line 235 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    { 
				DepRuleNodeList* nlist = new DepRuleNodeList();
				DepRuleNode* ntree = new DepRuleNode(NodeType::NdTree, "", "", -1);
				DepRuleNode* treex = new DepRuleNode(NodeType::NdTerminal, (yyvsp[(4) - (4)].sval), "", -1);
				nlist->push_back(ntree);
				nlist->push_back(treex);
				(yyval.node_ptr) = new DepRuleNode(); 
				(yyval.node_ptr)->set_children(nlist);
				delete (yyvsp[(4) - (4)].sval);
			}
    break;

  case 31:
/* Line 1792 of yacc.c  */
#line 245 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    { 
				DepRuleNodeList* nlist = new DepRuleNodeList();
				DepRuleNode* ntree_1 = new DepRuleNode(NodeType::NdTree, "", "", -1);
				DepRuleNode* ntree_2 = new DepRuleNode(NodeType::NdTree, "", "", -1);
				DepRuleNode* treex = new DepRuleNode(NodeType::NdTerminal, (yyvsp[(4) - (7)].sval), "", -1);
				nlist->push_back(ntree_1);
				nlist->push_back(treex);
				nlist->push_back(ntree_2);
				(yyval.node_ptr) = new DepRuleNode(); 
				(yyval.node_ptr)->set_children(nlist);
				delete (yyvsp[(4) - (7)].sval);
			}
    break;

  case 32:
/* Line 1792 of yacc.c  */
#line 257 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    { 
				DepRuleNodeList* nlist = new DepRuleNodeList();			
				DepRuleNode* ntree_2 = new DepRuleNode(NodeType::NdTree, "", "", -1);			
				DepRuleNode* treex = new DepRuleNode(NodeType::NdTerminal, (yyvsp[(1) - (4)].sval), "", -1);
				nlist->push_back(treex);
				nlist->push_back(ntree_2);
				(yyval.node_ptr) = new DepRuleNode(); 
				(yyval.node_ptr)->set_children(nlist);
				delete (yyvsp[(1) - (4)].sval);
			}
    break;

  case 33:
/* Line 1792 of yacc.c  */
#line 267 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    {
				(yyval.node_ptr) = new DepRuleNode(NodeType::NdSeqRule, "", (yyvsp[(3) - (3)].sval), -1);				
				free_tree_pos((yyvsp[(3) - (3)].sval));		
			}
    break;

  case 34:
/* Line 1792 of yacc.c  */
#line 271 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    {
				(yyval.node_ptr) = new DepRuleNode(NodeType::NdTree, "", (yyvsp[(3) - (3)].sval), -1);
				if((yyvsp[(1) - (3)].ival) == 1) (yyval.node_ptr)->set_passive(true);
				free_tree_pos((yyvsp[(3) - (3)].sval));	
			}
    break;

  case 35:
/* Line 1792 of yacc.c  */
#line 276 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    {
				(yyval.node_ptr) = new DepRuleNode(NodeType::NdTree, (yyvsp[(5) - (5)].sval), (yyvsp[(3) - (5)].sval), -1);
				if((yyvsp[(1) - (5)].ival) == 1) (yyval.node_ptr)->set_passive(true);
				delete (yyvsp[(5) - (5)].sval);
				free_tree_pos((yyvsp[(3) - (5)].sval));	
			}
    break;

  case 36:
/* Line 1792 of yacc.c  */
#line 282 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    {
				(yyval.node_ptr) = new DepRuleNode(NodeType::NdTree, (yyvsp[(3) - (3)].sval), "", -1);					
				if((yyvsp[(1) - (3)].ival) == 1) (yyval.node_ptr)->set_passive(true);
				delete (yyvsp[(3) - (3)].sval);
			}
    break;

  case 37:
/* Line 1792 of yacc.c  */
#line 287 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    {
				(yyval.node_ptr) = new DepRuleNode(NodeType::NdTree, "", "", -1);
				if((yyvsp[(1) - (2)].ival) == 1) (yyval.node_ptr)->set_passive(true);
				(yyval.node_ptr)->set_node_constraint(NodeConstraint::RuleClass, (yyvsp[(2) - (2)].node_const_t)->rule_class);
				(yyval.node_ptr)->set_node_constraint(NodeConstraint::RuleLabel, (yyvsp[(2) - (2)].node_const_t)->rule_label);
				(yyval.node_ptr)->set_node_constraint(NodeConstraint::NotRuleClass, (yyvsp[(2) - (2)].node_const_t)->not_rule_class);
				(yyval.node_ptr)->set_node_constraint(NodeConstraint::NotRuleLabel, (yyvsp[(2) - (2)].node_const_t)->not_rule_label);
				delete (yyvsp[(2) - (2)].node_const_t);
			}
    break;

  case 38:
/* Line 1792 of yacc.c  */
#line 296 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    {
				(yyval.node_ptr) = new DepRuleNode(NodeType::NdTree, "", (yyvsp[(4) - (4)].sval), -1);
				if((yyvsp[(1) - (4)].ival) == 1) (yyval.node_ptr)->set_passive(true);				
				(yyval.node_ptr)->set_node_constraint(NodeConstraint::RuleClass, (yyvsp[(2) - (4)].node_const_t)->rule_class);
				(yyval.node_ptr)->set_node_constraint(NodeConstraint::RuleLabel, (yyvsp[(2) - (4)].node_const_t)->rule_label);
				(yyval.node_ptr)->set_node_constraint(NodeConstraint::NotRuleClass, (yyvsp[(2) - (4)].node_const_t)->not_rule_class);
				(yyval.node_ptr)->set_node_constraint(NodeConstraint::NotRuleLabel, (yyvsp[(2) - (4)].node_const_t)->not_rule_label);							
				delete (yyvsp[(2) - (4)].node_const_t);
				free_tree_pos((yyvsp[(4) - (4)].sval));					
			}
    break;

  case 39:
/* Line 1792 of yacc.c  */
#line 306 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    {
				(yyval.node_ptr) = new DepRuleNode(NodeType::NdTree, (yyvsp[(4) - (4)].sval), "", -1);
				if((yyvsp[(1) - (4)].ival) == 1) (yyval.node_ptr)->set_passive(true);
				(yyval.node_ptr)->set_node_constraint(NodeConstraint::RuleClass, (yyvsp[(2) - (4)].node_const_t)->rule_class);
				(yyval.node_ptr)->set_node_constraint(NodeConstraint::RuleLabel, (yyvsp[(2) - (4)].node_const_t)->rule_label);
				(yyval.node_ptr)->set_node_constraint(NodeConstraint::NotRuleClass, (yyvsp[(2) - (4)].node_const_t)->not_rule_class);
				(yyval.node_ptr)->set_node_constraint(NodeConstraint::NotRuleLabel, (yyvsp[(2) - (4)].node_const_t)->not_rule_label);	
				delete (yyvsp[(2) - (4)].node_const_t);
			}
    break;

  case 40:
/* Line 1792 of yacc.c  */
#line 315 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    {
				(yyval.node_ptr) = new DepRuleNode(NodeType::NdTerminal, (yyvsp[(3) - (3)].sval), (yyvsp[(1) - (3)].sval), -1);
				free_tree_pos((yyvsp[(1) - (3)].sval));
				delete (yyvsp[(3) - (3)].sval);
			}
    break;

  case 41:
/* Line 1792 of yacc.c  */
#line 320 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    {
			(yyval.node_ptr) = new DepRuleNode(NodeType::NdTerminal, "", (yyvsp[(1) - (3)].sval), (yyvsp[(3) - (3)].ival));
			free_tree_pos((yyvsp[(1) - (3)].sval));
			// printf("node_content: {%s:%d}", $1, $3);
			}
    break;

  case 43:
/* Line 1792 of yacc.c  */
#line 330 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    { (yyval.ival) = 0; }
    break;

  case 44:
/* Line 1792 of yacc.c  */
#line 331 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    { (yyval.ival) = 1; }
    break;

  case 45:
/* Line 1792 of yacc.c  */
#line 333 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    { (yyval.sval) = ""; }
    break;

  case 47:
/* Line 1792 of yacc.c  */
#line 336 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    { (yyval.node_const_t) = new yyNodeConstraintType{"", (yyvsp[(2) - (2)].sval), "", ""}; free((yyvsp[(2) - (2)].sval)); }
    break;

  case 48:
/* Line 1792 of yacc.c  */
#line 337 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    { (yyval.node_const_t) = new yyNodeConstraintType{(yyvsp[(2) - (2)].sval), "", "", ""}; free((yyvsp[(2) - (2)].sval)); }
    break;

  case 49:
/* Line 1792 of yacc.c  */
#line 338 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    { (yyval.node_const_t) = new yyNodeConstraintType{(yyvsp[(2) - (5)].sval), (yyvsp[(5) - (5)].sval), "", ""}; free((yyvsp[(2) - (5)].sval)); free((yyvsp[(5) - (5)].sval));}
    break;

  case 50:
/* Line 1792 of yacc.c  */
#line 339 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    { (yyval.node_const_t) = new yyNodeConstraintType{"", "", (yyvsp[(2) - (5)].sval), (yyvsp[(5) - (5)].sval)};	free((yyvsp[(2) - (5)].sval));free((yyvsp[(5) - (5)].sval));}
    break;

  case 51:
/* Line 1792 of yacc.c  */
#line 341 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    { (yyval.rule_info) = (yyvsp[(2) - (2)].rule_info); (yyval.rule_info)->comments = string((yyvsp[(1) - (2)].sval)); delete (yyvsp[(1) - (2)].sval); }
    break;

  case 52:
/* Line 1792 of yacc.c  */
#line 342 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    { (yyval.rule_info) = (yyvsp[(1) - (1)].rule_info); }
    break;

  case 53:
/* Line 1792 of yacc.c  */
#line 344 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    { (yyval.rule_info) = new RuleInfoType{(yyvsp[(1) - (2)].ival), "", "", ""}; }
    break;

  case 54:
/* Line 1792 of yacc.c  */
#line 345 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    { (yyval.rule_info) = new RuleInfoType{(yyvsp[(1) - (4)].ival), (yyvsp[(3) - (4)].sval), "", ""}; free((yyvsp[(3) - (4)].sval));}
    break;

  case 55:
/* Line 1792 of yacc.c  */
#line 346 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    { (yyval.rule_info) = new RuleInfoType{(yyvsp[(1) - (6)].ival), (yyvsp[(3) - (6)].sval), (yyvsp[(5) - (6)].sval), ""}; free((yyvsp[(3) - (6)].sval)); free((yyvsp[(5) - (6)].sval));}
    break;

  case 56:
/* Line 1792 of yacc.c  */
#line 348 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    { (yyval.rc_list_t) = new RuleConstraintList(); (yyval.rc_list_t)->push_back((yyvsp[(1) - (1)].rule_const_t)); }
    break;

  case 57:
/* Line 1792 of yacc.c  */
#line 349 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    { (yyval.rc_list_t) = (yyvsp[(1) - (2)].rc_list_t); (yyval.rc_list_t)->push_back((yyvsp[(2) - (2)].rule_const_t)); }
    break;

  case 58:
/* Line 1792 of yacc.c  */
#line 351 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    { (yyval.rule_const_t) = (yyvsp[(2) - (2)].rule_const_t); (yyval.rule_const_t)->valence = (yyvsp[(1) - (2)].ival);	}
    break;

  case 59:
/* Line 1792 of yacc.c  */
#line 352 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    {
					(yyval.rule_const_t) = (yyvsp[(2) - (4)].rule_const_t);
					(yyval.rule_const_t)->valence = (yyvsp[(1) - (4)].ival);
					(yyval.rule_const_t)->weight = (yyvsp[(4) - (4)].ival);					
					}
    break;

  case 60:
/* Line 1792 of yacc.c  */
#line 358 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    {
					(yyval.rule_const_t) = new RuleConstraint(0, (yyvsp[(1) - (6)].sval), (yyvsp[(3) - (6)].ival), (yyvsp[(6) - (6)].sval), 0);
					(yyval.rule_const_t)->set_relational_operator((yyvsp[(5) - (6)].ival));
					free((yyvsp[(1) - (6)].sval)); free((yyvsp[(6) - (6)].sval));
					}
    break;

  case 61:
/* Line 1792 of yacc.c  */
#line 363 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    {
					(yyval.rule_const_t) = new RuleConstraint(0, (yyvsp[(1) - (4)].sval), (yyvsp[(3) - (4)].ival), "", 0);		
					free((yyvsp[(1) - (4)].sval));			
					}
    break;

  case 62:
/* Line 1792 of yacc.c  */
#line 367 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    {
					(yyval.rule_const_t) = new RuleConstraint(0, (yyvsp[(1) - (3)].sval), -1, "", 0);					
					free((yyvsp[(1) - (3)].sval));
					}
    break;

  case 63:
/* Line 1792 of yacc.c  */
#line 371 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    {
					(yyval.rule_const_t) = new RuleConstraint(0, (yyvsp[(1) - (3)].sval), -1, (yyvsp[(3) - (3)].sval), 0);
					(yyval.rule_const_t)->set_relational_operator((yyvsp[(2) - (3)].ival));
					free((yyvsp[(1) - (3)].sval)); free((yyvsp[(3) - (3)].sval));
					}
    break;

  case 64:
/* Line 1792 of yacc.c  */
#line 377 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    { (yyval.rp_list_t) = new vector<RuleProperty*>(); (yyval.rp_list_t)->push_back((yyvsp[(1) - (1)].rule_prop_t)); }
    break;

  case 65:
/* Line 1792 of yacc.c  */
#line 378 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    { (yyval.rp_list_t) = (yyvsp[(1) - (2)].rp_list_t); (yyval.rp_list_t)->push_back((yyvsp[(2) - (2)].rule_prop_t)); }
    break;

  case 66:
/* Line 1792 of yacc.c  */
#line 380 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    {
				(yyval.rule_prop_t) = new RuleProperty((yyvsp[(2) - (4)].sval), (yyvsp[(4) - (4)].sval));
				free((yyvsp[(2) - (4)].sval)); free((yyvsp[(4) - (4)].sval));
			}
    break;

  case 67:
/* Line 1792 of yacc.c  */
#line 385 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    { (yyval.sval) = (yyvsp[(1) - (1)].sval); }
    break;

  case 68:
/* Line 1792 of yacc.c  */
#line 386 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    { 
			int buflen = strlen((yyvsp[(1) - (2)].sval)) + strlen((yyvsp[(2) - (2)].sval)) + 1;
			(yyval.sval) = new char[buflen](); 
			strcat_s((yyval.sval), buflen, (yyvsp[(1) - (2)].sval));
			strcat_s((yyval.sval), buflen, (yyvsp[(2) - (2)].sval));
			delete (yyvsp[(1) - (2)].sval); delete (yyvsp[(2) - (2)].sval);
			}
    break;

  case 69:
/* Line 1792 of yacc.c  */
#line 394 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    { (yyval.ival)=1; }
    break;

  case 70:
/* Line 1792 of yacc.c  */
#line 395 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    { (yyval.ival)=-1; }
    break;

  case 71:
/* Line 1792 of yacc.c  */
#line 396 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    { (yyval.ival)=0; }
    break;

  case 72:
/* Line 1792 of yacc.c  */
#line 398 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    {(yyval.ival) = 1;}
    break;

  case 73:
/* Line 1792 of yacc.c  */
#line 399 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    {(yyval.ival) = 2;}
    break;

  case 74:
/* Line 1792 of yacc.c  */
#line 400 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    {(yyval.ival) = 3;}
    break;

  case 75:
/* Line 1792 of yacc.c  */
#line 401 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    {(yyval.ival) = 4;}
    break;

  case 76:
/* Line 1792 of yacc.c  */
#line 403 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    {(yyval.ival) = 1;}
    break;

  case 77:
/* Line 1792 of yacc.c  */
#line 404 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    {(yyval.ival) = 2;}
    break;

  case 78:
/* Line 1792 of yacc.c  */
#line 405 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    {(yyval.ival) = 3; }
    break;

  case 79:
/* Line 1792 of yacc.c  */
#line 406 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    { (yyval.ival) = 4; }
    break;

  case 80:
/* Line 1792 of yacc.c  */
#line 407 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"
    { (yyval.ival) = 5; }
    break;


/* Line 1792 of yacc.c  */
#line 2217 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.tab.cpp"
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


/* Line 2055 of yacc.c  */
#line 409 "E:\\Study\\14_LDA\\30_Codes\\Coeus\\Heimdallr\\dep_rule.y"


int bison_parse_dep_rule_file(const char* fpath) {
	// open a file handle to a particular file:	
	yydebug = 0;
	yylineno = 1;
	FILE *myfile = fopen(fpath, "r");
	// make sure it's valid:
	if (!myfile) {
		cout << "can't open " << fpath << endl;
		return -1;
	}

	// set lex to read from it instead of defaulting to STDIN:
	yyin = myfile;
	yyrestart(myfile);
	
	// parse through the input until there is no more:	
	do {		
		yyparse();
	} while (!feof(yyin));
	fclose(myfile);
	
	yylex_destroy();
	return 0;	
}


void yyerror(const char *s) {
	cout << "parse error @ Line:" << yylineno << "\n";
	cout << "Message: " << s << endl;
	// might as well halt now:
	// getchar();	
	if(yyin > 0) fclose(yyin);

	throw ParseException(yylineno);
}

