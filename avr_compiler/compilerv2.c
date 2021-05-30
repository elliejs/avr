#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "flex.h"
#include "compilerv2.h"

char *token_names[] = {
  "_EOF", "TYPE", "AUTO" "IF", "ELIF", "ELSE", "WHILE", "RETURN",

  "EQ", "NEQ", "LTE", "GTE",

  "COLON", "EXCLAIM", "AT", "ASSIGN", "SEMICOLON", "COMMA",

  "LANGLE", "RANGLE", "LPAREN", "RPAREN", "LBRACKET", "RBRACKET", "LBRACE", "RBRACE",

  "IDENTIFIER", "BASE10", "BASE2", "BASE16"
};

struct type newType(int len, char *name) {
  struct type t = {
    .name = malloc(len * sizeof(char));
    .name_len = len;
    .size = -1;
    .entity = NULL;
    .alloc_owner = T;
  };
  strncpy(t.name, name, len);
  // return t;
}

int main(int argc, char const *argv[]) {
  //OPEN INPUT FILE FOR READING AND OUTPUT FOR WRITING AND SUCH
  if (argc < 2) {
    printf("No input file provided.\n");
    exit(EXIT_FAILURE);
  }
  const char *in_name = argv[1];
  int in_len = strlen(in_name);
  FILE *input = fopen(in_name, "r");

  if(strcmp(in_name + in_len - 2, ".e")) {
    printf("file: %s was not an .e file. Won't compile this, sorry\n", argv[1]);
    exit(EXIT_FAILURE);
  }
  if(!input) {
    printf("could not open file: %s\n", argv[1]);
    exit(EXIT_FAILURE);
  }

  char *out_name = malloc((in_len + 1) * sizeof(char));
  strcpy(out_name, in_name);
  out_name[in_len - 1] = 'o';

  FILE *output = fopen(out_name, "w+");

  //PARSE INTO MANAGABLE FORMAT FROM CHARACTERS
  //setting up the type table, (to be added to with user types)
  struct type_table_t type_table = {.max_len = 50, .len = 2, .table = malloc(50 * sizeof(struct type))};
  {
    struct type table_inherents = newType(3, "reg");
    table_inherents.size = 8;
    table_inherents.entity = (struct type_internal) {.which = BASIC};
    type_table.table[0] = table_inherents;
    table_inherents = newType(4, "word");
    table_inherents.size = 16;
    table_inherents.entity = (struct type_internal) {.which = BASIC};
    type_table.table[1] = table_inherents;
  }
  yyin = input;

  enum tokens tok = yylex();
  while(tok != _EOF) {
    //token processing here:
    if(tok == TYPE) { //then we're adding to the type table
      checkTypeTableLength(&type_table); //increments len and assures len - 1 points to an allocated free slot
      verify(IDENTIFIER);
      for(int i = 0; i < type_table.len - 1; i++) {
        if(yyleng == type_table.table[i].name_len &&
          !strncmp(yytext, type_table.table[i].name, yyleng)) {
          printf("can't overwrite previous type definitions. (%.*s)\n", yyleng, yytext);
          exit(EXIT_FAILURE);
        }
      }

      struct type new_type = newType(yyleng, yytext);
      verify(ASSIGN);
      parseType(&type_table, &new_type, yylex());
      type_table.table[type_table.len - 1] = new_type;

      tok = yylex();
    } else if (tok == IDENTIFIER) {

    }
  }

  yylex_destroy();
  cleanup_type_table(&type_table);
  fclose(input);
  fclose(output);
  free(out_name);
  return EXIT_SUCCESS;
}

int parseType(struct type_table_t *type_table, struct type *new_type, enum tokens match) {
  //should only run into types already in the type_table here
  printf("parsing type: %.*s\n", yyleng, yytext);
  switch(match) {
    case AT: ;
      new_type->indirection++;
      parseType(type_table, new_type); //please do TCO
      new_type.size =

    case IDENTIFIER: ;
      for(int i = 0; i < type_table->len - 1; i++) {
        printf("matching %.*s to %.*s\n", yyleng, yytext, type_table->table[i].name_len, type_table->table[i].name);
        if(type_table->table[i].name_len == yyleng &&
          !strncmp(type_table->table[i].name, yytext, yyleng)) {
          printf("found\n");
          //the new type's defined as type exists! copy metadata
          new_type->size = type_table->table[i].size;
          new_type->indirection = type_table->table[i].indirection;
          new_type->family = type_table->table[i].family;
          new_type->compound = type_table->table[i].compound;
          new_type->referential = 1; //matching a name *is* a referential link
          return new_type->size;
        }
      }
      printf("could not find definition of %.*s, which was expected to be an extant type\n", yyleng, yytext);
      exit(EXIT_FAILURE);

    case LANGLE: ;
    case LBRACKET: ;
      new_type->family = COMPOUND;
      new_type->compound.type = match == LANGLE ? OR : AND;
      parseCompound(type_table, new_type);
      return new_type->size;

      new_type->family = COMPOUND;
      new_type->data.compound.type = AND;
      new_type->referential = 0;
      parseCompound(type_table, new_type);
      return new_type->size;

    case LPAREN: ;
      new_type->family = COMPOUND;
      new_type->data.compound.type = FUNC;
      new_type->referential = 0;

      new_type->data.compound.len = 2;
      new_type->data.compound.bundle = malloc(2 * sizeof(struct type));
      struct type *input = &(new_type->data.compound.bundle[0]);
      input->family = COMPOUND;
      input->data.compound.type = FUNC;
      input->referential = 0;
      input->name = NULL;
      input->name_len = 0;
      new_type->size = parseCompound(type_table, input); //maybe functions are actually the size of a pointer?
      verify(COLON);
      struct type *output = &(new_type->data.compound.bundle[1]);
      output->name = NULL;
      output->name_len = 0;
      parseType(type_table, output, yylex());
      return new_type->size;
    default:
      printf("Expected predefined type, or compound type (<>[]), got: %.*s, which is not one\n", yyleng, yytext);
      exit(EXIT_FAILURE);
  }
  printf("Expected name of predefined type, or compound type (<>[]), got: %.*s, which is not one\n", yyleng, yytext);
  exit(EXIT_FAILURE);
}


