%{
  #include "compiler.h"
%}

/*%option reentrant*/
%option header-file="./flex.h"

%%
  /*keywords*/
"type"                              return TYPE;
"auto"                              return AUTO;
"if"                                return IF;
"elif"                              return ELIF;
"else"                              return ELSE;
"while"                             return WHILE;
"return"                            return RETURN;
  /*comparison*/
"=="                                return EQ;
"!="                                return NEQ;
"<="                                return LTE;
">="                                return GTE;

  /*special characters*/
":"                                 return COLON;
"!"                                 return EXCLAIM;
"@"                                 return AT;
"="                                 return ASSIGN;

";"                                 return SEMICOLON;
","                                 return COMMA;


  /*groups*/
"<"                                 return LANGLE;
">"                                 return RANGLE;
"("                                 return LPAREN;
")"                                 return RPAREN;
"["                                 return LBRACKET;
"]"                                 return RBRACKET;
"{"                                 return LBRACE;
"}"                                 return RBRACE;

  /*literals*/
[a-zA-Z_]+[a-zA-Z0-9_]*             return IDENTIFIER;
[0-9]+                              return BASE10;
"0b"[0-1]+                          return BASE2;
"0"("h"|"x")[0-9a-fA-F]+            return BASE16;

[ \t\r\n]                           ;
<<EOF>>                             return _EOF;
.                                   ;
