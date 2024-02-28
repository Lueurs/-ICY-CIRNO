#include<vector>
#include<map>
#include<fstream>
#include"icy_ast/icy_ast.hpp"
#include<sys/stat.h>
#include"stack.hpp"
#include<thread>
#include<iostream>
#include<functional>
#include<mutex>
#include<memory>

extern const char* g_icykeywd_array[];

using ushort = unsigned short;
using uint   = unsigned int;
using byte   = char;


using SharedObjectIndexTable = std::map<std::string,unsigned int>;
using LocalObjectIndexTable  = std::map<std::string,unsigned int>;

using _TempPair = MyPair<StrSlice,uint>;
using TemporaryDataStruct = std::vector<_TempPair>;//这是无奈之举

TemporaryDataStruct::iterator temp_find(TemporaryDataStruct &_table,StrSlice _name)
{
    TemporaryDataStruct::iterator it;
    if(_table.size() == 0)
        return _table.end();
    for(it = _table.begin();it != _table.end(); it++)
        if((*it).first == _name)
            break;
    return it;
}



namespace Cirno{


	enum icyobj_t
	{
		OBJTP_NIL = 0,
		OBJTP_FUNCTION,

        OBJTP_INTEGER,
        OBJTP_REALNUM,

        OBJTP_LIST,

        OBJTP_CHAR,
        OBJTP_STRING,

        ICYOBJT_MAX_VALUE
	
	};

    enum icyapi_index:uint{
        ICYAPI_PRINT = 0u,
        ICYAPI_INPUT,
        
        ICYMATAPI_SQRT,

        ICYMATAPI_POW,
        ICYMATAPI_SIN,
        ICYMATAPI_COS,
        ICYMATAPI_TAN,
        ICYMATAPI_COT,
        ICYMATAPI_SEC,
        ICYMATAPI_CSC,


        _ICYAPI_SIZE_
    };

    const short REQUIRE_RVALUE = 0;
	const short REQUIRE_LVALUE = 1;

    using IcyInt   = int;
    using IcyFloat = double;
    using IcyChar  =  wchar_t;

    std::mutex mtx_mutualobj;

    void push_nodetp(Stack &_stack,icy_nodetype_t _value)
    {
        if(STACK_MAXSIZE - (_stack.top - _stack.base) < sizeof(uint))
            throw"Exception from \"push_nodetp\": no enough space to push in data.\n";
        *(icy_nodetype_t*)_stack.top = _value;
        _stack.top += sizeof(icy_nodetype_t);
    }

    void push_uint(Stack &_stack,uint _value)
    {
        if(STACK_MAXSIZE - (_stack.top - _stack.base) < sizeof(uint))
            throw"Exception from \"push_uint\": no enough space to push in data.\n";
        *((uint*)(_stack.top)) = _value;
        _stack.top += sizeof(uint);
    }

    void push_objtp(Stack &_stack,icyobj_t _value)
    {        
        if(STACK_MAXSIZE - (_stack.top - _stack.base) < sizeof(icyobj_t))
            throw"Exception from \"push_objtp\": no enough space to push in data.\n";
        *((icyobj_t*)(_stack.top)) = _value;
        _stack.top += sizeof(icyobj_t);
    }

    void push_icyint(Stack &_stack,IcyInt _value)
    {
        if(STACK_MAXSIZE - (_stack.top-_stack.base) < sizeof(IcyInt)+sizeof(icyobj_t))
            throw"Exception from \"push_icyint\": no enough space to push in data.\n";
        *((IcyInt*)(_stack.top)) = _value;
        _stack.top += sizeof(IcyInt); 
        *((icyobj_t*)_stack.top) = OBJTP_INTEGER;
        _stack.top += sizeof(icyobj_t);
    }

    void push_icyfloat(Stack &_stack,IcyFloat _value)
    {
        if(STACK_MAXSIZE - (_stack.top-_stack.base) < sizeof(IcyFloat)+sizeof(icyobj_t))
            throw"Exception from \"push_icyflooat\": no enough space to push in data.\n";
        *((IcyFloat*)(_stack.top)) = _value;
        _stack.top += sizeof(IcyFloat);
        *((icyobj_t*)_stack.top) = OBJTP_REALNUM;
        _stack.top += sizeof(icyobj_t); 
    }

    uint pop_uint(Stack &_stack)
    {
        if(_stack.top - _stack.base < sizeof(uint))
            throw"Exception from \"pop_uint\": no more data to pop.\n";
        _stack.top -= sizeof(int);
        return *((uint*)_stack.top);
    }

    icy_nodetype_t pop_nodetp(Stack &_stack)
    {
        if(_stack.top - _stack.base < sizeof(ushort))
            throw"Exception from \"pop_ushort\": no more data to pop.\n";        
        _stack.top -= sizeof(icy_nodetype_t);
        return *(icy_nodetype_t*)_stack.top;
    }

    icyobj_t pop_objt(Stack &_stack)
    {
        if(_stack.top - _stack.base < sizeof(icyobj_t))
            throw"Exception from \"pop_icyint\": no more data to pop.\n";
        _stack.top -= sizeof(icyobj_t);
        return *((icyobj_t*)_stack.top);        
    }

    IcyInt pop_icyint(Stack& _stack)
    {
        if(_stack.top - _stack.base < sizeof(IcyInt))
            throw"Exception from \"pop_icyint\":no more data to pop.\n";
        _stack.top -= sizeof(IcyInt);
        return *((IcyInt*)_stack.top);
    }

    IcyFloat pop_icyfloat(Stack& _stack)
    {
        if(_stack.top - _stack.base < sizeof(IcyFloat))
            throw"Exception from \"pop_icyint\": no more data to pop.\n";
        _stack.top -= sizeof(IcyFloat);
        return *((IcyFloat*)_stack.top);
    }
    void clear_stack(Stack &_stack)
    {
        _stack.top = _stack.base;
    }

    void deleter(byte *__resource)
    {
        delete[] __resource;
    }
	struct IcyObject//这个东西已经用处不大了
	{		
		icyobj_t  type;
		byte*     source_ptr;//我觉得这里可以用shared_ptr
        IcyObject(icyobj_t _type = OBJTP_NIL,byte *_source = nullptr);
        ~IcyObject();
        void clear();
        void operator = (IcyObject _icyobj);
	};
    IcyObject::IcyObject(icyobj_t _type,byte *_source)//注意！IcyObject在被创建后不是随机值，而是默认的空值
        :type(_type),source_ptr(_source){}
    
