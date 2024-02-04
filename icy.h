#pragma once

#include<vector>



using ushort = unsigned short;
using uint   = unsigned int;
using byte   = char;

enum icyobj_t
{
	NIL = 0,
	FUNCTION,
	
};

struct icyObject
{	
	uint  type;
	byte* source_ptr;
};


using icyFuncParamStruct = std::vector<icyObject>; //icyFunction 对象中保存函数参数的数据结构，我们暂且用std::vector吧

//CIRNO的函数
class IcyFunction
{
	icyFuncParamStruct m_param_table;
	icyObject          ret_value;

};
class IcyThread
{

};
class IcyProcess
{

};
