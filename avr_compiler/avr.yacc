{%
#import <stdio.h>
#import <stdlib.h>
%}

%token MAIN


%%
program:
  program definition ';'
  |
  ;

definition:
  type name
  | type name '=' value
  ;

type:
  INTEGRAL_TYPE
  | '[' typelist ']'
  | '<' typelist '>'
  ;

typelist:
  type
  | typelist ',' type
  ;

%%
