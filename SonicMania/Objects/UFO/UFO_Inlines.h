#if _arch_dreamcast
#ifndef __UFO_INLINES_H
#define __UFO_INLINES_H
#include <kos.h>

/*! 2D Vector type
 *
 *  Structure for holding coordinates of a 2-dimensional vector.
 *
 * \sa shz_vec3_t, shz_vec4_t
 */
typedef struct shz_vec2 {
    union {
        float e[2]; //!< <X, Y> coordinates as an array
        struct {
            float x; //!< X coordinate
            float y; //!< Y coordinate
        };
    };
} shz_vec2_t;

/*! 3D Vector type
 *
 *  Structure for holding coordinates of a 3-dimensional vector.
 *
 * \sa shz_vec2_t, shz_vec4_t
 */
typedef struct shz_vec3 {
    union {
        float e[3]; //!< <X, Y, Z> coordinates as an array
        struct {
            union {
                struct {
                    float x; //!< X coordinate
                    float y; //!< Y coordinate
                };
                shz_vec2_t xy; //!< Inner 2D vector containing <X, Y> coords
            };
            float z; //!< Z coordinate
        };
    };
} shz_vec3_t;

/*! 4D Vector type
 *
 *  Structure for holding coordinates of a 4-dimensional vector.
 *
 *  \sa shz_vec2_t, shz_vec3_t
 */
typedef struct shz_vec4 {
    union {
        float e[4]; //!< <X, Y, Z, W> coordinates as an array.
        struct {
            union {
                struct {
                    float x; //!< X coordinate
                    float y; //!< Y coordinate
                    float z; //!< Z coordinate
                };
                shz_vec3_t xyz; //!< <X, Y, Z> coordinates as a 3D vector
            };
            float w; //!< W coordinate
        };
        struct {
            shz_vec2_t xy; //!< <X, Y> coordinates as a 2D vector
            shz_vec2_t zw; //!< <Z, W> coordinates as a 2D vector
        };
    };
} shz_vec4_t;

typedef __attribute__((aligned(8))) union shz_matrix_4x4 {
    float elem[16];
    float elem2D[4][4];
    shz_vec4_t col[4];
    struct {
        shz_vec4_t left;
        shz_vec4_t up;
        shz_vec4_t forward;
        shz_vec4_t pos;
    };
} shz_matrix_4x4_t;

//! Calculates 1.0f/sqrtf( \p x ), using a fast approximation.
__always_inline float shz_inverse_sqrtf(float x) {
    asm("fsrra %0" : "+f" (x));
    return x;
}

__always_inline float shz_invf(float x) {
    float rx = shz_inverse_sqrtf(x * x);
    return x < 0 ? -rx : rx;
}

__always_inline float shz_divf(float num, float denom) {
    return num * shz_invf(denom);
}

__always_inline float shz_dot6f(float x1, float y1, float z1, float x2, float y2, float z2) {
    register float rx1 asm("fr8") = x1;
    register float ry1 asm("fr9") = y1;
    register float rz1 asm("fr10") = z1;
    register float rw1 asm("fr11") = 0.0f;
    register float rx2 asm("fr12") = x2;
    register float ry2 asm("fr13") = y2;
    register float rz2 asm("fr14") = z2;
    register float rw2 asm("fr15");

    asm("fipr fv8, fv12" : "=f"(rw2) : "f"(rx1), "f"(ry1), "f"(rz1), "f"(rw1), "f"(rx2), "f"(ry2), "f"(rz2));

    return rw2;
}

__always_inline float shz_mag_sqr3f(float x, float y, float z) {
    register float rx asm("fr8") = x;
    register float ry asm("fr9") = y;
    register float rz asm("fr10") = z;
    register float rw asm("fr11") = 0.0f;

    asm("fipr fv8, fv8" : "+f"(rw) : "f"(rx), "f"(ry), "f"(rz));

    return rw;
}

