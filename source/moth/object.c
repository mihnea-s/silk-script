#include <moth/object.h>

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <moth/ffi.h>
#include <moth/macros.h>
#include <moth/mem.h>
#include <moth/value.h>

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

    case O_VECTOR: {
      obj_size = sizeof(ObjectVector) + sizeof(double) * OBJ_VEC(obj)->card;
      break;
    }

    case O_DICTIONARY: {
      obj_size = sizeof(ObjectDictionary);
      release(
        OBJ_DCT(obj)->entries,
        sizeof(ObjectDictionaryEntry) * OBJ_DCT(obj)->cap);
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

    case O_HEAPVAL: {
      obj_size = sizeof(ObjectHeapval);
      break;
    }

    case O_FFI_FUNCTION: {
      obj_size = sizeof(ObjectFFIFunction);
      break;
    }

    case O_FFI_POINTER: {
      obj_size = sizeof(ObjectFFIPointer);
      obj_ffi_ptr_del(OBJ_FFI_PTR(obj));
      break;
    }
  }

  release(obj, obj_size);
}

bool equal_objects(Object *a, Object *b) {
  if (a->type != b->type) return false;

  switch (a->type) {
    case O_STRING: {
      return OBJ_STR(a)->hash == OBJ_STR(b)->hash &&
             strcmp(OBJ_STR(a)->data, OBJ_STR(b)->data) == 0;
    }

    case O_ARRAY: {
      if (OBJ_ARR(a)->size != OBJ_ARR(b)->size) return false;

      for (size_t i = 0; i < OBJ_ARR(a)->size; i++) {
        if (!equal_values(OBJ_ARR(a)->vals[i], OBJ_ARR(b)->vals[i])) {
          return false;
        }
      }

      return true;
    }

    case O_VECTOR: {
      if (OBJ_VEC(a)->card != OBJ_VEC(b)->card) return false;

      for (size_t i = 0; i < OBJ_VEC(a)->card; i++) {
        if (OBJ_VEC(a)->comp[i] != OBJ_VEC(b)->comp[i]) return false;
      }

      return true;
    }

    case O_DICTIONARY: // fallthrough
    case O_FUNCTION:   // fallthrough
    case O_CLOSURE: return a == b;
    
    case O_HEAPVAL: {
      return equal_values(OBJ_HPV(a)->val, OBJ_HPV(b)->val);
    }

    case O_FFI_FUNCTION: {
      return OBJ_FFI_FUN(a)->fun == OBJ_FFI_FUN(b)->fun;
    }

    case O_FFI_POINTER: {
      return OBJ_FFI_PTR(a)->ptr == OBJ_FFI_PTR(b);
    }
  }
}

