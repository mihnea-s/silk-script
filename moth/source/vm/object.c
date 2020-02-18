#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <mem.h>
#include <object.h>

Object* alloc_object(ObjType type, size_t size) {
  Object* obj = memory(NULL, 0, size);
  obj->mark   = false;
  obj->type   = type;
  return obj;
}

void free_object(Object* obj) {
  size_t obj_size = 0;
  switch (obj->type) {
    case O_STRING: {
      ObjectString* str = OBJ_STR(obj);
      obj_size          = sizeof(ObjectString) + sizeof(char) * str->size;
      break;
    }

    case O_LIST: {
      ObjectList* lst = OBJ_LST(obj);
      obj_size        = sizeof(ObjectList) + sizeof(Value) * lst->size;
      break;
    }

    case O_FUNCTION: {
      ObjectFunction* fct = OBJ_FCT(obj);
      obj_size            = sizeof(ObjectFunction) + sizeof(uint8_t) * fct->len;
      break;
    }

    case O_CLOSURE: {
      obj_size = sizeof(ObjectClosure);
      break;
    }

    case O_VECTOR: {
      obj_size = sizeof(ObjectVector);
      break;
    }

    case O_POINT: {
      obj_size = sizeof(ObjectPoint);
      break;
    }

    case O_MATRIX: {
      ObjectMatrix* mat = OBJ_MAT(obj);
      obj_size = sizeof(ObjectMatrix) + sizeof(double) * mat->cols * mat->rows;
      break;
    }
  }

  release(obj, obj_size);
}

void print_object(Object* obj) {
  switch (obj->type) {
    case O_STRING: printf("'%s'", OBJ_STR(obj)->data); break;
    case O_LIST: printf("[list]"); break;
    case O_FUNCTION: printf("{fct}"); break;
    case O_CLOSURE: printf("{closure}"); break;
    case O_MATRIX: printf("[[matrix]]"); break;
    case O_POINT: printf("(%lf, %lf)", OBJ_PCT(obj)->x, OBJ_PCT(obj)->y); break;
    case O_VECTOR:
      printf(
        "(%lf, %lf, %lf)", OBJ_VEC(obj)->x, OBJ_VEC(obj)->y, OBJ_VEC(obj)->z);
      break;
  }
}

bool obj_str_equal(ObjectString* a, ObjectString* b) {
  if (a->hash != b->hash) return false;
  return strcmp(a->data, b->data) == 0;
}

ObjectString* obj_str_from_raw(const char* s) {
  size_t str_size = strlen(s);

  ObjectString* obj =
    // allocate memory for the string and null byte
    (ObjectString*)alloc_object(
      O_STRING, sizeof(ObjectString) + str_size + sizeof(char));

  // initialize string object
  obj->size = str_size;
  obj->hash = hash(s);

  // copy string over (including null byte)
  memcpy(obj->data, s, str_size + sizeof(char));

  return obj;
}

ObjectString* obj_str_concat(const char* a, const char* b) {
  size_t a_size   = strlen(a);
  size_t b_size   = strlen(b);
  size_t str_size = a_size + b_size;

  ObjectString* obj =
    // allocate memory for the strings and null byte
    (ObjectString*)alloc_object(
      O_STRING, sizeof(ObjectString) + str_size + sizeof(char));

  // initialize string object
  obj->size = str_size;

  // copy first string over
  memcpy(obj->data, a, a_size);

  // copy second string (with null byte)
  memcpy(obj->data + a_size, b, b_size + sizeof(char));

  obj->hash = hash(obj->data);

  return obj;
}

ObjectString* obj_str_multiply(const char* s, int64_t n) {
  size_t s_size   = strlen(s);
  size_t str_size = s_size * n;

  ObjectString* obj =
    // allocate memory for the string and null byte
    (ObjectString*)alloc_object(
      O_STRING, sizeof(ObjectString) + str_size + sizeof(char));

  // initialize string object
  obj->size = str_size;

  // copy the string over n times
  for (int64_t i = 0; i < n; i++) {
    memcpy(obj->data + i * s_size, s, s_size);
  }

  // set null byte
  obj->data[str_size] = '\0';

  obj->hash = hash(obj->data);

  return obj;
}

ObjectClosure* obj_clj_from_fct(ObjectFunction* fct) {
  ObjectClosure* obj =
    (ObjectClosure*)alloc_object(O_CLOSURE, sizeof(ObjectClosure));

  obj->fct = fct;
  return obj;
}

ObjectPoint* obj_pct_from_raw(double x, double y) {
  ObjectPoint* obj = (ObjectPoint*)alloc_object(O_POINT, sizeof(ObjectPoint));

  obj->x = x;
  obj->y = y;

  return obj;
}

void obj_pct_plus(ObjectPoint* obj, ObjectPoint* b) {
  obj->x += b->x;
  obj->y += b->y;
}

void obj_pct_minus(ObjectPoint* obj, ObjectPoint* b) {
  obj->x -= b->x;
  obj->y -= b->y;
}

void obj_pct_scale(ObjectPoint* obj, double s) {
  obj->x *= s;
  obj->y *= s;
}
