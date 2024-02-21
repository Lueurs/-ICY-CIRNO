#include<vector>
#include"strslice.hpp"
#include"icydebug.hpp"
#include<list>
#include<string>

char SQUARE_BRACKET[] = "[]";

using TokenList = std::vector<StrSlice>;

TokenList::iterator find_right_pair(TokenList::iterator _begin,TokenList::iterator _end)
{
	char left_ch  = (*_begin)[0];
	char right_ch = get_pair_sign((*_begin)[0]);
	int level{0};
	TokenList::iterator it;
	for(it=_begin; it!=_end; it++)
	{
		if((*it)[0] == left_ch)
			level++;
		else if((*it)[0] == right_ch)
		{
			level--;
			if(level == 0)
				break;
		}
	}
	if(level != 0)
		throw"[Syntax Analyse]Exception from function find_right_pair: unpaired symbol.\n";
	return it;
}
TokenList::iterator find_left_pair(TokenList::iterator _end,TokenList::iterator _begin)
{
	char right_ch = (*_begin)[0];
	char left_ch = get_pair_sign(right_ch);
	int level{0};
	TokenList::iterator it;
	for(it = _end;;it--)
	{
		if((*it)[0] == right_ch)
			level++;
		if((*it)[0] == left_ch)
		{
			level--;
			if(level == 0)
				break;
		}
		if(it == _begin)
			break;
	}
	if(it == _begin && (*it)[0] != left_ch)
		throw"[Syntax Analyse]Exception from function find_left_pair: unpaired symbol.\n";
	return it;
}



TokenList cut_tokenlist(TokenList::iterator _begin,TokenList::iterator _end)
{
	TokenList result;
	result.reserve(_end-_begin);
	for(;_begin != _end;_begin++)
		result.emplace_back(*_begin);
	return result;
}

std::string strslice_to_string(StrSlice _slice)
{
	std::string result;
	result.reserve(_slice.len);
	for(uint i=0; i<_slice.len; i++)
		result.push_back(_slice[i]);
	return result;
}

namespace Cirno{


	const char* g_icykeywd_array[] = {
		"var",
		"mutual",
		"const",
		"and",
		"or",
		"func",
		"ret",
		"class",
		"hlt",
		"int",
		"float",
		"if",
		"else",
		"for",
		"foreach",
		"loopif",
		"loopuntil",
		"end"
	};

	const char* g_icy_operator_array[] = {
		"=",
		"+",
		"-",
		"*",
		"/",
		"^",
		"+=",
		"-=",
		"*=",
		"/=",
		"^=",
		".",
		"@",
		"&",
		"[",
		"]"
	};
	
	enum icy_nodetype_t:ushort
	{
		NODETP_NIL,
		NODETP_UNKNOWN,

		NODETP_CREATE_LOCAL_OBJ,
		NODETP_CREATE_MUTUAL_OBJ,
		NODETP_CREATE_CONST_OBJ,

	_NODETPSEC_OBJECT_SEC_,
		NODETP_LOCAL_OBJECT,
		NODETP_MUTUAL_OBJECT,
		NODETP_CONST_OBJECT,

//以下是双目运算符
	_NODETPSEC_BIN_BEGIN_,
		NODETP_MOV,

		NODETP_CALL,

		NODETP_ADD,
		NODETP_SUB,
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
		NODETP_ACCESS,	//	访问对象成员
	_NODETPSEC_BIN_END_,
//以上是双目运算符

		NODETP_NOT,
		NODETP_NEG,
		NODETP_POS,

		NODETP_SHIF_ACCESS,	//访问列表元素(其实这个也是双目运算符)
		NODETP_LIST,
		NODETP_SCOPE,

	_NODETPSEC_CONTROL_SECTION_,
		
		NODETP_IF,
		NODETP_LOOPIF,
		NODETP_LOOPUNTIL,
		NODETP_FOR,
		NODETP_FOREACH,

	_NODETP_END_
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

	bool is_icy_operator(StrSlice _slice)
	{
		for(auto s : g_icy_operator_array)
			if(compair_strslice_with_cstr(_slice,s))
				return true;
		return false;
	}


	//var a = -b[0]+1
	//<LOW>
	//MOV,
	//VAR,ADD,
	//SHIF
	//a,b,1
	//<HIGH>
	//赋值->对象声明->对象引用，创建列表

