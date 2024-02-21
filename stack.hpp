#pragma once

using byte = char;

const unsigned int STACK_MAXSIZE = 256;

struct Stack
{
    ::byte data[STACK_MAXSIZE];
    ::byte *top;
    ::byte *base;

    Stack():base(data),top(data){}
};

template<typename __Tp1,typename __Tp2>
struct MyPair{
    __Tp1 first;
    __Tp2 second;
    MyPair(__Tp1 __first,__Tp2 __second);
    MyPair();
};
template<typename __Tp1,typename __Tp2>
MyPair<__Tp1,__Tp2>::MyPair(__Tp1 __first,__Tp2 __second)
    :first(__first),second(__second){}
template<typename __Tp1,typename __Tp2>
MyPair<__Tp1,__Tp2>::MyPair(){}
 

