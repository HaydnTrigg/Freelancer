// deformable2.dll
//
// NOTE: this unit is built with /G6 (Pentium Pro scheduling) in addition to the
// shared /O2 flags - the original dalibs were compiled that way and the
// instruction scheduling only matches with it (see config.json).

// sub_65F1100 deliberately reads a couple of registers that the caller leaves
// set (the `vector constructor iterator' style edi value), which trips C4700.
#pragma warning(disable : 4700)

extern "C" void *__cdecl memset(void *, int, unsigned int);
extern "C" void *__cdecl memcpy(void *, const void *, unsigned int);
#pragma intrinsic(memset, memcpy)

// ---------------------------------------------------------------------------
// Globals (resolved against the shared data object by the delinker)
// ---------------------------------------------------------------------------
int dword_6603120;
int dword_6603134;

extern "C" int sub_6600DE0();

const char a3db[] = "*.3db";
const char aFilesystem[] = "FileSystem";
const char aDfmskeleton[] = "DFMSkeleton";
const char aBoneToRoot[] = "Bone to root";
int unk_6602020;

namespace DACOM_CRC
{
    __declspec(dllimport) unsigned long __cdecl GetCRC32(const char *);
}

// ---------------------------------------------------------------------------
// Identity-initialised 3x3 orientation + 3-float position (12 floats).
// ---------------------------------------------------------------------------
struct Matrix
{
    float m[12];
    Matrix *sub_65F13E0();
};

Matrix *Matrix::sub_65F13E0()
{
    m[8] = 1.0f;
    m[4] = 1.0f;
    m[0] = 1.0f;
    m[7] = 0.0f;
    m[6] = 0.0f;
    m[5] = 0.0f;
    m[3] = 0.0f;
    m[2] = 0.0f;
    m[1] = 0.0f;
    m[11] = 0.0f;
    m[10] = 0.0f;
    m[9] = 0.0f;
    return this;
}

// ---------------------------------------------------------------------------
// Deformable model.
// ---------------------------------------------------------------------------
struct DeformablePart // 0x34 bytes
{
    int handle;
    char pad[0x30];
};

struct CDeformable
{
    void *vftable;             // +0x0000
    int count;                 // +0x0004
    DeformablePart parts[150]; // +0x0008 .. +0x1E80
    int rootHandle;            // +0x1E80
    int refCount;              // +0x1E84

    int sub_65F10C0();
    int sub_65F1030(int a2);
    int sub_65F1100(char *name, int *fs);
};

