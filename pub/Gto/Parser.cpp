
//
//  Copyright (c) 2009, Tweak Software
//  All rights reserved.
// 
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//     * Redistributions of source code must retain the above
//       copyright notice, this list of conditions and the following
//       disclaimer.
//
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials
//       provided with the distribution.
//
//     * Neither the name of the Tweak Software nor the names of its
//       contributors may be used to endorse or promote products
//       derived from this software without specific prior written
//       permission.
// 
//  THIS SOFTWARE IS PROVIDED BY Tweak Software ''AS IS'' AND ANY EXPRESS
//  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL Tweak Software BE LIABLE FOR
//  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
//  OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
//  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
//  USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
//  DAMAGE.
//
/*    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define yyparse GTOparse
#define yylex GTOlex
#define yyerror GTOerror
#define yylval GTOlval
#define yychar GTOchar
#define yydebug GTOdebug
#define yynerrs GTOnerrs
#define	STRINGCONST	257
#define	INTCONST	258
#define	FLOATCONST	259
#define	GTOID	260
#define	AS	261
#define	INTTYPE	262
#define	FLOATTYPE	263
#define	DOUBLETYPE	264
#define	STRINGTYPE	265
#define	HALFTYPE	266
#define	BOOLTYPE	267
#define	SHORTTYPE	268
#define	BYTETYPE	269
#define	ELLIPSIS	270

#line 1 "Parser.y"

/***********************************************************************
// Copyright (c) 2002-2006 Tweak Films
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
*/

#define YYPARSE_PARAM state
#define YYLEX_PARAM state
#define YYDEBUG 1
#ifdef yyerror
#undef yyerror
#endif
#define yyerror(MSG) GTOParseError(YYPARSE_PARAM, MSG)
#define IN_GRAMMAR
#define READER (reinterpret_cast<GTOFlexLexer*>(state)->readerObject())

#include "FlexLexer.h"
#include <iostream>
#include "Reader.h"
#include "Utilities.h"
#include <stdarg.h>

#ifdef GTO_SUPPORT_HALF
#include <half.h>
#endif

int  yylex(void*, void*);
void GTOParseError(void*, const char *,...);
void GTOParseWarning(void*, const char *,...);


#line 50 "Parser.y"
typedef union
{
    int             _token;
    int             _int;
    unsigned int    _uint;
    double          _double;
    Gto::TypeSpec   _type;
    Gto::Number     _number;
} YYSTYPE;
#ifndef YYDEBUG
#define YYDEBUG 1
#endif

#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		92
#define	YYFLAG		-32768
#define	YYNTBASE	26

#define YYTRANSLATE(x) ((unsigned)(x) <= 270 ? yytranslate[x] : 55)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    17,
    18,     2,     2,     2,    25,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    21,     2,     2,
    22,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    23,     2,    24,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    19,     2,    20,     2,     2,     2,     2,     2,
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
     2,     2,     2,     2,     2,     1,     3,     4,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,    15,    16
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     1,     5,     6,    13,    15,    18,    19,    25,    26,
    34,    35,    46,    48,    51,    52,    55,    56,    63,    65,
    68,    69,    76,    77,    86,    88,    93,   101,   103,   105,
   107,   109,   111,   113,   115,   117,   118,   120,   123,   125,
   128,   130,   134,   136,   138,   140,   142,   144,   147,   149,
   151,   154,   156,   158,   160,   163,   165
};

