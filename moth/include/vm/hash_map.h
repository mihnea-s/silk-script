#ifndef MOTHVM_HASH_MAP_H
#define MOTHVM_HASH_MAP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#include "value.h"

#define MOTHVM_HMAP_LOAD 0.95

typedef struct {
  Symbol key;
  Value  value;
} Entry;

typedef struct {
  size_t cap;
  size_t len;
  Entry* ptr;
} HashMap;

void   init_hash_map(HashMap* hmap);
void   hash_map_set(HashMap* hmap, Symbol key, Value value);
bool   hash_map_set_existing(HashMap* hmap, Symbol key, Value value);
Entry* hash_map_get(HashMap* hmap, Symbol key);
void   hash_map_delete(HashMap* hmap, Symbol key);
void   free_hash_map(HashMap* hmap);

#ifdef __cplusplus
}
#endif

#endif