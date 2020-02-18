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
  rod->arr = init_len ? memory(NULL, 0, init_len * sizeof(Value)) : NULL;
}

void rodata_write(Rodata* rod, Value val) {
  if (rod->cap == rod->len) {
    uint32_t new_cap = GROW_CAP(rod->cap);

    uint32_t new_size = sizeof(Value) * new_cap;
    uint32_t old_size = sizeof(Value) * rod->cap;

    rod->cap = new_cap;
    rod->arr = memory(rod->arr, old_size, new_size);
  }

  rod->arr[rod->len] = val;
  rod->len++;
}

void free_rodata(Rodata* rod) {
  for (uint32_t i = 0; i < rod->len; i++) {
    switch (rod->arr[i].type) {
      case T_STR: {
        char*  str_ptr  = rod->arr[i].as.string;
        size_t str_size = strlen(str_ptr) + 1;
        release(str_ptr, str_size);
        break;
      }

      case T_OBJ: {
        free_object(rod->arr[i].as.object);
        break;
      }

      default: break;
    }
  }

  release(rod->arr, sizeof(Value) * rod->cap);
}