static const short yyrhs[] = {    -1,
     6,    27,    29,     0,     0,     6,    17,     4,    18,    28,
    29,     0,    30,     0,    29,    30,     0,     0,     3,    19,
    31,    34,    20,     0,     0,     3,    21,     3,    19,    32,
    34,    20,     0,     0,     3,    21,     3,    17,     4,    18,
    19,    33,    34,    20,     0,    36,     0,    34,    36,     0,
     0,     7,     3,     0,     0,     3,    35,    19,    37,    38,
    20,     0,    39,     0,    38,    39,     0,     0,    42,     3,
    35,    22,    40,    48,     0,     0,    42,     3,    35,    22,
    41,    23,    44,    24,     0,    43,     0,    43,    23,     4,
    24,     0,    43,    23,     4,    24,    23,     4,    24,     0,
     9,     0,     8,     0,    11,     0,    14,     0,    15,     0,
    12,     0,    13,     0,    10,     0,     0,    45,     0,    45,
    16,     0,    46,     0,    45,    46,     0,    48,     0,    23,
    47,    24,     0,    49,     0,    51,     0,    50,     0,    52,
     0,    50,     0,    49,    50,     0,     3,     0,    52,     0,
    51,    52,     0,    53,     0,    54,     0,     5,     0,    25,
     5,     0,     4,     0,    25,     4,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
    97,   103,   107,   112,   117,   119,   122,   127,   129,   133,
   135,   139,   142,   144,   147,   153,   159,   164,   167,   169,
   172,   178,   200,   204,   237,   245,   252,   260,   262,   263,
   264,   265,   266,   267,   268,   271,   273,   274,   277,   279,
   282,   284,   295,   297,   300,   302,   305,   307,   310,   329,
   331,   334,   419,   519,   521,   524,   526
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","STRINGCONST",
"INTCONST","FLOATCONST","GTOID","AS","INTTYPE","FLOATTYPE","DOUBLETYPE","STRINGTYPE",
"HALFTYPE","BOOLTYPE","SHORTTYPE","BYTETYPE","ELLIPSIS","'('","')'","'{'","'}'",
"':'","'='","'['","']'","'-'","file","@1","@2","object_list","object","@3","@4",
"@5","component_list","interp_string_opt","component","@6","property_list","property",
"@7","@8","type","basic_type","complex_element_list","element_list","element",
"atomic_value_list","atomic_value","string_value_list","string_value","numeric_value_list",
"numeric_value","float_num","int_num", NULL
};
#endif

static const short yyr1[] = {     0,
    27,    26,    28,    26,    29,    29,    31,    30,    32,    30,
    33,    30,    34,    34,    35,    35,    37,    36,    38,    38,
    40,    39,    41,    39,    42,    42,    42,    43,    43,    43,
    43,    43,    43,    43,    43,    44,    44,    44,    45,    45,
    46,    46,    47,    47,    48,    48,    49,    49,    50,    51,
    51,    52,    52,    53,    53,    54,    54
};

static const short yyr2[] = {     0,
     0,     3,     0,     6,     1,     2,     0,     5,     0,     7,
     0,    10,     1,     2,     0,     2,     0,     6,     1,     2,
     0,     6,     0,     8,     1,     4,     7,     1,     1,     1,
     1,     1,     1,     1,     1,     0,     1,     2,     1,     2,
     1,     3,     1,     1,     1,     1,     1,     2,     1,     1,
     2,     1,     1,     1,     2,     1,     2
};

static const short yydefact[] = {     0,
     1,     0,     0,     0,     0,     2,     5,     3,     7,     0,
     6,     0,     0,     0,     4,    15,     0,    13,     0,     9,
     0,     0,     8,    14,     0,     0,    16,    17,     0,     0,
     0,    11,    10,    29,    28,    35,    30,    33,    34,    31,
    32,     0,    19,     0,    25,     0,    18,    20,    15,     0,
     0,     0,     0,    12,    21,    26,     0,     0,     0,    49,
    56,    54,     0,    22,    45,    46,    52,    53,    36,     0,
    57,    55,     0,     0,    37,    39,    41,    27,     0,    43,
    47,    44,    50,    24,    38,    40,    42,    48,    51,     0,
     0,     0
};

static const short yydefgoto[] = {    90,
     3,    12,     6,     7,    13,    26,    46,    17,    22,    18,
    31,    42,    43,    57,    58,    44,    45,    74,    75,    76,
    79,    77,    80,    65,    82,    66,    67,    68
};