    void IcyObject::operator=(IcyObject _icyobj)
    {
        //清除原有的数据
        if(source_ptr)
            switch(type)
            {
                case OBJTP_INTEGER:
                    delete (IcyInt*)(source_ptr);
                    break;
                case OBJTP_REALNUM:
                    delete (IcyFloat*)(source_ptr);
                    break;
                default:
                    throw"（未完成的数据类型）前面的区域，以后再来探索吧！\n";
            }
        //拷贝类型
        type = _icyobj.type;
        //深拷贝数据
        switch(type)
        {
            case OBJTP_NIL:
                break;
            case OBJTP_INTEGER:
                source_ptr = (byte*)(new IcyInt);
                *((IcyInt*)(source_ptr)) = *((IcyInt*)(_icyobj.source_ptr));
                break;
            case OBJTP_REALNUM:
                source_ptr = (byte*)(new IcyFloat);
                *((IcyFloat*)(source_ptr)) = *((IcyFloat*)(_icyobj.source_ptr));
                break;
            default:
                throw"（未完成的数据类型）前面的区域，以后再来探索吧！\n";

        }
    }



    TokenList lexical_analyse(char *_code,uint _code_len)
    {
        TokenList result;
        StrSlice current_token;
        char* code_end = _code + _code_len;
        current_token.ptr = jump_space(_code);//如果代码全篇都是空格，那么这样就会直接跳转到代码尾部，不会进入下方的循环
        while(current_token.ptr != code_end)
        {
            current_token.len = 1;
            if(current_token[0] == '\"')
            {
                //这里看起来很危险。如果没有循环条件的第一个条件发现非法的内存地址，循环的第二个条件就会尝试去访问它
                while(current_token.ptr + current_token.len != code_end && current_token.next_ch() != '\"')//搜查到字符串常量的末尾
                    current_token.len++;
                if(current_token.ptr + current_token.len == code_end)
                    throw"[Lexical Error]unfinished string segment.";
                current_token.len += 1;
            }
            else if(is_ch_in_cstr(current_token[0],"+-*/^=<>!"))
            {
                if(code_end - current_token.ptr >= 2)
                    if(current_token.next_ch() == '=')
                    {
                        current_token.len = 2;
                        result.emplace_back(current_token);
                    }
                else if(is_ch_in_cstr(current_token[0],"+-"))//判断一下是正负号还是加减号
                {
                    if(result.size() == 0 ||
                       is_ch_in_cstr((result[result.size()-1])[0],"+-*/!=<>[]{}\n;"))//如果减号前面是这些符号，或者前面根本就没有什么，说明它其实是一个负号
                       current_token.property = UNARY_OPERATOR;
                    else
                        current_token.property = BINARY_OPERATOR;
                }
            }
            else if(is_ch_in_cstr(current_token[0],"[](){}.:\n,;@"))//保留这个分支是为了体现逻辑
            {
            //其实这里我们什么都不用做就可以了
            //  /\__/\
            // ( o  o )
            // \ =^=  |
            //  |      \_
            //  |        \_
            //  | | | |    \   
            //  | | | |    /~)~)~)~)~)~)~)~)
            //   ^ ^ ^ ^ ^ 
            //    result.emplace_back(current_token);
            //    current_token.ptr++;
            //    current_token.ptr = jump_space(current_token.ptr);
            //    continue;
            }
            else if(current_token[0] == '\'')
            {
                //未完工
            }
            else if(current_token[0] == '#')
            {
                while(current_token.ptr != code_end)
                {
                    if(current_token[0] == '\n')
                        break;
                    current_token.ptr++;
                }
                if(current_token.ptr == code_end)
                    break;
            }
            else if(current_token[0]  == '_' || is_letter(current_token[0]))
            {
                while(current_token.ptr + current_token.len != code_end && !is_ch_in_cstr(current_token.next_ch(),"\"\t\n+-*/^%!<>=() []{},.;&@:"))
                    current_token.len++;
            }
            else if(is_number(current_token[0])) //数字的解析。这里我改了很久，估计还是有严重的问题
            {
                bool dot{false};//是否有小数点
                while(current_token.ptr + current_token.len != code_end)
                {
                    if(current_token.next_ch() == '.')
                    {
                        if(dot)     //如果已经有小数点了，那就直接跳出循环，不再向下读取
                            break;
                        else if(current_token.ptr + current_token.len + 1 == code_end ||
                               !is_number(current_token.next_ch(2)))//如果这个小数点是代码中的最后一个字符,或者小数点后面不是数字,也跳出循环，不再读取
                            break;
                        else//否则(这段数字中之前没有出现过小数点，而且小数点不在代码的尾部出现，小数点的后面一个字符也是数字)
                            dot = true;
                    }
                    else if(!is_number(current_token.next_ch()))
                        break;
                    current_token.len++;            
                }   
            }
            else if(current_token[0] == '\"')
            {
                while(current_token.ptr + current_token.len != code_end && current_token[current_token.len - 1] != '\"')
                    current_token.len++;
                if(current_token.ptr + current_token.len == code_end)
                    throw"[Lexical Analyse]Exception from Cirno::IcyProcess::lexical_analyse: unpaired double quotation mark.\n";
            }
            else
                throw"[Lexical Analyse]Exception from Cirno::IcyProcess::illegal character.\n";
            result.emplace_back(current_token);     //将得到的语词插入列表
            current_token.ptr += current_token.len; //更新位置
            current_token.ptr = jump_space(current_token.ptr);//跳过可能存在的空格
            
        }
        return result;
        
    }

	struct IcyFunction
	{
        std::vector<std::shared_ptr<byte>>      m_localobj_table;    //函数的局部变量索引表
        std::map<uint,IcyObject*>    default_param_value_table;//参数默认值表
        //uint是对象的索引值(表明默认值应当送入哪个对象中)，IcyObject是对象的默认值（函数被调用时先送入相应对象的值）
        icyAstNode                  function_body;       //函数体的所有内容皆在此节点下
	};
	struct IcyThread
	{
        Stack       local_swap_stack;     //本线程使用的用于交换数据的栈
        IcyFunction main_function;        //本线程的主函数
	};
	class IcyProcess
	{
    public:
        IcyProcess();
        bool        load_script(std::string _file_name);
        void        compile();



        friend void icy_mov(Stack &_stack,IcyProcess *_pproc_context,IcyFunction *_pfunc_context);//这个友元函数是为了方便mov访问对象的索引表
	protected:
        icyAstNode  *generate_ast2(TokenList::iterator _begin,TokenList::iterator _end,IcyFunction *_pfunction_context);//上面那个函数的升级版

		void    	execute_ast(icyAstNode* _root,IcyThread &_thread_context,IcyFunction &_func_context,short _required = REQUIRE_RVALUE);        
        IcyObject   *solve_const_ast(icyAstNode *_root);
        IcyFunction *make_function(StrSlice _statement);
        IcyFunction make_function2(TokenList _code);    
        IcyThread   compile_script(char *_source_code, std::string _code_file_name,uint _code_len = 0u);
        

	private:
        std::string 	m_code;     //源代码
        uint            m_code_len; //代码长度
        std::string     m_file_name; //源代码文件名
        std::fstream    m_fs;
		struct stat     m_file_info;//文件信息

