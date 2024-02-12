#include<vector>
#include"strslice.hpp"
#include"icydebug.hpp"

char SQUARE_BRACKET[] = "[]";

namespace Cirno{


	const char* g_icykeywd_array[] = {
		"var",
		"mutual",
		"and",
		"or",
		"func",
		"ret",
		"class",
		"hlt"
	};
	
	enum icy_nodetype_t:ushort
	{
		NODETP_UNKNOWN,

		NODETP_CREATE_LOCAL_OBJ,
		NODETP_CREATE_MUTUAL_OBJ,

	_NODETPSEC_OBJECT_SEC_,
		NODETP_LOCAL_OBJECT,
		NODETP_MUTUAL_OBJECT,
		NODETP_CONST_OBJECT,


		NODETP_MOV,

		NODETP_CALL,

		NODETP_ADD,
		NODETP_MINUS,
		NODETP_MUL,
		NODETP_DIV,
		NODETP_POW,

		NODETP_EQUAL,
		NODETP_GREATER,
		NODETP_LESS,
		NODETP_LE,
		NODETP_GE,

		NODETP_AND,
		NODETP_OR,
		NODETP_NOT,

		NODETP_ACCESS,	//	访问对象成员
		NODETP_SHIF_ACCESS,	//访问列表元素

	_NODETPSEC_CONTROL_SECTION_,

		NODETP_IF,
		NODETP_LOOPIF,
		NODETP_LOOPUNTIL,
		NODETP_FOR,
		NODETP_FOREACH,

	};

    struct icyAstNode
	{
		std::vector<icyAstNode*>  sub_nodes; // 这里储存了子节点的指针，默认两个子节点。
		icy_nodetype_t            node_type;		//该节点的类型
		uint                      source;			//该节点的资源编号

		icyAstNode(uint _num_subnodes);
	};
	icyAstNode::icyAstNode(uint _num_subnodes = 3U)
		:source(0)	//默认情况下我们预留了三个子节点的空间...特殊情况下可以要求更多的初始空间，比方说这个节点是一个循环体。或者我们之后会换一种数据结构来存放子节点的指针
	{
		sub_nodes.reserve(_num_subnodes);
	}

	struct StrOperator
	{
		StrSlice 		slice;
		icy_nodetype_t	nodetype;
	};

	bool is_icy_keywd(StrSlice &_slice)
	{
		for(auto s : g_icykeywd_array)
			if(compair_strslice_with_cstr(_slice,s))
				return true;
		return false;
	}

	//或许会有更高效的映射方法？
	//直接的对象引用或者数据，优先级是0
	//数字越小运算优先级越高
    ushort slice_operation_priority_level(StrSlice& _slice)
	{
		if(compair_strslice_with_cstr(_slice,"[]"))
			return 1;
		else if(compair_strslice_with_cstr(_slice,"@"))
			return 2;
		else if(compair_strslice_with_cstr(_slice,"!"))
			return 3;
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
		else if(compair_strslice_with_cstr(_slice,"+=") || 
				compair_strslice_with_cstr(_slice,"-=") ||
				compair_strslice_with_cstr(_slice,"*=") ||
				compair_strslice_with_cstr(_slice,"/="))
			return 48;
		else if(compair_strslice_with_cstr(_slice,"ret"))
			return 64;
		else
			return 0;
	}


