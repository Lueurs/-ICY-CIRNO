#include<iostream>
#include"icy_ast.hpp"

using namespace std;

int main()
{
    StrSlice slice;
    char buff[64];

    char* pos{nullptr};

    while(!compair_strslice_with_cstr(slice,"quit"))
    {
        cin.getline(buff,63);

        slice.ptr = buff;
        slice.len = strlen(buff);

        try{
        slice = Cirno::icy_find_minlevel_token(slice);
        }

        catch(const char* _msg)
        {
            cerr << _msg;
        }
        if(slice.ptr)
        for(uint i=0; i<slice.len; i++)
        {
            cout.put(slice[i]);
        }
        cout.put('\n');
        //cout << (icy_naming_check(slice)?"Correct\n":"Incorrect\n");
    }
    return 0;
}

int main2()
{
    StrSlice slice;
    char buff[64];
    while(!compair_strslice_with_cstr(slice,"quit"))
    {
        cin.getline(buff,63);
        slice.ptr = buff;
        slice.len = strlen(buff);
        cout << (Cirno::is_icy_keywd(slice) ? "Yes\n" : "No\n");
    }
    return 0;
}