__always_inline void shz_xmtrx_load_apply_store_4x4(shz_matrix_4x4_t* out, const shz_matrix_4x4_t* matrix1,
                                               const shz_matrix_4x4_t* matrix2) {
    unsigned int prefetch_scratch;

    asm volatile(R"(
        mov     %[m1], %[prefscr]
        add     #32, %[prefscr]
        fschg
        pref    @%[prefscr]

        fmov.d  @%[m1]+, xd0
        fmov.d  @%[m1]+, xd2
        fmov.d  @%[m1]+, xd4
        fmov.d  @%[m1]+, xd6
        pref    @%[m1]
        fmov.d  @%[m1]+, xd8
        fmov.d  @%[m1]+, xd10
        fmov.d  @%[m1]+, xd12
        mov     %[m2], %[prefscr]
        add     #32, %[prefscr]
        fmov.d  @%[m1], xd14
        pref    @%[prefscr]

        fmov.d  @%[m2]+, dr0
        fmov.d  @%[m2]+, dr2
        fmov.d  @%[m2]+, dr4
        ftrv    xmtrx, fv0

        fmov.d  @%[m2]+, dr6
        fmov.d  @%[m2]+, dr8
        ftrv    xmtrx, fv4

        fmov.d  @%[m2]+, dr10
        fmov.d  @%[m2]+, dr12
        ftrv    xmtrx, fv8

        add     #16, %[out]
        fmov.d  dr2, @-%[out]
        fmov.d  dr0,  @-%[out]

        fmov.d  @%[m2], dr14
        ftrv    xmtrx, fv12

        add     #32, %[out]
        fmov.d  dr6, @-%[out]
        fmov.d  dr4, @-%[out]

        add     #32, %[out]
        fmov.d  dr10, @-%[out]
        fmov.d  dr8, @-%[out]

        add     #32, %[out]
        fmov.d  dr14, @-%[out]
        fmov.d  dr12, @-%[out]

        fschg
    )"
    : [m1] "+&r"(matrix1), [m2] "+r"(matrix2), [out] "+&r"(out),
    "=m"(*out), [prefscr] "=&r"(prefetch_scratch)
    : "m"(*matrix1), "m"(*matrix2)
    : "fr0", "fr1", "fr2", "fr3", "fr4", "fr5", "fr6", "fr7", "fr8", "fr9", "fr10", "fr11", "fr12", "fr13",
    "fr14", "fr15");
}

__always_inline float fast_isin(int angle) {
    return __builtin_sinf((float)(angle) / 10430.37835f);
}

__always_inline float fast_icos(int angle) {
    return __builtin_cosf((float)(angle) / 10430.37835f);
}

__always_inline float fast_itan(int angle) {
    return __builtin_tanf((float)(angle) / 10430.37835f);
}

__always_inline int32 Sin1024(int32 angle) { return (int32)(fast_isin((angle & 0x3FF) << 6) * 1024); }
__always_inline int32 Cos1024(int32 angle) { return (int32)(fast_icos((angle & 0x3FF) << 6) * 1024); }
__always_inline int32 Tan1024(int32 angle) { return (int32)(fast_itan((angle & 0x3FF) << 6) * 1024); }

__always_inline int32 Sin512(int32 angle) { return (int32)(fast_isin((angle & 0x1FF) << 7) * 512); }
__always_inline int32 Cos512(int32 angle) { return (int32)(fast_icos((angle & 0x1FF) << 7) * 512); }
__always_inline int32 Tan512(int32 angle) { return (int32)(fast_itan((angle & 0x1FF) << 7) * 512); }