	StrSlice icy_find_minlevel_token(StrSlice _slice)//找出运算优先级最低的那一个！我们会将其置于subtree的根节点上
	{
		StrSlice root_operation;
		ushort	 level_value{0};
		icy_nodetype_t node_type;

		StrSlice temp;
		ushort current_level{0};

		while(is_strslice_wrapped_by_brackets(_slice))//首先应该去除多余的括号
			decorticate_strslice(_slice);
		

		uint i{0};
		while(i < _slice.len)
		{
			//跳过圆括号
			if(_slice[i] == '(')	//直接跳过括号里的内容，因为括号中的必然要被先计算
			{
				char *new_pos = find_pair_sign(_slice.ptr + i,_slice.len);
				i += (new_pos - _slice.ptr);
				continue;
			}
			//跳过空格
			if(_slice[i] == ' ')
			{
				while(_slice[i] == ' ')
					i++;
				continue;
			}
			//调用函数的符号
			if(_slice[i] == '@')
			{
				if(i == _slice.len-1)
					throw"Exception from function \"icy_find_minlevel_token\": operator lose argument.";
				else
				{
					temp.ptr = _slice.ptr + i;
					temp.len = 1;
					if(slice_operation_priority_level(temp) >= level_value)
					{
						level_value = slice_operation_priority_level(temp);
						root_operation = temp;
					}
					i++;
					continue;
				}
			}

			//加减乘除等运算符
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
			else if(_slice[i] == '>' || _slice[i] == '<')
			{
				if(i == _slice.len - 1)//如果等于号右方已经不存在代码段，说明这个运算符的参数不足
					throw"Exception from function \"icy_find_minlevel_token\": operator lose argument";
				temp.ptr = _slice.ptr + i;
				if(_slice[i+1] == '=')
				{
					temp.len = 2;
					if(i == _slice.len-2)
						throw"Exception from function \"icy_find_minlevel_token\": operator lose argument";					
				}
				else
					temp.len = 1;
				if(slice_operation_priority_level(temp) >= level_value)
				{
					level_value = slice_operation_priority_level(temp);
					root_operation = temp;					
				}
				i += temp.len;
				continue;
			}
			//处理关键字或者引用
			else if(_slice[i] == '_' || is_letter(_slice[i]))
			{
				//先获取这一片段
				temp.ptr = _slice.ptr + i;
				temp.len = 1;
				i++;
				for(;i < _slice.len && (is_letter(_slice[i]) || is_number(_slice[i]) || _slice[i] == '_'); i++)
					temp.len++;
				//检查是否是关键字
				if(is_icy_keywd(temp))
				{
					if(slice_operation_priority_level(temp) >= level_value)
					{
						level_value = slice_operation_priority_level(temp);
						root_operation = temp;					
					}
				}
				else//不是关键字，但以这种形式出现，那只能是一个对象名，对象引用的运算优先级最高，因此会作为AST的叶子节点
				{
					if(0 >= level_value)
					{
						level_value = 0;
						root_operation = temp;
					}
				}
				continue;

			}
			//处理数字
			else if(is_number(_slice[i]))	//数字引用的运算优先级是最高的,将会被置于叶子节点上
			{
				temp = fetch_number(StrSlice(_slice.ptr + i));
				if(0 >= level_value)
				{
					level_value = 0;
					root_operation = temp;
				}
				i += temp.len;
				continue;
			}
			//处理[]
			//怎样判断运算符左边已经有参数了呢？
			/*
			var value = arr[i]
							=
			left:                         right:
			var value                     arr[i]
			*/
			else if(_slice[i] == '[')
			{
				char* end_bracket_pos = find_pair_sign(_slice.ptr + i,_slice.len);
				if(is_range_contain<char>(_slice.ptr+i,end_bracket_pos,','))	//如果方括号内有逗号那就是列表
				{
					temp.ptr = _slice.ptr + i;
					temp.len = end_bracket_pos - (_slice.ptr + i) + 1;
					if(0 >= level_value)
					{
						level_value = 0;
						root_operation = temp;
					}
				}				
				else
				{
					temp.ptr = SQUARE_BRACKET;
					temp.len = strlen(SQUARE_BRACKET);
					if(slice_operation_priority_level(temp) >= level_value)
					{
						level_value = slice_operation_priority_level(temp);
						root_operation = temp;
					}
				}
				i += end_bracket_pos - (_slice.ptr + i) + 1;
				continue;
			}

			i++;
		}
		//啊，农历的新年到了。今年是龙年。希望我能在开学前完成这个程序的主体部分。

		return root_operation;
	}




	//之后想办法换一种映射方法
	icyAstNode *make_ast_node_via_strslice(StrSlice _slice_operator)
	{
		icyAstNode *p_ret_astnode{nullptr};
		
		icy_nodetype_t node_type;

		if(is_strslice_number(_slice_operator))
			node_type = NODETP_CONST_OBJECT;
		else if(_slice_operator == "=")
			node_type = NODETP_MOV;
		else if(_slice_operator == "@")
			node_type = NODETP_CALL;
		else if(_slice_operator == "+")
			node_type = NODETP_ADD;
		else if(_slice_operator == "-")
			node_type = NODETP_MINUS;
		else if(_slice_operator == "*")
			node_type = NODETP_MUL;
		else if(_slice_operator == "/")
			node_type = NODETP_DIV;
		else if(_slice_operator == "^")
			node_type = NODETP_POW;
		else if(_slice_operator == "==")
			node_type = NODETP_EQUAL;
		else if(_slice_operator == ">")
			node_type = NODETP_GREATER;
		else if(_slice_operator == "<")
			node_type = NODETP_LESS;
		else if(_slice_operator == ">=")
			node_type = NODETP_GE;
		else if(_slice_operator == "<=")
			node_type = NODETP_LE;
		else if(_slice_operator == "and")
			node_type = NODETP_AND;
		else if(_slice_operator == "or")
			node_type = NODETP_OR;
		else if(_slice_operator == "!")
			node_type = NODETP_NOT;
		else if(_slice_operator == ".")
			node_type = NODETP_ACCESS;
		else if(_slice_operator == "[]")
			node_type = NODETP_SHIF_ACCESS;
		else if(_slice_operator == "if")
			node_type = NODETP_IF;
		else if(_slice_operator == "loopif")
			node_type = NODETP_LOOPIF;
		else if(_slice_operator == "loopuntil")
			node_type = NODETP_LOOPUNTIL;
		else if(_slice_operator == "for")
			node_type = NODETP_FOR;
		else if(_slice_operator == "foreach")
			node_type = NODETP_FOREACH;
		else if(_slice_operator == "var")
			node_type = NODETP_CREATE_LOCAL_OBJ;
		else if(_slice_operator == "mutual")
			node_type = NODETP_CREATE_MUTUAL_OBJ;
		else if(icy_naming_check(_slice_operator))
		{
			if(!is_icy_keywd(_slice_operator))//如果不是关键字，那么就是一个对象名
				node_type = _NODETPSEC_OBJECT_SEC_;
		}
		else
			throw"Exception from function\"make_ast_node_via_strslice\": unknown type.";

		if(node_type > _NODETPSEC_CONTROL_SECTION_)//如果是控制指令就多准备一些空间。
		{
			p_ret_astnode = new icyAstNode(8U);
		}
		else if(node_type == _NODETPSEC_OBJECT_SEC_)
		{
			p_ret_astnode = new icyAstNode(0u);
		}
		else
		{
			p_ret_astnode = new icyAstNode(2U);
		}

		p_ret_astnode->node_type = node_type;
		return p_ret_astnode;

	}


}