static const short yypact[] = {    11,
     3,    46,    49,    33,     9,    49,-32768,-32768,-32768,    50,
-32768,    49,    61,     8,    49,    58,    13,-32768,    62,-32768,
    64,    51,-32768,-32768,    53,    61,-32768,-32768,    54,    15,
    48,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,    29,-32768,    65,    52,    61,-32768,-32768,    58,    68,
    16,    47,    55,-32768,    57,    59,     4,    60,    70,-32768,
-32768,-32768,    43,-32768,-32768,-32768,-32768,-32768,     1,    63,
-32768,-32768,     4,    66,    -2,-32768,-32768,-32768,    67,    73,
-32768,     6,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    77,
    78,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,    69,    39,-32768,-32768,-32768,   -14,    35,   -17,
-32768,-32768,    44,-32768,-32768,-32768,-32768,-32768,-32768,    10,
-32768,    31,-32768,   -58,-32768,   -27,-32768,-32768
};


#define	YYLAST		91


static const short yytable[] = {    24,
    60,    61,    62,    60,    61,    62,    60,    61,    62,    61,
    62,    30,    24,    85,    81,    16,     1,    16,    16,     2,
    73,    88,    63,    73,    19,    63,    20,     9,    63,    10,
    63,    51,    23,    24,    33,    54,    34,    35,    36,    37,
    38,    39,    40,    41,    11,    83,    71,    72,    47,     4,
     8,     5,    14,    11,    89,    34,    35,    36,    37,    38,
    39,    40,    41,    16,    21,    25,    27,    49,    55,    28,
    29,    53,    32,    70,    50,    60,    91,    92,    56,   -23,
    15,    59,    69,    52,    86,    48,    78,    64,     0,    84,
    87
};

static const short yycheck[] = {    17,
     3,     4,     5,     3,     4,     5,     3,     4,     5,     4,
     5,    26,    30,    16,    73,     3,     6,     3,     3,    17,
    23,    80,    25,    23,    17,    25,    19,    19,    25,    21,
    25,    46,    20,    51,    20,    20,     8,     9,    10,    11,
    12,    13,    14,    15,     6,    73,     4,     5,    20,     4,
    18,     3,     3,    15,    82,     8,     9,    10,    11,    12,
    13,    14,    15,     3,     7,     4,     3,     3,    22,    19,
    18,     4,    19,     4,    23,     3,     0,     0,    24,    23,
    12,    23,    23,    49,    75,    42,    24,    57,    -1,    24,
    24
};
#define YYPURE 1

/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/share/bison.simple"
/* This file comes from bison-1.28.  */

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

#ifndef YYSTACK_USE_ALLOCA
#ifdef alloca
#define YYSTACK_USE_ALLOCA
#else /* alloca not defined */
#ifdef __GNUC__
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi) || (defined (__sun) && defined (__i386))
#define YYSTACK_USE_ALLOCA
#include <alloca.h>
#else /* not sparc */
/* We think this test detects Watcom and Microsoft C.  */
/* This used to test MSDOS, but that is a bad idea
   since that symbol is in the user namespace.  */
#if (defined (_MSDOS) || defined (_MSDOS_)) && !defined (__TURBOC__)
#if 0 /* No need for malloc.h, which pollutes the namespace;
	 instead, just don't use alloca.  */
#include <malloc.h>
#endif
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
/* I don't know what this was needed for, but it pollutes the namespace.
   So I turned it off.   rms, 2 May 1997.  */
/* #include <malloc.h>  */
 #pragma alloca
#define YYSTACK_USE_ALLOCA
#else /* not MSDOS, or __TURBOC__, or _AIX */
#if 0
#ifdef __hpux /* haible@ilog.fr says this works for HPUX 9.05 and up,
		 and on HPUX 10.  Eventually we can turn this on.  */
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#endif /* __hpux */
#endif
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc */
#endif /* not GNU C */
#endif /* alloca not defined */
#endif /* YYSTACK_USE_ALLOCA not defined */

