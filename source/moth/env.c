#include <moth/env.h>

#include <string.h>

#include <moth/mem.h>

static Entry *env_empty_bucket(Environment *env, Symbol key) {
  Entry *end    = env->ptr + env->cap;
  Entry *bucket = env->ptr + (key.hash % env->cap);

  while (bucket->key.str != NULL && bucket->key.str != key.str) {
    bucket++;
    if (bucket == end) { bucket = env->ptr; }
  }

  return bucket;
}

static Entry *env_find_bucket(Environment *env, Symbol key) {
  Entry *end    = env->ptr + env->cap;
  Entry *bucket = env->ptr + (key.hash % env->cap);

  while (bucket->key.str != key.str) {
    if (bucket->key.str == NULL && bucket->key.hash == 0x0) return NULL;
    bucket++;
    if (bucket == end) bucket = env->ptr;
  }

  return bucket;
}

static void env_resize(Environment *env, size_t new_cap) {
  Entry *old     = env->ptr;
  size_t old_cap = env->cap;

  size_t old_size = old_cap * sizeof(Entry);
  size_t new_size = new_cap * sizeof(Entry);

  env->cap = new_cap;
  env->ptr = memory(env->ptr, old_size, new_size);

  memset(env->ptr, 0x0, new_size);

  for (size_t i = 0; i < old_cap; i++) {
    if (old[i].key.str) {
      Entry *entry = env_empty_bucket(env, old[i].key);
      entry->key   = old[i].key;
      entry->value = old[i].value;
    }
  }

  release(old, old_size);
}

void init_env(Environment *env) {
  env->cap = 0;
  env->len = 0;
  env->ptr = NULL;
}

void env_set(Environment *env, Symbol key, Value value) {
  if (env->len == env->cap * MOTHVM_ENV_LOAD) {
    size_t new_cap = GROW_CAP(env->cap);
    env_resize(env, new_cap);
  }

  Entry *entry = env_empty_bucket(env, key);

  entry->value = value;
  if (entry->key.str == NULL) {
    entry->key = key;
    env->len++;
  }
}

bool env_set_existing(Environment *env, Symbol key, Value value) {
  if (env->len == 0) return false;

  Entry *bucket = env_find_bucket(env, key);
  if (bucket == NULL) return false;

  bucket->value = value;
  return true;
}

Entry *env_get(Environment *env, Symbol key) {
  if (env->len == 0) return NULL;

  Entry *bucket = env_find_bucket(env, key);
  return bucket;
}

void env_delete(Environment *env, Symbol key) {
  if (env->len == 0) return;

  Entry *bucket = env_find_bucket(env, key);
  if (bucket == NULL) return;

  bucket->key.hash = 0x1;
  bucket->key.str  = NULL;
}

void free_env(Environment *env) {
  release(env->ptr, sizeof(Entry) * env->cap);
}
