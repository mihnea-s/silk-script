#ifndef SILKVM_OBJECT_H
#define SILKVM_OBJECT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

typedef enum {
  O_STRING,
  O_LIST,
  O_VECTOR,
  O_POINT,
} ObjType;

typedef struct {
  ObjType type;
} Object;

#define IS_OBJ(val)     (val.type == T_OBJ)
#define IS_OBJ_STR(val) (IS_OBJ(val) && val.as.object->type == O_STRING)
#define IS_OBJ_LST(val) (IS_OBJ(val) && val.as.object->type == O_LIST)
#define IS_OBJ_VEC(val) (IS_OBJ(val) && val.as.object->type == O_VECTOR)
#define IS_OBJ_PCT(val) (IS_OBJ(val) && val.as.object->type == O_POINT)

typedef struct {
  Object obj;
  size_t size;
  char   data[];
} ObjectString;

ObjectString* obj_str_from_raw(const char* s);
ObjectString* obj_str_concat(const char* a, const char* b);
ObjectString* obj_str_multiply(const char* s, int64_t n);

typedef struct {
  Object  obj;
  size_t  size;
  Object* objs[];
} ObjectList;

ObjectList* obj_lst_from_raw(Object* objs, size_t n);
void        obj_lst_concat(ObjectList* list, ObjectList* b);
void        obj_lst_remove(ObjectList* list, size_t i);
void        obj_lst_add(ObjectList* list, size_t i);
void        obj_lst_swap(ObjectList* list, size_t i, size_t j);

typedef struct {
  Object obj;
  double x, y, z;
} ObjectVector;

ObjectVector* obj_vec_from_raw(double x, double y, double z);
void          obj_vec_dot(ObjectVector* obj, ObjectVector* b);
void          obj_vec_cross(ObjectVector* obj, ObjectVector* b);
void          obj_vec_plus(ObjectVector* obj, ObjectVector* b);
void          obj_vec_minus(ObjectVector* obj, ObjectVector* b);
void          obj_vec_scale(ObjectVector* obj, double s);

typedef struct {
  Object obj;
  double x, y;
} ObjectPoint;

ObjectPoint* obj_pct_from_raw(double x, double y);
void         obj_pct_plus(ObjectPoint* obj, ObjectPoint* b);
void         obj_pct_minus(ObjectPoint* obj, ObjectPoint* b);
void         obj_pct_scale(ObjectPoint* obj, double s);

#ifdef __cplusplus
}
#endif

#endif