#pragma once

using byte = char;

const unsigned int STACK_MAXSIZE = 256;

struct Stack
{
    byte data[STACK_MAXSIZE];
    byte *top;
    byte *base;

    Stack();
};
Stack::Stack()
    :top(data),base(data)
{}