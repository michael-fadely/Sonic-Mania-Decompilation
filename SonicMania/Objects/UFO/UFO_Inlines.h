#if _arch_dreamcast
#ifndef __UFO_INLINES_H
#define __UFO_INLINES_H
#include <kos.h>
#include <sh4zam/shz_sh4zam.h>

__always_inline float fast_isin(int angle) {
    return shz_sinf((float)(angle) / 10430.37835f);
}

__always_inline float fast_icos(int angle) {
    return shz_cosf((float)(angle) / 10430.37835f);
}

__always_inline float fast_itan(int angle) {
    return shz_tanf((float)(angle) / 10430.37835f);
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
    shz_xmtrx_load_apply_store_4x4((shz_mat4x4_t*)xmtrxC, (const shz_mat4x4_t*)xmtrxA, (const shz_mat4x4_t*)xmtrxB);
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