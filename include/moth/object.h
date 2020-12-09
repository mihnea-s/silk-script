#ifndef MOTHVM_OBJECT_H
#define MOTHVM_OBJECT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#include <moth/value.h>

typedef enum {
  O_STRING   = 2,
  O_ARRAY    = 3,
  O_FUNCTION = 5,
  O_CLOSURE  = 7,
  O_VECTOR   = 8,
  // O_STRUCT   = 9,
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
  Object   obj;
  uint32_t len;
  uint8_t  bytes[];
} ObjectFunction;

typedef struct {
  Object          obj;
  ObjectFunction *fct;
} ObjectClosure;

typedef struct {
  Object obj;
  size_t card;
  double comp[];
} ObjectVector;

typedef struct {
  Object obj;
} ObjectStruct;

#define IS_OBJ_STR(val) (IS_OBJ(val) && val.as.object->type == O_STRING)
#define IS_OBJ_ARR(val) (IS_OBJ(val) && val.as.object->type == O_ARRAY)
#define IS_OBJ_FCT(val) (IS_OBJ(val) && val.as.object->type == O_FUNCTION)
#define IS_OBJ_CLJ(val) (IS_OBJ(val) && val.as.object->type == O_CLOSURE)
#define IS_OBJ_VEC(val) (IS_OBJ(val) && val.as.object->type == O_VECTOR)

#define IS_OBJ_ENUM(val)
#define IS_OBJ_SUCT(val) (IS_OBJ(val) && val.as.object->type == O_STRUCT)

#define OBJ_STR(obj) ((ObjectString *)obj)
#define OBJ_ARR(obj) ((ObjectArray *)obj)
#define OBJ_FCT(obj) ((ObjectFunction *)obj)
#define OBJ_CLJ(obj) ((ObjectClosure *)obj)
#define OBJ_VEC(obj) ((ObjectVector *)obj)

#define OBJ_ENUM(obj)
#define OBJ_SUCT(obj)

Object *alloc_object(ObjType type, size_t size);
void    free_object(Object *obj);
void    print_object(Object *obj);

bool          obj_str_equal(ObjectString *a, ObjectString *b);
ObjectString *obj_str_from_raw(const char *s);
ObjectString *obj_str_concat(const char *a, const char *b);
ObjectString *obj_str_multiply(const char *s, int64_t n);

ObjectArray *obj_arr_from_raw(Object *objs, size_t n);
void         obj_arr_concat(ObjectArray *arr, ObjectArray *b);
void         obj_arr_append(ObjectArray *arr, Value value);
void         obj_arr_remove(ObjectArray *arr, size_t i);
void         obj_arr_swap(ObjectArray *arr, size_t i, size_t j);

ObjectClosure *obj_clj_from_fct(ObjectFunction *fct);

ObjectVector *obj_vec_from_raw(double *comps, size_t card);
void          obj_vec_dot(ObjectVector *obj, ObjectVector *b);
void          obj_vec_cross(ObjectVector *obj, ObjectVector *b);
void          obj_vec_plus(ObjectVector *obj, ObjectVector *b);
void          obj_vec_minus(ObjectVector *obj, ObjectVector *b);
void          obj_vec_scale(ObjectVector *obj, double s);

#ifdef __cplusplus
}
#endif

#endif