//用这个临时测试一下我写的这些函数能不能正常使用
#include<iostream>
#include"strslice.hpp"

using namespace std;

int main()
{
    StrSlice slice;

    char c{'\0'};

    char buffer[64];

    while(!compair_strslice_with_cstr(slice,"quit"))
    {
        cin.getline(buffer,63);
        slice = StrSlice(buffer);
        cout << (is_strslice_number(slice) ? "True\n":"False\n");
    }
}
