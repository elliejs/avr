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

#define T 1
#define F 0

// struct maybe {
//   type_internal* of;
// };
//
// struct indirect {
//   type_internal* of;
// };
//
// struct and_or {
//   int nmemb;
//   type_internal** of;
// };
//
// struct table {
//   type_internal* of;
// };

struct type_internal {
  enum {MAYBE, INDIRECT, AND_OR, TABLE, BASIC} which;
  int alloc_owner;
  union {
    struct {
      int nmemb;
      type_internal** of;
    };
    type_internal* of;
  };

  // union {
  //   struct maybe maybe;
  //   struct indirect indirect;
  //   struct and_or and_or;
  //   struct table table;
  //   int basic;
  // };
};

struct type {
  char *name;
  int name_len;
  int size;
  struct type_internal entity;
};

struct type_table_t {
  int max_len, len;
  struct type* table;
}
