#pragma once
#include<vector>
#include"strslice.hpp"

using ushort = unsigned short;
using uint   = unsigned int;

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

	struct icyAstNode
	{
		std::vector<icyAstNode*>  sub_nodes; // 这里储存了子节点的指针，默认两个子节点。
		icy_nodetype_t            node_type;		//该节点的类型
		uint                      source;			//该节点的资源编号

	};

	bool is_icy_keywd(StrSlice&);	//[]p检测代码中的字符片段是否是关键字

	ushort slice_operation_priority_level(StrSlice& _slice);	//p[]返回当前运算的运算优先级(数字越小优先级越高)

	StrSlice icy_fetch_current_token(StrSlice &_slice);	//[?]读取（或者说标记）当前完整的一个操作符或操作数

	StrSlice icy_find_minlevel_token(StrSlice);	//[?]找到该片段中优先级最低的操作符

	icyAstNode* icy_generate_ast(StrSlice &_slice);	//[?]通过分析字符串，生成相应的抽象语法树

	/*
	 * 生成AST的大致思路:
	 * 1.先找到一条指令中优先级最高的操作符,生成一个相应的icyAstNode
	 * 2.将被这个操作符操作的字符串部分分别提取出来，交给icy_generate_ast的下一层迭代处理，处理后返回的icyAstNode作为第一个Node的子节点
	 * 例:
	 * 处理print@[(1+2)*3]
	 * 首先找到优先级最高的运算符@,已知@有两个操作数分别在其左右，因此将@左右的两个字符串切片单独提取出来，
	 * * 这里有一个问题：我们是把函数的参数看作一个整体的列表，还是在icy_generate_ast中单独处理每个参数呢
	 * * 前者更灵活，但是怎样处理语法是一个问题————难道我要将小括号都理解为一个列表？还是把函数调用写成print@["Hello,World"]这样？
	 * * 我选择前者。我们只要在调用函数的时候将[]列表中的元素一个个拷贝给函数对象(由icyFunction定义)的参数就可以了
	 * 得到print和[(1+2)*3]
	 * * 问题2: 我们怎样通过print来找到相应的函数的索引？
	 * * icy_generatr_ast的参数中可以包含一个”求值请求“,来告诉函数自己期望的值类型。我们可以在迭代分析print的时候用参数告诉icy_generate_ast
	 * * 我们想要的是一个函数，因此icy_generate_ast在运行的时候可以优先到函数列表里查询（可能会更高效）
	 * * 我不想要太多混乱的参数
	 */




}


