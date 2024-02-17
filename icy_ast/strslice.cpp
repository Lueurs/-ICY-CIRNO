#include<math.h>
#include"icydebug.hpp"
//#define DEBUG 114514



#ifdef DEBUG
#include<iostream>
#endif

using uint   = unsigned int;
using ushort = unsigned short;

char NAME_DIVISION_CHARS[] = " \n;,()[]{}:+-*/&!%@=<>\'\"";

uint strlen(char* _str)
{
	if(!_str)
		return 0u;
	uint result{0u};
	while(*_str)
	{
		result++;
		_str++;
	}
	return result;
}

uint strlen(const char* _cstr)
{
	if(!_cstr)
		return 0u;
	uint result{0u};
	while(*_cstr)
	{
		result++;
		_cstr++;
	}
	return result;
}


struct StrSlice
{
	char* ptr;
	uint  len;
	ushort property;
	StrSlice();
	StrSlice(const char* _cstr);
	StrSlice(char* _str);
	char &operator[](uint _idx);
	bool operator == (const char* _cstr);
	bool operator == (StrSlice &_slice);
	bool operator !=(const char* _cstr);
	bool operator != (StrSlice &_slice);

};

bool compair_strslice_with_str(StrSlice &_strslice,char *_str)
{
	if(strlen(_str) != _strslice.len)
		return false;
	else
	{
		bool result = true;
		for(uint i=0; i < _strslice.len; i++)
			if(_str[i] != _strslice[i])
				result = false;
		return result;
	}
}

bool compair_strslice_with_cstr(StrSlice &_strslice,const char*_cstr)
{
	if(strlen(_cstr) != _strslice.len)
		return false;
	else
	{
		bool result = true;
		for(uint i=0; i < _strslice.len; i++)
			if(_cstr[i] != _strslice[i])
				result = false;
		return result;
	}
}

StrSlice::StrSlice()
	:ptr(nullptr),len(0){}

StrSlice::StrSlice(const char* _cstr)//[]
	:ptr(const_cast<char*>(_cstr))
	{
		uint n{0};
		char* p = ptr;
		for(; *p; n++,p++);
		len = n;
	}

StrSlice::StrSlice(char* _str)//[]
	:ptr(_str)
	{
		uint n{0};
		for(; *_str; n++,_str++);
		len = n;
	}

char &StrSlice::operator[](uint _idx)//[?]
{
	if(_idx >= len)
		throw "Error:strslice out of range\n"; //暂先throw这个吧
	else
		return ptr[_idx];

}

bool StrSlice::operator==(const char* _cstr)
{
	return compair_strslice_with_cstr(*this,_cstr);
}

bool StrSlice::operator == (StrSlice& _slice)
{
	if(_slice.ptr == ptr && _slice.len == len)
		return true;
	else if(_slice.len != len)
		return false;
	else
		for(uint i=0;i<len;i++)
			if(_slice[i] != *(ptr+i))
				return false;
	return true;
}

bool StrSlice::operator !=(const char* _cstr)
{
	return !(*this == _cstr);
}
bool StrSlice::operator != (StrSlice &_slice)
{
	return !(*this == _slice);
}



bool is_number(char _c)
{
	return _c>='0' && _c <= '9';
}

bool is_strslice_integer(StrSlice &_slice)
{
	bool result{true};
	for(uint i=0; i<_slice.len; i++)
		if(!is_number(_slice[i]))
			result = false;
	return result;
}

bool is_strslice_realnum(StrSlice &_slice)
{
	uint num_dots{0};
	bool result{true};
	if(!is_number(_slice[0]))
		return false;
	for(uint i=1; i < _slice.len; i++)
	{
		if(_slice[i] == '.')
			num_dots ++;
		else if(!is_number(_slice[i]))
			result = false;
	}
	if(num_dots != 1)//如果小数点多于一个则这不是一个浮点实数（显然一个数字我们最多只能写一个小数点
		return false;
	if(_slice[_slice.len-1] == '.')//不可以以小数点结尾
		return false;
	//if(num_dots == _slice.len)//不可以全都是小数点，这包含了StrSlice有且仅有一个小数点的情况.
	//	return false;
	return result;
}