        IcyThread       m_main_thread_context;

        //所有的函数也是对象，当完成一个make_function之后make_function的信息也会被存入下面两个表中
		TemporaryDataStruct                   m_mutualobj_index_table;	//共享对象的索引表
		std::vector<std::shared_ptr<byte>>    m_mutualobj_table;			//共享对象的地址表

        
        TemporaryDataStruct                   m_constobj_index_table; //常量索引表
        std::vector<std::shared_ptr<byte>>    m_constobj_table;       //常量地址表

        TemporaryDataStruct                   m_current_localobj_index_table;  //当前使用的对象索引查询表,在生成AST时使用

        //每次根据对象声明的次序分配索引存入表中，在之后生成AST的过程中如果扫描到相同的对象名，就可以

        std::vector<std::function<void(Stack&,IcyProcess*,IcyFunction*)>> m_api_table;//内部的api表       暂时还没用来做什么   
	};


///////////////////这里是一个保存各种节点对栈的操作的表///////////////////
    bool cirno_initialized{false};
    std::vector<std::function<void(Stack&,IcyProcess*,IcyFunction*)>> OperatorTable(0);
////////////////////////////////////////////////////////////////////


    struct IcyObjectBase{
        icyobj_t m_type;
        IcyObjectBase(icyobj_t __type = OBJTP_NIL)
            :m_type(__type){}
    };
    struct IcyIntObject:public IcyObjectBase{//整数类型
        IcyInt m_data;
        IcyIntObject(IcyInt __data = 0,icyobj_t __type = OBJTP_INTEGER)
            :IcyObjectBase(__type),m_data(__data){}
    };

    struct IcyFloatObject:public IcyObjectBase{//浮点实数类型
        IcyFloat m_data;
        IcyFloatObject(IcyFloat ___data = .0,icyobj_t __type = OBJTP_REALNUM)
            :IcyObjectBase(__type),m_data(___data){}
    };

    struct IcyFunctionObject:public IcyObjectBase{
        IcyFunction m_data;
        IcyFunctionObject(IcyFunction __function = IcyFunction(),icyobj_t __type = OBJTP_FUNCTION)
            :IcyObjectBase(__type),m_data(__function){}
    };


    byte* read_icy_constant_val(StrSlice _statement)
    {
        byte* pret_obj{nullptr};
        if(_statement.len == 0)
            return pret_obj;     //如果这里没有代码，直接返回空的对象引用
        else if(is_strslice_integer(_statement))
        {
            pret_obj = (byte*)(new IcyIntObject(strslice_to_integer(_statement)));
        }
        else if(is_strslice_realnum(_statement))
        {
            pret_obj = (byte*)new IcyFloatObject(strslice_to_realnum(_statement));
        }
        else
            throw"（未完成的数据类型）前面的区域，以后再来探索吧。\n";
        return pret_obj;
        
    }


/////////////////////////////////////
//以下是真正执行抽象语法树节点相应的任务的函数
//////////////////////////////////////


    void icy_mov(Stack &_stack,IcyProcess *_pproc_context,IcyFunction *_pfunc_context)
    {
        IcyInt intVal;
        IcyFloat floatVal;

        icyobj_t r_type = pop_objt(_stack);//获得右值的类型
        switch(r_type)
        {
            case OBJTP_INTEGER:
                intVal = pop_icyint(_stack);
                break;
            case OBJTP_REALNUM:
                floatVal = pop_icyfloat(_stack);
                break;
        }
        icy_nodetype_t l_prop = pop_nodetp(_stack);//获取左值的性质:是共享值还是局部值
        uint source = pop_uint(_stack);//获取左值的索引

        std::shared_ptr<byte> l_value_index;

        switch(l_prop)
        {
            case NODETP_LOCAL_OBJECT:
                l_value_index = _pfunc_context->m_localobj_table[source];
                break;
            case NODETP_MUTUAL_OBJECT://下面一段让人心惊胆战
                std::lock_guard<std::mutex> lock(mtx_mutualobj);//上锁
                std::shared_ptr<byte> &rf_obj = _pproc_context->m_mutualobj_table[source];//获取左操作数的对象的左值引用
                rf_obj.reset();//销毁原来的数据
                switch(r_type)
                {
                    case OBJTP_INTEGER:
                        rf_obj = std::shared_ptr<byte>((byte*)(new IcyIntObject(intVal)),deleter);//重新构造出对象...
                        reinterpret_cast<IcyIntObject*>(rf_obj.get())->m_data = intVal;//传入新的值
                        break;
                    case OBJTP_REALNUM:
                        rf_obj = std::shared_ptr<byte>((byte*)(new IcyFloatObject(floatVal)),deleter);//重新构造出对象...
                        reinterpret_cast<IcyFloatObject*>(rf_obj.get())->m_data = floatVal;//传入新的值
                        break;
                }
                
        }

    }

    void trans_to_integer(Stack &_stack,IcyProcess *_pproc_context,IcyFunction *_pfunc_context)//类型转换操作
    {
        IcyInt value{0};
        icyobj_t type = pop_objt(_stack);//将数据类型标识出栈
        switch(type)
        {
            case OBJTP_INTEGER:
                value = pop_icyint(_stack);
                break;
            case OBJTP_REALNUM:
                value = (IcyInt)pop_icyfloat(_stack);
                break;
            //其他需要被转换的类型在这下面追加...
        }
        push_icyint(_stack,value);         //将转化结果入栈

    }

    void icy_operator_add(Stack &_stack,IcyProcess *_pproc_context,IcyFunction *_pfunc_context)    //加法操作
    {
        icyobj_t type_1 = pop_objt(_stack); //将第二个参数的数据类型标识出栈
        icyobj_t type_2;
        if(type_1 == OBJTP_INTEGER)         //检测数据类型——如果是整数
        {
            IcyInt value_1 = pop_icyint(_stack);  //出栈整数
            type_2 = pop_objt(_stack);      //出栈第一个参数的类型
            if(type_2 != OBJTP_INTEGER)     //如果两个参数类型不同则报错
                throw"Exception from execute_ast/operator + : astnode return type mismatching.\n";
            IcyInt value_2 = pop_icyint(_stack);
            push_icyint(_stack,value_1 + value_2);  //将运算结果压入栈
        }
        else if(type_1 == OBJTP_REALNUM)
        {
            IcyFloat value_1 = pop_icyfloat(_stack);
            type_2 = pop_objt(_stack);
            if(type_2 != OBJTP_REALNUM)
                throw"Exception from execute_ast/operator + : astnode return type mismatching.\n";
            IcyFloat value_2 = pop_icyfloat(_stack);
            push_icyfloat(_stack,value_1 + value_2);
        }
        else
            throw "Error:data type currently does not support addition operation.\n";//其他数据类型暂时不支持加法操作
    }

