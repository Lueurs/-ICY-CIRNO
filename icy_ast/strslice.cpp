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

bool is_strslice_number(StrSlice &_slice)
{
	bool result{true};
	for(uint i=0; i<_slice.len; i++)
		if(!is_number(_slice[i]))
			result = false;
	return result;
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
