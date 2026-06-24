
typedef int(__cdecl *FDUMP_fn)(int, const char *, const char *, int, const char *);
FDUMP_fn *FDUMP;

int dword_6255064;
float flt_6257880[12];
int dword_62578B0;

int dword_62578B4[12];

int dword_625791C;
int dword_6257BC0;
int dword_6257BC4;
char byte_6257918;
char byte_6257919;

extern "C" void sub_623F450();
extern "C" int __fastcall sub_6201CC0(int thisptr);

int dword_62578E4;
int dword_62578E8;
int dword_62578EC;
int dword_62578F0;
int dword_62578F4;
int dword_62578F8;
int dword_62578FC;
int dword_6257900;
int dword_6257904;

extern "C" void sub_6201000()
{
    dword_62578E4 = 0;
    dword_62578E8 = 0;
    dword_62578EC = 0;
    dword_62578F0 = 0;
    dword_62578F4 = 0;
    dword_62578F8 = 0;
    dword_62578FC = 0;
    dword_6257900 = 0;
    dword_6257904 = 0;
}

extern "C" int* sub_6201030()
{
    int *result = &dword_62578B4[2];
    int n = 4;
    do
    {
        result[-2] = 0;
        result[-1] = 0;
        result[0] = 0;
        result += 3;
    } while (--n);
    return result;
}

extern "C" void sub_6201050()
{
    dword_62578B0 = 0;
}

extern "C" void _cfltcvt_init()
{
    flt_6257880[8] = 1.0f;
    flt_6257880[4] = 1.0f;
    flt_6257880[0] = 1.0f;
    flt_6257880[7] = 0.0f;
    flt_6257880[6] = 0.0f;
    flt_6257880[5] = 0.0f;
    flt_6257880[3] = 0.0f;
    flt_6257880[2] = 0.0f;
    flt_6257880[1] = 0.0f;
    flt_6257880[11] = 0.0f;
    flt_6257880[10] = 0.0f;
    flt_6257880[9] = 0.0f;
}

extern "C" int sub_62010E0()
{
    int result = dword_6257BC4;
    if (dword_6257BC4)
        result = (*(int(__stdcall **)(int, int *))(*(int *)dword_6257BC4 + 16))(dword_6257BC4, &dword_625791C);
    dword_625791C = 0;
    return result;
}

extern "C" char sub_6201100()
{
    int v0 = dword_6257BC4;
    if (!dword_6257BC4)
        goto LABEL_7;
    if (dword_625791C)
    {
        (*(void(__stdcall **)(int, int *))(*(int *)dword_6257BC4 + 16))(dword_6257BC4, &dword_625791C);
        v0 = dword_6257BC4;
        dword_625791C = 0;
    }
    if ((*(int(__stdcall **)(int, int, int, int *, int))(*(int *)v0 + 12))(v0, 322, dword_6255064, &dword_625791C, 1) < 0)
    {
        (*FDUMP)(1048577, "%s(%d) : ERROR:General:%s",
                 "C:\\work\\builds\\dalibs\\dalibs-build\\build\\Src\\Alchemy\\Alchemy.cpp", 67,
                 "Alchemy unable to create Vertex Buffer.  Effects will not work");
        if (dword_6257BC4)
            (*(void(__stdcall **)(int, int *))(*(int *)dword_6257BC4 + 16))(dword_6257BC4, &dword_625791C);
    LABEL_7:
        dword_625791C = 0;
        return 0;
    }
    return 1;
}

// TODO: sub_6201190 (0x6201190, 416 bytes) - base destructor ~Class() of a
// multiple-inheritance class (resets two vtables: off_624B0E8 @+0, off_624B0BC @+4).
// Blocked on reconstructing DA's custom red-black-tree map container (NOT std::map:
// 20-byte layout {_Myhead@+4, _Nil@+8, _Mysize@+0x10}, ops shared module-wide via
// sub_6202DA0/62029C0/6202460/6202DE0/6202E40/6202FF0) + its value-wrapper class
// (Releases an interface) + the two base interfaces. Foundational ~10-fn subproject.

extern "C" int __stdcall sub_6201330(int a1)
{
    int v1 = dword_6257BC4;
    if (!dword_6257BC4)
        goto LABEL_7;
    if (dword_625791C)
    {
        (*(void(__stdcall **)(int, int *))(*(int *)dword_6257BC4 + 16))(dword_6257BC4, &dword_625791C);
        v1 = dword_6257BC4;
        dword_625791C = 0;
    }
    if ((*(int(__stdcall **)(int, int, int, int *, int))(*(int *)v1 + 12))(v1, 322, dword_6255064, &dword_625791C, 1) < 0)
    {
        (*FDUMP)(1048577, "%s(%d) : ERROR:General:%s",
                 "C:\\work\\builds\\dalibs\\dalibs-build\\build\\Src\\Alchemy\\Alchemy.cpp", 67,
                 "Alchemy unable to create Vertex Buffer.  Effects will not work");
        if (dword_6257BC4)
            (*(void(__stdcall **)(int, int *))(*(int *)dword_6257BC4 + 16))(dword_6257BC4, &dword_625791C);
    LABEL_7:
        dword_625791C = 0;
        return 0;
    }
    return 0;
}

