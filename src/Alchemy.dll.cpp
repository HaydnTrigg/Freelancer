
int dword_62578B4[12];

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