bool is_strslice_number(StrSlice &_slice)
{
	return (is_strslice_realnum(_slice) || is_strslice_integer(_slice));	//是整数或者浮点实数都返回真
}

bool is_letter(char _c)
{
	return (_c >= 'a' && _c <= 'z') || (_c >= 'A' && _c <= 'Z');
}

bool icy_naming_check(StrSlice &_slice)
{
	if(!is_letter(_slice[0]) && _slice[0] != '_')
		return false;
	
	bool result{true};
	for(uint i = 1; i < _slice.len; i++)
	{
		if(!(is_letter(_slice[i]) || is_number(_slice[i]) || _slice[i] == '_'))
			result = false;
	}
	return result;
}

char* jump_space(char* _pos,uint _range = 0)
{
	if(_range)
		for(uint i=0;i<_range && *_pos == ' ';i++)
			_pos++;
	else
		while(*_pos == ' ')
			_pos++;
	return _pos;
}


char *jump_space_et_linefd(char * _pos,uint _range = 0)
{
	if(_range)
		for(uint i=0;i<_range && (*_pos == ' ' || *_pos == '\n');i++)
			_pos++;
	else
		while(*_pos == ' ' || *_pos == '\n')
			_pos++;
	return _pos;
}

char *find_first_char(StrSlice& _slice,char _ch)
{
	char *result{nullptr};
	uint i;
	for(i=0; i < _slice.len; i++)
	{
		if(_slice[i] == _ch)
			break;
	}
	if(i != _slice.len)
		result = _slice.ptr + i;
	return result;
}

char get_pair_sign(char _c)
{
	switch(_c)
	{
		case '(':	return ')';
		case ')':	return '(';
		case '[':	return ']';
		case ']':	return '[';
		case '{':	return '}';
		case '}':	return '{';
		default:
			throw "Exception from function pair_sign";
	}
}

char* find_pair_sign(char *_begin,int _range = -1)
{
	char pair_prev = *_begin;
	char pair_post = get_pair_sign(pair_prev);

	int level{0};
	if(_range < 0)//_range小于零表示不设置搜找长度限制，一直搜索到字符串末尾为止
	{
		while(*_begin)
		{
			if(*_begin == pair_prev)
				level++;
			else if(*_begin == pair_post)
			{
				level--;
				if(level == 0)
					break;
			}

			_begin++;
		}
	}
	else//否则按照指定的长度范围搜索配对符号
	{
		for(uint i=0; i<_range && *_begin; i++)
		{
			if(*_begin == pair_prev)
				level++;
			else if(*_begin == pair_post)
			{
				level--;
				if(level == 0)
					break;
			}
			_begin++;
		}
	}
	if(level != 0)
		throw"Exception from function find_pair_sign:unpaired bracket\n";//如果level不等于零说明代码中的括号没有匹配完整，抛出错误。

	return _begin;

}

StrSlice fetch_name(StrSlice _slice)//找到StrSlice中符合命名规范的第一个片段
{
	StrSlice name;//默认地，name的指针为空，长度为0
	uint i;
	for(i=0; i<_slice.len; i++)
	{
		if(_slice[i] == '_' || is_letter(_slice[i])) //一直扫描到名字的首字母（英文字母或者下划线），然后跳出循环
			break;
	}
	if(i == _slice.len)//如果遍历到末尾还没有找到符合要求的段落，就返回空的片段
		return name;
	name.ptr = &_slice[i];
	uint begin {i};
	while(i != _slice.len && (is_letter(_slice[i]) || is_number(_slice[i]) || _slice[i] == '_'))//遍历完一整个name
		i++;
	name.len = i - begin;
	return name;

}

StrSlice fetch_number(StrSlice _slice)
{
	StrSlice number;
	uint i=0;
	bool dot{false};	//此时还未发现小数点
	for(; i<_slice.len; i++)
	{
		if(is_number(_slice[i]))
			break;									//如果遇到小数点或者数字，就跳出循环，在下面尝试读取这个[可能是数字]的片段
	}
	number.ptr = _slice.ptr + i;
	while(i != _slice.len && (is_number(_slice[i]) || _slice[i] == '.'))
	{
		if(_slice[i] == '.')
		{
			if(dot)
				break;	//	如果前面已经有小数点了，那就跳出
			dot = true;//做标记
		}
		number.len++;
		i++;
	}
	return number;
}