extern "C" int __stdcall sub_62013C0(int a1)
{
    if (dword_6257BC4)
        (*(void(__stdcall **)(int, int *))(*(int *)dword_6257BC4 + 16))(dword_6257BC4, &dword_625791C);
    dword_625791C = 0;
    return 0;
}

// TODO: sub_62013F0 (0x62013F0, 541 bytes) - class Init method (QueryInterface chain
// for IRenderPipeline/ITextureLibrary/IRPVertexBuffer/IRPIndexBuffer/IRPDraw, then
// DACOM_Acquire("FxNode") + scene-graph setup). Blocked: MSVC custom calling convention
// passes args in ecx (this) AND edi - no C/C++ construct emits an edi parameter.
// Needs __declspec(naked)+asm, or recovering the static caller that sets edi.

extern "C" int __stdcall sub_6201610(int a1)
{
    return 0;
}

// TODO: sub_6201620 (0x6201620, 145 bytes) - recursive scene-graph walk (copies the
// flt_6257880 transform via rep movsd, recurses over child nodes). Blocked: MSVC custom
// calling convention passes args in edi AND esi - not expressible in C/C++.
// Needs __declspec(naked)+asm, or the static-caller register context.

// TODO: sub_62016C0 (0x62016C0, 326 bytes) - scene-graph traversal applying the
// flt_6257880 transform. Blocked: MSVC custom calling convention (arg in ebp).
// Needs __declspec(naked)+asm, or the static-caller register context.

// TODO: sub_6201810 (0x6201810, 92 bytes) - recursive node visitor. Blocked: MSVC
// custom calling convention (args in ecx + edi). Not expressible in C/C++.

// TODO: sub_6201870 (0x6201870, 155 bytes) - recursive node visitor. Blocked: MSVC
// custom calling convention (arg in edi). Not expressible in C/C++.

// TODO: sub_6201910 (0x6201910, 193 bytes) - scene-graph draw walk. Blocked: MSVC
// custom calling convention (arg in edi). Not expressible in C/C++.

extern "C" int __stdcall sub_62019E0(int a1)
{
    if (byte_6257918)
    {
        (*FDUMP)(1048578, "%s(%d) : WARNING:General:%s",
                 "C:\\work\\builds\\dalibs\\dalibs-build\\build\\Src\\Alchemy\\Alchemy.cpp", 337,
                 "You must call end_draw method before call beging_draw method.\n");
        return -1;
    }
    byte_6257918 = 1;
    if ((*(int(__stdcall **)(int, int))(*(int *)dword_6257BC4 + 40))(dword_6257BC4, dword_625791C) < 0)
    {
        int v2 = dword_6257BC4;
        if (!dword_6257BC4)
            goto LABEL_10;
        if (dword_625791C)
        {
            (*(void(__stdcall **)(int, int *))(*(int *)dword_6257BC4 + 16))(dword_6257BC4, &dword_625791C);
            v2 = dword_6257BC4;
            dword_625791C = 0;
        }
        if ((*(int(__stdcall **)(int, int, int, int *, int))(*(int *)v2 + 12))(v2, 322, dword_6255064, &dword_625791C, 1) < 0)
        {
            (*FDUMP)(1048577, "%s(%d) : ERROR:General:%s",
                     "C:\\work\\builds\\dalibs\\dalibs-build\\build\\Src\\Alchemy\\Alchemy.cpp", 67,
                     "Alchemy unable to create Vertex Buffer.  Effects will not work");
            if (dword_6257BC4)
                (*(void(__stdcall **)(int, int *))(*(int *)dword_6257BC4 + 16))(dword_6257BC4, &dword_625791C);
        LABEL_10:
            dword_625791C = 0;
            return -1;
        }
    }
    if (byte_6257919)
    {
        int *v3 = (int *)dword_62578B4 + 2;
        do
        {
            v3[-2] = 0;
            v3[-1] = 0;
            v3[0] = 0;
            v3 += 3;
        } while ((int)v3 < (int)&dword_62578EC);
    }
    (*(void(__stdcall **)(int, int, int))(*(int *)dword_6257BC0 + 244))(dword_6257BC0, 22, a1 + 44);
    (*(void(__stdcall **)(int, int, int))(*(int *)dword_6257BC0 + 244))(dword_6257BC0, 137, a1 + 60);
    (*(void(__stdcall **)(int, int, int))(*(int *)dword_6257BC0 + 244))(dword_6257BC0, 14, a1 + 64);
    (*(void(__stdcall **)(int, int, int))(*(int *)dword_6257BC0 + 244))(dword_6257BC0, 27, a1 + 48);
    (*(void(__stdcall **)(int, int, int))(*(int *)dword_6257BC0 + 244))(dword_6257BC0, 19, a1 + 52);
    (*(void(__stdcall **)(int, int, int))(*(int *)dword_6257BC0 + 244))(dword_6257BC0, 20, a1 + 56);
    sub_623F450();
    dword_62578E4 = 0;
    dword_62578E8 = 0;
    dword_62578EC = 0;
    dword_62578F0 = 0;
    dword_62578F4 = 0;
    dword_62578F8 = 0;
    dword_62578FC = 0;
    dword_6257900 = 0;
    dword_6257904 = 0;
    return 0;
}

