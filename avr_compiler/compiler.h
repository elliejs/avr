#ifndef COMPILER_H
#define COMPILER_H

enum tokens {
  _EOF,
  TYPE,
  AUTO,
  IF,
  ELIF,
  ELSE,
  WHILE,
  RETURN,

  EQ,
  NEQ,
  LTE,
  GTE,

  COLON,
  EXCLAIM,
  AT,
  ASSIGN,
  SEMICOLON,
  COMMA,

  LANGLE,
  RANGLE,
  LPAREN,
  RPAREN,
  LBRACKET,
  RBRACKET,
  LBRACE,
  RBRACE,

  IDENTIFIER,
  BASE10,
  BASE2,
  BASE16
};

#define LT LANGLE
#define GT RANGLE

#define TO EXCLAIM
#define NOT EXCLAIM

//common groups

//type structures
enum type_families {BASIC, COMPOUND};
enum compound_types {FUNC, AND, OR, TABLE};
// enum basic_types {REG, WORD};

struct type;
struct type {
  char *name;
  int name_len;
  int size;
  int indirection;
  enum type_families family;
  // union {
    // enum basic_types basic;
    struct {
      int len;
      struct type *bundle;
      enum compound_types type;
    } compound;
  // } data;
  int referential;
};

struct type_table_t {
  int max_len, len;
  struct type *table;
};

struct type_list {
  struct type this;
  struct type_list *next;
};

#endif
