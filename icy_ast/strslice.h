#pragma once

//所有打上问号的函数都是未经测试验证的，打上!的是发现有问题的

using uint = unsigned int;
using ushort = unsigned short;

struct StrSlice
{
	char* ptr;
	uint  len;
	char operator[](uint _idx)
	{
		if(_idx >= len)
			throw "Error:strslice out of range";
		else
			return ptr[_idx];

	}
};


uint strlen(char* _str);	//p[?]获取字符串长度的函数

bool compair_strslice_with_str(StrSlice& _strslice,char* _str);		//[?]判断一个字符串和一个字符串切片内容是否相同的函数

bool is_number(char _c);    //p[?]检测一个字符是否是数字

bool is_strslice_number(StrSlice &_slice);  //p[?]检测一个字符串切片是否是数字

bool is_letter(char _c);//p[?]检测一个字符是否是英文字母

bool icy_naming_check(StrSlice &_slice);    //[?]检测一个切片是否符合命名规范
