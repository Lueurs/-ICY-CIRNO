#include<vector>
#include"strslice.hpp"

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

	StrSlice icy_fetch_current_token(StrSlice &_slice)
	{
		StrSlice current_token;
		if(_slice[0] == '[')	//[]会生成一个array(大概，也可能是别的什么数据结构，比如说rbt)

	}

	//或许会有更高效的映射方法？
    ushort slice_operation_priority_level(StrSlice& _slice)
	{
		if(compair_strslice_with_cstr(_slice,"@"))
			return 1;
		else if(compair_strslice_with_cstr(_slice,"!"))
			return 2;
		else if(compair_strslice_with_cstr(_slice,"^"))
			return 3;
		else if(compair_strslice_with_cstr(_slice,"*") || compair_strslice_with_cstr(_slice,"/"))
			return 4;
		else if(compair_strslice_with_cstr(_slice,"+") || compair_strslice_with_cstr(_slice,"-"))
			return 5;
		else if(compair_strslice_with_cstr(_slice,"and") || compair_strslice_with_cstr(_slice,"or"))
			return 6;


		else
			return 0;
	}
}
