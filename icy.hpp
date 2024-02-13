#pragma once

#include<vector>
#include<map>
#include<unordered_map>
#include"icy_ast/icy_ast.hpp"


using ushort = unsigned short;
using uint   = unsigned int;
using byte   = char;



namespace Cirno{
	enum icyobj_t
	{
		OBJTP_NIL = 0,
		OBJTP_FUNCTION,

        OBJTP_INTEGER,
        OBJTP_REALNUM,

        OBJTP_CHAR,
        OBJTP_STRING
	
	};

	using IcyInt   = int;
	using IcyFloat = double;

	struct IcyObject
	{		
		icyobj_t  type;
		byte*     source_ptr;
		IcyObject(icyobj_t _type = OBJTP_NIL,byte *_source);
		~IcyObject();
		void operator = (IcyObject &_icyobj);
	};

	IcyObject read_icy_constant_val(StrSlice _statement);	//	读取一段字符，转化为相应的icy常量对象

	using icyFuncParamStruct = std::vector<IcyObject>; //icyFunction 对象中保存函数参数的数据结构，我们暂且用std::vector吧

	//CIRNO的函数
	struct IcyFunction
	{
		icyFuncParamStruct m_param_table;
		IcyObject          m_ret_value;

	};
	struct IcyThread
	{

	};
	class IcyProcess
	{
	protected:
		icyAstNode  *generate_ast(StrSlice _statement,icyAstNode *_root); ///生成ast
		IcyObject	execute_ast(icyAstNode* _root,IcyThread &_thread_context,IcyFunction &_func_context);	//执行ast
        IcyObject   *solve_const_expr(StrSlice _statement);	//此函数用于解决编译期常表达式(直接把表达式的值算出来)
        IcyFunction *make_function(StrSlice _statement);
	private:
		std::unordered_map<StrSlice,uint>	m_mutualobj_index_table;	//共享对象的索引表
		std::vector<char*>					m_mutualobj_table;			//共享对象的地址表
	};


	IcyFunction icy_make_function(StrSlice &_slice);

	IcyObject icy_execute_ast();


}