__always_inline int32 Sin256(int32 angle) { return (int32)(fast_isin((angle & 0xFF) << 8) * 256); }
__always_inline int32 Cos256(int32 angle) { return (int32)(fast_icos((angle & 0xFF) << 8) * 256); }
__always_inline int32 Tan256(int32 angle) { return (int32)(fast_itan((angle & 0xFF) << 8) * 256); }

 __always_inline float shz_dot8f(float x1, float y1, float z1, float w1, float x2, float y2, float z2, float w2) {
    register float rx1 asm("fr8") = x1;
    register float ry1 asm("fr9") = y1;
    register float rz1 asm("fr10") = z1;
    register float rw1 asm("fr11") = w1;
    register float rx2 asm("fr12") = x2;
    register float ry2 asm("fr13") = y2;
    register float rz2 asm("fr14") = z2;
    register float rw2 asm("fr15") = w2;

    asm("fipr fv8, fv12" : "+f"(rw2) : "f"(rx1), "f"(ry1), "f"(rz1), "f"(rw1), "f"(rx2), "f"(ry2), "f"(rz2));

    return rw2;
}

void MatrixMultiply(Matrix *dest, Matrix *matrixA, Matrix *matrixB)
{
    float __attribute__((aligned(32))) xmtrxB[16];
    int32 *mtrxB = (int32 *)matrixB->values;
    for (int i=0;i<16;i++) {
        xmtrxB[i] = (float)(*mtrxB++);
    }

    float __attribute__((aligned(32))) xmtrxA[16];
    int32 *mtrxA = (int32 *)matrixA->values;
    for (int i=0;i<16;i++) {
        xmtrxA[i] = (float)(*mtrxA++);
    }

    float __attribute__((aligned(32))) xmtrxC[16];
    int32 *mtrxC = (int32 *)dest->values;
    shz_xmtrx_load_apply_store_4x4((shz_matrix_4x4_t*)xmtrxC, (const shz_matrix_4x4_t*)xmtrxA, (const shz_matrix_4x4_t*)xmtrxB);
    for (int i=0;i<16;i++) {
        *mtrxC++ = ((int32)xmtrxC[i]) >> 8;
    }
}

void MatrixTranslateXYZ(Matrix *matrix, int32 x, int32 y, int32 z, bool32 setIdentity)
{
    if (setIdentity) {
        matrix->values[0][0] = 0x100;
        matrix->values[1][0] = 0;
        matrix->values[2][0] = 0;
        matrix->values[0][1] = 0;
        matrix->values[1][1] = 0x100;
        matrix->values[2][1] = 0;
        matrix->values[0][2] = 0;
        matrix->values[1][2] = 0;
        matrix->values[2][2] = 0x100;
        matrix->values[3][0] = 0;
        matrix->values[3][1] = 0;
        matrix->values[3][2] = 0;
        matrix->values[3][3] = 0x100;
    }

    matrix->values[0][3] = x >> 8;
    matrix->values[1][3] = y >> 8;
    matrix->values[2][3] = z >> 8;
}

void MatrixScaleXYZ(Matrix *matrix, int32 scaleX, int32 scaleY, int32 scaleZ)
{
    matrix->values[0][0] = scaleX;
    matrix->values[1][0] = 0;
    matrix->values[2][0] = 0;
    matrix->values[3][0] = 0;
    matrix->values[0][1] = 0;
    matrix->values[1][1] = scaleY;
    matrix->values[2][1] = 0;
    matrix->values[3][1] = 0;
    matrix->values[0][2] = 0;
    matrix->values[1][2] = 0;
    matrix->values[2][2] = scaleZ;
    matrix->values[3][2] = 0;
    matrix->values[0][3] = 0;
    matrix->values[1][3] = 0;
    matrix->values[2][3] = 0;
    matrix->values[3][3] = 0x100;
}

