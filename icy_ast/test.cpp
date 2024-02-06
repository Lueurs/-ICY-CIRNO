//用这个临时测试一下我写的这些函数能不能正常使用
#include<iostream>

#include"strslice.hpp"
#include"icy_ast.hpp"

using namespace std;

int main()
{
    StrSlice all = "(print@(12))";
    try
    {
        StrSlice first = fetch_name(all);

        for(uint i=0; i<first.len; i++)
            cout.put(first[i]);

        cout <<*(find_pair_sign(all.ptr,-1));

        StrSlice first_token = Cirno::icy_fetch_current_token(all);
        for(uint i=0; i<first_token.len; i++)
            cout.put(first_token[i]);

    }
    catch(const char* msg)
    {
        cerr << msg << '\n';
    }




    //error_log.close();
}
