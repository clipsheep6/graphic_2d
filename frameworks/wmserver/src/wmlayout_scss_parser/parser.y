%skeleton "lalr1.cc"
%require "3.0.4"
%define api.namespace { OHOS }
%define api.parser.class { Parser }
%define api.token.constructor
%define api.value.type variant
%define api.token.prefix { TOKEN_ }
%define parse.assert
%defines
%code requires
{
    #include <iostream>
    #include <string>
    #include <vector>
    #include <stdint.h>
    #include <cmath>
    using namespace std;

    namespace OHOS {
        class Scanner;
        class Driver;
    }
}

%code top
{
    #include <iostream>
    #include "scanner.h"
    #include "parser.hpp"
    #include "driver.h"
    #include "location.hh"

    static OHOS::Parser::symbol_type yylex(OHOS::Scanner& scanner,OHOS::Driver &driver)
    {
        return scanner.nextToken();
    }
    using namespace OHOS;
}

%lex-param { OHOS::Scanner& scanner }
%lex-param { OHOS::Driver& driver }
%parse-param { OHOS::Scanner& scanner }
%parse-param { OHOS::Driver& driver }

%locations
// %define parse-trace
%define parse.error verbose

%token END 0

/* import from lexer.l */
%token<string> SELECTOR
%token<string> ATTRIBUTE
%token<string> VALUE
%token CHAR_L_BRACE              /* ( */
%token CHAR_R_BRACE              /* ) */
%token CHAR_COLON                /* : */
%token CHAR_SEMICOLON            /* ; */

/* non-terminate-symbol */
%start statements
%type<string> css_block css_block_begin css_block_end
%type<string> declare

%%

statements: css_block statements
{
}

statements: declare statements
{
}

statements: %empty
{
}

css_block: css_block_begin css_block_end
{
}

css_block_begin: SELECTOR CHAR_L_BRACE
{
    driver.current->blocks[$1].parent = driver.current;
    driver.current = &driver.current->blocks[$1];
}

css_block_end: statements CHAR_R_BRACE
{
    driver.current = driver.current->parent;
}

declare: ATTRIBUTE CHAR_COLON VALUE CHAR_SEMICOLON
{
    $$ = $1 + ": " + $3 + ";";
    driver.current->declares[$1] = $3;
}

%%

void OHOS::Parser::error(const OHOS::location& location,const std::string& message)
{
    printf("error: (%d,%d) - (%d,%d): %s\n",
          location.begin.line, location.begin.column,
          location.end.line, location.end.column,
          message.c_str());
}
