#ifndef MOTHVM_OBJECT_H
#define MOTHVM_OBJECT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#include "value.h"

typedef enum {
  O_NONE,
  O_STRING,
  O_LIST,
  O_FUNCTION,
  O_VECTOR,
  O_POINT,
  O_MATRIX,
} ObjType;

typedef struct Object {
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
} ObjectList;

typedef struct {
  Object   obj;
  uint32_t len;
  uint8_t  ins[];
} ObjectFunction;

typedef struct {
  Object obj;
  double x, y, z;
} ObjectVector;

typedef struct {
  Object obj;
  double x, y;
} ObjectPoint;

typedef struct {
  Object obj;
  size_t cols;
  size_t rows;
  double data[];
} ObjectMatrix;

#define IS_OBJ_STR(val) (IS_OBJ(val) && val.as.object->type == O_STRING)
#define IS_OBJ_LST(val) (IS_OBJ(val) && val.as.object->type == O_LIST)
#define IS_OBJ_FCT(val) (IS_OBJ(val) && val.as.object->type == O_FUNCTION)
#define IS_OBJ_VEC(val) (IS_OBJ(val) && val.as.object->type == O_VECTOR)
#define IS_OBJ_PCT(val) (IS_OBJ(val) && val.as.object->type == O_POINT)
#define IS_OBJ_MAT(val) (IS_OBJ(val) && val.as.object->type == O_MATRIX)

#define OBJ_STR(obj) ((ObjectString*)obj)
#define OBJ_LST(obj) ((ObjectList*)obj)
#define OBJ_FCT(obj) ((ObjectFunction*)obj)
#define OBJ_VEC(obj) ((ObjectVector*)obj)
#define OBJ_PCT(obj) ((ObjectPoint*)obj)
#define OBJ_MAT(obj) ((ObjectMatrix*)obj)

void init_object(Object* obj);
void free_object(Object* obj);

bool          obj_str_equal(ObjectString* a, ObjectString* b);
ObjectString* obj_str_from_raw(const char* s);
ObjectString* obj_str_concat(const char* a, const char* b);
ObjectString* obj_str_multiply(const char* s, int64_t n);

ObjectList* obj_lst_from_raw(Object* objs, size_t n);
void        obj_lst_concat(ObjectList* list, ObjectList* b);
void        obj_lst_remove(ObjectList* list, size_t i);
void        obj_lst_add(ObjectList* list, size_t i);
void        obj_lst_swap(ObjectList* list, size_t i, size_t j);

ObjectVector* obj_vec_from_raw(double x, double y, double z);
void          obj_vec_dot(ObjectVector* obj, ObjectVector* b);
void          obj_vec_cross(ObjectVector* obj, ObjectVector* b);
void          obj_vec_plus(ObjectVector* obj, ObjectVector* b);
void          obj_vec_minus(ObjectVector* obj, ObjectVector* b);
void          obj_vec_scale(ObjectVector* obj, double s);

ObjectPoint* obj_pct_from_raw(double x, double y);
void         obj_pct_plus(ObjectPoint* obj, ObjectPoint* b);
void         obj_pct_minus(ObjectPoint* obj, ObjectPoint* b);
void         obj_pct_scale(ObjectPoint* obj, double s);

#ifdef __cplusplus
}
#endif

#endif