bool is_strslice_wrapped_by_brackets(StrSlice &_slice)
{
	if(_slice[0] != '(')
		return false;
	ushort level{1};

	uint i{1};
	for(; i<_slice.len; i++)
	{
		if(_slice[i] == '(')
			level++;
		else if(_slice[i] == ')')
			level--;
		if(level == 0)
			break;
	}
	//跳出之后i的值就是第一个圆括号完成配对的地方。如果这个括号是在末尾才被配对，说明这段代码完全被一个无意义圆括号包裹着。
	if(level != 0)
		throw"Exception from function \"is_strslice_wrapped_by_bracket\":unpaired bracket\n";
	if(i == _slice.len-1)
		return true;
	else
		return false;
}

void decorticate_strslice(StrSlice &_slice)
{
	if(_slice.len <= 2)
		throw "Exception from function \"decorticate_strslice\":too short slice to be decorticated.\n";
	else{
		_slice.ptr++;
		_slice.len -= 2;
	}
	
}

StrSlice fetch_icystr(char *_begin,uint _range)
{
	char end_sign = *_begin;//如果是双引号就用双引号结尾，如果是单引号就用单引号结尾
	//unfinished


}

int strslice_to_integer(StrSlice _slice)
{
	int result{0};
	for(int i = _slice.len-1; i >= 0; i--)
		result += (_slice[i] - '0') * pow(10, _slice.len-i-1);
	return result;
}

double strslice_to_realnum(StrSlice _slice)
{
	double level{.0f};
	uint i{0};
	for(; i<_slice.len; i++)
		if(_slice[i] == '.')
			break;
	//3.14159
	//6543210
	level = pow(10,_slice.len-i-1);
	
	double big_value{.0};
	double small_value{.0};
	for(int j = i-1;j>=0;j--)
		big_value += (_slice[j] - '0') * pow(10,(i-1)-j);
	for(int j = _slice.len-1;j>i;j--)
		small_value += (_slice[j]-'0') * pow(10,_slice.len - j - 1);
	small_value /= level;
	return big_value + small_value;
	
}

bool is_in_namediv_char(char _ch)
{
	for(auto c : NAME_DIVISION_CHARS)
		if(_ch == c)
			return true;
	return false;
}

/*
char *find_strslice(char* _begin,StrSlice _target,uint _range = 0)
{
	if(!_range)
		_range = strlen(_begin);
	char *sk{_begin};
	bool find{false};
	bool independent{false};
	for(;sk != _begin + _range - _target.len; sk++)
	{
		if(is_in_division_char(*sk))
			independent = true;
		if(*sk == _target[0])
		{
			find = true;
			for(uint i=1; i<_target.len; i++)
				if(sk[i] != _target[i])
					find = false;
		}
		if(find)
			break;
	}
	return sk;

}
*/
char *find_strslice(char *_begin,StrSlice _target,uint _range = 0)
{
	if(!_range)
		_range = strlen(_begin);
	char *segment_begin{_begin};
	char *segment_end{nullptr};
	bool find(false);
	while(segment_begin != _begin + _range)
	{
		while(segment_begin != _begin + _range && is_in_namediv_char(*segment_begin))//一个指针定位到名称开头
			segment_begin++;
		segment_end = segment_begin;
		while(segment_end != _begin + _range && !is_in_namediv_char(*segment_end))//另一个指针定位到名称结尾
			segment_end++;
		if(segment_end - segment_begin == _target.len)
		{
			find = true;	//不是真的找到了，只是可能找到了，下面进行验证。
			for(char *scanner = segment_begin; scanner != segment_end; scanner++)
				if(*scanner != _target[scanner-segment_begin])
					find = false;
		}
		if(find)
			return segment_begin;
		segment_begin = segment_end;
	}
	return nullptr;
}

bool is_ch_in_cstr(char _ch,const char *_cstr)
{
	uint len = strlen(_cstr);
	for(uint i=0; i<len; i++)
		if(_ch == _cstr[i])
			return true;
	return false;
}
