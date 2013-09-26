#include "ptest.h"
#include "../mpc.h"

#include <stdlib.h>
#include <string.h>

static bool int_eq(void* x, void* y) { return (*(int*)x == *(int*)y); }
static void int_print(void* x) { printf("'%i'", *((int*)x)); }
static bool string_eq(void* x, void* y) { return (strcmp(x, y) == 0); }
static void string_print(void* x) { printf("'%s'", (char*)x); }

void test_ident(void) {

  /* ^[a-zA-Z_][a-zA-Z0-9_]*$ */
  
  mpc_parser_t* Ident = mpc_enclose(
    mpc_also(
      mpc_else(mpc_alpha(), mpc_underscore()),
      mpc_many1(mpc_or(3, mpc_alpha(), mpc_underscore(), mpc_digit()), mpcf_strfold),
      free, mpcf_strfold),
    free
  );
  
  PT_ASSERT(mpc_match(Ident, "test", "test", string_eq, free, string_print));
  PT_ASSERT(mpc_unmatch(Ident, "  blah", "", string_eq, free, string_print));
  PT_ASSERT(mpc_match(Ident, "anoth21er", "anoth21er", string_eq, free, string_print));
  PT_ASSERT(mpc_match(Ident, "du__de", "du__de", string_eq, free, string_print));
  PT_ASSERT(mpc_unmatch(Ident, "some spaces", "", string_eq, free, string_print));
  PT_ASSERT(mpc_unmatch(Ident, "", "", string_eq, free, string_print));
  PT_ASSERT(mpc_unmatch(Ident, "18nums", "", string_eq, free, string_print));
  
  mpc_delete(Ident);

}

void test_maths(void) {

  mpc_parser_t* Expr   = mpc_new("expr");
  mpc_parser_t* Factor = mpc_new("factor");
  mpc_parser_t* Term   = mpc_new("term");
  mpc_parser_t* Maths  = mpc_new("maths");

  mpc_define(Expr, mpc_else(
    mpc_and(3, mpcf_maths, Factor, mpc_oneof("*/"), Factor, free, free),
    Factor
  ));
  
  mpc_define(Factor, mpc_else(
    mpc_and(3, mpcf_maths, Term, mpc_oneof("+-"), Term, free, free),
    Term
  ));
  
  mpc_define(Term, mpc_else(
    mpc_int(),
    mpc_parens(Expr, free)
  ));
  
  mpc_define(Maths, mpc_enclose(Expr, free));
  
  PT_ASSERT(mpc_match(Maths, "1", (int[]){ 1 }, int_eq, free, int_print));
  PT_ASSERT(mpc_match(Maths, "(5)", (int[]){ 5 }, int_eq, free, int_print));
  PT_ASSERT(mpc_match(Maths, "(4*2)+5", (int[]){ 13 }, int_eq, free, int_print));
  PT_ASSERT(mpc_unmatch(Maths, "a", (int[]){ 0 }, int_eq, free, int_print));
  PT_ASSERT(mpc_unmatch(Maths, "2b+4", (int[]){ 2 }, int_eq, free, int_print));
  
  mpc_cleanup(4, Expr, Factor, Term, Maths);
}

void suite_core(void) {
  pt_add_test(test_ident, "Test Ident", "Suite Core");
  pt_add_test(test_maths, "Test Maths", "Suite Core");
}