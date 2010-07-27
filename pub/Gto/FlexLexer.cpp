#define yyFlexLexer GTOFlexLexer

#line 4 "FlexLexer.cpp"
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

/* Scanner skeleton version:
 * $Header: /cvs/root/flex/flex/skel.c,v 1.2 2004/05/07 00:28:17 jkh Exp $
 */

#define FLEX_SCANNER
#define YY_FLEX_MAJOR_VERSION 2
#define YY_FLEX_MINOR_VERSION 5



/* cfront 1.2 defines "c_plusplus" instead of "__cplusplus" */
#ifdef c_plusplus
#ifndef __cplusplus
#define __cplusplus
#endif
#endif


#ifdef __cplusplus

#include <stdlib.h>
#include <iostream>
using namespace std;
//#include <unistd.h>

/* Use prototypes in function declarations. */
#define YY_USE_PROTOS

/* The "const" storage-class-modifier is valid. */
#define YY_USE_CONST

#else	/* ! __cplusplus */

#if __STDC__

#define YY_USE_PROTOS
#define YY_USE_CONST

#endif	/* __STDC__ */
#endif	/* ! __cplusplus */

#ifdef __TURBOC__
 #pragma warn -rch
 #pragma warn -use
#include <io.h>
#include <stdlib.h>
#define YY_USE_CONST
#define YY_USE_PROTOS
#endif

#ifdef YY_USE_CONST
#define yyconst const
#else
#define yyconst
#endif


#ifdef YY_USE_PROTOS
#define YY_PROTO(proto) proto
#else
#define YY_PROTO(proto) ()
#endif

/* Returned upon end-of-file. */
#define YY_NULL 0

/* Promotes a possibly negative, possibly signed char to an unsigned
 * integer for use as an array index.  If the signed char is negative,
 * we want to instead treat it as an 8-bit unsigned char, hence the
 * double cast.
 */
#define YY_SC_TO_UI(c) ((unsigned int) (unsigned char) c)

/* Enter a start condition.  This macro really ought to take a parameter,
 * but we do it the disgusting crufty way forced on us by the ()-less
 * definition of BEGIN.
 */
#define BEGIN yy_start = 1 + 2 *

/* Translate the current start state into a value that can be later handed
 * to BEGIN to return to the state.  The YYSTATE alias is for lex
 * compatibility.
 */
#define YY_START ((yy_start - 1) / 2)
#define YYSTATE YY_START

/* Action number for EOF rule of a given start state. */
#define YY_STATE_EOF(state) (YY_END_OF_BUFFER + state + 1)

/* Special action meaning "start processing a new file". */
#define YY_NEW_FILE yyrestart( yyin )

#define YY_END_OF_BUFFER_CHAR 0

/* Size of default input buffer. */
#define YY_BUF_SIZE 16384

typedef struct yy_buffer_state *YY_BUFFER_STATE;

extern int yyleng;

#define EOB_ACT_CONTINUE_SCAN 0
#define EOB_ACT_END_OF_FILE 1
#define EOB_ACT_LAST_MATCH 2

/* The funky do-while in the following #define is used to turn the definition
 * int a single C statement (which needs a semi-colon terminator).  This
 * avoids problems with code like:
 *
 * 	if ( condition_holds )
 *		yyless( 5 );
 *	else
 *		do_something_else();
 *
 * Prior to using the do-while the compiler would get upset at the
 * "else" because it interpreted the "if" statement as being all
 * done when it reached the ';' after the yyless() call.
 */

/* Return all but the first 'n' matched characters back to the input stream. */

#define yyless(n) \
	do \
		{ \
		/* Undo effects of setting up yytext. */ \
		*yy_cp = yy_hold_char; \
		YY_RESTORE_YY_MORE_OFFSET \
		yy_c_buf_p = yy_cp = yy_bp + n - YY_MORE_ADJ; \
		YY_DO_BEFORE_ACTION; /* set up yytext again */ \
		} \
	while ( 0 )

#define unput(c) yyunput( c, yytext_ptr )

/* The following is because we cannot portably get our hands on size_t
 * (without autoconf's help, which isn't available because we want
 * flex-generated scanners to compile on their own).
 */
typedef unsigned int yy_size_t;


struct yy_buffer_state
	{
	istream* yy_input_file;

	char *yy_ch_buf;		/* input buffer */
	char *yy_buf_pos;		/* current position in input buffer */

	/* Size of input buffer in bytes, not including room for EOB
	 * characters.
	 */
	yy_size_t yy_buf_size;

	/* Number of characters read into yy_ch_buf, not including EOB
	 * characters.
	 */
	int yy_n_chars;

	/* Whether we "own" the buffer - i.e., we know we created it,
	 * and can realloc() it to grow it, and should free() it to
	 * delete it.
	 */
	int yy_is_our_buffer;

	/* Whether this is an "interactive" input source; if so, and
	 * if we're using stdio for input, then we want to use getc()
	 * instead of fread(), to make sure we stop fetching input after
	 * each newline.
	 */
	int yy_is_interactive;

	/* Whether we're considered to be at the beginning of a line.
	 * If so, '^' rules will be active on the next match, otherwise
	 * not.
	 */
	int yy_at_bol;

	/* Whether to try to fill the input buffer when we reach the
	 * end of it.
	 */
	int yy_fill_buffer;

	int yy_buffer_status;
#define YY_BUFFER_NEW 0
#define YY_BUFFER_NORMAL 1
	/* When an EOF's been seen but there's still some text to process
	 * then we mark the buffer as YY_EOF_PENDING, to indicate that we
	 * shouldn't try reading from the input source any more.  We might
	 * still have a bunch of tokens to match, though, because of
	 * possible backing-up.
	 *
	 * When we actually see the EOF, we change the status to "new"
	 * (via yyrestart()), so that the user can continue scanning by
	 * just pointing yyin at a new input file.
	 */
#define YY_BUFFER_EOF_PENDING 2
	};


/* We provide macros for accessing buffer states in case in the
 * future we want to put the buffer states in a more general
 * "scanner state".
 */
#define YY_CURRENT_BUFFER yy_current_buffer



static void *yy_flex_alloc YY_PROTO(( yy_size_t ));
static void *yy_flex_realloc YY_PROTO(( void *, yy_size_t ));
static void yy_flex_free YY_PROTO(( void * ));

#define yy_new_buffer yy_create_buffer

#define yy_set_interactive(is_interactive) \
	{ \
	if ( ! yy_current_buffer ) \
		yy_current_buffer = yy_create_buffer( yyin, YY_BUF_SIZE ); \
	yy_current_buffer->yy_is_interactive = is_interactive; \
	}

#define yy_set_bol(at_bol) \
	{ \
	if ( ! yy_current_buffer ) \
		yy_current_buffer = yy_create_buffer( yyin, YY_BUF_SIZE ); \
	yy_current_buffer->yy_at_bol = at_bol; \
	}

#define YY_AT_BOL() (yy_current_buffer->yy_at_bol)


