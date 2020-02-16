#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <hash_map.h>
#include <mem.h>

static Entry* hmap_empty_bucket(HashMap* hmap, Symbol key) {
  Entry* end    = hmap->ptr + hmap->cap;
  Entry* bucket = hmap->ptr + (key.hash % hmap->cap);

  while (bucket->key.str != NULL && bucket->key.str != key.str) {
    bucket++;
    if (bucket == end) { bucket = hmap->ptr; }
  }

  return bucket;
}

static Entry* hmap_find_bucket(HashMap* hmap, Symbol key) {
  Entry* end    = hmap->ptr + hmap->cap;
  Entry* bucket = hmap->ptr + (key.hash % hmap->cap);
  Entry* start  = bucket;

  while (bucket->key.str != key.str) {
    if (bucket->key.str == NULL && bucket->key.hash == 0x0) { return NULL; }

    bucket++;

    if (bucket == start) return NULL;
    if (bucket == end) bucket = hmap->ptr;
  }

  return bucket;
}

static void hmap_resize(HashMap* hmap, size_t new_size) {
  Entry* old      = hmap->ptr;
  size_t old_size = hmap->cap;

  hmap->ptr = memory(NULL, 0x0, new_size);
  hmap->cap = new_size;

  memset(hmap->ptr, 0x0, hmap->cap * sizeof(Entry));

  for (size_t i = 0; i < old_size; i++) {
    if (old[i].key.str) {
      Entry* entry = hmap_empty_bucket(hmap, old[i].key);
      entry->key   = old[i].key;
      entry->value = old[i].value;
    }
  }

  release(old, sizeof(Entry) * old_size);
}

void init_hash_map(HashMap* hmap) {
  hmap->cap = 0;
  hmap->len = 0;
  hmap->ptr = NULL;
}

void hash_map_set(HashMap* hmap, Symbol key, Value value) {
  if (hmap->len == hmap->cap * MOTHVM_HMAP_LOAD) {
    size_t new_size = GROW_ARRAY(hmap->cap);
    hmap_resize(hmap, new_size);
  }

  Entry* entry = hmap_empty_bucket(hmap, key);

  entry->value = value;
  if (entry->key.str == NULL) {
    entry->key = key;
    hmap->len++;
  }
}

bool hash_map_set_existing(HashMap* hmap, Symbol key, Value value) {
  if (hmap->len == 0) return false;

  Entry* bucket = hmap_find_bucket(hmap, key);
  if (bucket == NULL) return false;

  bucket->value = value;
  return true;
}

Entry* hash_map_get(HashMap* hmap, Symbol key) {
  if (hmap->len == 0) return NULL;

  Entry* bucket = hmap_find_bucket(hmap, key);
  if (bucket == NULL) return NULL;

  return bucket;
}

void hash_map_delete(HashMap* hmap, Symbol key) {
  if (hmap->len == 0) return;

  Entry* bucket = hmap_find_bucket(hmap, key);
  if (bucket == NULL) return;

  bucket->key.hash = 0x1;
  bucket->key.str  = NULL;

  hmap->len--;
}

void free_hash_map(HashMap* hmap) {
  FREE_ARRAY(hmap->ptr, Entry, hmap->cap);
}