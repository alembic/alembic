%{
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

%}

//%defines
//%name-prefix="GTO"

%union
{
    int             _token;
    int             _int;
    unsigned int    _uint;
    double          _double;
    Gto::TypeSpec   _type;
    Gto::Number     _number;
}

//
//  Make it reentrant
//

%pure_parser

%token <_int>       GTO_STRINGCONST
%token <_int>       GTO_INTCONST
%token <_double>    GTO_FLOATCONST
%token <_int>       GTO_GTOID
%token <_token>     GTO_AS
%token <_token>     GTO_INTTYPE
%token <_token>     GTO_FLOATTYPE
%token <_token>     GTO_DOUBLETYPE
%token <_token>     GTO_STRINGTYPE
%token <_token>     GTO_HALFTYPE
%token <_token>     GTO_BOOLTYPE
%token <_token>     GTO_SHORTTYPE
%token <_token>     GTO_BYTETYPE
%token <_token>     GTO_ELLIPSIS

%type <_type>   type
%type <_type>   basic_type
%type <_int>    interp_string_opt
%type <_double> float_num
%type <_int>    int_num
%type <_number> numeric_value
%type <_uint>   numeric_value_list;
%type <_uint>   atomic_value_list;
%type <_uint>   string_value_list;
%type <_int>    string_value;
%type <_token>  complex_element_list

%start file

%%

file:
    GTO_GTOID 
    {
        READER->beginHeader(GTO_VERSION);
    }
    object_list 
    { 
        READER->endFile(); 
    }

    | GTO_GTOID '(' GTO_INTCONST ')' 
    {
        READER->beginHeader($3);
    }
    object_list 
    { 
        READER->endFile(); 
    }
;

object_list:
    object
    | object_list object
;

object:
    GTO_STRINGCONST '{' 
    {
        READER->beginObject($1, READER->internString("object"));
    }
    component_list '}'

    | GTO_STRINGCONST ':' GTO_STRINGCONST '{' 
    {
        READER->beginObject($1, $3);
    }
    component_list '}'

    | GTO_STRINGCONST ':' GTO_STRINGCONST '(' GTO_INTCONST ')' '{' 
    {
        READER->beginObject($1, $3, $5);
    }
    component_list '}'
;

component_list:
    component
    | component_list component
;

interp_string_opt:
    /* empty */
    {
        $$ = READER->internString("");
    }

    | GTO_AS GTO_STRINGCONST
    {
        $$ = $2;
    }
;

component:
    GTO_STRINGCONST interp_string_opt '{' 
    {
        READER->beginComponent($1, $2);
    }
    property_list '}'
;

property_list:
    property
    | property_list property
;

property:
    type GTO_STRINGCONST interp_string_opt '=' 
    {
        READER->beginProperty($2, $3, $1.width, $1.size, $1.type);
    }
    atomic_value
    {
        if (READER->currentType().width != 1)
        {
            GTOParseError(state, "expected data width of %d, found 1",
                          READER->currentType().width);
            YYERROR;
        }
        else if ($1.size != 0 && 
                 READER->numAtomicValuesInBuffer() != $1.size)
        {
            GTOParseError(state, 
                          "property size mismatch, found %d, expect %d",
                          READER->numAtomicValuesInBuffer(),
                          $1.size);
            YYERROR;
        }
        else
        {
            READER->endProperty();
        }
    }

    | type GTO_STRINGCONST interp_string_opt '=' 
    {
        READER->beginProperty($2, $3, $1.width, $1.size, $1.type);
    }
    '[' complex_element_list ']'
    {
        size_t nelements = READER->numElementsInBuffer();

        if ($1.size != 0 && nelements != $1.size)
        {
            if ($7 == GTO_ELLIPSIS)
            {
                READER->fillToSize($1.size);
                READER->endProperty();
            }
            else
            {
                GTOParseError(state, 
                              "property size mismatch, found %d, expect %d",
                              nelements,
                              $1.size);
                YYERROR;
            }
        }
        else if ($1.size == 0 && $7 == GTO_ELLIPSIS)
        {
            GTOParseError(state,
              "use of ... requires fixed property size but none was provided");
            YYERROR;
        }
        else
        {
            READER->endProperty();
        }
    }
;

type:
    basic_type
    {
        $$.type  = $1.type;
        $$.width = 1;
        $$.size  = 0;
    }

    | basic_type '[' GTO_INTCONST ']'
    {
        $$.type  = $1.type;
        $$.width = $3;
        $$.size  = 0;
    }

    | basic_type '[' GTO_INTCONST ']' '[' GTO_INTCONST ']'
    {
        $$.type  = $1.type;
        $$.width = $3;
        $$.size  = $6;
    }
;

basic_type:
    GTO_FLOATTYPE       { $$.type = Gto::Float; }
    | GTO_INTTYPE       { $$.type = Gto::Int; }
    | GTO_STRINGTYPE    { $$.type = Gto::String; }
    | GTO_SHORTTYPE     { $$.type = Gto::Short; }
    | GTO_BYTETYPE      { $$.type = Gto::Byte; }
    | GTO_HALFTYPE      { $$.type = Gto::Half; }
    | GTO_BOOLTYPE      { $$.type = Gto::Boolean; }
    | GTO_DOUBLETYPE    { $$.type = Gto::Double; }
;

complex_element_list:
    /* empty */ { $$ = 0; }
    | element_list { $$ = 0; }
    | element_list GTO_ELLIPSIS { $$ = GTO_ELLIPSIS; }
;

element_list:
    element
    | element_list element
;

element:
    atomic_value
    | '[' atomic_value_list ']'
    {
        if ($2 != READER->currentType().width)
        {
            GTOParseError(state, "expected data width of %d, found %d",
                          READER->currentType().width, $2);
            YYERROR;
        }
    }