enum tokens verify(enum tokens token_type) {
  enum tokens r = yylex();
  if(r != token_type) {
    printf("Expected %s, got: %.*s, which is not one\n", token_names[token_type], yyleng, yytext);
    exit(EXIT_FAILURE);
  }
  printf("verified type %s (%.*s)\n", token_names[token_type], yyleng, yytext);
  return r;
}

void checkTypeTableLength(struct type_table_t *type_table) {
  if(++(type_table->len) >= type_table->max_len) {
    type_table->max_len *= 2;
    type_table->table = realloc(type_table->table, type_table->max_len * sizeof(struct type));
  } else if (type_table->len < (type_table->max_len / 4)) {
    type_table->max_len /= 2;
    type_table->table = realloc(type_table->table, type_table->max_len * sizeof(struct type));
  }
}

void cleanup_type_table(struct type_table_t *type_table) {
  for(int i = 0; i < type_table->len; i++) {
    cleanup_type(type_table->table[i]);
  }
  free(type_table->table);
}

void cleanup_type(struct type t) {
  printf("cleaning up type: %.*s (referential bundle: %d)\n", t.name_len, t.name, t.referential);
  if(t.name) {
    free(t.name);
  }
  if(t.family == COMPOUND && !t.referential) {
    printf("bundle length to be cleaned up: %d\n", t.data.compound.len);
    for(int i = 0; i < t.compound.len; i++) {
      cleanup_type(t.compound.bundle[i]);
    }
    free(t.compound.bundle);
  }
}

int parseCompound(struct type_table_t *type_table, struct type *new_type) {
  enum tokens family = new_type->compound.type;
  enum tokens terminator = (family == OR ? RANGLE : family == AND ? RBRACKET : RPAREN); //FUNC

  int size = 0;

  struct type_list **tl;
  struct type_list *tl_root;
  //ouchie
  int first = 1;

  int nmemb = 0;
  enum tokens tok = yylex();
  while(tok != terminator) {
    struct type *working = malloc(sizeof(struct type));

    int subSize = 0;
    int working_size = parseType(type_table, working, tok); //parse type of subunit.
    enum tokens names = verify(IDENTIFIER);
    while (names != SEMICOLON) {
      // printf("%.*s\n", yyleng, yytext);
      if(names != IDENTIFIER) {
        printf("Expected name, got something bad\n");
        exit(EXIT_FAILURE);
      }
      struct type_list *working_named = malloc(sizeof(struct type_list));
      if(first) {tl = &working_named, tl_root = working_named; first = 0;}
      *tl = working_named;

      (*tl)->this = *working;
      (*tl)->this.name_len = yyleng;
      (*tl)->this.name = malloc(yyleng * sizeof(char));
      strncpy((*tl)->this.name, yytext, yyleng);

      switch(family) {
        case OR:
          subSize = working_size;
          break;
        case AND:
        case FUNC:
          subSize += working_size;
          break;
      }

      working->referential = 1;
      nmemb++;

      tl = &((*tl)->next);
      *tl = NULL;
      names = yylex();
    }
    switch(new_type->data.compound.type) {
      case OR:
        size = max(size, subSize);
        break;
      case AND:
      case FUNC:
        size += subSize;
        break;
    }
    printf("size of type is now: %d\n", size);

    until_right = yylex();
    free(working);
  }
  printf("closing the type, nmemb: %d\n", nmemb);
  if(!nmemb) {
    printf("compound type contained no members. Couldn't create type.\n");
    exit(EXIT_FAILURE);
  }

  new_type->size = size;

  new_type->data.compound.len = nmemb;
  new_type->data.compound.bundle = malloc(nmemb * sizeof(struct type));

  *tl = tl_root;
  printf("constructing type from discovery\n");
  for(int i = 0; i < nmemb; i++) {
    if(!tl) {
      printf("probably a recursion error in type creation. SOL 1\n");
      exit(EXIT_FAILURE);
    }


    printf("member name: %.*s\n", (*tl)->this.name_len, (*tl)->this.name);
    new_type->data.compound.bundle[i] = (*tl)->this;
    //shallow copy. only heap objects in type is name and maybe data.compound.bundle
    //but im not freeing those so copying ref is fine

    struct type i_t = new_type->data.compound.bundle[i];
    for (int j = i - 1; j >= 0; j--) {
      struct type j_t = new_type->data.compound.bundle[j];
      if(i_t.name_len == j_t.name_len &&
        !strncmp(i_t.name, j_t.name, i_t.name_len)) {

        printf("duplicate names not allowed due to ambiguity.\n");
        exit(EXIT_FAILURE);
      }
    }

    struct type_list *old = (*tl);
    *tl = (*tl)->next;
    free(old);
  }
  printf("\n");
  return new_type->size;
}