    void icy_operator_sub(Stack &_stack,IcyProcess *_pproc_context,IcyFunction *_pfunc_context)    //减法操作
    {
        icyobj_t type_1 = pop_objt(_stack); //将第二个参数的数据类型标识出栈
        icyobj_t type_2;
        if(type_1 == OBJTP_INTEGER)         //检测数据类型——如果是整数
        {
            IcyInt value_1 = pop_icyint(_stack);  //出栈整数
            type_2 = pop_objt(_stack);      //出栈第一个参数的类型
            if(type_2 != OBJTP_INTEGER)     //如果两个参数类型不同则报错
                throw"Exception from execute_ast/operator + : astnode return type mismatching.\n";
            IcyInt value_2 = pop_icyint(_stack);
            push_icyint(_stack,value_2 - value_1);  //将运算结果压入栈
        }
        else if(type_1 == OBJTP_REALNUM)
        {
            IcyFloat value_1 = pop_icyfloat(_stack);
            type_2 = pop_objt(_stack);
            if(type_2 != OBJTP_REALNUM)
                throw"Exception from execute_ast/operator + : astnode return type mismatching.\n";
            IcyFloat value_2 = pop_icyfloat(_stack);
            push_icyfloat(_stack,value_2 - value_1);
        }
        else
            throw "Error:data type currently does not support subtraction operation.\n";
    }

    void icy_operator_mul(Stack &_stack,IcyProcess *_pproc_context,IcyFunction *_pfunc_context)    //乘法操作
    {
        icyobj_t type_1 = pop_objt(_stack); //将第二个参数的数据类型标识出栈
        icyobj_t type_2;
        if(type_1 == OBJTP_INTEGER)         //检测数据类型——如果是整数
        {
            IcyInt value_1 = pop_icyint(_stack);  //出栈整数
            type_2 = pop_objt(_stack);      //出栈第一个参数的类型
            if(type_2 != OBJTP_INTEGER)     //如果两个参数类型不同则报错
                throw"Exception from execute_ast/operator + : astnode return type mismatching.\n";
            IcyInt value_2 = pop_icyint(_stack);
            push_icyint(_stack,value_1 * value_2);  //将运算结果压入栈
        }
        else if(type_1 == OBJTP_REALNUM)
        {
            IcyFloat value_1 = pop_icyfloat(_stack);
            type_2 = pop_objt(_stack);
            if(type_2 != OBJTP_REALNUM)
                throw"Exception from execute_ast/operator * : astnode return type mismatching.\n";
            IcyFloat value_2 = pop_icyfloat(_stack);
            push_icyfloat(_stack,value_1 * value_2);
        }
        else
            throw "Error:data type currently does not support multiplication operation.\n";
    }

    void icy_operator_div(Stack &_stack,IcyProcess *_pproc_context,IcyFunction *_pfunc_context)    //乘法操作
    {
        icyobj_t type_1 = pop_objt(_stack); //将第二个参数的数据类型标识出栈
        icyobj_t type_2;
        if(type_1 == OBJTP_INTEGER)         //检测数据类型——如果是整数
        {
            IcyInt value_1 = pop_icyint(_stack);  //出栈整数
            type_2 = pop_objt(_stack);      //出栈第一个参数的类型
            if(type_2 != OBJTP_INTEGER)     //如果两个参数类型不同则报错
                throw"Exception from execute_ast/operator + : astnode return type mismatching.\n";
            IcyInt value_2 = pop_icyint(_stack);
            push_icyint(_stack,value_1 / value_2);  //将运算结果压入栈
        }
        else if(type_1 == OBJTP_REALNUM)
        {
            IcyFloat value_1 = pop_icyfloat(_stack);
            type_2 = pop_objt(_stack);
            if(type_2 != OBJTP_REALNUM)
                throw"Exception from execute_ast/operator / : astnode return type mismatching.\n";
            IcyFloat value_2 = pop_icyfloat(_stack);
            push_icyfloat(_stack,value_2 / value_1);
        }
        else
            throw "Error:data type currently does not support division operation.\n";
    }  

    void icy_operator_and(Stack &_stack,IcyProcess *_pproc_context,IcyFunction *_pfunc_context)    //乘法操作
    {
        icyobj_t type_1 = pop_objt(_stack); //将第二个参数的数据类型标识出栈
        icyobj_t type_2;
        if(type_1 == OBJTP_INTEGER)         //检测数据类型——如果是整数
        {
            IcyInt value_1 = pop_icyint(_stack);  //出栈整数
            type_2 = pop_objt(_stack);      //出栈第一个参数的类型
            if(type_2 != OBJTP_INTEGER)     //如果两个参数类型不同则报错
                throw"Exception from execute_ast/operator and : astnode return type mismatching.\n";
            IcyInt value_2 = pop_icyint(_stack);
            push_icyint(_stack,value_1 && value_2);  //将运算结果压入栈
        }
        else if(type_1 == OBJTP_REALNUM)
        {
            IcyFloat value_1 = pop_icyfloat(_stack);
            type_2 = pop_objt(_stack);
            if(type_2 != OBJTP_REALNUM)
                throw"Exception from execute_ast/operator + : astnode return type mismatching.\n";
            IcyFloat value_2 = pop_icyfloat(_stack);
            push_icyfloat(_stack,value_1 && value_2);
        }
        else
            throw "Error:data type currently does not support and operation.\n";
    }

    void icy_operator_or(Stack &_stack,IcyProcess *_pproc_context,IcyFunction *_pfunc_context)    //乘法操作
    {
        icyobj_t type_1 = pop_objt(_stack); //将第二个参数的数据类型标识出栈
        icyobj_t type_2;
        if(type_1 == OBJTP_INTEGER)         //检测数据类型——如果是整数
        {
            IcyInt value_1 = pop_icyint(_stack);  //出栈整数
            type_2 = pop_objt(_stack);      //出栈第一个参数的类型
            if(type_2 != OBJTP_INTEGER)     //如果两个参数类型不同则报错
                throw"Exception from execute_ast/operator or : astnode return type mismatching.\n";
            IcyInt value_2 = pop_icyint(_stack);
            push_icyint(_stack,value_1 || value_2);  //将运算结果压入栈
        }
        else if(type_1 == OBJTP_REALNUM)
        {
            IcyFloat value_1 = pop_icyfloat(_stack);
            type_2 = pop_objt(_stack);
            if(type_2 != OBJTP_REALNUM)
                throw"Exception from execute_ast/operator or : astnode return type mismatching.\n";
            IcyFloat value_2 = pop_icyfloat(_stack);
            push_icyfloat(_stack,value_1 || value_2);
        }
        else
            throw "Error:data type currently does not support or operation.\n";
    }

