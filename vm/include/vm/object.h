#ifndef SILKVM_OBJECT_H
#define SILKVM_OBJECT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

typedef enum {
  O_STRING,
  O_LIST,
  O_VECTOR,
  O_POINT,
} ObjType;

typedef struct {
  ObjType type;
} Object;

typedef struct {
  Object obj;
  size_t size;
  char   data[];
} ObjectString;

typedef struct {
  Object obj;
  size_t size;
} ObjectList;

typedef struct {
  Object obj;
  double x, y, z;
} ObjectVector;

typedef struct {
  Object obj;
  double x, y;
} ObjectPoint;

#ifdef __cplusplus
}
#endif

#endif