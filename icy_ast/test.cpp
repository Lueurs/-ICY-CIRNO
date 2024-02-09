#include<iostream>
#include"strslice.hpp"


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
        slice = fetch_name(slice);
        }

        catch(const char* _msg)
        {
            cerr << _msg;
        }
        if(slice)
        for(uint i=0; i<slice.len; i++)
        {
            cout.put(slice[i]);
        }
        cout.put('\n');
        //cout << (icy_naming_check(slice)?"Correct\n":"Incorrect\n");
    }


}
/*
int main()
{
    DEBUG_LOG("Debug log:\n");
    FINISH_DEBUG_LOG
}

*/