    void icy_operator_not(Stack &_stack,IcyProcess *_pproc_context,IcyFunction *_pfunc_context)    //乘法操作
    {
        icyobj_t type_1 = pop_objt(_stack); //将第二个参数的数据类型标识出栈
        if(type_1 == OBJTP_INTEGER)         //检测数据类型——如果是整数
        {
            IcyInt value_1 = pop_icyint(_stack);  //出栈整数
            push_icyint(_stack,!value_1);  //将运算结果压入栈
        }
        else if(type_1 == OBJTP_REALNUM)
        {
            IcyFloat value_1 = pop_icyfloat(_stack);
            push_icyfloat(_stack,!value_1);
        }
        else
            throw "Error:data type currently does not support not operation.\n";
    }   

//////////////////////////////////////
//以上是真正执行抽象语法树节点相应的任务的函数
//////////////////////////////////////
//////////////////////////////////////
//以下是内置api
//////////////////////////////////////

    void icyapi_print(Stack &_stack,IcyProcess *_pproc_context,IcyFunction *_pfunc_context)
    {
        icyobj_t object_type;
        IcyInt intVal;
        IcyFloat floatVal;
        IcyInt size_param_list = pop_icyint(_stack);
        for(auto i = 0; i < size_param_list; i++)
        {
            object_type = pop_objt(_stack);
            switch(object_type)
            {
                case OBJTP_INTEGER:
                    intVal = pop_icyint(_stack);
                    std::cout << intVal;
                    break;
                case OBJTP_REALNUM:
                    floatVal = pop_icyfloat(_stack);
                    std::cout << floatVal;
                    break;
            }

        }
    }


    void refer_value(Stack &_stack,std::vector<std::shared_ptr<byte>> _resource_table,icyAstNode* _node,short _require = REQUIRE_RVALUE)
    {
        if(_require == REQUIRE_LVALUE)//如果请求的是左值
        {
            push_uint(_stack,_node->source);
            push_nodetp(_stack,_node->node_type);//直接将类型和索引入栈
            return;//退出函数
        }
        else if(_require == REQUIRE_RVALUE)
        {
            register icyobj_t type = *(icyobj_t*)_resource_table[_node->source].get();
            switch(type)
            {
                case OBJTP_INTEGER:
                    push_icyint(_stack,reinterpret_cast<IcyIntObject*>(_resource_table[_node->source].get())->m_data);
                    push_objtp(_stack,OBJTP_INTEGER);
                    break;
                case OBJTP_REALNUM:
                    push_icyfloat(_stack,reinterpret_cast<IcyFloatObject*>(_resource_table[_node->source].get())->m_data);
                    push_objtp(_stack,OBJTP_REALNUM);
                    break;
                //前面的区域，以后再来探索吧
            }

        }

    }

    IcyProcess::IcyProcess()
    :m_mutualobj_index_table(0),
    m_mutualobj_table(0),
    m_constobj_index_table(0),
    m_constobj_table(0),
    m_current_localobj_index_table(0),
    m_api_table(_ICYAPI_SIZE_)
    {
        if(!cirno_initialized)
        {
            OperatorTable.resize(_NODETP_END_);

            OperatorTable[NODETP_ADD] = icy_operator_add;
            OperatorTable[NODETP_SUB] = icy_operator_sub;
            OperatorTable[NODETP_MUL] = icy_operator_mul;
            OperatorTable[NODETP_DIV] = icy_operator_div;

            OperatorTable[NODETP_AND] = icy_operator_and;
            OperatorTable[NODETP_OR]  = icy_operator_or;
            OperatorTable[NODETP_NOT] = icy_operator_not;

            m_api_table[ICYAPI_PRINT] = icyapi_print;


            cirno_initialized = true; 
            
        }
    }

