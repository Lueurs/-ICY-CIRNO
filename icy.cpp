#include<vector>
#include<map>
#include<unordered_map>
#include"icy_ast/icy_ast.hpp"


using ushort = unsigned short;
using uint   = unsigned int;
using byte   = char;

//std::size_t _strslice_hash(StrSlice &_slicce);

struct strslice_cmp
{
    bool operator()(StrSlice sliceA,StrSlice sliceB)
    {
        return sliceA == sliceB;
    }
};

namespace Cirno{
	enum icyobj_t
	{
		NIL = 0,
		FUNCTION,
	
	};

	struct IcyObject
	{		
		uint  type;
		byte* source_ptr;
	};


	using icyFuncParamStruct = std::vector<IcyObject>; //icyFunction 对象中保存函数参数的数据结构，我们暂且用std::vector吧

	//CIRNO的函数
	class IcyFunction
	{
		icyFuncParamStruct m_param_table;
		IcyObject          m_ret_value;

	};
	class IcyThread
	{

	};
	class IcyProcess
	{
	protected:
		icyAstNode *generate_ast(StrSlice _statement,icyAstNode *_root = nullptr,IcyFunction &_this_function);//第四个参数主要是提供上下文信息。
	private:
		std::map<StrSlice,uint,strslice_cmp>     m_mutualobj_index_table;	//共享对象的索引表
		std::vector<char*>  		             m_mutualobj_table;			//共享对象的地址表
	};

    icyAstNode *IcyProcess::generate_ast(StrSlice _statement,icyAstNode *_root = nullptr,IcyFunction &_this_function)
    {
        icyAstNode *pAstNode;
        StrSlice current_operator = icy_find_minlevel_token(_statement);                //获取当前一级的操作符
        icyAstNode *current_node = make_ast_node_via_strslice(current_operator);    //根据当前操作符生成相应的节点

        if(current_node->node_type == NODETP_CREATE_LOCAL_OBJ)
        {

        }

        if(current_node->node_type == NODETP_OBJECT)
        {
        //下面在各个作用域查找这个对象
            std::map<StrSlice,uint,strslice_cmp>::iterator it;
            it = m_mutualobj_index_table.find(current_operator);
            if(it != m_mutualobj_index_table.end())//如果在全局共享对象中可以找到
            {
                current_node->source = it->second;  //将索引值置入节点
            }
            else if(it != )
        }

        if(current_node->node_type == NODETP_SHIF_ACCESS)
        {
            StrSlice param1,param2;

            //获取第一个参数
            param1.ptr = _statement.ptr;
            param1.len = current_operator.ptr - _statement.ptr;
            //arr[i+1]
            //012345
            //^ v ^
            //获取第二个参数
            param2.ptr = current_operator.ptr+1;
            char* end_pos = find_pair_sign(_statement.ptr,_statement.len - (current_operator.ptr - _statement.ptr));
            param2.len = end_pos - param2.ptr;


        }


    }

	IcyFunction icy_make_function(StrSlice &_slice);

	IcyObject icy_execute_ast();


}
