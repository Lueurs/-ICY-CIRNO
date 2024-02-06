#include<vector>
#include"strslice.hpp"
#include"error.hpp"

using namespace Cirno;

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

    struct icyAstNode
	{
		std::vector<icyAstNode*>  sub_nodes; // 这里储存了子节点的指针，默认两个子节点。
		icy_nodetype_t            node_type;		//该节点的类型
		uint                      source;			//该节点的资源编号
	};

    icyAstNode* icy_generate_ast(StrSlice &_slice)
    {

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
				throw_exception("Exception from function icy_fetch_current_token:unpaired square bracket\n");
			current_token.ptr = "[]";
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
	 *
	 * arr = [0,1,2,3,4,5]
	 * arr[0]
	 *
	 */

	StrSlice icy_find_minlevel_token(StrSlice)
	{

	}

	//或许会有更高效的映射方法？
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


		else
			return 0;
	}