    icyAstNode *IcyProcess::generate_ast2(TokenList::iterator _begin,TokenList::iterator _end,IcyFunction *_pfunc_context)
    {
        icyAstNode *current_node{nullptr};
        TokenList::iterator current_token = icy_find_minlevel_token2(_begin,_end);//先找到这一段中执行优先级最低的节点中在表达式最右侧的那个
        current_node = make_ast_node_via_strslice(*current_token);                                        //根据字符生成相应的抽象语法树节点

        uint current_index{0};
        
        auto obj_name = current_token+1;

        if(current_node->node_type == NODETP_CREATE_CONST_OBJ)  //如果是定义常量的节点
        {
            if(obj_name == _end)
                throw"[Syntax Analyse]Exception from Cirno::IcyProcess::generate_ast2: command missing argument.\n";//没有被定义的常量名
            if(!icy_naming_check(*obj_name))
                throw"[Lexical Analyse]Exception from Cirno::IcyProcess::generate_ast2: illegal object name.\n";//命名不合法错误
            //检测该名称是否已经被使用
            //std::string str_objname = strslice_to_string(*obj_name);
            bool found{
                temp_find(m_constobj_index_table,*obj_name) != m_constobj_index_table.end() ||
                temp_find(m_mutualobj_index_table,*obj_name) != m_mutualobj_index_table.end() ||
                temp_find(m_current_localobj_index_table,*obj_name) != m_current_localobj_index_table.end() 
            };            
            if(found)
                throw"[Syntax Analyse]Exception from Cirno::IcyProcess::generate_ast2: multiple definition of an object.\n";//重复定义错误
            
            current_index = m_constobj_table.size();
            m_constobj_index_table.emplace_back(_TempPair(*obj_name,current_index));
            m_constobj_table.emplace_back((byte*)new IcyObjectBase());

            current_node->node_type = NODETP_CONST_OBJECT;  //直接将创建对象的节点转化为对节点本身的引用的节点
            current_node->source = current_index;           
        }
        else if(current_node->node_type == NODETP_CREATE_MUTUAL_OBJ)  //如果是定义常量的节点
        {
            if(obj_name == _end)
                throw"[Syntax Analyse]Exception from Cirno::IcyProcess::generate_ast2: command missing argument.\n";//没有被定义的常量名
            if(!icy_naming_check(*obj_name))
                throw"[Lexical Analyse]Exception from Cirno::IcyProcess::generate_ast2: illegal object name.\n";//命名不合法错误
            //检测该名称是否已经被使用
            bool found{
                temp_find(m_constobj_index_table,*obj_name) != m_constobj_index_table.end() ||
                temp_find(m_mutualobj_index_table,*obj_name) != m_mutualobj_index_table.end() ||
                temp_find(m_current_localobj_index_table,*obj_name) != m_current_localobj_index_table.end() 
            };
            if(found)
                throw"[Syntax Analyse]Exception from Cirno::IcyProcess::generate_ast2: multiple definition of an object.\n";//重复定义错误
            
            current_index = m_mutualobj_table.size();
            m_mutualobj_index_table.emplace_back(_TempPair(*obj_name,current_index));
            m_mutualobj_table.emplace_back((byte*)new IcyObjectBase());

            current_node->node_type = NODETP_MUTUAL_OBJECT;  //直接将创建对象的节点转化为对节点本身的引用的节点
            current_node->source = current_index;           
        }
        else if(current_node->node_type == NODETP_CREATE_LOCAL_OBJ)  //如果是定义常量的节点
        {
            auto obj_name = current_token+1;
            if(obj_name == _end)
                throw"[Syntax Analyse]Exception from Cirno::IcyProcess::generate_ast2: command missing argument.\n";//没有被定义的常量名
            if(!icy_naming_check(*obj_name))
                throw"[Lexical Analyse]Exception from Cirno::IcyProcess::generate_ast2: illegal object name.\n";//命名不合法错误
            bool found{
                temp_find(m_constobj_index_table,*obj_name) != m_constobj_index_table.end() ||
                temp_find(m_mutualobj_index_table,*obj_name) != m_mutualobj_index_table.end() ||
                temp_find(m_current_localobj_index_table,*obj_name) != m_current_localobj_index_table.end() 
            };
            if(found)
                throw"[Syntax Analyse]Exception from Cirno::IcyProcess::generate_ast2: multiple definition of an object.\n";//重复定义错误
            current_index = _pfunc_context->m_localobj_table.size();
            m_current_localobj_index_table.emplace_back(_TempPair(*obj_name,current_index));
            _pfunc_context->m_localobj_table.emplace_back((byte*)new IcyObjectBase());

            current_node->node_type = NODETP_LOCAL_OBJECT;  //直接将创建对象的节点转化为对节点本身的引用的节点
            current_node->source = current_index;           
        }
        else if(current_node->node_type == _NODETPSEC_OBJECT_SEC_)//如果是一个对象名
        {
            auto it = temp_find(m_constobj_index_table,*current_token);
            if(it != m_constobj_index_table.end())
            {
                current_node->source = it->second;
                current_node->node_type = NODETP_CONST_OBJECT;
                return current_node;
            }
            it = temp_find(m_mutualobj_index_table,*current_token);
            if(it != m_mutualobj_index_table.end())
            {
                current_node->source = it->second;
                current_node->node_type = NODETP_MUTUAL_OBJECT;
                return current_node;
            }
            auto it_local = temp_find(m_current_localobj_index_table,*current_token);
            if(it_local != m_current_localobj_index_table.end())
            {
                current_node->source = it->second;
                current_node->node_type = NODETP_LOCAL_OBJECT;
                return current_node;
            }
            else
                throw"[Syntax Analyse]Exception from Cirno::IcyProcess::generate_ast2: undefined token.\n";            

        }
        else if(current_node->node_type == NODETP_SHIF_ACCESS)//这个是用[符号表示的
        {
            auto left_mark = find_left_pair(current_token,_begin);
            auto sub_node1 = generate_ast2(_begin,left_mark,_pfunc_context);//递归生成其第一个子节点

            auto sub_node2 = generate_ast2(left_mark+1,current_token,_pfunc_context);   //递归生成其第二个子节点
            current_node->sub_nodes.emplace_back(sub_node1);
            current_node->sub_nodes.emplace_back(sub_node2);

        }
        else if(current_node->node_type > _NODETPSEC_BIN_BEGIN_ && current_node->node_type < _NODETPSEC_BIN_END_)//统一处理其他双目运算符
        {
            auto sub_node1 = generate_ast2(_begin,current_token,_pfunc_context);//递归生成两个子节点
            auto sub_node2 = generate_ast2(current_token+1,_end,_pfunc_context);
            current_node->sub_nodes.emplace_back(sub_node1);//加入两个子节点
            current_node->sub_nodes.emplace_back(sub_node2);
        }
        else if(current_node->node_type == NODETP_CONST_OBJECT)
        {
            auto it = temp_find(m_constobj_index_table,*current_token);
            if(it == m_constobj_index_table.end())  //如果尚未录入则去录入一下
            {
                current_index = m_constobj_table.size();
                m_constobj_index_table.emplace_back(_TempPair(*current_token,current_index));
                m_constobj_table.emplace_back(read_icy_constant_val(*current_token));
                current_node->source = current_index;
            }
            else
                current_node->source = it->second;
        }
        else if(current_node->node_type == NODETP_NOT)
        {
            auto sub_node = generate_ast2(current_token+1,_end,_pfunc_context);
            current_node->sub_nodes.emplace_back(sub_node);
        }
        else if(current_node->node_type == NODETP_POS)
        {
            auto sub_node = generate_ast2(current_token+1,_end,_pfunc_context);
            return sub_node;//其实这个在算数上没有任何作用...要不要删掉这个呢
        }
        else if(current_node->node_type == NODETP_NEG)
        {
            auto sub_node = generate_ast2(current_token+1,_end,_pfunc_context);
            current_node->sub_nodes.emplace_back(sub_node);
        }
        else if(current_node->node_type == NODETP_LIST)
        {
            TokenList::iterator element_begin = current_token+1;
            TokenList::iterator element_end;
            icyAstNode *pSubTree{nullptr};
            TokenList::iterator end_square_bracket = find_right_pair(current_token,_end);

            while(element_begin != end_square_bracket)
            {
                element_begin++;
                while(element_begin != end_square_bracket && *element_begin == "\n")//第一个迭代器定位到第元素值的起始位置
                    element_begin++;
                element_end = element_begin;
                while(element_end != end_square_bracket && !(*element_end == "," || *element_end == ")"))//另一个迭代器定位到元素末尾--那个括号是什么意思？
                    element_end++;
                pSubTree = generate_ast2(element_begin,element_end,_pfunc_context);
                current_node->sub_nodes.emplace_back(pSubTree);                     //插入子节点
                element_begin = element_end;
            }
        }
        /*
            if conditionA and condition B:                            
        */
       /*[if]
          |---[0][CONDITION]
          |---[1][EXECUTE_IF_TRUE]
          |---[2][ELSE]
       */

        else if(current_node->node_type == NODETP_IF)//if else分支结构的构建
        {
            auto condition_begin = current_token;
            auto condition_end = condition_begin;
            auto scanner = condition_begin;
            while(condition_end != _end && !(*condition_end == ":"))
                condition_end++;
            auto ConditionTreeRoot = generate_ast2(condition_begin,condition_end,_pfunc_context);
            current_node->sub_nodes.emplace_back(ConditionTreeRoot);//获取分支条件
 /*           
            TokenList::iterator execute_segment_begin = condition_end;
            while(execute_segment_begin != _end && *execute_segment_begin == "\n")//获取IF为真将要执行的段落
                execute_segment_begin++;
            if(execute_segment_begin == _end)
                throw"[Syntax Error]:no execution segment for if brach.\n";
*/
            icyAstNode *pAstRootIfTrue  = new icyAstNode(6u);
            pAstRootIfTrue->node_type = NODETP_SCOPE;
            icyAstNode *pAstRootIfFalse = new icyAstNode(6u);
            pAstRootIfFalse->node_type = NODETP_SCOPE;
            bool else_segment{false};

            int unfinished_segment{1};//遇到if则加一，遇到end则减一，等于零的时候说明if分支段落完成
            int unpaired_if{1}; //未被else配对的if数量

            icyAstNode **ppThisAstRoot{&pAstRootIfTrue};

            TokenList::iterator sub_statement_begin = condition_end;
            TokenList::iterator sub_statement_end;
            while(sub_statement_begin != _end)
            {
                sub_statement_end = sub_statement_begin;
                if(*sub_statement_begin == "if")
                    unpaired_if ++;
                if(*sub_statement_begin == "if" ||
                   *sub_statement_begin == "for"||
                   *sub_statement_begin == "foreach"||
                   *sub_statement_begin == "loopif"||
                   *sub_statement_begin == "loopuntil")
                    unfinished_segment++;
                    
                else if(*sub_statement_begin == "else")
                {
                    unpaired_if--;
                    if(unpaired_if == 0)//如果进入了这一级的
                        ppThisAstRoot = &pAstRootIfFalse;
                }
                else if(*sub_statement_begin == "end")
                    unfinished_segment--;

                if(unfinished_segment == 0)//如果if分支段落结束就跳出循环
                    break;

                        //未扫描到表示结束的换行或者分号                               或者       仍然存在尚未配对完成的循环/分支结构段落
                while(sub_statement_end != _end && (!(*sub_statement_end == "\n" || *sub_statement_end == ";") || unfinished_segment > 1))
                {
                    sub_statement_end++;    //向后扫描
                    if(*sub_statement_end == "if" ||
                       *sub_statement_end == "for"||
                       *sub_statement_end == "foreach"||
                       *sub_statement_end == "loopif"||
                       *sub_statement_end == "loopuntil")
                        unfinished_segment++;
                    if(*sub_statement_end == "end")
                        unfinished_segment--;
                }
                if(unfinished_segment > 1)//如果搜找到尾部仍然没能配对，直接抛出错误
                    throw"[Syntax Error]unfinished code segment.\n";
                (**ppThisAstRoot).sub_nodes.emplace_back(generate_ast2(sub_statement_begin,sub_statement_end,_pfunc_context));//获得一个段落后生成这个段落的抽象语法树
                sub_statement_begin = sub_statement_end+1;
            }
            current_node->sub_nodes.emplace_back(pAstRootIfTrue);
            current_node->sub_nodes.emplace_back(pAstRootIfFalse);
        }
        else
            throw"[Unfinished part]:currently unsupport type.\n";
        return current_node;

    }