#define yywrap() 1
#define YY_SKIP_YYWRAP
typedef unsigned char YY_CHAR;
#define yytext_ptr yytext
#define YY_INTERACTIVE

#include <Gto/FlexLexer.h>


/* Done after the current pattern has been matched and before the
 * corresponding action - sets up yytext.
 */
#define YY_DO_BEFORE_ACTION \
	yytext_ptr = yy_bp; \
	yyleng = (int) (yy_cp - yy_bp); \
	yy_hold_char = *yy_cp; \
	*yy_cp = '\0'; \
	yy_c_buf_p = yy_cp;

#define YY_NUM_RULES 39
#define YY_END_OF_BUFFER 40
static yyconst short int yy_accept[129] =
    {   0,
        0,    0,    0,    0,    0,    0,   40,   38,    1,    2,
       38,   15,   38,   38,   38,   29,   29,   37,   37,   37,
       37,   37,   37,   37,   37,   37,   38,   39,   28,   18,
       28,   27,   25,   28,    1,    2,    0,   14,    0,    0,
        0,    0,   31,   32,   29,   37,   37,   37,   37,   37,
        4,   37,   37,   37,   37,   37,   37,   37,   37,    0,
       18,   24,    0,   26,   19,   22,   21,   23,   20,    0,
       35,    0,    0,   13,    0,    0,   37,    0,   32,   30,
       37,   37,   37,   37,   37,   37,    7,   37,   37,    0,
        0,    0,   33,    0,    0,   31,   32,    3,    8,   10,

       37,   37,    9,   37,   37,    0,   17,    0,    0,   37,
        5,   11,   37,    0,    0,    0,    6,   12,    0,   16,
        0,    0,   34,    0,    0,    0,   36,    0
    } ;

static yyconst int yy_ec[256] =
    {   0,
        1,    1,    1,    1,    1,    1,    1,    1,    2,    3,
        1,    1,    4,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    2,    1,    5,    6,    1,    7,    1,    8,    1,
        1,    1,    9,    1,    9,   10,    1,   11,   12,   12,
       12,   13,   13,   13,   13,   14,   14,    1,    1,    1,
        1,    1,    1,    1,   15,   15,   15,   15,   16,   15,
       17,   18,   18,   18,   18,   18,   18,   18,   19,   18,
       18,   18,   18,   20,   18,   18,   18,   18,   18,   18,
        1,   21,    1,    1,   18,    1,   22,   23,   15,   24,

       25,   26,   27,   28,   29,   18,   18,   30,   18,   31,
       32,   18,   18,   33,   34,   35,   36,   18,   18,   37,
       38,   18,   39,    1,   40,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,

        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1
    } ;

static yyconst int yy_meta[41] =
    {   0,
        1,    1,    2,    1,    1,    1,    1,    3,    1,    4,
        5,    5,    5,    5,    6,    5,    7,    7,    7,    7,
        3,    6,    8,    6,    5,    8,    7,    7,    7,    7,
        9,    7,    9,    7,    9,    9,    7,    7,    1,    1
    } ;

static yyconst short int yy_base[146] =
    {   0,
        0,    0,    0,    0,   38,   44,  276,  277,  273,  277,
      271,  277,   41,  252,   43,   56,   84,    0,  252,  237,
       26,  238,  239,  246,  236,   43,  259,  277,  277,  277,
      262,  277,  277,  105,  262,  277,   47,  277,   57,  255,
       54,  252,   57,   58,    0,    0,   75,  131,    0,  242,
        0,  228,  224,  222,  225,  225,  216,  215,  210,  232,
      277,  277,   64,  277,  277,  277,  277,  277,  277,    0,
      277,  227,    0,  277,   90,  109,    0,   94,  100,    0,
      209,  197,  192,  193,  193,  188,    0,  161,  123,  115,
      121,    0,  277,    0,  137,  147,  151,    0,    0,    0,

      107,  100,    0,   95,   98,  102,  277,    0,    0,  100,
        0,    0,   97,   96,    0,    0,    0,    0,   79,    0,
       84,   64,  277,   78,   73,   39,  277,  277,  165,  174,
      183,  192,  198,  203,  210,   58,  215,  220,  224,  228,
      232,  236,  240,  244,  248
    } ;

static yyconst short int yy_def[146] =
    {   0,
      128,    1,  129,  129,  130,  130,  128,  128,  128,  128,
      128,  128,  131,  132,  128,  133,  133,  134,  134,  134,
      134,  134,  134,  134,  134,  134,  128,  128,  128,  128,
      128,  128,  128,  128,  128,  128,  131,  128,  131,  128,
      135,  128,  136,  136,   17,  137,  137,  137,  134,  134,
      134,  134,  134,  134,  134,  134,  134,  134,  134,  128,
      128,  128,  128,  128,  128,  128,  128,  128,  128,  138,
      128,  128,  139,  128,  128,  128,  137,  128,  137,   48,
      134,  134,  134,  134,  134,  134,  134,  134,  134,  128,
      128,  140,  128,  141,  128,  128,  128,  134,  134,  134,

      134,  134,  134,  134,  134,  128,  128,  142,  143,  134,
      134,  134,  134,  128,  144,  145,  134,  134,  128,  144,
      145,  128,  128,  128,  128,  128,  128,    0,  128,  128,
      128,  128,  128,  128,  128,  128,  128,  128,  128,  128,
      128,  128,  128,  128,  128
    } ;

static yyconst short int yy_nxt[318] =
    {   0,
        8,    9,   10,   11,   12,   13,    8,   14,    8,   15,
       16,   17,   17,   17,   18,   18,   19,   18,   18,   18,
        8,   20,   21,   22,   18,   23,   18,   24,   25,   18,
       18,   18,   18,   26,   18,   18,   18,   18,   27,    8,
       30,   31,   32,   38,   39,   33,   30,   31,   32,   38,
       39,   33,   42,   43,   43,   43,   43,   52,   34,   38,
       39,   71,   43,   53,   34,   44,   45,   45,   45,   45,
       58,   47,   75,   76,   91,   91,   91,   59,  127,  126,
       47,   75,   76,   78,  125,   79,   79,   79,   79,   73,
      124,  123,   48,   44,   45,   45,   45,   45,   95,   47,

       96,   96,   96,   96,   97,   97,   97,   97,   47,   62,
       79,   79,   79,   79,  122,   63,   63,   78,  119,   97,
       97,   97,   97,  118,  117,   64,  114,   65,  113,  112,
       66,  107,  107,  107,  111,   67,  110,   68,  106,   69,
       70,   80,   80,   80,   80,   80,   80,   96,   96,   96,
       96,  105,   80,   80,   80,   80,   80,   96,   96,   96,
       96,   97,   97,   97,   97,   28,   28,   28,   28,   28,
       28,   28,   28,   28,   29,   29,   29,   29,   29,   29,
       29,   29,   29,   37,   37,   37,   37,   37,   37,   37,
       37,   37,   40,  104,   40,   40,   40,   40,   40,   40,

       40,   46,   46,   46,   46,   46,   46,   49,   49,   49,
       49,   49,   72,  103,  102,  101,  100,   72,   72,   77,
       77,   77,   77,   77,   92,   92,   99,   92,   94,   94,
       98,   94,  108,  108,   93,  108,  109,  109,   90,  109,
      115,  115,   89,  115,  116,  116,   88,  116,  120,  120,
       87,  120,  121,  121,   86,  121,   85,   84,   83,   82,
       81,   74,   71,   35,   61,   60,   57,   56,   55,   54,
       51,   50,   41,   36,   35,  128,    7,  128,  128,  128,
      128,  128,  128,  128,  128,  128,  128,  128,  128,  128,
      128,  128,  128,  128,  128,  128,  128,  128,  128,  128,

      128,  128,  128,  128,  128,  128,  128,  128,  128,  128,
      128,  128,  128,  128,  128,  128,  128
    } ;

