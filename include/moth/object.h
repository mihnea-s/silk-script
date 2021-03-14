#ifndef MOTHVM_OBJECT_H
#define MOTHVM_OBJECT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#include <moth/value.h>

typedef enum {
  O_STRING       = 2,
  O_ARRAY        = 3,
  O_VECTOR       = 5,
  O_DICTIONARY   = 7,
  O_FUNCTION     = 11,
  O_CLOSURE      = 13,
  O_HEAPVAL      = 17,
  O_FFI_FUNCTION = 19,
  O_FFI_POINTER  = 23,
} ObjType;

typedef struct Object {
  bool    reachable;
  ObjType type;
} Object;

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
} ObjectArray;

typedef struct {
  Object obj;
  size_t card;
  double comp[];
} ObjectVector;

typedef struct {
  Value key;
  Value value;
} ObjectDictionaryEntry;

typedef struct {
  Object                 obj;
  size_t                 cap;
  size_t                 len;
  ObjectDictionaryEntry *entries;
} ObjectDictionary;

typedef struct {
  Object  obj;
  size_t  len;
  uint8_t bytes[];
} ObjectFunction;

typedef struct {
  Object          obj;
  ObjectFunction *fct;
} ObjectClosure;

typedef struct {
  Object obj;
  Value  val;
} ObjectHeapval;

#define IS_OBJ_STR(val) (IS_OBJ(val) && val.as.object->type == O_STRING)
#define IS_OBJ_ARR(val) (IS_OBJ(val) && val.as.object->type == O_ARRAY)
#define IS_OBJ_VEC(val) (IS_OBJ(val) && val.as.object->type == O_VECTOR)
#define IS_OBJ_DCT(val) (IS_OBJ(val) && val.as.object->type == O_DICTIONARY)
#define IS_OBJ_FCT(val) (IS_OBJ(val) && val.as.object->type == O_FUNCTION)
#define IS_OBJ_CLJ(val) (IS_OBJ(val) && val.as.object->type == O_CLOSURE)
#define IS_OBJ_HPV(val) (IS_OBJ(val) && val.as.object->type == O_HEAPVAL)

#define OBJ_STR(obj) ((ObjectString *)obj)
#define OBJ_ARR(obj) ((ObjectArray *)obj)
#define OBJ_VEC(obj) ((ObjectVector *)obj)
#define OBJ_DCT(obj) ((ObjectDictionary *)obj)
#define OBJ_FCT(obj) ((ObjectFunction *)obj)
#define OBJ_CLJ(obj) ((ObjectClosure *)obj)
#define OBJ_HPV(obj) ((ObjectHeapval *)obj)

Object *alloc_object(ObjType type, size_t size);
void    free_object(Object *obj);

bool equal_objects(Object *a, Object *b);
void print_object(Object *obj);

bool          obj_str_equal(ObjectString *a, ObjectString *b);
ObjectString *obj_str_from_raw(const char *s);
ObjectString *obj_str_concat(const char *a, const char *b);
ObjectString *obj_str_concat_sep(const char *a, const char sep, const char *b);
ObjectString *obj_str_multiply(const char *s, int64_t n);

ObjectArray *obj_arr_from_raw(Value *vals, size_t n);
ObjectArray *obj_arr_with_size(size_t n);
ObjectArray *obj_arr_concat(ObjectArray *arr, ObjectArray *b);
ObjectArray *obj_arr_append(ObjectArray *arr, Value value);
void         obj_arr_remove(ObjectArray *arr, size_t i);
void         obj_arr_swap(ObjectArray *arr, size_t i, size_t j);

ObjectVector *obj_vec_from_raw(double *comps, size_t card);
ObjectVector *obj_vec_clone(ObjectVector *obj);
double        obj_vec_dot(ObjectVector *obj, ObjectVector *b);
ObjectVector *obj_vec_cross(ObjectVector *obj, ObjectVector *b);
ObjectVector *obj_vec_plus(ObjectVector *obj, ObjectVector *b);
ObjectVector *obj_vec_minus(ObjectVector *obj, ObjectVector *b);
ObjectVector *obj_vec_scale(ObjectVector *obj, double s);

ObjectDictionary *obj_dct_new();
ObjectDictionary *obj_dct_from_raw(Value *keys, Value *vals, size_t len);
ObjectDictionary *obj_dct_with_cap(size_t n);
void              obj_dct_insert(ObjectDictionary *obj, Value key, Value value);
bool              obj_dct_has_key(ObjectDictionary *obj, Value key);
Value             obj_dct_get(ObjectDictionary *obj, Value key);
Value             obj_dct_delete(ObjectDictionary *obj, Value key);
ObjectArray *     obj_dct_keys(ObjectDictionary *obj);
ObjectArray *     obj_dct_values(ObjectDictionary *obj);
void              obj_dct_merge(ObjectDictionary *obj, ObjectDictionary *with);

ObjectClosure *obj_clj_from_fct(ObjectFunction *fct);

ObjectHeapval *obj_hpv_promote(Value val);

#ifdef __cplusplus
}
#endif

#endif