int CDeformable::sub_65F1100(char *name, int *fs)
{
    int v21;
    int v22[2];
    int *v23;
    int name_1;
    char *v25;
    void *CRC32;
    int n48;
    int _DFMSkeleton__1;
    int v30[26];
    Matrix src;
    char v32[40];
    char v33[4];
    char v34[272];
    char *v19;

    int v5 = *fs;
    int *self_1 = (int *)this;
    CRC32 = this;
    int v7 = (*(int(__stdcall **)(int *, const char *, char *))(v5 + 72))(fs, a3db, &((char *)&src)[44]);
    v22[0] = v7;
    if (v7 != -1)
    {
        v19 = (char *)self_1; // placeholder for the caller-supplied edi value
        do
        {
            v30[13] = (int)aFilesystem;
            memset(&v30[14], 0, 0x2C);
            v5 = *fs;
            v30[15] = (int)v33;
            v21 = 0;
            v30[16] = 0x80000000;
            v30[17] = 1;
            v30[19] = 3;
            v30[20] = 134217856;
            v30[12] = 52;
            v30[24] = v7;
            if ((*(int(__stdcall **)(int *, int *, int *, char *))(v5 + 12))(fs, &v30[12], &v21, v19) >= 0)
            {
                int v10 = (*(int(__stdcall **)(int, const char *))(*(int *)v22[0] + 96))(v22[0], aDfmskeleton);
                _DFMSkeleton__1 = v10;
                v23 = (int *)((char *)self_1 + 0x34 * self_1[1] + 8);
                v30[1] = (int)aFilesystem;
                memset(&v30[2], 0, 0x2C);
                v30[4] = 0x80000000;
                v30[5] = 1;
                v30[7] = 3;
                v30[8] = 134217856;
                v30[3] = (int)aBoneToRoot;
                v30[0] = 52;
                v30[12] = -1;
                src.sub_65F13E0();
                int name_2 = 0;
                int v13 = (*(int(__stdcall **)(int, int *))(*(int *)v22[0] + 124))(v22[0], v30);
                if (v13 != -1)
                {
                    if ((*(int(__stdcall **)(int, int, Matrix *, int, int *, int))(*(int *)v22[0] + 20))(v22[0], v13, &src, 48, &n48, 0) && n48 == 48)
                    {
                        memcpy(v23 + 1, &src, 0x30);
                        name_1 = 1;
                    }
                    else
                    {
                        name_1 = 0;
                    }
                    (*(void(__stdcall **)(int, int))(*(int *)v22[0] + 16))(v22[0], v13);
                    name_2 = name_1;
                }
                signed int v14 = name_2 != 0 ? 0 : 0x80004005;
                if (_DFMSkeleton__1)
                    (*(void(__stdcall **)(int *, void *))(*fs + 96))(fs, &unk_6602020);
                if (v14 >= 0)
                {
                    v25 = v34;
                    CRC32 = (void *)DACOM_CRC::GetCRC32(v34);
                    int v15 = (*(int(__stdcall **)(int, char **, int))(*(int *)dword_6603134 + 48))(dword_6603134, &v25, v22[0]);
                    *v23 = v15;
                    ++self_1[1];
                }
                v7 = v22[1];
            }
            if (v22[0])
                (*(void(__stdcall **)(int))(*(int *)v22[0] + 8))(v22[0]);
            v19 = v32;
        } while ((*(int(__stdcall **)(int *, int))(*fs + 76))(fs, v7));
        (*(void(__stdcall **)(int *, int))(*fs + 80))(fs, v7);
    }
    (*(void(__stdcall **)(int, int *))(*(int *)dword_6603134 + 36))(dword_6603134, v22);
    (*(void(__stdcall **)(int, int *))(*(int *)dword_6603134 + 32))(dword_6603134, fs);
    name_1 = (int)name;
    v25 = (char *)DACOM_CRC::GetCRC32(name);
    int v16 = (*(int(__stdcall **)(int, int *, int *))(*(int *)dword_6603134 + 48))(dword_6603134, &name_1, fs);
    int v17 = v22[0];
    self_1[1952] = v16;
    (*(void(__stdcall **)(int, int))(*(int *)dword_6603134 + 32))(dword_6603134, v17);
    (*(void(__stdcall **)(int))(*(int *)v22[0] + 8))(v22[0]);
    return self_1[1952] != -1;
}

int CDeformable::sub_65F1030(int a2)
{
    Matrix dst;
    Matrix src;
    Matrix src_1;
    int v2 = dword_6603120;
    int v4 = *((int *)this + 2) + 4;
    if (!dword_6603120)
    {
        v2 = sub_6600DE0();
        dword_6603120 = v2;
    }
    (*(void(__stdcall **)(int, Matrix *, int *, int))(*(int *)v2 + 28))(v2, &src, (int *)this + 3, v4);
    int v5 = dword_6603120;
    dst = src;
    if (!dword_6603120)
    {
        v5 = sub_6600DE0();
        dword_6603120 = v5;
    }
    int result = (*(int(__stdcall **)(int, Matrix *, int, Matrix *))(*(int *)v5 + 28))(v5, &src_1, a2, &dst);
    *(Matrix *)((char *)this + 0x40) = src_1;
    return result;
}

int CDeformable::sub_65F10C0()
{
    int v2 = 0;
    if (count > 0)
    {
        DeformablePart *v3 = parts;
        do
        {
            (*(int(__stdcall **)(int, int))(*(int *)dword_6603134 + 60))(dword_6603134, v3->handle);
            ++v2;
            ++v3;
        } while (v2 < count);
    }
    return (*(int(__stdcall **)(int, int))(*(int *)dword_6603134 + 60))(dword_6603134, rootHandle);
}

// ---------------------------------------------------------------------------
// `vector constructor iterator' (compiler helper, hand-written here).
// ---------------------------------------------------------------------------
extern "C" int __stdcall sub_65F1000(int a1, int a2, int a3, int(__fastcall *a4)(int))
{
    if (--a3 < 0)
        return a3;
    int result;
    int v6 = a3 + 1;
    do
    {
        result = a4(a1);
        a1 += a2;
        --v6;
    } while (v6);
    return result;
}
