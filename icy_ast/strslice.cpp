
#include"icydebug.hpp"
//#define DEBUG 114514

#ifdef DEBUG
#include<iostream>
#endif

using uint   = unsigned int;
using ushort = unsigned short;

uint strlen(char* _str)
{
	uint result{0};
	while(*_str)
	{
		result++;
		_str++;
	}
	return result;
}

uint strlen(const char* _cstr)
{
	uint result{0};
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

	StrSlice();
	StrSlice(const char* _cstr);
	StrSlice(char* _str);
	char &operator[](uint _idx);
};

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






bool is_number(char _c)
{
	return _c>='0' && _c <= '9';
}

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

char* jump_space(char* _pos)
{
	while(*_pos != ' ' && *_pos)	//先定位到有空格的地方
		_pos++;
	while(*_pos == ' ' && *_pos)	//再跳过所有空格
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
		throw"Exception from function find_pair_sign:unpaired bracket";//如果level不等于零说明代码中的括号没有匹配完整，抛出错误。

	return _begin;

}

StrSlice fetch_name(StrSlice &_slice)//找到StrSlice中符合命名规范的第一个片段
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

StrSlice fetch_number(StrSlice &_slice)
{
	StrSlice number;
	uint i=0;
	bool dot{false};	//此时还未发现小数点
	for(; i<_slice.len; i++)
	{
		if(_slice[i] == '.')
		{
			dot = true;		//对小数点已经出现的情况进行标记
			break;
		}
		else if(is_number(_slice[i]))
			break;									//如果遇到小数点或者数字，就跳出循环，在下面尝试读取这个[可能是数字]的片段
	}
	while(i != _slice.len && (is_number(_slice[i]) || _slice[i] == '.'))
	{
		if(_slice[i] == '.')
		{
			if(dot)
				break;	//	如果前面已经有小数点了，那就跳出
			dot = true;//做标记
		}
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
		throw"Exception from function \"is_strslice_wrapped_by_bracket\":unpaired bracket";
	if(i == _slice.len-1)
		return true;
	else
		return false;
}

void decorticate_strslice(StrSlice &_slice)
{
	if(_slice.len <= 2)
		throw "Exception from function \"decorticate_strslice\":too short slice to be decorticated.";
	else{
		_slice.ptr++;
		_slice.len -= 2;
	}
	
}