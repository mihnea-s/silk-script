#include <stddef.h>
#include <string.h>

#include <constants.h>
#include <mem.h>
#include <value.h>

void init_constants(Constants* cnst) {
  cnst->len  = 0;
  cnst->cap  = 0;
  cnst->vals = NULL;
}

void write_constant(Constants* cnst, Value val) {
  if (cnst->cap == cnst->len) {
    size_t size = GROW_ARRAY(cnst->cap);
    cnst->vals  = REALLOC_ARRAY(cnst->vals, Value, cnst->cap, size);
    cnst->cap   = size;
  }

  cnst->vals[cnst->len] = val;
  cnst->len++;
}

void free_constants(Constants* cnst) {
  for (size_t i = 0; i < cnst->len; i++) {
    if (cnst->vals[i].type == T_STR) {
      char*  str_ptr  = cnst->vals[i].as.string;
      size_t str_size = strlen(str_ptr) + 1;
      release(str_ptr, str_size);
    }
  }

  FREE_ARRAY(cnst->vals, Value, cnst->cap);
}