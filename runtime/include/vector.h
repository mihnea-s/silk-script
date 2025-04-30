#ifndef SSRI_VECTOR_H
#define SSRI_VECTOR_H

#include "mem.h"

typedef double* vector_t;

vector_t new_vector_scalar(double real) {
    vector_t vector =  (vector_t)memory(NULL, 0, sizeof(double));

    return vector;
}

#endif
