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
	char &operator[](uint _idx);//[?]
	bool operator == (const char* _cstr);//[?]
	bool operator == (StrSlice& _slice);//[?]
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

bool icy_naming_check(StrSlice &_slice);    //p[]检测一个切片是否符合命名规范

char* jump_space(char* _pos,uint _range = 0);	//p[?]从当前字符起向后，跳过第一段空格，返回这一段空格后的第一个字符的指针

char *jump_space_et_linefd(char *_pos,uint _range = 0);	//[?]从当前字符起向后，跳过第一段空格和回车，返回这一段空格或回车后的第一个字符的指针

char *find_first_char(StrSlice& _slice,char _ch); //p[]找到字符串切片中指定字符首次出现的位置

char get_pair_sign(char _c);		//p[?]为字符配对。传入'('则返回')',传入'['则返回']'...

char* find_pair_sign(char* _begin,int _range = -1);	//p[]找到该字符配对的另一个字符的位置

bool is_current_token_correct(StrSlice &_slice);	//检查当前的token是否合法

StrSlice fetch_name(StrSlice &_slice);	//p[]找到第一个符合命名规范的片段,如在"var a = 12"中提取出"var"

StrSlice fetch_number(StrSlice _slice);	//p[?]找到第一段数字字符切片

bool is_strslice_wrapped_by_brackets(StrSlice &_slice);		//p[]检查这一段代码是否被无意义的圆括号包裹

void decorticate_strslice(StrSlice &_slice);			//[?]无条件地去除代码中的起始和结尾字符

template<typename T>									//p[?]检查一段内存中是否有指定的值
bool is_range_contain(T *_begin, T *_end, T _targ)
{

	for(;_begin != _end; _begin++)
		if(*_begin == _targ)
			return true;
	return false;
}			

StrSlice fetch_icystr(char *_begin,uint _range);

int strslice_to_integer(StrSlice _slice);//[!]

double strslice_to_realnum(StrSlice _slice);//[?]