;

atomic_value_list:
    string_value_list 
    | numeric_value_list
;

atomic_value:
    string_value { }
    | numeric_value { }
;

string_value_list:
    string_value { $$ = 1; }
    | string_value_list string_value { $$ = $1 + 1; }
;

string_value:
    GTO_STRINGCONST
    {
        if (READER->currentType().type != Gto::String)
        {
            GTOParseError(state, 
                          "expected a numeric value, found string \"%s\"",
                          READER->stringFromId($1).c_str());
            YYERROR;
        }
        else
        {
            READER->addToPropertyBuffer($1);
        }

        $$ = $1;
    }
;

numeric_value_list:
    numeric_value { $$ = 1; }
    | numeric_value_list numeric_value { $$ = $1 + 1; }
;

numeric_value:
    float_num
    {
        Gto::DataType t = Gto::DataType(READER->properties().back().type);

        switch (t)
        {
          case Gto::Int:
              if ($1 != int($1))
              {
                  GTOParseWarning(state, "floating point value truncated "
                                  "to match integer property type (%f => %d)",
                                  $1, int($1));
              }
              
              $$.type = Gto::Int;
              $$._int = int($1);
              READER->addToPropertyBuffer(int($1));
              break;
              
          case Gto::Float:
              $$.type = Gto::Float;
              $$._double = $1;
              READER->addToPropertyBuffer(float($1));
              break;

          case Gto::Double:
              $$.type = Gto::Double;
              $$._double = $1;
              READER->addToPropertyBuffer(double($1));
              break;

#ifdef GTO_SUPPORT_HALF
          case Gto::Half:
              $$.type = Gto::Half;
              $$._double = $1;
              READER->addToPropertyBuffer(half($1));
              break;
#else
          case Gto::Half:
              GTOParseError(state, "numeric type '%s' is currently unsupported "
                            "by the parser", typeName(t));
              YYERROR;
#endif

          case Gto::Short:
              if ($1 != short($1))
              {
                  GTOParseWarning(state, "floating point value truncated "
                                  "to match short property type (%f => %d)",
                                  $1, int(short($1)));
              }
              
              $$.type = Gto::Short;
              $$._int = short($1);
              READER->addToPropertyBuffer(short($1));
              break;

          case Gto::Byte:
              if ($1 != (unsigned char)($1))
              {
                  GTOParseWarning(state, "floating point value truncated "
                                  "to match byte property type (%f => %d)",
                                  $1, int((unsigned char)($1)));
              }
              
              $$.type = Gto::Byte;
              $$._int = (unsigned char)($1);
              READER->addToPropertyBuffer((unsigned char)($1));
              break;

          case Gto::String:
              GTOParseError(state, "string expected; got a floating "
                            "point number (%f) instead", $1);
              YYERROR;
              break;

          default:
              GTOParseError(state, "unsupported type '%s'; got a floating "
                            "point number (%f) instead", typeName(t), $1);
              YYERROR;
              break;
        }
    }

    | int_num
    {
        Gto::DataType t = Gto::DataType(READER->properties().back().type);

        switch (t)
        {
          case Gto::Int:
              $$.type = Gto::Int;
              $$._int = $1;
              READER->addToPropertyBuffer(int($1));
              break;
              
          case Gto::Float:
              if ($1 != float($1))
              {
                  GTOParseWarning(state, "integer cannot be represented "
                                  "as floating point (%d => %f)",
                                  $1, float($1));
              }
              
              $$.type = Gto::Float;
              $$._double = $1;
              READER->addToPropertyBuffer(float($1));
              break;

          case Gto::Double:
              if ($1 != double($1))
              {
                  GTOParseWarning(state, "integer cannot be represented "
                                  "as double (%d => %f)",
                                  $1, double($1));
              }
              
              $$.type = Gto::Double;
              $$._double = $1;
              READER->addToPropertyBuffer(double($1));
              break;

#ifdef GTO_SUPPORT_HALF
          case Gto::Half:
              if ($1 != half($1))
              {
                  GTOParseWarning(state, "integer cannot be represented "
                                  "as half (%d => %f)",
                                  $1, float($1));
              }
              
              $$.type = Gto::Half;
              $$._double = $1;
              READER->addToPropertyBuffer(half($1));
              break;
#endif

          case Gto::Short:
              if ($1 != short($1))
              {
                  GTOParseWarning(state, "integer cannot be represented "
                                  "as short (%d => %d)",
                                  $1, short($1));
              }
              
              $$.type = Gto::Short;
              $$._int = short($1);
              READER->addToPropertyBuffer(short($1));
              break;

          case Gto::Byte:
              if ($1 != (unsigned char)($1))
              {
                  GTOParseWarning(state, "integer cannot be represented "
                                  "as byte (%d => %d)",
                                  $1, (unsigned char)($1));
              }
              
              $$.type = Gto::Byte;
              $$._int = (unsigned char)($1);
              READER->addToPropertyBuffer((unsigned char)($1));
              break;

          case Gto::String:
              GTOParseError(state, "string expected; got an integer "
                            "(%d) instead", $1);
              YYERROR;
              break;

          case Gto::Boolean:
              GTOParseError(state, "numeric type '%s' is currently unsupported "
                            "by the parser", typeName(t));
              YYERROR;
              break;

          default:
              GTOParseError(state, "unsupported type '%s'; got an integer "
                            "(%d) instead", typeName(t), $1);
              YYERROR;
              break;
        }
    }
;

float_num:
    GTO_FLOATCONST { $$ = $1; }
    | '-' GTO_FLOATCONST { $$ = -$2; }
;

int_num:
    GTO_INTCONST { $$ = $1; }
    | '-' GTO_INTCONST { $$ = -$2; }
;


%%

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