void print_object(Object *obj) {
  switch (obj->type) {
    case O_STRING: printf("'%s'", OBJ_STR(obj)->data); break;
    case O_ARRAY: printf("[array]"); break;
    case O_VECTOR: printf("<vector>"); break;
    case O_DICTIONARY: printf("#{dict}"); break;
    case O_FUNCTION: printf("{fun}"); break;
    case O_CLOSURE: printf("{closure}"); break;
    case O_HEAPVAL: print_value(OBJ_HPV(obj)->val); break;
    case O_FFI_FUNCTION: printf("{ffi fun @ <%lxd>}", (long) OBJ_FFI_FUN(obj)->fun);
    case O_FFI_POINTER: printf("{ffi ptr @ <%lxd>}", (long) OBJ_FFI_PTR(obj)->ptr);
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

  // calculate the hash
  obj->hash = hash(obj->data);

  return obj;
}

ObjectString *obj_str_concat_sep(const char *a, const char sep, const char *b) {
  size_t a_size   = strlen(a);
  size_t b_size   = strlen(b);
  size_t str_size = a_size + b_size + sizeof(sep);

  ObjectString *obj =
    // allocate memory for the strings, separator and null byte
    (ObjectString *)alloc_object(
      O_STRING, sizeof(ObjectString) + str_size + sizeof(char));

  // initialize string object
  obj->size = str_size;

  // copy first string over
  memcpy(obj->data, a, a_size);

  // set the separator
  obj->data[a_size] = sep;

  // copy second string (with null byte)
  memcpy(obj->data + a_size + 1, b, b_size + sizeof(char));

  // calculate the hash
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

ObjectArray *obj_arr_from_raw(Value *vals, size_t n) {
  ObjectArray *obj = obj_arr_with_size(n);

  memcpy(obj->vals, vals, n * sizeof(Value));

  return obj;
}

ObjectArray *obj_arr_with_size(size_t n) {
  ObjectArray *obj = (ObjectArray *)alloc_object(
    O_ARRAY, sizeof(ObjectArray) + sizeof(Value) * n);

  obj->size = n;
  return obj;
}

ObjectArray *obj_arr_concat(ObjectArray *arr, ObjectArray *b) {
  ObjectArray *obj = (ObjectArray *)alloc_object(
    O_ARRAY, sizeof(ObjectArray) + sizeof(Value) * (arr->size + b->size));

  obj->size = arr->size + b->size;

  memcpy(obj->vals, arr->vals, arr->size * sizeof(Value));
  memcpy(obj->vals + arr->size, b->vals, b->size * sizeof(Value));

  return obj;
}

ObjectArray *obj_arr_append(ObjectArray *arr, Value value) {
  ObjectArray *obj = (ObjectArray *)alloc_object(
    O_ARRAY, sizeof(ObjectArray) + sizeof(Value) * (arr->size + 1));

  obj->size = arr->size + 1;

  memcpy(obj->vals, arr->vals, arr->size * sizeof(Value));
  memcpy(obj->vals + arr->size, &value, sizeof(Value));

  return obj;
}

void obj_arr_remove(ObjectArray *arr, size_t i) {
  assert(0 <= i && i < arr->size);

  for (size_t j = i + 1; j < arr->size; j++) {
    arr->vals[j - 1] = arr->vals[j];
  }

  arr->size--;
}

void obj_arr_swap(ObjectArray *arr, size_t i, size_t j) {
  assert(0 <= i && i < arr->size);
  assert(0 <= j && j < arr->size);

  Value fst = arr->vals[i];
  Value snd = arr->vals[j];

  arr->vals[i] = snd;
  arr->vals[j] = fst;
}

ObjectVector *obj_vec_from_raw(double *comps, size_t card) {
  ObjectVector *obj = (ObjectVector *)alloc_object(
    O_VECTOR, sizeof(ObjectVector) + sizeof(double) * card);

  memcpy(obj->comp, comps, sizeof(double) * card);
  return obj;
}

ObjectVector *obj_vec_clone(ObjectVector *obj) {
  return obj_vec_from_raw(obj->comp, obj->card);
}

double obj_vec_dot(ObjectVector *obj, ObjectVector *b) {
  size_t card = MIN(obj->card, b->card);

  double result = 0.0;

  for (size_t i = 0; i < card; i++) {
    result += obj->comp[i] * b->comp[i];
  }

  return result;
}

ObjectVector *obj_vec_cross(ObjectVector *obj, ObjectVector *b) {
  // TODO cant be arsed rn
  return NULL;
}

ObjectVector *obj_vec_plus(ObjectVector *a, ObjectVector *b) {
  ObjectVector *obj = obj_vec_clone(a);

  for (double *x = obj->comp, *y = b->comp;
       x < obj->comp + obj->card && y < b->comp + b->card;
       x++, y++) {
    *x += *y;
  }

  return obj;
}

ObjectVector *obj_vec_minus(ObjectVector *a, ObjectVector *b) {
  ObjectVector *obj = obj_vec_clone(a);

  for (double *x = obj->comp, *y = b->comp;
       x < obj->comp + obj->card && y < b->comp + b->card;
       x++, y++) {
    *x -= *y;
  }

  return obj;
}

ObjectVector *obj_vec_scale(ObjectVector *obj, double s) {
  ObjectVector *scaled = obj_vec_clone(obj);

  for (double *x = scaled->comp; x < scaled->comp + scaled->card; x++) {
    *x *= s;
  }

  return scaled;
}

ObjectDictionary *obj_dct_new() {
  static const size_t DICTIONARY_INIT_CAP = 5;
  return obj_dct_with_cap(DICTIONARY_INIT_CAP);
}

ObjectDictionary *obj_dct_from_raw(Value *keys, Value *vals, size_t len) {
  ObjectDictionary *obj = obj_dct_with_cap(len);

  for (size_t i = 0; i < len; i++) {
    obj_dct_insert(obj, keys[i], vals[i]);
  }

  return obj;
}

ObjectDictionary *obj_dct_with_cap(size_t n) {
  size_t cap = GROW_CAP(n);

  ObjectDictionary *obj =
    (ObjectDictionary *)alloc_object(O_DICTIONARY, sizeof(ObjectDictionary));

  obj->len     = 0;
  obj->cap     = cap;
  obj->entries = memory(NULL, 0x0, sizeof(ObjectDictionaryEntry) * cap);

  for (size_t i = 0; i < obj->cap; i++) {
    obj->entries[i] = (ObjectDictionaryEntry){
      .key   = VOID_VAL,
      .value = VOID_VAL,
    };
  }

  return obj;
}

static const int DICTIONARY_TOMBSTONE_VALUE = 0x46;

static const double DICTIONARY_LOAD_FACTOR = 0.65;

static bool obj_dct_entry_tomb(ObjectDictionaryEntry *entry) {
  return IS_VOID(entry->key) && IS_INT(entry->value) &&
         entry->value.as.integer == DICTIONARY_TOMBSTONE_VALUE;
}

static bool obj_dct_entry_empty(ObjectDictionaryEntry *entry) {
  return IS_VOID(entry->key) && IS_VOID(entry->value);
}

static bool obj_dct_entry_occupied(ObjectDictionaryEntry *entry) {
  return !IS_VOID(entry->key);
}

static void obj_dct_resize(ObjectDictionary *obj, size_t new_n) {
  size_t                 old_cap     = obj->cap;
  ObjectDictionaryEntry *old_entries = obj->entries;

  obj->cap     = GROW_CAP(new_n);
  obj->entries = memory(NULL, 0x0, sizeof(ObjectDictionaryEntry) * obj->cap);

  // Initialize all new entries to empty
  for (size_t i = 0; i < obj->cap; i++) {
    obj->entries[i] = (ObjectDictionaryEntry){
      .key   = VOID_VAL,
      .value = VOID_VAL,
    };
  }

  // Insert old entries
  for (size_t i = 0; i < old_cap; i++) {
    if (obj_dct_entry_occupied(&old_entries[i])) {
      obj_dct_insert(obj, old_entries[i].key, old_entries[i].value);
    }
  }

  // Free old entries
  release(old_entries, sizeof(ObjectDictionaryEntry) * old_cap);
}

void obj_dct_insert(ObjectDictionary *obj, Value key, Value value) {
  uint32_t key_hash = hash_value(key);

  if (obj->len + 1 > obj->cap * DICTIONARY_LOAD_FACTOR) {
    obj_dct_resize(obj, obj->len + 1);
  }

  ObjectDictionaryEntry *end   = obj->entries + obj->cap;
  ObjectDictionaryEntry *entry = obj->entries + (key_hash % obj->cap);

  while (!obj_dct_entry_empty(entry)) {
    if (equal_values(entry->key, key)) break;

    entry++;
    if (entry == end) entry = obj->entries;
  }

  entry->key   = key;
  entry->value = value;
}

bool obj_dct_has_key(ObjectDictionary *obj, Value key) {
  return !IS_VOID(obj_dct_get(obj, key));
}

Value obj_dct_get(ObjectDictionary *obj, Value key) {
  uint32_t key_hash = hash_value(key);

  ObjectDictionaryEntry *end   = obj->entries + obj->cap;
  ObjectDictionaryEntry *entry = obj->entries + (key_hash % obj->cap);

  while (!obj_dct_entry_empty(entry)) {
    if (equal_values(entry->key, key)) return entry->value;

    entry++;
    if (entry == end) entry = obj->entries;
  }

  return VOID_VAL;
}

Value obj_dct_delete(ObjectDictionary *obj, Value key) {
  uint32_t key_hash = hash_value(key);

  ObjectDictionaryEntry *end   = obj->entries + obj->cap;
  ObjectDictionaryEntry *entry = obj->entries + (key_hash % obj->cap);

  while (!obj_dct_entry_empty(entry)) {
    if (equal_values(entry->key, key)) {
      Value deleted = entry->value;
      entry->key    = VOID_VAL;
      entry->value  = INT_VAL(DICTIONARY_TOMBSTONE_VALUE);
      return deleted;
    }

    entry++;
    if (entry == end) entry = obj->entries;
  }

  return VOID_VAL;
}

ObjectArray *obj_dct_keys(ObjectDictionary *obj) {
  ObjectArray *keys = obj_arr_with_size(obj->len);

  for (size_t i = 0; i < obj->cap; i++) {
    if (obj_dct_entry_occupied(&obj->entries[i])) {
      keys->vals[i] = obj->entries->key;
    }
  }

  return keys;
}

ObjectArray *obj_dct_values(ObjectDictionary *obj) {
  ObjectArray *values = obj_arr_with_size(obj->len);

  for (size_t i = 0; i < obj->cap; i++) {
    if (obj_dct_entry_occupied(&obj->entries[i])) {
      values->vals[i] = obj->entries->value;
    }
  }

  return values;
}

void obj_dct_merge(ObjectDictionary *obj, ObjectDictionary *with) {
  obj_dct_resize(obj, obj->cap + with->len);

  for (size_t i = 0; i < with->cap; i++) {
    if (obj_dct_entry_occupied(&with->entries[i])) {
      obj_dct_insert(obj, with->entries[i].key, with->entries[i].value);
    }
  }
}

ObjectClosure *obj_clj_from_fct(ObjectFunction *fct) {
  ObjectClosure *obj =
    (ObjectClosure *)alloc_object(O_CLOSURE, sizeof(ObjectClosure));

  obj->fct = fct;
  return obj;
}

ObjectHeapval *obj_hpv_promote(Value val) {
  ObjectHeapval *obj =
    (ObjectHeapval *)alloc_object(O_HEAPVAL, sizeof(ObjectHeapval));

  obj->val = val;
  return obj;
}