    void IcyProcess::execute_ast(icyAstNode* _root,IcyThread &_thread_context,IcyFunction &_func_context,short _required)
    {
        if(_root->node_type == NODETP_LOCAL_OBJECT)//如果是局部对象的索引
        {
            refer_value(_thread_context.local_swap_stack,_func_context.m_localobj_table,_root,_required);//一个函数搞定读取局部变量并将其入栈的操作
        }
        else if(_root->node_type == NODETP_CONST_OBJECT)
        {
            if(_required == REQUIRE_LVALUE)
                throw"Error: can not convert a constant to l-value.\n";
            else
                refer_value(_thread_context.local_swap_stack,m_constobj_table,_root,REQUIRE_RVALUE);//一个函数搞定读取局部变量并将其入栈的操作            
        }
        else if(_root->node_type == NODETP_LIST)//如果是创建列表指令
        {
            if(_required == REQUIRE_LVALUE)
                throw"Exception from function execute_ast: can not convert temporary list to l-value.\n";
            for(uint i = _root->sub_nodes.size()-1; i >= 0; i--)//将列表节点的子节点的值全部解析出来并且入栈（按照相反顺序来，这样列表被访问的时候出栈顺序就是列表顺序）
                execute_ast(_root->sub_nodes[i],_thread_context,_func_context,REQUIRE_RVALUE);
            push_objtp(_thread_context.local_swap_stack,OBJTP_LIST);
                 
        }
        else if(_root->node_type > _NODETPSEC_BIN_BEGIN_ && _root->node_type < _NODETPSEC_BIN_END_)//如果是双目运算符
        {
            execute_ast(_root->sub_nodes[0],_thread_context,_func_context,REQUIRE_RVALUE);//向两个子节点请求右值
            execute_ast(_root->sub_nodes[1],_thread_context,_func_context,REQUIRE_RVALUE);
            auto fn = OperatorTable[_root->node_type];//在表中找到相应的处理函数
            fn(_thread_context.local_swap_stack,nullptr,nullptr);//处理数据
        }
        else if(_root->node_type > _NODETPSEC_UNARY_BEGIN_ && _root->node_type < _NODETPSEC_UNARY_END_)//如果是单目运算符
        {
            execute_ast(_root->sub_nodes[0],_thread_context,_func_context,REQUIRE_RVALUE);//向子节点请求右值
            auto fn = OperatorTable[_root->node_type];
            fn(_thread_context.local_swap_stack,nullptr,nullptr);
        }
        else if(_root->node_type == NODETP_IF)
        {
            execute_ast(_root->sub_nodes[0],_thread_context,_func_context,REQUIRE_RVALUE);//请求计算分支条件的右值
            icyobj_t type = pop_objt(_thread_context.local_swap_stack);//将数据类型出栈
            bool condition;

            IcyInt intVal;
            IcyFloat floatVal;

            switch(type)//判断条件表达式值的类型，并得知条件为真还是假
            {
                case OBJTP_INTEGER:
                    intVal = pop_icyint(_thread_context.local_swap_stack);
                    condition = intVal ? true:false;
                    break;
                case OBJTP_REALNUM:
                    floatVal = pop_icyfloat(_thread_context.local_swap_stack);
                    condition = floatVal ? true:false;
            }
            if(condition)//条件为真的分支
                execute_ast(_root->sub_nodes[1],_thread_context,_func_context);//这里默认得到的是右值
            else if(_root->sub_nodes.size() > 2)//条件为假的分支
                execute_ast(_root->sub_nodes[2],_thread_context,_func_context);
            clear_stack(_thread_context.local_swap_stack);//清空栈中的数据
        }

    }