static yyconst short int yy_chk[318] =
    {   0,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        5,    5,    5,   13,   13,    5,    6,    6,    6,   37,
       37,    6,   15,   15,   15,   15,   15,   21,    5,   39,
       39,   41,  136,   21,    6,   16,   16,   16,   16,   16,
       26,   16,   43,   44,   63,   63,   63,   26,  126,  125,
       16,   43,   44,   47,  124,   47,   47,   47,   47,   41,
      122,  121,   16,   17,   17,   17,   17,   17,   75,   17,

       75,   75,   75,   75,   78,   78,   78,   78,   17,   34,
       79,   79,   79,   79,  119,   34,   34,   76,  114,   76,
       76,   76,   76,  113,  110,   34,  106,   34,  105,  104,
       34,   91,   91,   91,  102,   34,  101,   34,   90,   34,
       34,   48,   48,   48,   48,   48,   48,   95,   95,   95,
       95,   89,   48,   48,   48,   48,   48,   96,   96,   96,
       96,   97,   97,   97,   97,  129,  129,  129,  129,  129,
      129,  129,  129,  129,  130,  130,  130,  130,  130,  130,
      130,  130,  130,  131,  131,  131,  131,  131,  131,  131,
      131,  131,  132,   88,  132,  132,  132,  132,  132,  132,

      132,  133,  133,  133,  133,  133,  133,  134,  134,  134,
      134,  134,  135,   86,   85,   84,   83,  135,  135,  137,
      137,  137,  137,  137,  138,  138,   82,  138,  139,  139,
       81,  139,  140,  140,   72,  140,  141,  141,   60,  141,
      142,  142,   59,  142,  143,  143,   58,  143,  144,  144,
       57,  144,  145,  145,   56,  145,   55,   54,   53,   52,
       50,   42,   40,   35,   31,   27,   25,   24,   23,   22,
       20,   19,   14,   11,    9,    7,  128,  128,  128,  128,
      128,  128,  128,  128,  128,  128,  128,  128,  128,  128,
      128,  128,  128,  128,  128,  128,  128,  128,  128,  128,

      128,  128,  128,  128,  128,  128,  128,  128,  128,  128,
      128,  128,  128,  128,  128,  128,  128
    } ;

/* The intent behind this definition is that it'll catch
 * any uses of REJECT which flex missed.
 */
#define REJECT reject_used_but_not_detected
#define yymore() yymore_used_but_not_detected
#define YY_MORE_ADJ 0
#define YY_RESTORE_YY_MORE_OFFSET
#line 1 "Lexer.l"
#define INITIAL 0
#line 2 "Lexer.l"
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
#include "Utilities.h"
#include "Parser.h"
#include "Reader.h"
#include <iostream>
using namespace std;
using namespace Gto;

extern int	GTOdebug;
#define LVAL reinterpret_cast<YYSTYPE*>(_yystype)
#define YY_NO_UNISTD_H

#define YY_ALWAYS_INTERACTIVE 1
#define COMMENT 1

#define STRING 2

#line 478 "FlexLexer.cpp"

/* Macros after this point can all be overridden by user definitions in
 * section 1.
 */

#ifndef YY_SKIP_YYWRAP
#ifdef __cplusplus
extern "C" int yywrap YY_PROTO(( void ));
#else
extern int yywrap YY_PROTO(( void ));
#endif
#endif


#ifndef yytext_ptr
static void yy_flex_strncpy YY_PROTO(( char *, yyconst char *, int ));
#endif

#ifdef YY_NEED_STRLEN
static int yy_flex_strlen YY_PROTO(( yyconst char * ));
#endif

#ifndef YY_NO_INPUT
#endif

#if YY_STACK_USED
static int yy_start_stack_ptr = 0;
static int yy_start_stack_depth = 0;
static int *yy_start_stack = 0;
#ifndef YY_NO_PUSH_STATE
static void yy_push_state YY_PROTO(( int new_state ));
#endif
#ifndef YY_NO_POP_STATE
static void yy_pop_state YY_PROTO(( void ));
#endif
#ifndef YY_NO_TOP_STATE
static int yy_top_state YY_PROTO(( void ));
#endif

#else
#define YY_NO_PUSH_STATE 1
#define YY_NO_POP_STATE 1
#define YY_NO_TOP_STATE 1
#endif

#ifdef YY_MALLOC_DECL
YY_MALLOC_DECL
#else
#if __STDC__
#ifndef __cplusplus
#include <stdlib.h>
#endif
#else
/* Just try to get by without declaring the routines.  This will fail
 * miserably on non-ANSI systems for which sizeof(size_t) != sizeof(int)
 * or sizeof(void*) != sizeof(int).
 */
#endif
#endif

/* Amount of stuff to slurp up with each read. */
#ifndef YY_READ_BUF_SIZE
#define YY_READ_BUF_SIZE 8192
#endif

/* Copy whatever the last rule matched to the standard output. */

#ifndef ECHO
#define ECHO LexerOutput( yytext, yyleng )
#endif

/* Gets input and stuffs it into "buf".  number of characters read, or YY_NULL,
 * is returned in "result".
 */
#ifndef YY_INPUT
#define YY_INPUT(buf,result,max_size) \
	if ( (result = LexerInput( (char *) buf, max_size )) < 0 ) \
		YY_FATAL_ERROR( "input in flex scanner failed" );
#endif

/* No semi-colon after return; correct usage is to write "yyterminate();" -
 * we don't want an extra ';' after the "return" because that will cause
 * some compilers to complain about unreachable statements.
 */
#ifndef yyterminate
#define yyterminate() return YY_NULL
#endif

/* Number of entries by which start-condition stack grows. */
#ifndef YY_START_STACK_INCR
#define YY_START_STACK_INCR 25
#endif

/* Report a fatal error. */
#ifndef YY_FATAL_ERROR
#define YY_FATAL_ERROR(msg) LexerError( msg )
#endif

/* Default declaration of generated scanner - a define so the user can
 * easily add parameters.
 */
#ifndef YY_DECL
#define YY_DECL int yyFlexLexer::yylex()
#endif

