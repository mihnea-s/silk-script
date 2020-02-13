#include <stdint.h>
#include <string.h>

#include <mem.h>
#include <object.h>

void free_object(Object* obj) {
  release(obj, 0x0); // todo
}

ObjectString* obj_str_from_raw(const char* s) {
  size_t str_size = strlen(s);

  ObjectString* obj =
    // allocate memory for the string and null byte
    memory(NULL, 0x0, sizeof(ObjectString) + str_size + sizeof(char));

  // initialize string object
  obj->obj.type = O_STRING;
  obj->size     = str_size;

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
    memory(NULL, 0x0, sizeof(ObjectString) + str_size + sizeof(char));

  // initialize string object
  obj->obj.type = O_STRING;
  obj->size     = str_size;

  // copy first string over
  memcpy(obj->data, a, a_size);

  // copy second string (with null byte)
  memcpy(obj->data + a_size, b, b_size + sizeof(char));

  return obj;
}

ObjectString* obj_str_multiply(const char* s, int64_t n) {
  size_t s_size   = strlen(s);
  size_t str_size = s_size * n;

  ObjectString* obj =
    // allocate memory for the string and null byte
    memory(NULL, 0x0, sizeof(ObjectString) + str_size + sizeof(char));

  // initialize string object
  obj->obj.type = O_STRING;
  obj->size     = str_size;

  // copy the string over n times
  for (int64_t i = 0; i < n; i++) {
    memcpy(obj->data + i * s_size, s, s_size);
  }

  // set null byte
  obj->data[str_size] = '\0';

  return obj;
}

ObjectPoint* obj_pct_from_raw(double x, double y) {
  ObjectPoint* obj = memory(NULL, 0x0, sizeof(ObjectPoint));

  obj->obj.type = O_POINT;
  obj->x        = x;
  obj->y        = y;

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