    IcyFunction IcyProcess::make_function2(TokenList _code)
    {
        m_current_localobj_index_table.clear();//因为是要创建一个新的函数，所以清空之前函数残留的信息
        IcyFunction ret_func;
        StrSlice function_name = _code[1];//获取函数名
        /*检查命名合法性*/

        uint local_index{0};

        //对参数列表左右两个括号进行定位
        auto left_bracket = _code.begin()+2;
        auto right_bracket = find_right_pair(left_bracket,_code.end());

        for(auto param = left_bracket+1;param != right_bracket;param++)
        {
            if(*param == ",")
                continue;
            else if(*param == "\n")
                continue;
            else
            {
                m_current_localobj_index_table.emplace_back(_TempPair(*param,local_index));
                ret_func.m_localobj_table.emplace_back((byte*)new IcyObjectBase());
                local_index++;
            }
        }
        TokenList::iterator param_begin = left_bracket+1;
        TokenList::iterator param_end;

        icyAstNode *initialize_expr{nullptr};

        while(param_begin <= right_bracket)
        {
            while(*param_begin == "\n")
                param_begin++;
            param_end = param_begin;
            while(!(*param_end == ",") && param_end != right_bracket)//参数和参数之后的逗号之间不允许换行
                param_end++;
            if(param_begin != right_bracket)
            {
                uint param_index = ret_func.m_localobj_table.size();
                initialize_expr = generate_ast2(param_begin,param_end,&ret_func);//参数在这里就已经被注册了
                if(initialize_expr->node_type == NODETP_MOV)
                {
                    
                    if(!is_ast_const_expr(initialize_expr->sub_nodes[1]))
                        throw"[Syntax Error]initial value of a parameter should be a constant expression.\n";
                    auto initial_value = solve_const_ast(initialize_expr->sub_nodes[1]);//获取参数默认值
                    ret_func.default_param_value_table.insert(std::map<uint,IcyObject*>::value_type(param_index,initial_value));//记录初始值
                }
                else if(initialize_expr->node_type != NODETP_CREATE_LOCAL_OBJ)
                    throw"[Syntax Error]unsupported operation here.\n";
            }
            param_begin = param_end + 1;
        }
        if(right_bracket == _code.end())
            throw"[Syntax Error]unfinished function.\n";
        if(*(right_bracket+1) == ";")
        {
            ret_func.function_body.node_type = NODETP_NIL;
            return ret_func;
        }
        auto body_begin = param_end+1;
        icyAstNode* root = generate_ast2(body_begin,_code.end(),&ret_func);
        ret_func.function_body = *root;
        delete root;
        return ret_func;
    }

    IcyObject* IcyProcess::solve_const_ast(icyAstNode *_root)
    {
        IcyObject* result = new IcyObject();
        IcyThread temp_thread_context;
        execute_ast(_root,temp_thread_context,temp_thread_context.main_function);   //执行抽象语法树(这里的函数上下文随便用一个就可以，反正这个函数执行完就会销毁函数上下文)
        icyobj_t value_type = pop_objt(temp_thread_context.local_swap_stack);//取出execute_ast运行后在线程上下文的栈里留下的数据
        
        switch(value_type)//根据返回的类型取出具体的返回值
        {
            case OBJTP_INTEGER:
                *((IcyInt*)result->source_ptr) = pop_icyint(temp_thread_context.local_swap_stack);
                break;
            case OBJTP_REALNUM:
                result->source_ptr = (byte*)(new IcyFloat);
                *((IcyFloat*)result->source_ptr) = pop_icyfloat(temp_thread_context.local_swap_stack);
                break;
            default://遇到我还没做的数据类型就直接抛出错误
                throw"Exception from Cirno::IcyProcess::solve_const_expr: currently unsupported data type.\n";
        }
        return result;

    }

    IcyThread IcyProcess::compile_script(char *_source_code, std::string _code_file_name,uint _code_len)
    {
        IcyThread main_thread;  //主线程
        if(_source_code && !_code_len)      //如果不是空的指针且未声明代码长度，则自动获取长度
            _code_len = strlen(_source_code);
        TokenList token_list = lexical_analyse(_source_code,_code_len);//完成词法分析
        
        for(auto it = token_list.begin(); it != token_list.end(); it++)
        {
            if(*it == "func")//定义函数的段落
            {
                IcyFunction *pIcyFunction = new IcyFunction;
                std::string function_name = strslice_to_string(*(it+1));//获取函数名
                auto sk = it+2;
                sk = find_right_pair(sk,token_list.end())+1;
                if(sk == token_list.end())//如果函数参数列表后什么都没有就抛出错误。要么有函数体，要么有一个分号表示缺省
                    throw"[Syntax Error]unfinished function.";
                sk++;
                if(*sk != ";")//如果函数没有声明为缺省的就去寻找函数体的结束部分
                {
                    while(sk != token_list.end() && *sk != "{")//定位到函数实现的代码段落开头
                        sk++;
                    if(sk == token_list.end())//如果找不到代码段的开头就抛出异常
                        throw"[Syntax Error]function undefined.\n";
                    sk = find_right_pair(sk,token_list.end());//找到代码段的结尾
                    if(sk == token_list.end())//如果找不到结尾，也抛出异常
                        throw"[Syntax Error]unclosed function body.\n";
                    sk++;
                }
                
                *pIcyFunction = make_function2(cut_tokenlist(it,sk));//构建函数对象，作为全局的常量
                uint current_index = m_constobj_table.size();
                m_constobj_index_table.emplace_back(MyPair(*(it+1),current_index));
                //转化为脚本中的对象类型

                m_constobj_table.emplace_back((byte*)(new Cirno::IcyFunctionObject(*pIcyFunction)));//将函数追加到常量表中

                if(function_name == _code_file_name)
                    main_thread.main_function = *pIcyFunction;

                it = sk;
            }
            else if(*it == "mutual" || *it == "const")
            {
                auto sk = it;
                int unfinished_segment{0};
                //       没有扫描到末尾        且      不是换行也不是分号者分号         或者有未完成的段落        
                while(sk != token_list.end() && ((*sk != "\n" && *sk != ";") || unfinished_segment != 0))
                {
                    if(is_ch_in_cstr((*it)[0],"[{("))
                        unfinished_segment++;
                    else if(is_ch_in_cstr((*it)[0],")]}"))
                        unfinished_segment--;
                    sk++;
                }
                icyAstNode *pAstRoot{nullptr};
                pAstRoot = generate_ast2(it,sk,nullptr);
                //is_ast_const_expr;
                if(pAstRoot->node_type != NODETP_MOV)
                    throw"[Error]:unsupported gloabal operation.\n";
                if(!is_ast_const_expr(pAstRoot->sub_nodes[1]))//如果初始化的值不是常量表达式，那么抛出异常
                    throw"[Error]:initial value should be constant.\n";
                execute_ast(pAstRoot,main_thread,main_thread.main_function);//直接执行就完了
                clear_stack(main_thread.local_swap_stack);//清空栈
                it = sk;
            }

        }
        return main_thread;
    }

    bool IcyProcess::load_script(std::string _file_name)
    {
        int not_ok = stat(_file_name.c_str(),&m_file_info);//获取文件信息
        if(not_ok)
            return false;
        if(m_code.length() > 0)
            m_code.clear();
        m_code.resize(m_file_info.st_size);
        m_fs.open(_file_name,std::ios::in|std::ios::binary);//打开文件
        m_fs.read(m_code.data(),m_file_info.st_size);//读取代码

        std::cout << m_code << '\n';
        return true;
    }
    void IcyProcess::compile()
    {
        if(m_code.length()==0)
            throw"Error:no script loaded.";
        m_main_thread_context = compile_script(m_code.data(),m_file_name.c_str(),m_code.length());
    }
}