/* Code executed at the beginning of each rule, after yytext and yyleng
 * have been set up.
 */
#ifndef YY_USER_ACTION
#define YY_USER_ACTION
#endif

/* Code executed at the end of each rule. */
#ifndef YY_BREAK
#define YY_BREAK break;
#endif

#define YY_RULE_SETUP \
	YY_USER_ACTION

YY_DECL
	{
	register yy_state_type yy_current_state;
	register char *yy_cp, *yy_bp;
	register int yy_act;

#line 52 "Lexer.l"


#line 608 "FlexLexer.cpp"

	if ( yy_init )
		{
		yy_init = 0;

#ifdef YY_USER_INIT
		YY_USER_INIT;
#endif

		if ( ! yy_start )
			yy_start = 1;	/* first start state */

		if ( ! yyin )
			yyin = &cin;

		if ( ! yyout )
			yyout = &cout;

		if ( ! yy_current_buffer )
			yy_current_buffer =
				yy_create_buffer( yyin, YY_BUF_SIZE );

		yy_load_buffer_state();
		}

	while ( 1 )		/* loops until end-of-file is reached */
		{
		yy_cp = yy_c_buf_p;

		/* Support of yytext. */
		*yy_cp = yy_hold_char;

		/* yy_bp points to the position in yy_ch_buf of the start of
		 * the current run.
		 */
		yy_bp = yy_cp;

		yy_current_state = yy_start;
yy_match:
		do
			{
			register YY_CHAR yy_c = yy_ec[YY_SC_TO_UI(*yy_cp)];
			if ( yy_accept[yy_current_state] )
				{
				yy_last_accepting_state = yy_current_state;
				yy_last_accepting_cpos = yy_cp;
				}
			while ( yy_chk[yy_base[yy_current_state] + yy_c] != yy_current_state )
				{
				yy_current_state = (int) yy_def[yy_current_state];
				if ( yy_current_state >= 129 )
					yy_c = yy_meta[(unsigned int) yy_c];
				}
			yy_current_state = yy_nxt[yy_base[yy_current_state] + (unsigned int) yy_c];
			++yy_cp;
			}
		while ( yy_base[yy_current_state] != 277 );

yy_find_action:
		yy_act = yy_accept[yy_current_state];
		if ( yy_act == 0 )
			{ /* have to back up */
			yy_cp = yy_last_accepting_cpos;
			yy_current_state = yy_last_accepting_state;
			yy_act = yy_accept[yy_current_state];
			}

		YY_DO_BEFORE_ACTION;


do_action:	/* This label is used only to access EOF actions. */


		switch ( yy_act )
	{ /* beginning of action switch */
			case 0: /* must back up */
			/* undo the effects of YY_DO_BEFORE_ACTION */
			*yy_cp = yy_hold_char;
			yy_cp = yy_last_accepting_cpos;
			yy_current_state = yy_last_accepting_state;
			goto yy_find_action;

case 1:
YY_RULE_SETUP
#line 54 "Lexer.l"
{ reader->m_charnum += yyleng; }
	YY_BREAK
case 2:
YY_RULE_SETUP
#line 55 "Lexer.l"
{ reader->m_linenum++; reader->m_charnum=0; }
	YY_BREAK
case 3:
YY_RULE_SETUP
#line 57 "Lexer.l"
{ reader->m_charnum+=yyleng; return GTOID; }
	YY_BREAK
case 4:
YY_RULE_SETUP
#line 58 "Lexer.l"
{ reader->m_charnum+=yyleng; return AS; }
	YY_BREAK
case 5:
YY_RULE_SETUP
#line 59 "Lexer.l"
{ reader->m_charnum+=yyleng; return FLOATTYPE; }
	YY_BREAK
case 6:
YY_RULE_SETUP
#line 60 "Lexer.l"
{ reader->m_charnum+=yyleng; return DOUBLETYPE; }
	YY_BREAK
case 7:
YY_RULE_SETUP
#line 61 "Lexer.l"
{ reader->m_charnum+=yyleng; return INTTYPE; }
	YY_BREAK
case 8:
YY_RULE_SETUP
#line 62 "Lexer.l"
{ reader->m_charnum+=yyleng; return BOOLTYPE; }
	YY_BREAK
case 9:
YY_RULE_SETUP
#line 63 "Lexer.l"
{ reader->m_charnum+=yyleng; return HALFTYPE; }
	YY_BREAK
case 10:
YY_RULE_SETUP
#line 64 "Lexer.l"
{ reader->m_charnum+=yyleng; return BYTETYPE; }
	YY_BREAK
case 11:
YY_RULE_SETUP
#line 65 "Lexer.l"
{ reader->m_charnum+=yyleng; return SHORTTYPE; }
	YY_BREAK
case 12:
YY_RULE_SETUP
#line 66 "Lexer.l"
{ reader->m_charnum+=yyleng; return STRINGTYPE; }
	YY_BREAK
case 13:
YY_RULE_SETUP
#line 67 "Lexer.l"
{ reader->m_charnum+=yyleng; return ELLIPSIS; }
	YY_BREAK
case 14:
YY_RULE_SETUP
#line 69 "Lexer.l"
{ reader->m_linenum++; reader->m_charnum=0; }
	YY_BREAK
case 15:
YY_RULE_SETUP
#line 70 "Lexer.l"
{ reader->m_charnum++; yyNewString(); BEGIN STRING; }
	YY_BREAK
case 16:
YY_RULE_SETUP
#line 72 "Lexer.l"
{ 
			  reader->m_charnum += yyleng; 
			  unsigned int u = strtol(yytext+2,(char**)0,16);
			  yyAddToString(u); 
			}
	YY_BREAK
case 17:
YY_RULE_SETUP
#line 78 "Lexer.l"
{ 
			  reader->m_charnum += yyleng; 
			  unsigned int u = strtol(yytext+1,(char**)0,8);
			  yyAddToString(u); 
			}
	YY_BREAK
case 18:
YY_RULE_SETUP
#line 84 "Lexer.l"
{ reader->m_linenum++; reader->m_charnum=0; yyAddToString('\n'); }
	YY_BREAK
case 19:
YY_RULE_SETUP
#line 85 "Lexer.l"
{ reader->m_charnum += yyleng; yyAddToString('\b'); }
	YY_BREAK
case 20:
YY_RULE_SETUP
#line 86 "Lexer.l"
{ reader->m_charnum += yyleng; yyAddToString('\t'); }
	YY_BREAK
case 21:
YY_RULE_SETUP
#line 87 "Lexer.l"
{ reader->m_charnum += yyleng; yyAddToString('\n'); }
	YY_BREAK
case 22:
YY_RULE_SETUP
#line 88 "Lexer.l"
{ reader->m_charnum += yyleng; yyAddToString('\f'); }
	YY_BREAK
case 23:
YY_RULE_SETUP
#line 89 "Lexer.l"
{ reader->m_charnum += yyleng; yyAddToString('\r'); }
	YY_BREAK
case 24:
YY_RULE_SETUP
#line 90 "Lexer.l"
{ reader->m_charnum += yyleng; yyAddToString('"'); }
	YY_BREAK
case 25:
YY_RULE_SETUP
#line 91 "Lexer.l"
{ reader->m_charnum += yyleng; yyAddToString('\''); }
	YY_BREAK
case 26:
YY_RULE_SETUP
#line 92 "Lexer.l"
{ reader->m_charnum += yyleng; yyAddToString('\\'); }
	YY_BREAK
case 27:
YY_RULE_SETUP
#line 94 "Lexer.l"
{ 
			  reader->m_charnum++; 
                          BEGIN 0;

                          LVAL->_int = yyReturnString();
                          return STRINGCONST; 
			}
	YY_BREAK
case 28:
YY_RULE_SETUP
#line 102 "Lexer.l"
{ reader->m_charnum++; yyAddToString(*yytext);}
	YY_BREAK
case 29:
#line 105 "Lexer.l"
case 30:
YY_RULE_SETUP
#line 105 "Lexer.l"
{
			    LVAL->_int = strtol(yytext,0,0);
			    reader->m_charnum += yyleng;
			    return INTCONST;
			}
	YY_BREAK
case 31:
#line 112 "Lexer.l"
case 32:
YY_RULE_SETUP
#line 112 "Lexer.l"
{
			    LVAL->_double = atof(yytext);
			    reader->m_charnum += yyleng;
			    return FLOATCONST;
			}
	YY_BREAK
case 33:
YY_RULE_SETUP
#line 119 "Lexer.l"
{
                            switch (yytext[2])
                            {
                              case 'b': LVAL->_int = int('\b'); break;
                              case 't': LVAL->_int = int('\t'); break;
                              case 'n': LVAL->_int = int('\n'); break;
                              case 'f': LVAL->_int = int('\f'); break;
                              case 'r': LVAL->_int = int('\r'); break;
                              default:
                                  LVAL->_int = yytext[2]; break;
                            }

                            reader->m_charnum += yyleng;
                            return INTCONST;
                        }
	YY_BREAK
case 34:
YY_RULE_SETUP
#line 135 "Lexer.l"
{ 
                            reader->m_charnum += yyleng; 
                            unsigned int u = strtol(yytext+3,(char**)0,16);
                            LVAL->_int = u;
                            return INTCONST;
                        }
	YY_BREAK
case 35:
YY_RULE_SETUP
#line 142 "Lexer.l"
{ 
                            reader->m_charnum += yyleng; 
                            LVAL->_int = int(yytext[1]);
                            return INTCONST;
                        }
	YY_BREAK
case 36:
YY_RULE_SETUP
#line 148 "Lexer.l"
{
			    GTOdebug=!GTOdebug;
                            if (GTOdebug) debug();
			    reader->m_charnum += yyleng;
			}
	YY_BREAK
case 37:
YY_RULE_SETUP
#line 154 "Lexer.l"
{
			    reader->m_charnum += yyleng;
			    LVAL->_int = reader->internString(yytext);
                            return STRINGCONST;
			}
	YY_BREAK
case YY_STATE_EOF(INITIAL):
case YY_STATE_EOF(COMMENT):
case YY_STATE_EOF(STRING):
#line 160 "Lexer.l"
{
			    yyterminate();
			}
	YY_BREAK
case 38:
YY_RULE_SETUP
#line 164 "Lexer.l"
{
                            reader->m_charnum++;
			    return *yytext;
			}
	YY_BREAK
case 39:
YY_RULE_SETUP
#line 169 "Lexer.l"
ECHO;
	YY_BREAK
#line 944 "FlexLexer.cpp"

	case YY_END_OF_BUFFER:
		{
		/* Amount of text matched not including the EOB char. */
		int yy_amount_of_matched_text = (int) (yy_cp - yytext_ptr) - 1;

		/* Undo the effects of YY_DO_BEFORE_ACTION. */
		*yy_cp = yy_hold_char;
		YY_RESTORE_YY_MORE_OFFSET

		if ( yy_current_buffer->yy_buffer_status == YY_BUFFER_NEW )
			{
			/* We're scanning a new file or input source.  It's
			 * possible that this happened because the user
			 * just pointed yyin at a new source and called
			 * yylex().  If so, then we have to assure
			 * consistency between yy_current_buffer and our
			 * globals.  Here is the right place to do so, because
			 * this is the first action (other than possibly a
			 * back-up) that will match for the new input source.
			 */
			yy_n_chars = yy_current_buffer->yy_n_chars;
			yy_current_buffer->yy_input_file = yyin;
			yy_current_buffer->yy_buffer_status = YY_BUFFER_NORMAL;
			}

		/* Note that here we test for yy_c_buf_p "<=" to the position
		 * of the first EOB in the buffer, since yy_c_buf_p will
		 * already have been incremented past the NUL character
		 * (since all states make transitions on EOB to the
		 * end-of-buffer state).  Contrast this with the test
		 * in input().
		 */
		if ( yy_c_buf_p <= &yy_current_buffer->yy_ch_buf[yy_n_chars] )
			{ /* This was really a NUL. */
			yy_state_type yy_next_state;

			yy_c_buf_p = yytext_ptr + yy_amount_of_matched_text;

			yy_current_state = yy_get_previous_state();

			/* Okay, we're now positioned to make the NUL
			 * transition.  We couldn't have
			 * yy_get_previous_state() go ahead and do it
			 * for us because it doesn't know how to deal
			 * with the possibility of jamming (and we don't
			 * want to build jamming into it because then it
			 * will run more slowly).
			 */

			yy_next_state = yy_try_NUL_trans( yy_current_state );

			yy_bp = yytext_ptr + YY_MORE_ADJ;

			if ( yy_next_state )
				{
				/* Consume the NUL. */
				yy_cp = ++yy_c_buf_p;
				yy_current_state = yy_next_state;
				goto yy_match;
				}

			else
				{
				yy_cp = yy_c_buf_p;
				goto yy_find_action;
				}
			}

		else switch ( yy_get_next_buffer() )
			{
			case EOB_ACT_END_OF_FILE:
				{
				yy_did_buffer_switch_on_eof = 0;

				if ( yywrap() )
					{
					/* Note: because we've taken care in
					 * yy_get_next_buffer() to have set up
					 * yytext, we can now set up
					 * yy_c_buf_p so that if some total
					 * hoser (like flex itself) wants to
					 * call the scanner after we return the
					 * YY_NULL, it'll still work - another
					 * YY_NULL will get returned.
					 */
					yy_c_buf_p = yytext_ptr + YY_MORE_ADJ;

					yy_act = YY_STATE_EOF(YY_START);
					goto do_action;
					}

				else
					{
					if ( ! yy_did_buffer_switch_on_eof )
						YY_NEW_FILE;
					}
				break;
				}

			case EOB_ACT_CONTINUE_SCAN:
				yy_c_buf_p =
					yytext_ptr + yy_amount_of_matched_text;

				yy_current_state = yy_get_previous_state();

				yy_cp = yy_c_buf_p;
				yy_bp = yytext_ptr + YY_MORE_ADJ;
				goto yy_match;

			case EOB_ACT_LAST_MATCH:
				yy_c_buf_p =
				&yy_current_buffer->yy_ch_buf[yy_n_chars];

				yy_current_state = yy_get_previous_state();

				yy_cp = yy_c_buf_p;
				yy_bp = yytext_ptr + YY_MORE_ADJ;
				goto yy_find_action;
			}
		break;
		}

	default:
		YY_FATAL_ERROR(
			"fatal flex scanner internal error--no action found" );
	} /* end of action switch */
		} /* end of scanning one token */
	} /* end of yylex */

