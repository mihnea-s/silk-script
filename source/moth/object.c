#include <moth/mem.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <moth/object.h>

Object *alloc_object(ObjType type, size_t size) {
  Object *obj    = memory(NULL, 0, size);
  obj->reachable = false;
  obj->type      = type;
  return obj;
}

void free_object(Object *obj) {
  size_t obj_size = 0;
  switch (obj->type) {
    case O_STRING: {
      obj_size = sizeof(ObjectString) + sizeof(char) * OBJ_STR(obj)->size;
      break;
    }

    case O_ARRAY: {
      obj_size = sizeof(ObjectArray) + sizeof(Value) * OBJ_ARR(obj)->size;
      break;
    }

    case O_FUNCTION: {
      obj_size = sizeof(ObjectFunction) + sizeof(uint8_t) * OBJ_FCT(obj)->len;
      break;
    }

    case O_CLOSURE: {
      obj_size = sizeof(ObjectClosure);
      break;
    }

    case O_VECTOR: {
      obj_size = sizeof(ObjectVector) + sizeof(double) * OBJ_VEC(obj)->card;
      break;
    }

      // case O_STRUCT: {
      //   obj_size = sizeof(ObjectStruct);
      //   break;
      // }
  }

  release(obj, obj_size);
}

void print_object(Object *obj) {
  switch (obj->type) {
    case O_STRING: printf("'%s'", OBJ_STR(obj)->data); break;
    case O_ARRAY: printf("[array]"); break;
    case O_FUNCTION: printf("{fun}"); break;
    case O_CLOSURE: printf("{closure}"); break;
    case O_VECTOR:
      printf("<vector>");
      break;
      // case O_STRUCT: printf("{struct}"); break;
  }
}

bool obj_str_equal(ObjectString *a, ObjectString *b) {
  if (a->hash != b->hash) return false;
  return strcmp(a->data, b->data) == 0;
}

ObjectString *obj_str_from_raw(const char *s) {
  size_t str_size = strlen(s);

  ObjectString *obj =
    // allocate memory for the string and null byte
    (ObjectString *)alloc_object(
      O_STRING, sizeof(ObjectString) + str_size + sizeof(char));

  // initialize string object
  obj->size = str_size;
  obj->hash = hash(s);

  // copy string over (including null byte)
  memcpy(obj->data, s, str_size + sizeof(char));

  return obj;
}

ObjectString *obj_str_concat(const char *a, const char *b) {
  size_t a_size   = strlen(a);
  size_t b_size   = strlen(b);
  size_t str_size = a_size + b_size;

  ObjectString *obj =
    // allocate memory for the strings and null byte
    (ObjectString *)alloc_object(
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

ObjectString *obj_str_multiply(const char *s, int64_t n) {
  size_t s_size   = strlen(s);
  size_t str_size = s_size * n;

  ObjectString *obj =
    // allocate memory for the string and null byte
    (ObjectString *)alloc_object(
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

ObjectArray *obj_arr_from_raw(Object *objs, size_t n) {
}

void obj_arr_concat(ObjectArray *arr, ObjectArray *b) {
}

void obj_arr_remove(ObjectArray *arr, size_t i) {
}

void obj_arr_append(ObjectArray *arr, Value value) {
}

void obj_arr_swap(ObjectArray *arr, size_t i, size_t j) {
}

ObjectClosure *obj_clj_from_fct(ObjectFunction *fct) {
  ObjectClosure *obj =
    (ObjectClosure *)alloc_object(O_CLOSURE, sizeof(ObjectClosure));

  obj->fct = fct;
  return obj;
}

ObjectVector *obj_vec_from_raw(double *comps, size_t card) {
  ObjectVector *obj = (ObjectVector *)alloc_object(
    O_VECTOR, sizeof(ObjectVector) + sizeof(double) * card);

  memcpy(obj->comp, comps, sizeof(double) * card);
  return obj;
}

void obj_vec_dot(ObjectVector *obj, ObjectVector *b) {
}

void obj_vec_cross(ObjectVector *obj, ObjectVector *b) {
}

void obj_vec_plus(ObjectVector *obj, ObjectVector *b) {
}

void obj_vec_minus(ObjectVector *obj, ObjectVector *b) {
}

void obj_vec_scale(ObjectVector *obj, double s) {
}
