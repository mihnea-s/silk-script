#ifndef MOTHVM_OBJECT_H
#define MOTHVM_OBJECT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#include "macros.h"
#include "value.h"

typedef enum {
  O_ERROR      = 2,
  O_VECTOR     = 3,
  O_STRING     = 5,
  O_DATA       = 7,
  O_SET        = 11,
  O_DICTIONARY = 17,
  O_FUNCTION   = 19,
  O_CLOSURE    = 23,
  O_HEAPVAL    = 29,
} ObjType;

typedef struct Object {
  bool    reachable;
  ObjType type;
} Object;

typedef struct {
  Object      obj;
  Symbol      kind;
  const char* message;
  size_t      size;
  Symbol      callstack[];
} ObjectError;

typedef struct {
  Object obj;
  size_t dimension[VEC_DIMEN_BYTES];
  double components[];
} ObjectVector;

typedef struct {
  Object   obj;
  uint32_t hash;
  size_t   size;
  char     data[];
} ObjectString;

typedef struct {
  Object obj;
  size_t size;
  Value  vals[];
} ObjectData;

typedef struct {
  Object obj;
  size_t cap;
  size_t len;
  Value  entries[];
} ObjectSet;

typedef struct {
  Value key;
  Value value;
} ObjectDictionaryEntry;

typedef struct {
  Object                obj;
  size_t                cap;
  size_t                len;
  ObjectDictionaryEntry entries[];
} ObjectDictionary;

typedef struct {
  Object  obj;
  size_t  len;
  uint8_t bytes[];
} ObjectFunction;

typedef struct {
  Object          obj;
  ObjectFunction* fct;
} ObjectClosure;

typedef struct {
  Object obj;
  Value  val;
} ObjectHeapval;

Object* alloc_object(ObjType type, size_t size);
void    free_object(Object* obj);

bool equal_objects(Object* a, Object* b);
void print_object(Object* obj);

ObjectError* obj_err_new(Symbol kind, const char* message);
ObjectError* obj_err_append_call(ObjectError* obj, Symbol name);
bool         obj_err_equal(ObjectError* obj, ObjectError* other);

ObjectVector* obj_vec_new();
ObjectVector* obj_vec_add(ObjectVector* obj, ObjectVector* other);
ObjectVector* obj_vec_multiply(ObjectVector* obj, ObjectVector* other);
bool          obj_vec_equal(ObjectVector* obj, ObjectVector* other);
size_t        obj_vec_size(ObjectVector* obj);

bool          obj_str_equal(ObjectString* a, ObjectString* b);
ObjectString* obj_str_from_raw(const char* s);
ObjectString* obj_str_concat(const char* a, const char* b);
ObjectString* obj_str_concat_sep(const char* a, const char sep, const char* b);
ObjectString* obj_str_multiply(const char* s, int64_t n);

ObjectData* obj_dat_from_raw(Value* vals, size_t n);
ObjectData* obj_dat_with_size(size_t n);
ObjectData* obj_dat_concat(ObjectData* obj, ObjectData* other);
bool        obj_dat_equal(ObjectData* obj, ObjectData* other);
ObjectData* obj_dat_append(ObjectData* obj, Value value);
void        obj_dat_remove(ObjectData* obj, size_t i);
void        obj_dat_swap(ObjectData* obj, size_t i, size_t j);

ObjectSet* obj_set_new();
ObjectSet* obj_set_from_raw(Value* vals, size_t len);
ObjectSet* obj_set_with_cap(size_t n);
bool       obj_set_has(ObjectSet* obj, Value key);
Value      obj_set_get(ObjectSet* obj, Value key);
void       obj_set_insert(ObjectSet* obj, Value value);
Value      obj_set_delete(ObjectSet* obj, Value key);
void       obj_set_merge(ObjectSet* obj, ObjectSet* with);
bool       obj_set_equal(ObjectSet* obj, ObjectSet* with);

ObjectDictionary* obj_dct_new();
ObjectDictionary* obj_dct_from_raw(Value* keys, Value* vals, size_t len);
ObjectDictionary* obj_dct_with_cap(size_t n);
void              obj_dct_insert(ObjectDictionary* obj, Value key, Value value);
bool              obj_dct_has_key(ObjectDictionary* obj, Value key);
Value             obj_dct_get(ObjectDictionary* obj, Value key);
Value             obj_dct_delete(ObjectDictionary* obj, Value key);
ObjectSet*        obj_dct_keys(ObjectDictionary* obj);
ObjectData*       obj_dct_values(ObjectDictionary* obj);
void              obj_dct_merge(ObjectDictionary* obj, ObjectDictionary* with);

ObjectClosure* obj_clj_from_fct(ObjectFunction* fct);

ObjectHeapval* obj_hpv_promote(Value val);

#ifdef __cplusplus
}
#endif

#endif