yyFlexLexer::yyFlexLexer( istream* arg_yyin, ostream* arg_yyout )
	{
	yyin = arg_yyin;
	yyout = arg_yyout;
	yy_c_buf_p = 0;
	yy_init = 1;
	yy_start = 0;
	yy_flex_debug = 0;
	yylineno = 1;	// this will only get updated if %option yylineno

	yy_did_buffer_switch_on_eof = 0;

	yy_looking_for_trail_begin = 0;
	yy_more_flag = 0;
	yy_more_len = 0;
	yy_more_offset = yy_prev_more_offset = 0;

	yy_start_stack_ptr = yy_start_stack_depth = 0;
	yy_start_stack = 0;

	yy_current_buffer = 0;

#ifdef YY_USES_REJECT
	yy_state_buf = new yy_state_type[YY_BUF_SIZE + 2];
#else
	yy_state_buf = 0;
#endif
	}

yyFlexLexer::~yyFlexLexer()
	{
	delete yy_state_buf;
	yy_delete_buffer( yy_current_buffer );
	}

void yyFlexLexer::switch_streams( istream* new_in, ostream* new_out )
	{
	if ( new_in )
		{
		yy_delete_buffer( yy_current_buffer );
		yy_switch_to_buffer( yy_create_buffer( new_in, YY_BUF_SIZE ) );
		}

	if ( new_out )
		yyout = new_out;
	}

