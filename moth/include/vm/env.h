#ifndef MOTHVM_ENV_H
#define MOTHVM_ENV_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#include "value.h"

#define MOTHVM_ENV_LOAD 0.95

typedef struct {
  Symbol key;
  Value  value;
} Entry;

typedef struct {
  size_t cap;
  size_t len;
  Entry* ptr;
} Environment;

void   init_env(Environment* env);
void   env_set(Environment* env, Symbol key, Value value);
bool   env_set_existing(Environment* env, Symbol key, Value value);
Entry* env_get(Environment* env, Symbol key);
void   env_delete(Environment* env, Symbol key);
void   free_env(Environment* env);

#ifdef __cplusplus
}
#endif

#endif