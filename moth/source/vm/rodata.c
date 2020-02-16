#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <mem.h>
#include <object.h>
#include <rodata.h>
#include <value.h>

void init_rodata(Rodata* rod, uint32_t init_len) {
  rod->len = init_len;
  rod->cap = init_len;
  rod->vls = NULL;
  if (init_len) rod->vls = memory(NULL, 0x0, init_len * sizeof(Value));
}

void rodata_write(Rodata* rod, Value val) {
  if (rod->cap == rod->len) {
    uint32_t new_size = GROW_ARRAY(rod->cap);
    rod->vls          = REALLOC_ARRAY(rod->vls, Value, rod->cap, new_size);
    rod->cap          = new_size;
  }

  rod->vls[rod->len] = val;
  rod->len++;
}

void free_rodata(Rodata* rod) {
  for (uint32_t i = 0; i < rod->len; i++) {
    switch (rod->vls[i].type) {
      case T_STR: {
        char*  str_ptr  = rod->vls[i].as.string;
        size_t str_size = strlen(str_ptr) + 1;
        release(str_ptr, str_size);
      }

      case T_OBJ: {
        free_object(rod->vls[i].as.object);
      }

      default: break;
    }
  }

  FREE_ARRAY(rod->vls, Value, rod->cap);
}