#ifdef YY_INTERACTIVE
int yyFlexLexer::LexerInput( char* buf, int /* max_size */ )
#else
int yyFlexLexer::LexerInput( char* buf, int max_size )
#endif
	{
	if ( yyin->eof() || yyin->fail() )
		return 0;

#ifdef YY_INTERACTIVE
	yyin->get( buf[0] );

	if ( yyin->eof() )
		return 0;

	if ( yyin->bad() )
		return -1;

	return 1;

#else
	(void) yyin->read( buf, max_size );

	if ( yyin->bad() )
		return -1;
	else
		return yyin->gcount();
#endif
	}

void yyFlexLexer::LexerOutput( const char* buf, int size )
	{
	(void) yyout->write( buf, size );
	}

/* yy_get_next_buffer - try to read in a new buffer
 *
 * Returns a code representing an action:
 *	EOB_ACT_LAST_MATCH -
 *	EOB_ACT_CONTINUE_SCAN - continue scanning from current position
 *	EOB_ACT_END_OF_FILE - end of file
 */

int yyFlexLexer::yy_get_next_buffer()
	{
	register char *dest = yy_current_buffer->yy_ch_buf;
	register char *source = yytext_ptr;
	register int number_to_move, i;
	int ret_val;

	if ( yy_c_buf_p > &yy_current_buffer->yy_ch_buf[yy_n_chars + 1] )
		YY_FATAL_ERROR(
		"fatal flex scanner internal error--end of buffer missed" );

	if ( yy_current_buffer->yy_fill_buffer == 0 )
		{ /* Don't try to fill the buffer, so this is an EOF. */
		if ( yy_c_buf_p - yytext_ptr - YY_MORE_ADJ == 1 )
			{
			/* We matched a single character, the EOB, so
			 * treat this as a final EOF.
			 */
			return EOB_ACT_END_OF_FILE;
			}

		else
			{
			/* We matched some text prior to the EOB, first
			 * process it.
			 */
			return EOB_ACT_LAST_MATCH;
			}
		}

	/* Try to read more data. */

	/* First move last chars to start of buffer. */
	number_to_move = (int) (yy_c_buf_p - yytext_ptr) - 1;

	for ( i = 0; i < number_to_move; ++i )
		*(dest++) = *(source++);

	if ( yy_current_buffer->yy_buffer_status == YY_BUFFER_EOF_PENDING )
		/* don't do the read, it's not guaranteed to return an EOF,
		 * just force an EOF
		 */
		yy_current_buffer->yy_n_chars = yy_n_chars = 0;

	else
		{
		int num_to_read =
			yy_current_buffer->yy_buf_size - number_to_move - 1;

		while ( num_to_read <= 0 )
			{ /* Not enough room in the buffer - grow it. */
#ifdef YY_USES_REJECT
			YY_FATAL_ERROR(
"input buffer overflow, can't enlarge buffer because scanner uses REJECT" );
#else

			/* just a shorter name for the current buffer */
			YY_BUFFER_STATE b = yy_current_buffer;

			int yy_c_buf_p_offset =
				(int) (yy_c_buf_p - b->yy_ch_buf);

			if ( b->yy_is_our_buffer )
				{
				int new_size = b->yy_buf_size * 2;

				if ( new_size <= 0 )
					b->yy_buf_size += b->yy_buf_size / 8;
				else
					b->yy_buf_size *= 2;

				b->yy_ch_buf = (char *)
					/* Include room in for 2 EOB chars. */
					yy_flex_realloc( (void *) b->yy_ch_buf,
							 b->yy_buf_size + 2 );
				}
			else
				/* Can't grow it, we don't own it. */
				b->yy_ch_buf = 0;

			if ( ! b->yy_ch_buf )
				YY_FATAL_ERROR(
				"fatal error - scanner input buffer overflow" );

			yy_c_buf_p = &b->yy_ch_buf[yy_c_buf_p_offset];

			num_to_read = yy_current_buffer->yy_buf_size -
						number_to_move - 1;
#endif
			}

		if ( num_to_read > YY_READ_BUF_SIZE )
			num_to_read = YY_READ_BUF_SIZE;

		/* Read in more data. */
		YY_INPUT( (&yy_current_buffer->yy_ch_buf[number_to_move]),
			yy_n_chars, num_to_read );

		yy_current_buffer->yy_n_chars = yy_n_chars;
		}

	if ( yy_n_chars == 0 )
		{
		if ( number_to_move == YY_MORE_ADJ )
			{
			ret_val = EOB_ACT_END_OF_FILE;
			yyrestart( yyin );
			}

		else
			{
			ret_val = EOB_ACT_LAST_MATCH;
			yy_current_buffer->yy_buffer_status =
				YY_BUFFER_EOF_PENDING;
			}
		}

	else
		ret_val = EOB_ACT_CONTINUE_SCAN;

	yy_n_chars += number_to_move;
	yy_current_buffer->yy_ch_buf[yy_n_chars] = YY_END_OF_BUFFER_CHAR;
	yy_current_buffer->yy_ch_buf[yy_n_chars + 1] = YY_END_OF_BUFFER_CHAR;

	yytext_ptr = &yy_current_buffer->yy_ch_buf[0];

	return ret_val;
	}