void MatrixRotateX(Matrix *matrix, int16 rotationX)
{
    int32 sine   = Sin1024(rotationX) >> 2;
    int32 cosine = Cos1024(rotationX) >> 2;

    matrix->values[0][0] = 0x100;
    matrix->values[1][0] = 0;
    matrix->values[2][0] = 0;
    matrix->values[3][0] = 0;
    matrix->values[0][1] = 0;
    matrix->values[1][1] = cosine;
    matrix->values[2][1] = sine;
    matrix->values[3][1] = 0;
    matrix->values[0][2] = 0;
    matrix->values[1][2] = -sine;
    matrix->values[2][2] = cosine;
    matrix->values[3][2] = 0;
    matrix->values[0][3] = 0;
    matrix->values[1][3] = 0;
    matrix->values[2][3] = 0;
    matrix->values[3][3] = 0x100;
}

void MatrixRotateY(Matrix *matrix, int16 rotationY)
{
    int32 sine           = Sin1024(rotationY) >> 2;
    int32 cosine         = Cos1024(rotationY) >> 2;
    matrix->values[0][0] = cosine;
    matrix->values[1][0] = 0;
    matrix->values[2][0] = sine;
    matrix->values[3][0] = 0;
    matrix->values[0][1] = 0;
    matrix->values[1][1] = 0x100;
    matrix->values[2][1] = 0;
    matrix->values[3][1] = 0;
    matrix->values[0][2] = -sine;
    matrix->values[1][2] = 0;
    matrix->values[2][2] = cosine;
    matrix->values[3][2] = 0;
    matrix->values[0][3] = 0;
    matrix->values[1][3] = 0;
    matrix->values[2][3] = 0;
    matrix->values[3][3] = 0x100;
}

void MatrixRotateZ(Matrix *matrix, int16 rotationZ)
{
    int32 sine           = Sin1024(rotationZ) >> 2;
    int32 cosine         = Cos1024(rotationZ) >> 2;
    matrix->values[0][0] = cosine;
    matrix->values[1][0] = -sine;
    matrix->values[2][0] = 0;
    matrix->values[3][0] = 0;
    matrix->values[0][1] = sine;
    matrix->values[1][1] = cosine;
    matrix->values[2][1] = 0;
    matrix->values[3][1] = 0;
    matrix->values[0][2] = 0;
    matrix->values[1][2] = 0;
    matrix->values[2][2] = 0x100;
    matrix->values[3][2] = 0;
    matrix->values[0][3] = 0;
    matrix->values[1][3] = 0;
    matrix->values[2][3] = 0;
    matrix->values[3][3] = 0x100;
}

void MatrixRotateXYZ(Matrix *matrix, int16 rotationX, int16 rotationY, int16 rotationZ)
{
    int32 sinX = Sin1024(rotationX) >> 2;
    int32 cosX = Cos1024(rotationX) >> 2;
    int32 sinY = Sin1024(rotationY) >> 2;
    int32 cosY = Cos1024(rotationY) >> 2;
    int32 sinZ = Sin1024(rotationZ) >> 2;
    int32 cosZ = Cos1024(rotationZ) >> 2;

    matrix->values[0][0] = (cosZ * cosY >> 8) + (sinZ * (sinY * sinX >> 8) >> 8);
    matrix->values[0][1] = -(sinZ * cosX) >> 8;
    matrix->values[0][2] = (sinZ * (cosY * sinX >> 8) >> 8) - (cosZ * sinY >> 8);
    matrix->values[0][3] = 0;
    matrix->values[1][0] = (sinZ * cosY >> 8) - (cosZ * (sinY * sinX >> 8) >> 8);
    matrix->values[1][1] = cosZ * cosX >> 8;
    matrix->values[1][2] = (-(sinZ * sinY) >> 8) - (cosZ * (cosY * sinX >> 8) >> 8);
    matrix->values[1][3] = 0;
    matrix->values[2][0] = sinY * cosX >> 8;
    matrix->values[2][1] = sinX;
    matrix->values[2][2] = cosY * cosX >> 8;
    matrix->values[2][3] = 0;
    matrix->values[3][0] = 0;
    matrix->values[3][1] = 0;
    matrix->values[3][2] = 0;
    matrix->values[3][3] = 0x100;
}
#endif//__UFO_INLINES_H
#endif//_arch_dreamcast