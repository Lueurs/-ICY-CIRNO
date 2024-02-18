//如果可以重新开始，我会让程序先完成对整段代码的词法分析。
/*
规则:
1.每个可独立执行的源代码文件应当有一个入口函数。入口函数名与文件名相同
2.所有的脚本文件一旦被加载就会共享函数、常量和用mutual定义的对象
3.最先执行的脚本可以在脚本内加载其他脚本文件，并且可以为该脚本文件开启一个新的线程，让它和最初的脚本文件并行执行
4.所有的mutual对象不仅是全局的，还支持多线程互斥访问。不同的icy脚本之间可以互斥访问同一个对象，调用了cirno语言库的C++程序也可以加载启动脚本，并与脚本互斥地访问脚本的mutual对象
*/
/*
This is an example for CIRNO-script's syntax 
func myfunc(var parameter)
{
    if condition_a:
        print@["segment_layer:1"]
        do_something@[args]
        if condition_aa:
            do_something@[args]
            print@["segment_layer:2"]
        else:
            do_something_else@[args]
        end
    else
        do_something@[args]
    end
}
[name]myfunc
 |
[if]
 |---[CONDITION]condition_a
 |---[EXECUTE_IF_TRUUE]
 |           |----------[CALL]
 |           |             |----print
 |           |             |----list
 |           |                    |----"segment_layer:1"
 |           |
 |           |-----------[CALL]
 |           |             |----do_something
 |           |             |----list
 |           |                    |----args
 |           |
 |           |------------[if]
 |                          |---[CONDITION]condition_aa
 |                          |---[EXECUTE_IF_TRUE]
 |                          |           |----------[CALL]
 |                          |           |            |----do_something
 |                          |           |            |----list
 |                          |           |                   |----args
 |                          |           |
 |                          |           |-----------[CALL]
 |                          |                         |----print
 |                          |                         |----list
 |                          |                               |----"segment_layer:1"
 |                          |----[ELSE]
 |                                 |----------------[CALL]
 |                                                    |----do_something_else
 |                                                    |----list
 |                                                           |----args
 |--------[ELSE]
             |-----------[CALL]
                            |----do_something
                            |----list
                                   |----args
*/
//运算优先级最低的运算符中最右侧的运算符置于根节点的位置。
//var a = 12+5+3
/*
	[MOV]
	  |----[VAR]
	  |      |----a
	  |
	  |----[ADD]
	         |----[ADD]
			 |      |----12
			 |      |----5
			 |----5
*/
#pragma once

#include<vector>
#include<map>
#include<unordered_map>
#include<sys/stat.h>
#include<fstream>
#include<iostream>
#include"icy_ast/icy_ast.hpp"
#include"stack.hpp"


using ushort = unsigned short;
using uint   = unsigned int;
using byte   = char;

using SharedObjectIndexTable = std::unordered_map<std::string,uint>;//试试哈希表吧
using LocalObjectIndexTable  = std::unordered_map<std::string,uint>;//殊途同归了最后


namespace Cirno{
	enum icyobj_t
	{
		OBJTP_NIL = 0,
		OBJTP_FUNCTION,

        OBJTP_INTEGER,
        OBJTP_REALNUM,

        OBJTP_CHAR,
        OBJTP_STRING,

		ICYOBJT_MAX_VALUE
	
	};

	using IcyInt   = int;
	using IcyFloat = double;

	struct IcyObject
	{		
		icyobj_t  type;
		::byte*     source_ptr;
		IcyObject(icyobj_t _type = OBJTP_NIL,::byte *_source = nullptr);
		~IcyObject();
		void operator = (IcyObject _icyobj);
	};

	TokenList   lexical_analyse(char *_code,uint _code_len);	//进行词法分析。正负号和加减号在这个函数里会得到区分。

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
		Stack 		local_swap_stack;
		IcyFunction main_function;
	};
	class IcyProcess
	{
	public:
		void        load_script(std::string file_name);
		void		compile();
	protected:
        TokenList   lexical_analyse();  //词法分析函数
		icyAstNode  *generate_ast(StrSlice _statement,icyAstNode *_root); ///生成ast
        icyAstNode  *generate_ast2(TokenList _statement,IcyFunction *_pfunction_context);//上面那个函数的升级版

		void    	execute_ast(icyAstNode* _root,IcyThread &_thread_context,IcyFunction &_func_context);	//执行ast
        IcyObject   *solve_const_expr(StrSlice _statement);	//此函数用于解决编译期常表达式(直接把表达式的值算出来)
		IcyObject  solve_const_ast(icyAstNode *_root);
        IcyFunction *make_function(StrSlice _statement);
		IcyThread   compile_script(char *_source_code, char* _code_file_name);	//编译一个脚本，生成相应的线程环境对象

	private:
        std::string 	m_code;
        uint            m_code_len; //代码长度
        std::string     m_file_name; //源代码文件名
		std::fstream	m_fs;
		struct stat m_file_info;//文件信息

        //所有的函数也是对象，当完成一个make_function之后make_function的信息也会被存入下面两个表中
		SharedObjectIndexTable    			 	 m_mutualobj_index_table;	//共享对象的索引表
		std::vector<IcyObject>  		         m_mutualobj_table;			//共享对象的地址表

        
        SharedObjectIndexTable        	  		m_constobj_index_table; //常量索引表
        std::vector<IcyObject>                  m_constobj_table;       //常量地址表

        LocalObjectIndexTable				    m_current_localobj_index_table;  //当前使用的对象索引查询表,在生成AST时使用

        //每次根据对象声明的次序分配索引存入表中，在之后生成AST的过程中如果扫描到相同的对象名，就可以
	};


	IcyFunction icy_make_function(StrSlice &_slice);

	IcyObject icy_execute_ast();


}


