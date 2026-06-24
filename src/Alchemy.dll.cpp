
typedef int(__cdecl *FDUMP_fn)(int, const char *, const char *, int, const char *);
FDUMP_fn *FDUMP;

int dword_6255064;
float flt_6257880[12];
int dword_62578B0;

int dword_62578B4[12];

int dword_625791C;
int dword_6257BC4;

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

extern "C" int __stdcall sub_6201610(int a1)
{
    return 0;
}