extern "C" int __stdcall sub_6201BA0(int *a1)
{
    if (!byte_6257918)
    {
        (*FDUMP)(1048577, "%s(%d) : ERROR:General:%s",
                 "C:\\work\\builds\\dalibs\\dalibs-build\\build\\Src\\Alchemy\\Alchemy.cpp", 384,
                 "You must call begin_draw method before call end_draw method.\n");
        return -1;
    }
    byte_6257918 = 0;
    if (byte_6257919)
        sub_6201CC0((int)a1);
    (*(void(__stdcall **)(int, int, int))(*(int *)dword_6257BC0 + 240))(dword_6257BC0, 22, a1[11]);
    (*(void(__stdcall **)(int, int, int))(*(int *)dword_6257BC0 + 240))(dword_6257BC0, 137, a1[15]);
    (*(void(__stdcall **)(int, int, int))(*(int *)dword_6257BC0 + 240))(dword_6257BC0, 14, a1[16]);
    (*(void(__stdcall **)(int, int, int))(*(int *)dword_6257BC0 + 240))(dword_6257BC0, 27, a1[12]);
    (*(void(__stdcall **)(int, int, int))(*(int *)dword_6257BC0 + 240))(dword_6257BC0, 19, a1[13]);
    (*(void(__stdcall **)(int, int, int))(*(int *)dword_6257BC0 + 240))(dword_6257BC0, 20, a1[14]);
    return 0;
}

extern "C" int __cdecl sub_6201C70(int a1, int a2)
{
    int *v3 = dword_62578B4 + 3 * a1;
    int v2 = v3[1];
    v3[1] = a2;
    if (!v2)
    {
        v3[0] = a2;
        *(int *)(a2 + 8) = 0;
        *(int *)(a2 + 4) = 0;
    }
    else
    {
        *(int *)(v2 + 8) = a2;
        *(int *)(a2 + 8) = 0;
        *(int *)(a2 + 4) = v2;
    }
    ++v3[2];
    return 0;
}

extern "C" int __fastcall sub_6201CC0(int thisptr)
{
    int *v0 = dword_62578B4;
    int *v9 = dword_62578B4;
    int result = 0;
    do
    {
        int *v1 = (int *)*v0;
        result = 0;
        int *v3 = (int *)*v0;
        if (*v0)
        {
            int v4 = 0;
            int v5 = 0;
            do
            {
                int v7 = 0;
                int v8 = 0;
                (*(void(__stdcall **)(int *, int *))(*v3 + 208))(v3, &v7);
                (*(void(__stdcall **)(int *, int *))(*v3 + 212))(v3, &v8);
                v3 = (int *)v3[2];
                v4 += v7;
                v5 += v8;
            } while (v3);
            int *v6 = v1;
            (*(void(__stdcall **)(int *, int, int))(*v1 + 156))(v1, v4, v5);
            do
            {
                (*(void(__stdcall **)(int *))(*v6 + 164))(v6);
                v6 = (int *)v6[2];
            } while (v6);
            result = (*(int(__stdcall **)(int *))(*v1 + 160))(v1);
            v0 = v9;
        }
        v0 += 3;
        v9 = v0;
    } while ((int)v0 < (int)&dword_62578E4);
    return result;
}

// TODO: sub_6201D60 (0x6201D60, 152 bytes) - factory that operator-new's an object
// (off_624B280/off_624B230 vtables, ctor sub_623FB80) with C++ exception handling (SEH
// frame + handler SEH_6201D60). Blocked on reconstructing that class + its EH, same
// foundation as sub_6201190.

// TODO: sub_6201E00 (0x6201E00, 144 bytes) - scene-graph operation. Blocked: MSVC custom
// calling convention (args in ebp + esi). Not expressible in C/C++.
