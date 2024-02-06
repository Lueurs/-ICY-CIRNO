//用这个临时测试一下我写的这些函数能不能正常使用
#include<iostream>
#include"strslice.hpp"

using namespace std;

int main()
{
    StrSlice all = "print@(12)";
    StrSlice first = fetch_name(all);

    for(uint i=0; i<first.len; i++)
        cout.put(first[i]);

}