/* yy_get_previous_state - get the state just before the EOB char was reached */

yy_state_type yyFlexLexer::yy_get_previous_state()
	{
	register yy_state_type yy_current_state;
	register char *yy_cp;

	yy_current_state = yy_start;

	for ( yy_cp = yytext_ptr + YY_MORE_ADJ; yy_cp < yy_c_buf_p; ++yy_cp )
		{
		register YY_CHAR yy_c = (*yy_cp ? yy_ec[YY_SC_TO_UI(*yy_cp)] : 1);
		if ( yy_accept[yy_current_state] )
			{
			yy_last_accepting_state = yy_current_state;
			yy_last_accepting_cpos = yy_cp;
			}
		while ( yy_chk[yy_base[yy_current_state] + yy_c] != yy_current_state )
			{
			yy_current_state = (int) yy_def[yy_current_state];
			if ( yy_current_state >= 129 )
				yy_c = yy_meta[(unsigned int) yy_c];
			}
		yy_current_state = yy_nxt[yy_base[yy_current_state] + (unsigned int) yy_c];
		}

	return yy_current_state;
	}


/* yy_try_NUL_trans - try to make a transition on the NUL character
 *
 * synopsis
 *	next_state = yy_try_NUL_trans( current_state );
 */

yy_state_type yyFlexLexer::yy_try_NUL_trans( yy_state_type yy_current_state )
	{
	register int yy_is_jam;
	register char *yy_cp = yy_c_buf_p;

	register YY_CHAR yy_c = 1;
	if ( yy_accept[yy_current_state] )
		{
		yy_last_accepting_state = yy_current_state;
		yy_last_accepting_cpos = yy_cp;
		}
	while ( yy_chk[yy_base[yy_current_state] + yy_c] != yy_current_state )
		{
		yy_current_state = (int) yy_def[yy_current_state];
		if ( yy_current_state >= 129 )
			yy_c = yy_meta[(unsigned int) yy_c];
		}
	yy_current_state = yy_nxt[yy_base[yy_current_state] + (unsigned int) yy_c];
	yy_is_jam = (yy_current_state == 128);

	return yy_is_jam ? 0 : yy_current_state;
	}


void yyFlexLexer::yyunput( int c, register char* yy_bp )
	{
	register char *yy_cp = yy_c_buf_p;

	/* undo effects of setting up yytext */
	*yy_cp = yy_hold_char;

	if ( yy_cp < yy_current_buffer->yy_ch_buf + 2 )
		{ /* need to shift things up to make room */
		/* +2 for EOB chars. */
		register int number_to_move = yy_n_chars + 2;
		register char *dest = &yy_current_buffer->yy_ch_buf[
					yy_current_buffer->yy_buf_size + 2];
		register char *source =
				&yy_current_buffer->yy_ch_buf[number_to_move];

		while ( source > yy_current_buffer->yy_ch_buf )
			*--dest = *--source;

		yy_cp += (int) (dest - source);
		yy_bp += (int) (dest - source);
		yy_current_buffer->yy_n_chars =
			yy_n_chars = yy_current_buffer->yy_buf_size;

		if ( yy_cp < yy_current_buffer->yy_ch_buf + 2 )
			YY_FATAL_ERROR( "flex scanner push-back overflow" );
		}

	*--yy_cp = (char) c;


	yytext_ptr = yy_bp;
	yy_hold_char = *yy_cp;
	yy_c_buf_p = yy_cp;
	}


int yyFlexLexer::yyinput()
	{
	int c;

	*yy_c_buf_p = yy_hold_char;

	if ( *yy_c_buf_p == YY_END_OF_BUFFER_CHAR )
		{
		/* yy_c_buf_p now points to the character we want to return.
		 * If this occurs *before* the EOB characters, then it's a
		 * valid NUL; if not, then we've hit the end of the buffer.
		 */
		if ( yy_c_buf_p < &yy_current_buffer->yy_ch_buf[yy_n_chars] )
			/* This was really a NUL. */
			*yy_c_buf_p = '\0';

		else
			{ /* need more input */
			int offset = yy_c_buf_p - yytext_ptr;
			++yy_c_buf_p;

			switch ( yy_get_next_buffer() )
				{
				case EOB_ACT_LAST_MATCH:
					/* This happens because yy_g_n_b()
					 * sees that we've accumulated a
					 * token and flags that we need to
					 * try matching the token before
					 * proceeding.  But for input(),
					 * there's no matching to consider.
					 * So convert the EOB_ACT_LAST_MATCH
					 * to EOB_ACT_END_OF_FILE.
					 */

					/* Reset buffer status. */
					yyrestart( yyin );

					/* fall through */

				case EOB_ACT_END_OF_FILE:
					{
					if ( yywrap() )
						return EOF;

					if ( ! yy_did_buffer_switch_on_eof )
						YY_NEW_FILE;
#ifdef __cplusplus
					return yyinput();
#else
					return input();
#endif
					}

				case EOB_ACT_CONTINUE_SCAN:
					yy_c_buf_p = yytext_ptr + offset;
					break;
				}
			}
		}

	c = *(unsigned char *) yy_c_buf_p;	/* cast for 8-bit char's */
	*yy_c_buf_p = '\0';	/* preserve yytext */
	yy_hold_char = *++yy_c_buf_p;


	return c;
	}


void yyFlexLexer::yyrestart( istream* input_file )
	{
	if ( ! yy_current_buffer )
		yy_current_buffer = yy_create_buffer( yyin, YY_BUF_SIZE );

	yy_init_buffer( yy_current_buffer, input_file );
	yy_load_buffer_state();
	}


void yyFlexLexer::yy_switch_to_buffer( YY_BUFFER_STATE new_buffer )
	{
	if ( yy_current_buffer == new_buffer )
		return;

	if ( yy_current_buffer )
		{
		/* Flush out information for old buffer. */
		*yy_c_buf_p = yy_hold_char;
		yy_current_buffer->yy_buf_pos = yy_c_buf_p;
		yy_current_buffer->yy_n_chars = yy_n_chars;
		}

	yy_current_buffer = new_buffer;
	yy_load_buffer_state();

	/* We don't actually know whether we did this switch during
	 * EOF (yywrap()) processing, but the only time this flag
	 * is looked at is after yywrap() is called, so it's safe
	 * to go ahead and always set it.
	 */
	yy_did_buffer_switch_on_eof = 1;
	}


void yyFlexLexer::yy_load_buffer_state()
	{
	yy_n_chars = yy_current_buffer->yy_n_chars;
	yytext_ptr = yy_c_buf_p = yy_current_buffer->yy_buf_pos;
	yyin = yy_current_buffer->yy_input_file;
	yy_hold_char = *yy_c_buf_p;
	}


