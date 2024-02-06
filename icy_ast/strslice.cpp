
using uint   = unsigned int;
using ushort = unsigned short;

const char* g_icy_keywords[] = {
	"func",
	"var",
	"ret",
	"mutual",
	"const",
	"if",
	"for",
	"foreach",
	"loopif"
};


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
	char operator[](uint _idx);
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

char StrSlice::operator[](uint _idx)//[?]
{
	if(_idx >= len)
		throw "Error:strslice out of range"; //暂先throw这个吧
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
	for(uint i=0; i < _slice.len; i++)
	{
		if(_slice[i] == '.')
			num_dots ++;
		else if(!is_number(_slice[i]))
			result = false;
	}
	if(num_dots != 1)//如果小数点多于一个则这不是一个浮点实数（显然一个数字我们最多只能写一个小数点
		return false;
	if(num_dots == _slice.len)//不可以全都是小数点，这包含了StrSlice有且仅有一个小数点的情况.
		return false;
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

char pair_sign(char _c)
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
