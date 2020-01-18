#include <stddef.h>

#include <constants.h>
#include <mem.h>
#include <value.h>

void init_constants(Constants *cnst) {
  cnst->len = 0;
  cnst->cap = 0;
  cnst->vals = NULL;
}

void write_constant(Constants *cnst, Value val) {
  if (cnst->cap == cnst->len) {
    size_t size = GROW_ARRAY(cnst->cap);
    cnst->vals = REALLOC_ARRAY(cnst->vals, Value, cnst->cap, size);
    cnst->cap = size;
  }

  cnst->vals[cnst->len] = val;
  cnst->len++;
}

void free_constants(Constants *cnst) {
  FREE_ARRAY(cnst->vals, Value, cnst->cap);
}