#ifdef YYSTACK_USE_ALLOCA
#define YYSTACK_ALLOC alloca
#else
#define YYSTACK_ALLOC malloc
#endif

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Define __yy_memcpy.  Note that the size argument
   should be passed with type unsigned int, because that is what the non-GCC
   definitions require.  With GCC, __builtin_memcpy takes an arg
   of type size_t, but it can handle unsigned int.  */

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     unsigned int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, unsigned int count)
{
  register char *t = to;
  register char *f = from;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 217 "/usr/share/bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
#ifdef YYPARSE_PARAM
int yyparse (void *);
#else
int yyparse (void);
#endif
#endif

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;
  int yyfree_stacks = 0;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  if (yyfree_stacks)
	    {
	      free (yyss);
	      free (yyvs);
#ifdef YYLSP_NEEDED
	      free (yyls);
#endif
	    }
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
#ifndef YYSTACK_USE_ALLOCA
      yyfree_stacks = 1;
#endif
      yyss = (short *) YYSTACK_ALLOC (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1,
		   size * (unsigned int) sizeof (*yyssp));
      yyvs = (YYSTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1,
		   size * (unsigned int) sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1,
		   size * (unsigned int) sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 1:
#line 99 "Parser.y"
{
        READER->beginHeader(GTO_VERSION);
    ;
    break;}
case 2:
#line 103 "Parser.y"
{ 
        READER->endFile(); 
    ;
    break;}
case 3:
#line 108 "Parser.y"
{
        READER->beginHeader(yyvsp[-1]._int);
    ;
    break;}
case 4:
#line 112 "Parser.y"
{ 
        READER->endFile(); 
    ;
    break;}
case 7:
#line 124 "Parser.y"
{
        READER->beginObject(yyvsp[-1]._int, READER->internString("object"));
    ;
    break;}
case 9:
#line 130 "Parser.y"
{
        READER->beginObject(yyvsp[-3]._int, yyvsp[-1]._int);
    ;
    break;}
case 11:
#line 136 "Parser.y"
{
        READER->beginObject(yyvsp[-6]._int, yyvsp[-4]._int, yyvsp[-2]._int);
    ;
    break;}
case 15:
#line 149 "Parser.y"
{
        yyval._int = READER->internString("");
    ;
    break;}
case 16:
#line 154 "Parser.y"
{
        yyval._int = yyvsp[0]._int;
    ;
    break;}
case 17:
#line 161 "Parser.y"
{
        READER->beginComponent(yyvsp[-2]._int, yyvsp[-1]._int);
    ;
    break;}
case 21:
#line 174 "Parser.y"
{
        READER->beginProperty(yyvsp[-2]._int, yyvsp[-1]._int, yyvsp[-3]._type.width, yyvsp[-3]._type.size, yyvsp[-3]._type.type);
    ;
    break;}
case 22:
#line 178 "Parser.y"
{
        if (READER->currentType().width != 1)
        {
            GTOParseError(state, "expected data width of %d, found 1",
                          READER->currentType().width);
            YYERROR;
        }
        else if (yyvsp[-5]._type.size != 0 && 
                 READER->numAtomicValuesInBuffer() != yyvsp[-5]._type.size)
        {
            GTOParseError(state, 
                          "property size mismatch, found %d, expect %d",
                          READER->numAtomicValuesInBuffer(),
                          yyvsp[-5]._type.size);
            YYERROR;
        }
        else
        {
            READER->endProperty();
        }
    ;
    break;}
case 23:
#line 201 "Parser.y"
{
        READER->beginProperty(yyvsp[-2]._int, yyvsp[-1]._int, yyvsp[-3]._type.width, yyvsp[-3]._type.size, yyvsp[-3]._type.type);
    ;
    break;}
case 24:
#line 205 "Parser.y"
{
        size_t nelements = READER->numElementsInBuffer();

        if (yyvsp[-7]._type.size != 0 && nelements != yyvsp[-7]._type.size)
        {
            if (yyvsp[-1]._token == ELLIPSIS)
            {
                READER->fillToSize(yyvsp[-7]._type.size);
                READER->endProperty();
            }
            else
            {
                GTOParseError(state, 
                              "property size mismatch, found %d, expect %d",
                              nelements,
                              yyvsp[-7]._type.size);
                YYERROR;
            }
        }
        else if (yyvsp[-7]._type.size == 0 && yyvsp[-1]._token == ELLIPSIS)
        {
            GTOParseError(state,
              "use of ... requires fixed property size but none was provided");
            YYERROR;
        }
        else
        {
            READER->endProperty();
        }
    ;
    break;}
case 25:
#line 239 "Parser.y"
{
        yyval._type.type  = yyvsp[0]._type.type;
        yyval._type.width = 1;
        yyval._type.size  = 0;
    ;
    break;}
case 26:
#line 246 "Parser.y"
{
        yyval._type.type  = yyvsp[-3]._type.type;
        yyval._type.width = yyvsp[-1]._int;
        yyval._type.size  = 0;
    ;
    break;}
case 27:
#line 253 "Parser.y"
{
        yyval._type.type  = yyvsp[-6]._type.type;
        yyval._type.width = yyvsp[-4]._int;
        yyval._type.size  = yyvsp[-1]._int;
    ;
    break;}
case 28:
#line 261 "Parser.y"
{ yyval._type.type = Gto::Float; ;
    break;}
case 29:
#line 262 "Parser.y"
{ yyval._type.type = Gto::Int; ;
    break;}
case 30:
#line 263 "Parser.y"
{ yyval._type.type = Gto::String; ;
    break;}
case 31:
#line 264 "Parser.y"
{ yyval._type.type = Gto::Short; ;
    break;}
case 32:
#line 265 "Parser.y"
{ yyval._type.type = Gto::Byte; ;
    break;}
case 33:
#line 266 "Parser.y"
{ yyval._type.type = Gto::Half; ;
    break;}
case 34:
#line 267 "Parser.y"
{ yyval._type.type = Gto::Boolean; ;
    break;}
case 35:
#line 268 "Parser.y"
{ yyval._type.type = Gto::Double; ;
    break;}
case 36:
#line 272 "Parser.y"
{ yyval._token = 0; ;
    break;}
case 37:
#line 273 "Parser.y"
{ yyval._token = 0; ;
    break;}
case 38:
#line 274 "Parser.y"
{ yyval._token = ELLIPSIS; ;
    break;}
case 42:
#line 285 "Parser.y"
{
        if (yyvsp[-1]._uint != READER->currentType().width)
        {
            GTOParseError(state, "expected data width of %d, found %d",
                          READER->currentType().width, yyvsp[-1]._uint);
            YYERROR;
        }
    ;
    break;}
case 45:
#line 301 "Parser.y"
{ ;
    break;}
case 46:
#line 302 "Parser.y"
{ ;
    break;}
case 47:
#line 306 "Parser.y"
{ yyval._uint = 1; ;
    break;}
case 48:
#line 307 "Parser.y"
{ yyval._uint = yyvsp[-1]._uint + 1; ;
    break;}
case 49:
#line 312 "Parser.y"
{
        if (READER->currentType().type != Gto::String)
        {
            GTOParseError(state, 
                          "expected a numeric value, found string \"%s\"",
                          READER->stringFromId(yyvsp[0]._int).c_str());
            YYERROR;
        }
        else
        {
            READER->addToPropertyBuffer(yyvsp[0]._int);
        }

        yyval._int = yyvsp[0]._int;
    ;
    break;}
case 50:
#line 330 "Parser.y"
{ yyval._uint = 1; ;
    break;}
case 51:
#line 331 "Parser.y"
{ yyval._uint = yyvsp[-1]._uint + 1; ;
    break;}
case 52:
#line 336 "Parser.y"
{
        Gto::DataType t = Gto::DataType(READER->properties().back().type);

        switch (t)
        {
          case Gto::Int:
              if (yyvsp[0]._double != int(yyvsp[0]._double))
              {
                  GTOParseWarning(state, "floating point value truncated "
                                  "to match integer property type (%f => %d)",
                                  yyvsp[0]._double, int(yyvsp[0]._double));
              }
              
              yyval._number.type = Gto::Int;
              yyval._number._int = int(yyvsp[0]._double);
              READER->addToPropertyBuffer(int(yyvsp[0]._double));
              break;
              
          case Gto::Float:
              yyval._number.type = Gto::Float;
              yyval._number._double = yyvsp[0]._double;
              READER->addToPropertyBuffer(float(yyvsp[0]._double));
              break;

          case Gto::Double:
              yyval._number.type = Gto::Double;
              yyval._number._double = yyvsp[0]._double;
              READER->addToPropertyBuffer(double(yyvsp[0]._double));
              break;

#ifdef GTO_SUPPORT_HALF
          case Gto::Half:
              yyval._number.type = Gto::Half;
              yyval._number._double = yyvsp[0]._double;
              READER->addToPropertyBuffer(half(yyvsp[0]._double));
              break;
#else
          case Gto::Half:
              GTOParseError(state, "numeric type '%s' is currently unsupported "
                            "by the parser", typeName(t));
              YYERROR;
#endif

          case Gto::Short:
              if (yyvsp[0]._double != short(yyvsp[0]._double))
              {
                  GTOParseWarning(state, "floating point value truncated "
                                  "to match short property type (%f => %d)",
                                  yyvsp[0]._double, int(short(yyvsp[0]._double)));
              }
              
              yyval._number.type = Gto::Short;
              yyval._number._int = short(yyvsp[0]._double);
              READER->addToPropertyBuffer(short(yyvsp[0]._double));
              break;

          case Gto::Byte:
              if (yyvsp[0]._double != (unsigned char)(yyvsp[0]._double))
              {
                  GTOParseWarning(state, "floating point value truncated "
                                  "to match byte property type (%f => %d)",
                                  yyvsp[0]._double, int((unsigned char)(yyvsp[0]._double)));
              }
              
              yyval._number.type = Gto::Byte;
              yyval._number._int = (unsigned char)(yyvsp[0]._double);
              READER->addToPropertyBuffer((unsigned char)(yyvsp[0]._double));
              break;

          case Gto::String:
              GTOParseError(state, "string expected; got a floating "
                            "point number (%f) instead", yyvsp[0]._double);
              YYERROR;
              break;

          default:
              GTOParseError(state, "unsupported type '%s'; got a floating "
                            "point number (%f) instead", typeName(t), yyvsp[0]._double);
              YYERROR;
              break;
        }
    ;
    break;}
case 53:
#line 420 "Parser.y"
{
        Gto::DataType t = Gto::DataType(READER->properties().back().type);

        switch (t)
        {
          case Gto::Int:
              yyval._number.type = Gto::Int;
              yyval._number._int = yyvsp[0]._int;
              READER->addToPropertyBuffer(int(yyvsp[0]._int));
              break;
              
          case Gto::Float:
              if (yyvsp[0]._int != float(yyvsp[0]._int))
              {
                  GTOParseWarning(state, "integer cannot be represented "
                                  "as floating point (%d => %f)",
                                  yyvsp[0]._int, float(yyvsp[0]._int));
              }
              
              yyval._number.type = Gto::Float;
              yyval._number._double = yyvsp[0]._int;
              READER->addToPropertyBuffer(float(yyvsp[0]._int));
              break;

          case Gto::Double:
              if (yyvsp[0]._int != double(yyvsp[0]._int))
              {
                  GTOParseWarning(state, "integer cannot be represented "
                                  "as double (%d => %f)",
                                  yyvsp[0]._int, double(yyvsp[0]._int));
              }
              
              yyval._number.type = Gto::Double;
              yyval._number._double = yyvsp[0]._int;
              READER->addToPropertyBuffer(double(yyvsp[0]._int));
              break;

#ifdef GTO_SUPPORT_HALF
          case Gto::Half:
              if (yyvsp[0]._int != half(yyvsp[0]._int))
              {
                  GTOParseWarning(state, "integer cannot be represented "
                                  "as half (%d => %f)",
                                  yyvsp[0]._int, float(yyvsp[0]._int));
              }
              
              yyval._number.type = Gto::Half;
              yyval._number._double = yyvsp[0]._int;
              READER->addToPropertyBuffer(half(yyvsp[0]._int));
              break;
#endif

          case Gto::Short:
              if (yyvsp[0]._int != short(yyvsp[0]._int))
              {
                  GTOParseWarning(state, "integer cannot be represented "
                                  "as short (%d => %d)",
                                  yyvsp[0]._int, short(yyvsp[0]._int));
              }
              
              yyval._number.type = Gto::Short;
              yyval._number._int = short(yyvsp[0]._int);
              READER->addToPropertyBuffer(short(yyvsp[0]._int));
              break;

          case Gto::Byte:
              if (yyvsp[0]._int != (unsigned char)(yyvsp[0]._int))
              {
                  GTOParseWarning(state, "integer cannot be represented "
                                  "as byte (%d => %d)",
                                  yyvsp[0]._int, (unsigned char)(yyvsp[0]._int));
              }
              
              yyval._number.type = Gto::Byte;
              yyval._number._int = (unsigned char)(yyvsp[0]._int);
              READER->addToPropertyBuffer((unsigned char)(yyvsp[0]._int));
              break;

          case Gto::String:
              GTOParseError(state, "string expected; got an integer "
                            "(%d) instead", yyvsp[0]._int);
              YYERROR;
              break;

          case Gto::Boolean:
              GTOParseError(state, "numeric type '%s' is currently unsupported "
                            "by the parser", typeName(t));
              YYERROR;
              break;

          default:
              GTOParseError(state, "unsupported type '%s'; got an integer "
                            "(%d) instead", typeName(t), yyvsp[0]._int);
              YYERROR;
              break;
        }
    ;
    break;}
case 54:
#line 520 "Parser.y"
{ yyval._double = yyvsp[0]._double; ;
    break;}
case 55:
#line 521 "Parser.y"
{ yyval._double = -yyvsp[0]._double; ;
    break;}
case 56:
#line 525 "Parser.y"
{ yyval._int = yyvsp[0]._int; ;
    break;}
case 57:
#line 526 "Parser.y"
{ yyval._int = -yyvsp[0]._int; ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 543 "/usr/share/bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;

 yyacceptlab:
  /* YYACCEPT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 0;

 yyabortlab:
  /* YYABORT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 1;
}
#line 530 "Parser.y"


int yylex(void* yylval, void* state)
{
    GTOFlexLexer* lexer = reinterpret_cast<GTOFlexLexer*>(state);
    lexer->setYYSTYPE(yylval);
    return lexer->yylex();
}

void
GTOParseError(void* state, const char *text, ...)
{
    char temp[256];
    //GTOFlexLexer* lexer = reinterpret_cast<GTOFlexLexer*>(state);

    va_list ap;
    va_start(ap,text);
    vsprintf(temp,text,ap);
    va_end(ap);

    READER->parseError(temp);
}

void
GTOParseWarning(void* state, const char *text, ...)
{
    char temp[256];
    //GTOFlexLexer* lexer = (GTOFlexLexer*)state;

    va_list ap;
    va_start(ap,text);
    vsprintf(temp,text,ap);
    va_end(ap);

    READER->parseWarning(temp);
}

int
GTOParse(Gto::Reader* reader)
{
    //
    //  Initialize state
    //

    yydebug = 0;
    GTOFlexLexer lexer(reader->in(), &std::cerr);
    lexer.init(reader);

    //
    //  Call the (bison) parser
    //

    return yyparse(&lexer) == 0;
}   


// Local Variables:
// mode: c++
// compile-command: "bison -v Parser.y"
// End:


