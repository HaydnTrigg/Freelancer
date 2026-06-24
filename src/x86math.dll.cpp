//
// x86math.dll - x86 implementation of I3DMathEngine.
//
// Decompiled to match 052103_release_1149_Ipatch_ver1254.
// Reference source: Conquest/Liberty x86math.cpp (B. Baldwin, Digital Anvil).
//
// Single translation unit. Functions are emitted in target address order and
// named to objdiff's normalized target symbol names.
//

#include <math.h>
#include <stdlib.h>
#include <string.h>

typedef long GENRESULT;
typedef unsigned long U32;
typedef char C8;
typedef float SINGLE;

#define COMAPI __stdcall

#define GR_OK                    0
#define GR_GENERIC             (-1)
#define GR_INVALID_PARMS       (-2)
#define GR_INTERFACE_UNSUPPORTED (-3)

#define DACOM_LOW_PRIORITY 0x40000000

// This is an arbitrarily small constant used to circumvent matrix inversions
// that might overflow due to small determinants.
#define MIN_DET 1e-8f

// ---------------------------------------------------------------------------
// Math types (layouts taken from the disassembly)
// ---------------------------------------------------------------------------

struct Vector
{
    SINGLE x, y, z;
};

struct Matrix
{
    SINGLE d[3][3];
};

struct Transform
{
    SINGLE d[3][3];
    Vector translation;
};

// ---------------------------------------------------------------------------
// DACOM interfaces (only the layout needed for vtable offsets is reproduced)
// ---------------------------------------------------------------------------

struct DACOMDESC
{
    U32  size;
    C8 * interface_name;
};

struct IDAComponent
{
    virtual GENRESULT COMAPI QueryInterface(const C8 *interface_name, void **instance) = 0;
    virtual U32       COMAPI AddRef(void) = 0;
    virtual U32       COMAPI Release(void) = 0;
};

struct IComponentFactory : public IDAComponent
{
    virtual GENRESULT COMAPI CreateInstance(DACOMDESC *descriptor, void **instance) = 0;
};

struct I3DMathEngine : public IDAComponent
{
    virtual GENRESULT COMAPI inverse(Matrix &dst, const Matrix &m) = 0;
    virtual GENRESULT COMAPI scale(Matrix &dst, const Matrix &m, SINGLE s) = 0;
    virtual SINGLE    COMAPI det(const Matrix &m) = 0;
};

struct ICOManager : public IDAComponent
{
    virtual GENRESULT COMAPI _reserved3(void) = 0;
    virtual GENRESULT COMAPI RegisterComponent(IComponentFactory *component, const C8 *interface_name, U32 priority) = 0;
};

// ---------------------------------------------------------------------------
// Inverse-square-root lookup table (Graphics Gems V, Ken Turkowski)
// ---------------------------------------------------------------------------

#define LOOKUP_BITS 9
#define EXP_POS     23
#define EXP_BIAS    127
#define LOOKUP_POS  (EXP_POS - LOOKUP_BITS)
#define SEED_POS    (EXP_POS - 8)
#define TABLE_SIZE  (2 << LOOKUP_BITS)

union _flint
{
    unsigned long i;
    float         f;
};

// ---------------------------------------------------------------------------
// Imports / globals
// ---------------------------------------------------------------------------

extern "C" __declspec(dllimport) int          __stdcall DisableThreadLibraryCalls(void *hLibModule);
extern "C" __declspec(dllimport) ICOManager * __stdcall DACOM_Acquire(void);

void *operator new(unsigned int size);

extern "C" void *DAComponent_x86MathEngine_I3DMathEngine_vtbl[];
extern "C" void *DAComponent_x86MathEngine_IComponentFactory_vtbl[];

#define IID_I3DMathEngine   "3DMathEngine"
#define implementation_name "x86"

unsigned char  byte_6F74030[TABLE_SIZE];   // inv_sqrt_obj.iSqrt
unsigned char  byte_6F74430;               // run-once guard
void          *pMathEngine;

// Forward declarations (single source file, target address order)
extern "C" void sub_6F71010(void);
extern "C" int  sub_6F71060(void);
extern "C" void __cdecl sub_6F71070(void);

// ---------------------------------------------------------------------------
// 0x6F71000 - dynamic initializer for inv_sqrt_obj
// ---------------------------------------------------------------------------
extern "C" int sub_6F71000(void)
{
    sub_6F71010();
    return sub_6F71060();
}

// ---------------------------------------------------------------------------
// 0x6F71010 - ISQRT::ISQRT (build inverse-square-root lookup table)
// ---------------------------------------------------------------------------
extern "C" void sub_6F71010(void)
{
    int f;
    union _flint fi, fo;

    for (f = 0; f < TABLE_SIZE; f++)
    {
        fi.i = ((EXP_BIAS - 1) << EXP_POS) | (f << LOOKUP_POS);
        fo.f = 1.0f / sqrt(fi.f);
        byte_6F74030[f] = (unsigned char)(((fo.i + (1 << (SEED_POS - 2))) >> SEED_POS) & 0xFF);
    }
    byte_6F74030[TABLE_SIZE / 2] = 0xFF;
}

// ---------------------------------------------------------------------------
// 0x6F71060 - register inv_sqrt_obj destructor with atexit
// ---------------------------------------------------------------------------
extern "C" int sub_6F71060(void)
{
    return atexit(sub_6F71070);
}

// ---------------------------------------------------------------------------
// 0x6F71070 - inv_sqrt_obj atexit thunk
// ---------------------------------------------------------------------------
extern "C" void __cdecl sub_6F71070(void)
{
    if ((byte_6F74430 & 1) == 0)
        byte_6F74430 |= 1;
}

// ---------------------------------------------------------------------------
// 0x6F71090 - DllMain
// ---------------------------------------------------------------------------
extern "C" int __stdcall DllMain(void *hinstDLL, unsigned int fdwReason, void *lpvReserved)
{
    if (fdwReason == 1)
    {
        DisableThreadLibraryCalls(hinstDLL);

        void *self = ::operator new(8u);
        if (self == 0)
        {
            pMathEngine = 0;
            return 1;
        }

        ((void **)self)[0] = (void *)DAComponent_x86MathEngine_I3DMathEngine_vtbl;
        ((void **)self)[1] = (void *)DAComponent_x86MathEngine_IComponentFactory_vtbl;

        {
            int f;
            union _flint fi, fo;
            for (f = 0; f < TABLE_SIZE; f++)
            {
                fi.i = ((EXP_BIAS - 1) << EXP_POS) | (f << LOOKUP_POS);
                fo.f = 1.0f / sqrt(fi.f);
                byte_6F74030[f] = (unsigned char)(((fo.i + (1 << (SEED_POS - 2))) >> SEED_POS) & 0xFF);
            }
            byte_6F74030[TABLE_SIZE / 2] = 0xFF;
        }

        pMathEngine = self;

        ICOManager *DACOM = DACOM_Acquire();
        if (DACOM != 0)
        {
            IComponentFactory *factory =
                pMathEngine ? (IComponentFactory *)((char *)pMathEngine + 4) : (IComponentFactory *)0;
            DACOM->RegisterComponent(factory, IID_I3DMathEngine, DACOM_LOW_PRIORITY);
        }
    }
    return 1;
}

// ---------------------------------------------------------------------------
// 0x6F71150 - x86MathEngine::QueryInterface
// ---------------------------------------------------------------------------
extern "C" GENRESULT __stdcall x86MathEngine_QueryInterface(void *self, const C8 *interface_name, void **instance)
{
    *instance = 0;
    return GR_GENERIC;
}

// ---------------------------------------------------------------------------
// 0x6F71160 - x86MathEngine::AddRef / Release (identical, folded)
// ---------------------------------------------------------------------------
extern "C" U32 __stdcall x86MathEngine_AddRef_Release(void *self)
{
    return 1;
}

// ---------------------------------------------------------------------------
// 0x6F71170 - x86MathEngine::IComponentFactory::CreateInstance
// ---------------------------------------------------------------------------
extern "C" GENRESULT __stdcall x86MathEngine_CreateInstance(IComponentFactory *self, DACOMDESC *descriptor, void **instance)
{
    DACOMDESC *info = descriptor;

    *instance = 0;

    if (info == 0 || info->interface_name == 0)
        return GR_INVALID_PARMS;

    if (info->size == sizeof(DACOMDESC) + 4 &&
        strcmp(IID_I3DMathEngine, info->interface_name) == 0 &&
        (((C8 **)info)[2] == 0 || stricmp(implementation_name, ((C8 **)info)[2]) == 0))
    {
        I3DMathEngine *engine = (I3DMathEngine *)((char *)self - 4);
        engine->AddRef();
        *instance = engine;
        return GR_OK;
    }

    return GR_INTERFACE_UNSUPPORTED;
}

// ---------------------------------------------------------------------------
// 0x6F711F0 - x86MathEngine::inverse
// ---------------------------------------------------------------------------
extern "C" GENRESULT __stdcall sub_6F711F0(I3DMathEngine *self, Matrix &dst, const Matrix &m)
{
    GENRESULT result;

    const SINGLE determinant = self->det(m);

    if (fabs(determinant) > MIN_DET)
    {
        const SINGLE dt = 1.0f / determinant;

        dst.d[0][0] = (m.d[1][1] * m.d[2][2] - m.d[1][2] * m.d[2][1]) * dt;
        dst.d[1][0] = -(m.d[1][0] * m.d[2][2] - m.d[1][2] * m.d[2][0]) * dt;
        dst.d[2][0] = (m.d[1][0] * m.d[2][1] - m.d[1][1] * m.d[2][0]) * dt;
        dst.d[0][1] = -(m.d[0][1] * m.d[2][2] - m.d[0][2] * m.d[2][1]) * dt;
        dst.d[1][1] = (m.d[0][0] * m.d[2][2] - m.d[0][2] * m.d[2][0]) * dt;
        dst.d[2][1] = -(m.d[0][0] * m.d[2][1] - m.d[0][1] * m.d[2][0]) * dt;
        dst.d[0][2] = (m.d[0][1] * m.d[1][2] - m.d[0][2] * m.d[1][1]) * dt;
        dst.d[1][2] = -(m.d[0][0] * m.d[1][2] - m.d[0][2] * m.d[1][0]) * dt;
        dst.d[2][2] = (m.d[0][0] * m.d[1][1] - m.d[0][1] * m.d[1][0]) * dt;

        result = GR_OK;
    }
    else
    {
        // this is the adjoint which can still be usefull
        dst.d[0][0] = (m.d[1][1] * m.d[2][2] - m.d[1][2] * m.d[2][1]);
        dst.d[1][0] = -(m.d[1][0] * m.d[2][2] - m.d[1][2] * m.d[2][0]);
        dst.d[2][0] = (m.d[1][0] * m.d[2][1] - m.d[1][1] * m.d[2][0]);
        dst.d[0][1] = -(m.d[0][1] * m.d[2][2] - m.d[0][2] * m.d[2][1]);
        dst.d[1][1] = (m.d[0][0] * m.d[2][2] - m.d[0][2] * m.d[2][0]);
        dst.d[2][1] = -(m.d[0][0] * m.d[2][1] - m.d[0][1] * m.d[2][0]);
        dst.d[0][2] = (m.d[0][1] * m.d[1][2] - m.d[0][2] * m.d[1][1]);
        dst.d[1][2] = -(m.d[0][0] * m.d[1][2] - m.d[0][2] * m.d[1][0]);
        dst.d[2][2] = (m.d[0][0] * m.d[1][1] - m.d[0][1] * m.d[1][0]);

        result = GR_INVALID_PARMS;
    }

    return result;
}

// ---------------------------------------------------------------------------
// 0x6F71380 - x86MathEngine::general_inverse (assumes last row is 0 0 0 1)
// ---------------------------------------------------------------------------
extern "C" GENRESULT __stdcall sub_6F71380(void *self, Transform &dst, SINGLE &w, const Transform &t)
{
    GENRESULT result;

    const SINGLE determinant = t.d[0][0] * (t.d[1][1] * t.d[2][2] - t.d[2][1] * t.d[1][2]) -
                               t.d[0][1] * (t.d[1][0] * t.d[2][2] - t.d[2][0] * t.d[1][2]) +
                               t.d[0][2] * (t.d[1][0] * t.d[2][1] - t.d[2][0] * t.d[1][1]);

    SINGLE dt;
    if (fabs(determinant) > MIN_DET)
    {
        dt = 1.0f / determinant;
        result = GR_OK;
    }
    else
    {
        dt = 1.0f; // this will at least give us the adjoint
        result = GR_INVALID_PARMS;
    }

    dst.d[0][0] = (t.d[1][1] * t.d[2][2] - t.d[2][1] * t.d[1][2]) * dt;
    dst.d[0][1] = -(t.d[0][1] * t.d[2][2] - t.d[2][1] * t.d[0][2]) * dt;
    dst.d[0][2] = (t.d[0][1] * t.d[1][2] - t.d[1][1] * t.d[0][2]) * dt;

    dst.d[1][0] = -(t.d[1][0] * t.d[2][2] - t.d[2][0] * t.d[1][2]) * dt;
    dst.d[1][1] = (t.d[0][0] * t.d[2][2] - t.d[2][0] * t.d[0][2]) * dt;
    dst.d[1][2] = -(t.d[0][0] * t.d[1][2] - t.d[1][0] * t.d[0][2]) * dt;

    dst.d[2][0] = (t.d[1][0] * t.d[2][1] - t.d[2][0] * t.d[1][1]) * dt;
    dst.d[2][1] = -(t.d[0][0] * t.d[2][1] - t.d[2][0] * t.d[0][1]) * dt;
    dst.d[2][2] = (t.d[0][0] * t.d[1][1] - t.d[1][0] * t.d[0][1]) * dt;

    dst.translation.x = -(t.d[0][1] * t.d[1][2] * t.translation.z +
                          t.d[1][1] * t.d[2][2] * t.translation.x +
                          t.d[2][1] * t.d[0][2] * t.translation.y -
                          t.d[0][1] * t.d[2][2] * t.translation.y -
                          t.d[1][1] * t.d[0][2] * t.translation.z -
                          t.d[2][1] * t.d[1][2] * t.translation.x) * dt;

    dst.translation.y = (t.d[0][0] * t.d[1][2] * t.translation.z +
                         t.d[1][0] * t.d[2][2] * t.translation.x +
                         t.d[2][0] * t.d[0][2] * t.translation.y -
                         t.d[0][0] * t.d[2][2] * t.translation.y -
                         t.d[1][0] * t.d[0][2] * t.translation.z -
                         t.d[2][0] * t.d[1][2] * t.translation.x) * dt;

    dst.translation.z = -(t.d[0][0] * t.d[1][1] * t.translation.z +
                          t.d[1][0] * t.d[2][1] * t.translation.x +
                          t.d[2][0] * t.d[0][1] * t.translation.y -
                          t.d[0][0] * t.d[2][1] * t.translation.y -
                          t.d[1][0] * t.d[0][1] * t.translation.z -
                          t.d[2][0] * t.d[1][1] * t.translation.x) * dt;

    w = (t.d[0][0] * t.d[1][1] * t.d[2][2] +
         t.d[1][0] * t.d[2][1] * t.d[0][2] +
         t.d[2][0] * t.d[0][1] * t.d[1][2] -
         t.d[2][0] * t.d[1][1] * t.d[0][2] -
         t.d[0][0] * t.d[2][1] * t.d[1][2] -
         t.d[1][0] * t.d[0][1] * t.d[2][2]) * dt;

    return result;
}

// ---------------------------------------------------------------------------
// 0x6F715B0 - x86MathEngine::scale
// ---------------------------------------------------------------------------
extern "C" GENRESULT __stdcall sub_6F715B0(void *self, Matrix &dst, const Matrix &m, SINGLE s)
{
    dst.d[0][0] = m.d[0][0] * s;
    dst.d[0][1] = m.d[0][1] * s;
    dst.d[0][2] = m.d[0][2] * s;
    dst.d[1][0] = m.d[1][0] * s;
    dst.d[1][1] = m.d[1][1] * s;
    dst.d[1][2] = m.d[1][2] * s;
    dst.d[2][0] = m.d[2][0] * s;
    dst.d[2][1] = m.d[2][1] * s;
    dst.d[2][2] = m.d[2][2] * s;

    return GR_OK;
}

// ---------------------------------------------------------------------------
// 0x6F71620 - x86MathEngine::det
// ---------------------------------------------------------------------------
extern "C" SINGLE __stdcall sub_6F71620(void *self, const Matrix &m)
{
    return (m.d[0][0] * m.d[1][1] * m.d[2][2] +
            m.d[0][1] * m.d[1][2] * m.d[2][0] +
            m.d[0][2] * m.d[1][0] * m.d[2][1] -
            m.d[0][0] * m.d[1][2] * m.d[2][1] -
            m.d[0][1] * m.d[1][0] * m.d[2][2] -
            m.d[0][2] * m.d[1][1] * m.d[2][0]);
}

// ---------------------------------------------------------------------------
// 0x6F71670 - x86MathEngine::mul (Matrix)
// ---------------------------------------------------------------------------
extern "C" GENRESULT __stdcall sub_6F71670(void *self, Matrix &dst, const Matrix &m1, const Matrix &m2)
{
    //FOR SOME UNKNOWN REASON, THE VC++ 5.0 OPTIMIZER CHOKES THE UNROLLED VERSION
    for (int i = 0; i < 3; i++)
    {
        dst.d[i][0] = m1.d[i][0] * m2.d[0][0] + m1.d[i][1] * m2.d[1][0] + m1.d[i][2] * m2.d[2][0];
        dst.d[i][1] = m1.d[i][0] * m2.d[0][1] + m1.d[i][1] * m2.d[1][1] + m1.d[i][2] * m2.d[2][1];
        dst.d[i][2] = m1.d[i][0] * m2.d[0][2] + m1.d[i][1] * m2.d[1][2] + m1.d[i][2] * m2.d[2][2];
    }

    return GR_OK;
}

// ---------------------------------------------------------------------------
// 0x6F716F0 - x86MathEngine::mul (Transform)
// ---------------------------------------------------------------------------
extern "C" GENRESULT __stdcall sub_6F716F0(void *self, Transform &dst, const Transform &m1, const Transform &m2)
{
    dst.d[0][0] = m1.d[0][0] * m2.d[0][0] + m1.d[0][1] * m2.d[1][0] + m1.d[0][2] * m2.d[2][0];
    dst.d[0][1] = m1.d[0][0] * m2.d[0][1] + m1.d[0][1] * m2.d[1][1] + m1.d[0][2] * m2.d[2][1];
    dst.d[0][2] = m1.d[0][0] * m2.d[0][2] + m1.d[0][1] * m2.d[1][2] + m1.d[0][2] * m2.d[2][2];

    dst.translation.x = m1.d[0][0] * m2.translation.x + m1.d[0][1] * m2.translation.y + m1.d[0][2] * m2.translation.z + m1.translation.x;

    dst.d[1][0] = m1.d[1][0] * m2.d[0][0] + m1.d[1][1] * m2.d[1][0] + m1.d[1][2] * m2.d[2][0];
    dst.d[1][1] = m1.d[1][0] * m2.d[0][1] + m1.d[1][1] * m2.d[1][1] + m1.d[1][2] * m2.d[2][1];
    dst.d[1][2] = m1.d[1][0] * m2.d[0][2] + m1.d[1][1] * m2.d[1][2] + m1.d[1][2] * m2.d[2][2];

    dst.translation.y = m1.d[1][0] * m2.translation.x + m1.d[1][1] * m2.translation.y + m1.d[1][2] * m2.translation.z + m1.translation.y;

    dst.d[2][0] = m1.d[2][0] * m2.d[0][0] + m1.d[2][1] * m2.d[1][0] + m1.d[2][2] * m2.d[2][0];
    dst.d[2][1] = m1.d[2][0] * m2.d[0][1] + m1.d[2][1] * m2.d[1][1] + m1.d[2][2] * m2.d[2][1];
    dst.d[2][2] = m1.d[2][0] * m2.d[0][2] + m1.d[2][1] * m2.d[1][2] + m1.d[2][2] * m2.d[2][2];

    dst.translation.z = m1.d[2][0] * m2.translation.x + m1.d[2][1] * m2.translation.y + m1.d[2][2] * m2.translation.z + m1.translation.z;

    return GR_OK;
}

// ---------------------------------------------------------------------------
// 0x6F71830 - x86MathEngine::transform (Matrix) / rotate (identical, folded)
// ---------------------------------------------------------------------------
extern "C" GENRESULT __stdcall sub_6F71830(void *self, Vector &dst, const Matrix &m, const Vector &v)
{
    dst.x = m.d[0][0] * v.x + m.d[0][1] * v.y + m.d[0][2] * v.z;
    dst.y = m.d[1][0] * v.x + m.d[1][1] * v.y + m.d[1][2] * v.z;
    dst.z = m.d[2][0] * v.x + m.d[2][1] * v.y + m.d[2][2] * v.z;

    return GR_OK;
}
