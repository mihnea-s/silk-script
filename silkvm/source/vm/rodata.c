#include <stddef.h>
#include <string.h>

#include <mem.h>
#include <rodata.h>
#include <value.h>

void init_rodata(Rodata* rod) {
  rod->len  = 0;
  rod->cap  = 0;
  rod->vals = NULL;
}

void rodata_write(Rodata* rod, Value val) {
  if (rod->cap == rod->len) {
    size_t size = GROW_ARRAY(rod->cap);
    rod->vals  = REALLOC_ARRAY(rod->vals, Value, rod->cap, size);
    rod->cap   = size;
  }

  rod->vals[rod->len] = val;
  rod->len++;
}

void free_rodata(Rodata* rod) {
  for (size_t i = 0; i < rod->len; i++) {
    if (rod->vals[i].type == T_STR) {
      char*  str_ptr  = rod->vals[i].as.string;
      size_t str_size = strlen(str_ptr) + 1;
      release(str_ptr, str_size);
    }
  }

  FREE_ARRAY(rod->vals, Value, rod->cap);
}