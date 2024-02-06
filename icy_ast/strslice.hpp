#pragma once

//所有打上问号的函数都是未经测试验证的，打上!的是发现有问题的

using uint = unsigned int;
using ushort = unsigned short;

uint strlen(char* _str);	//p[?]获取字符串长度的函数

uint strlen(const char* _cstr);//p[?]同上

struct StrSlice
{
	char* ptr;
	uint  len;

	StrSlice();
	StrSlice(const char* _cstr);
	StrSlice(char* _str);	//[?]
	char operator[](uint _idx);//[?]
};




bool compair_strslice_with_str(StrSlice& _strslice,char* _str);		//p[]判断一个字符串和一个字符串切片内容是否相同的函数

bool compair_strslice_with_cstr(StrSlice& _strslice,const char* _cstr); //p[]和上一个函数相同，只不过是常量字符串的版本.

bool is_number(char _c);    //p[]检测一个字符是否是数字


//下面有一些检查StrSlice是否是数字的函数     vvvvvv
//如果StrSlice的长度为零的话会直接抛出错误。
bool is_strslice_integer(StrSlice &_slice);  //p[]检测一个字符串切片是否是整数
//规则如下:
/*
 * 如果长度为零，抛出错误（我认为这种情况一定是我编写程序出了问题
 * 所有的字符都应该是数字，否则返回假
 */
bool is_strslice_realnum(StrSlice &_slice);	//p[]检测一个字符串切片是否是实数（带小数点
//规则如下:
/*
 * 如果只有一个字符，则返回假
 * 有且只有一个小数点
 * 不能只含有小数点
 * 除小数点以外的字符都是数字
 */
bool is_strslice_number(StrSlice &_slice);//p[]判断一个字符串切片是否是数字


/////检查StrSlice是否是数字的函数就到这里啦    ^^^^

bool is_letter(char _c);//p[]检测一个字符是否是英文字母

bool icy_naming_check(StrSlice &_slice);    //p[?]检测一个切片是否符合命名规范

char* jump_space(char* _in);	//p[?]从当前字符起向后，跳过第一段空格，返回这一段空格后的第一个字符的指针

char pair_sign(char _c);		//[?]为字符配对。传入'('则返回')',传入'['则返回']'...

char* find_pair_sign(char _sign)	//