YY_BUFFER_STATE yyFlexLexer::yy_create_buffer( istream* file, int size )
	{
	YY_BUFFER_STATE b;

	b = (YY_BUFFER_STATE) yy_flex_alloc( sizeof( struct yy_buffer_state ) );
	if ( ! b )
		YY_FATAL_ERROR( "out of dynamic memory in yy_create_buffer()" );

	b->yy_buf_size = size;

	/* yy_ch_buf has to be 2 characters longer than the size given because
	 * we need to put in 2 end-of-buffer characters.
	 */
	b->yy_ch_buf = (char *) yy_flex_alloc( b->yy_buf_size + 2 );
	if ( ! b->yy_ch_buf )
		YY_FATAL_ERROR( "out of dynamic memory in yy_create_buffer()" );

	b->yy_is_our_buffer = 1;

	yy_init_buffer( b, file );

	return b;
	}


void yyFlexLexer::yy_delete_buffer( YY_BUFFER_STATE b )
	{
	if ( ! b )
		return;

	if ( b == yy_current_buffer )
		yy_current_buffer = (YY_BUFFER_STATE) 0;

	if ( b->yy_is_our_buffer )
		yy_flex_free( (void *) b->yy_ch_buf );

	yy_flex_free( (void *) b );
	}


extern "C" int isatty YY_PROTO(( int ));
void yyFlexLexer::yy_init_buffer( YY_BUFFER_STATE b, istream* file )

	{
	yy_flush_buffer( b );

	b->yy_input_file = file;
	b->yy_fill_buffer = 1;

	b->yy_is_interactive = 0;
	}


void yyFlexLexer::yy_flush_buffer( YY_BUFFER_STATE b )
	{
	if ( ! b )
		return;

	b->yy_n_chars = 0;

	/* We always need two end-of-buffer characters.  The first causes
	 * a transition to the end-of-buffer state.  The second causes
	 * a jam in that state.
	 */
	b->yy_ch_buf[0] = YY_END_OF_BUFFER_CHAR;
	b->yy_ch_buf[1] = YY_END_OF_BUFFER_CHAR;

	b->yy_buf_pos = &b->yy_ch_buf[0];

	b->yy_at_bol = 1;
	b->yy_buffer_status = YY_BUFFER_NEW;

	if ( b == yy_current_buffer )
		yy_load_buffer_state();
	}


#ifndef YY_NO_SCAN_BUFFER
#endif


#ifndef YY_NO_SCAN_STRING
#endif


#ifndef YY_NO_SCAN_BYTES
#endif


#ifndef YY_NO_PUSH_STATE
void yyFlexLexer::yy_push_state( int new_state )
	{
	if ( yy_start_stack_ptr >= yy_start_stack_depth )
		{
		yy_size_t new_size;

		yy_start_stack_depth += YY_START_STACK_INCR;
		new_size = yy_start_stack_depth * sizeof( int );

		if ( ! yy_start_stack )
			yy_start_stack = (int *) yy_flex_alloc( new_size );

		else
			yy_start_stack = (int *) yy_flex_realloc(
					(void *) yy_start_stack, new_size );

		if ( ! yy_start_stack )
			YY_FATAL_ERROR(
			"out of memory expanding start-condition stack" );
		}

	yy_start_stack[yy_start_stack_ptr++] = YY_START;

	BEGIN(new_state);
	}
#endif


#ifndef YY_NO_POP_STATE
void yyFlexLexer::yy_pop_state()
	{
	if ( --yy_start_stack_ptr < 0 )
		YY_FATAL_ERROR( "start-condition stack underflow" );

	BEGIN(yy_start_stack[yy_start_stack_ptr]);
	}
#endif


#ifndef YY_NO_TOP_STATE
int yyFlexLexer::yy_top_state()
	{
	return yy_start_stack[yy_start_stack_ptr - 1];
	}
#endif

#ifndef YY_EXIT_FAILURE
#define YY_EXIT_FAILURE 2
#endif


void yyFlexLexer::LexerError( yyconst char msg[] )
	{
	cerr << msg << '\n';
	exit( YY_EXIT_FAILURE );
	}


/* Redefine yyless() so it works in section 3 code. */

#undef yyless
#define yyless(n) \
	do \
		{ \
		/* Undo effects of setting up yytext. */ \
		yytext[yyleng] = yy_hold_char; \
		yy_c_buf_p = yytext + n; \
		yy_hold_char = *yy_c_buf_p; \
		*yy_c_buf_p = '\0'; \
		yyleng = n; \
		} \
	while ( 0 )


/* Internal utility routines. */

#ifndef yytext_ptr
#ifdef YY_USE_PROTOS
static void yy_flex_strncpy( char *s1, yyconst char *s2, int n )
#else
static void yy_flex_strncpy( s1, s2, n )
char *s1;
yyconst char *s2;
int n;
#endif
	{
	register int i;
	for ( i = 0; i < n; ++i )
		s1[i] = s2[i];
	}
#endif

#ifdef YY_NEED_STRLEN
#ifdef YY_USE_PROTOS
static int yy_flex_strlen( yyconst char *s )
#else
static int yy_flex_strlen( s )
yyconst char *s;
#endif
	{
	register int n;
	for ( n = 0; s[n]; ++n )
		;

	return n;
	}
#endif


#ifdef YY_USE_PROTOS
static void *yy_flex_alloc( yy_size_t size )
#else
static void *yy_flex_alloc( size )
yy_size_t size;
#endif
	{
	return (void *) malloc( size );
	}

#ifdef YY_USE_PROTOS
static void *yy_flex_realloc( void *ptr, yy_size_t size )
#else
static void *yy_flex_realloc( ptr, size )
void *ptr;
yy_size_t size;
#endif
	{
	/* The cast to (char *) in the following accommodates both
	 * implementations that use char* generic pointers, and those
	 * that use void* generic pointers.  It works with the latter
	 * because both ANSI C and C++ allow castless assignment from
	 * any pointer type to void*, and deal with argument conversions
	 * as though doing an assignment.
	 */
	return (void *) realloc( (char *) ptr, size );
	}

#ifdef YY_USE_PROTOS
static void yy_flex_free( void *ptr )
#else
static void yy_flex_free( ptr )
void *ptr;
#endif
	{
	free( ptr );
	}

#if YY_MAIN
int main()
	{
	yylex();
	return 0;
	}
#endif
#line 169 "Lexer.l"


void 
yyFlexLexer::yyNewString()
{
#if COMPILER == GCC2_96
    stringBuffer = "";
#else
    stringBuffer.clear();
#endif
}

void 
yyFlexLexer::yyAddToString(char c)
{
    stringBuffer.push_back(c);
}

int
yyFlexLexer::yyReturnString()
{
    return reader->internString(stringBuffer);
}

void
yyFlexLexer::init(Reader* r)
{
    reader = r; 
    reader->m_linenum = 1;
    reader->m_charnum = 1;
}
