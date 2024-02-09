#include<vector>
#include"strslice.hpp"
#include"icydebug.hpp"

namespace Cirno{
	enum icy_nodetype_t:ushort
	{
		OBJECT,
		CONST_OBJECT,


		MOV,

		CALL,

		ADD,
		MINUS,
		MUL,
		DIV,
		POW,


	};

	const char* g_icykeywd_array[] = {
		"var",
		"and",
		"or",
		"func",
		"ret"
	};
	

    struct icyAstNode
	{
		std::vector<icyAstNode*>  sub_nodes; // 这里储存了子节点的指针，默认两个子节点。
		icy_nodetype_t            node_type;		//该节点的类型
		uint                      source;			//该节点的资源编号
	};



	bool is_icy_keywd(StrSlice &_slice)
	{
		for(auto s : g_icykeywd_array)
			if(compair_strslice_with_cstr(_slice,s))
				return true;
		return false;
	}


    /*
	 *call a function
	 *not
	 *pow
	 *multiply/divide
	 *add/minus
	 *and/or
	 */


	/*
	 * 问题:
	 * 有些运算符是分散的，我们应该怎样才能...
	 * 方案:不让StrSlice索引向原来的代码段，而是重新开一块堆内存，放一个连续完整的运算符进去。
	 * 对于同一种运算符，开一块内存就够了，因为所有索引到这块内存的StrSlice只会读不会改
	 *
	 */



	//或许会有更高效的映射方法？
	//直接的对象引用或者数据，优先级是0
    ushort slice_operation_priority_level(StrSlice& _slice)
	{
		if(compair_strslice_with_cstr(_slice,"@"))
			return 1;
		else if(compair_strslice_with_cstr(_slice,"!"))
			return 2;
		else if(compair_strslice_with_cstr(_slice,"^"))
			return 4;
		else if(compair_strslice_with_cstr(_slice,"*") || compair_strslice_with_cstr(_slice,"/"))
			return 8;
		else if(compair_strslice_with_cstr(_slice,"+") || compair_strslice_with_cstr(_slice,"-"))
			return 16;
		if(compair_strslice_with_cstr(_slice,"="))
			return 20;
		else if(compair_strslice_with_cstr(_slice,"==") ||
				compair_strslice_with_cstr(_slice,"<=") ||
				compair_strslice_with_cstr(_slice,">=") ||
				compair_strslice_with_cstr(_slice,"<")  ||
				compair_strslice_with_cstr(_slice,">")  ||
				compair_strslice_with_cstr(_slice,"!="))
			return 24;

		else if(compair_strslice_with_cstr(_slice,"and") || compair_strslice_with_cstr(_slice,"or"))
			return 32;
		else if(compair_strslice_with_cstr(_slice,"ret"))
			return 64;
		else
			return 0;
	}

	//var myValue = (1+2)*3
	//
	//从左到右扫描，找到最低一级优先级运算符中位置在最右边的，这个运算符会生成ast的根节点
	//以此类推，从左到右每次提取最右边的最低优先级操作符（运算符），向下迭代生成ast，当递归执行这个ast的时候就相当于从左向右执行原代码的指令
	//遇到方括号怎么办？这个函数是在icy_find_minlevel_token中被调用的，icy_find_minlevel_token可以检查这个函数抽取出了什么内容，然后再对下一步的解析策略做调整。
	StrSlice icy_fetch_current_token(StrSlice &_slice)
	{
		StrSlice current_token;
		if(_slice[0] == '[')	//[]会生成一个array(大概，也可能是别的什么数据结构，比如说rbt)
		{
			int level{0};
			for(uint i=0; i < _slice.len; i++)
			{
				if(_slice[i] == '[')
					level ++;
				else if(_slice[i] == ']')
				{
					level --;
					if(level == 0)
						break;
				}
			}
			if(level != 0)
				throw"Exception from function icy_fetch_current_token:unpaired square bracket\n";
			current_token.ptr = const_cast<char*>("[]");
			current_token.len = 2;
		}
		else if(_slice[0] == '(')
		{
			char* end = find_pair_sign(_slice.ptr,_slice.len);
			current_token.ptr = _slice.ptr;
			current_token.len = end - _slice.ptr;
		}
		return current_token;

	}

	/*
	*/
	/*
	 *
	 * arr = [0,1,2,3,4,5]
	 * arr[0]
	 *
	 */

	StrSlice icy_find_minlevel_token(StrSlice _slice)//找出运算优先级最低的那一个！我们会将其置于subtree的根节点上
	{
		StrSlice root_operation;
		ushort	 level_value{0};

		while(is_strslice_wrapped_by_brackets(_slice))//首先应该去除多余的括号
			decorticate_strslice(_slice);
		
		StrSlice temp;
		for(uint i=0; i<_slice.len;i++)
		{
			if(_slice[i] == '(')	//直接跳过括号里的内容，因为括号中的必然要被先计算
			{
				char *new_pos = find_pair_sign(_slice.ptr,_slice.len);
				i += (new_pos - _slice.ptr);
				continue;
			}
			if(_slice[i] == ' ')
			{
				for(; _slice[i] == ' ';i++);
				continue;
			}
			if(_slice[i] == '+'	||
			   _slice[i] == '-'	||
			   _slice[i] == '*'	||
			   _slice[i] == '/'	||
			   _slice[i] == '=')
			{
//Error MSG  vv
				if(i == _slice.len - 1)//如果等于号右方已经不存在代码段，说明这个运算符的参数不足
					throw"Exception from function \"icy_find_minlevel_token\": operator lose argument";
//Error MSG  ^^
				if(_slice[i+1] == '=')
				{
//Error MSG vv
					if(i == _slice.len - 2)//如果赋值号右方已经不存在代码段，说明这个运算符的参数不足
						throw"Exception from function \"icy_find_minlevel_token\": operator lose argument";
//Error MSG ^^
					temp.ptr = _slice.ptr + i;
					temp.len = 2;
					if(slice_operation_priority_level(temp) >= level_value)
					{
						level_value = slice_operation_priority_level(temp);
						root_operation = temp;
						i += 2;
						continue; 
					}
				}
				else
				{
					temp.ptr = _slice.ptr + i;
					temp.len = 1;
					if(slice_operation_priority_level(temp) >= level_value)
					{
						level_value = slice_operation_priority_level(temp);
						root_operation = temp;
						i += 2;
						continue; 
					}

				}
			}

			else if(_slice[i] == '_' || is_letter(_slice[i]))
			{
				//先获取这一片段
				temp.ptr = _slice.ptr + i;
				temp.len = 1;
				i++;
				for(;is_letter(_slice[i] || is_number(_slice[i] || _slice[i] == '_')); i++)
					temp.len++;
				//检查是否是关键字
				if(is_icy_keywd(temp))
					if(slice_operation_priority_level(temp) >= level_value)
					{
						level_value = slice_operation_priority_level(temp);
						root_operation = temp;
						continue;
					}
				else//不是关键字，但以这种形式出现，那只能是一个对象名，对象引用的运算优先级最高，因此会作为AST的叶子节点
				{
					if(0 >= level_value)
					{
						level_value = 0;
						root_operation = temp;
						continue;
					}
				}

			}
		}
		//啊，农历的新年到了。今年是龙年。希望我能在开学前完成这个程序的主体部分。
		return root_operation;
	}


/*
	icyAstNode* icy_generate_ast(StrSlice &_slice)
    {
		return nullptr;
    }
*/

}



