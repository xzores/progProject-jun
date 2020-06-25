
#include "30010_io.h" // Input/output library for this course

struct vector_t {
    int32_t x,y;
};

void printFix(int32_t i);
int32_t expand(int32_t i);
int16_t abs(int16_t k);
int16_t mysin(int16_t angle);
int16_t mycos(int16_t angle);
void initVector(struct vector_t* v);
int32_t multiply(int32_t a, int32_t b);
void rotateVector(struct vector_t* v, int16_t angle);