	//这一次数字越小，优先级越低,被计算的越晚
	ushort slice_operation_priority_level(StrSlice _slice)
	{
		if(_slice == "ret")
			return 0;
		else if(_slice == "=" ||
		   _slice == "+="||_slice == "-="||
		   _slice == "*="|| _slice == "/="||
		   _slice == "^=")//各种赋值符号
			return 10;

		else if(_slice == "and" ||_slice == "or")//与或
			return 20;
		
		else if(_slice == ">" || _slice == "<" ||
			    _slice == ">="|| _slice == "<="||
				_slice == "=="|| _slice == "!=")//比较运算符
			return 25;
		else if(_slice == "+" || _slice == "-")//加减法
		{
			if(_slice.property == BINARY_OPERATOR)//加减法
				return 30;
			else								  //正负号
				return 60;
		}

		else if(_slice == "*" || _slice == "/")//乘法除法
			return 40;

		else if(_slice == "^")//幂
			return 50;
	
		else if(_slice == "!")//否
			return 60;
		
		else if(_slice == "@")
			return 75;
								//entiy.member@[param]
		else if(_slice == ".")
			return 70;				

		else if(_slice == "[" && _slice.property == ACCESS_LIST)//访问数组元素
			return 80;
		
		else if(_slice == "var" || _slice == "const" || _slice == "mutual")
			return 90;

		else if(is_strslice_number(_slice) ||
				icy_naming_check(_slice) ||
				(_slice == "[" && _slice.property == CREATE_LIST) ||
				_slice[0] == '\"' && _slice[_slice.len-1] == '\"')
			return 100;
		else 
			throw"Unknown type.\n";
	}

//优先级最低的在根节点
	TokenList::iterator icy_find_minlevel_token2(TokenList::iterator _begin,TokenList::iterator _end)
	{
		ushort 		level_value{150};
		TokenList::iterator 	root_token;
		while((*_begin)[0] == '(' && (_end ==find_right_pair(_begin,_end)++))//去除多余括号
		{
			_begin++;
			_end--;
		}
		auto it = _begin;
		while(it != _end)
		{
			if((*it)[0] == '(')//跳过括号
			{
				auto next_pos = find_right_pair(it,_end);
				it = next_pos;
			}
			if(*it == "[")//很麻烦吧，方括号要单独拿出来处理。
			{
				ushort current_level{0};
				//以下条件判断是否是在生成一个列表
				if(it == _begin)
					current_level = 100u;
				else if(is_ch_in_cstr((*(it-1))[0],"+*="))
					current_level = 100u;
				else
					current_level = 80u;
				if(current_level <= level_value)
				{
					if(current_level == 80u)//这个是“数组访问”的分支
						it->property = ACCESS_LIST;
					else//这个是“列表创建”的分支
						it->property = CREATE_LIST;

					root_token = it;
					level_value = it->property == ACCESS_LIST ? 80u:100u;
				}
				it = find_right_pair(it,_end);
			}
			else//大多数的在这里被处理了。上面两个if是需要特殊处理的情况
			{
				if(slice_operation_priority_level(*it) <= level_value)
				{
					root_token = it;
					level_value = slice_operation_priority_level(*it);					
				}
			}
			it++;
		}
		return root_token;
		
	}


	//之后想办法换一种映射方法
	icyAstNode *make_ast_node_via_strslice(StrSlice _slice_operator)
	{
		icyAstNode *p_ret_astnode{nullptr};
		
		icy_nodetype_t node_type;

		if(is_strslice_number(_slice_operator))//需要增加对字符串型常量的支持
			node_type = NODETP_CONST_OBJECT;
		else if(_slice_operator == "=")
			node_type = NODETP_MOV;
		else if(_slice_operator == "@")
			node_type = NODETP_CALL;
		else if(_slice_operator == "+")
		{
			if(_slice_operator.property == BINARY_OPERATOR)
				node_type = NODETP_ADD;
			else
				node_type = NODETP_POS;
		}
		else if(_slice_operator == "-")
			node_type = _slice_operator.property == BINARY_OPERATOR ? NODETP_SUB : NODETP_NEG;
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
		else if(_slice_operator == "[")
			node_type = _slice_operator.property == ACCESS_LIST ? NODETP_ACCESS:NODETP_LIST;
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
		else if(_slice_operator == "const")
			node_type = NODETP_CREATE_CONST_OBJ;
		else if(icy_naming_check(_slice_operator))
		{
			if(!is_icy_keywd(_slice_operator))//如果不是关键字，那么就是一个对象名
				node_type = _NODETPSEC_OBJECT_SEC_;
		}
		else if(_slice_operator[0] == '\"')
			node_type = NODETP_CONST_OBJECT;
		else if (_slice_operator[0] == '\'')
			node_type = NODETP_CONST_OBJECT;
		else
			throw"Exception from function\"make_ast_node_via_strslice\": unknown type.";

		if(node_type > _NODETPSEC_CONTROL_SECTION_)//如果是控制指令就多准备一些空间。
			p_ret_astnode = new icyAstNode(3U);
		else if(node_type == _NODETPSEC_OBJECT_SEC_)
			p_ret_astnode = new icyAstNode(0u);
		else if(node_type == NODETP_NOT)
			p_ret_astnode = new icyAstNode(1u);
		else
			p_ret_astnode = new icyAstNode(2U);

		p_ret_astnode->node_type = node_type;
		return p_ret_astnode;

	}

	bool is_ast_const_expr(icyAstNode* _root)
	{
		if(!_root)
			return true;
		if(_root->node_type == NODETP_MUTUAL_OBJECT || _root->node_type == NODETP_LOCAL_OBJECT)
			return false;
		for(auto node:_root->sub_nodes)
			if(!is_ast_const_expr(node))
				return false;
		return true;		
	}


}



