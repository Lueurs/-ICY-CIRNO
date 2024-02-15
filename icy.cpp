#include<vector>
#include<map>
#include"icy_ast/icy_ast.hpp"
#include"stack.hpp"

extern const char* g_icykeywd_array[];

using ushort = unsigned short;
using uint   = unsigned int;
using byte   = char;




//🥰🥰

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
    using IcyChar  =  wchar_t;

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
            throw"Exception from \"pop_icyint\": no more data to pop.\n";
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

/////////////////////////////////////
//以下是真正执行抽象语法树节点相应的任务的函数
//////////////////////////////////////

    void trans_to_integer(Stack &_stack)//类型转换操作
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

    void icy_operator_add(Stack &_stack)    //加法操作
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

    void icy_operator_sub(Stack &_stack)    //减法操作
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

    void icy_operator_mul(Stack &_stack)    //乘法操作
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
                throw"Exception from execute_ast/operator + : astnode return type mismatching.\n";
            IcyFloat value_2 = pop_icyfloat(_stack);
            push_icyfloat(_stack,value_1 * value_2);
        }
        else
            throw "Error:data type currently does not support multiplication operation.\n";
    }  

    void icy_print(Stack &_stack);




//////////////////////////////////////
//以上是真正执行抽象语法树节点相应的任务的函数
//////////////////////////////////////
	struct IcyObject
	{		
		icyobj_t  type;
		byte*     source_ptr;
        IcyObject(icyobj_t _type = OBJTP_NIL,byte *_source = nullptr);
        ~IcyObject();
        void operator = (IcyObject &_icyobj);
	};
    IcyObject::IcyObject(icyobj_t _type = OBJTP_NIL,byte *_source = nullptr)//注意！IcyObject在被创建后不是随机值，而是默认的空值
        :type(_type),source_ptr(_source){}
    IcyObject::~IcyObject()
    {
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

    }
    void IcyObject::operator=(IcyObject &_icyobj)
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

    IcyObject read_icy_constant_val(StrSlice _statement)
    {
        IcyObject ret_obj;
        if(_statement.len == 0)
            return ret_obj;     //如果这里没有代码，直接返回空的对象引用
        else if(is_strslice_integer(_statement))
        {
            ret_obj.type = OBJTP_INTEGER;
            ret_obj.source_ptr = (byte*)(new IcyInt);
            *((IcyInt*)(ret_obj.source_ptr)) = strslice_to_integer(_statement);
        }
        else if(is_strslice_realnum(_statement))
        {
            ret_obj.type = OBJTP_REALNUM;
            ret_obj.source_ptr = (byte*)(new IcyInt);
            *((IcyInt*)(ret_obj.source_ptr)) = strslice_to_integer(_statement);
        }
        else
            throw"（未完成的数据类型）前面的区域，以后再来探索吧。\n";
        return ret_obj;
        
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
            if(is_ch_in_cstr(current_token[0],"*/^=<>!"))
            {
                if(code_end - current_token.ptr >= 2)
                    if(current_token[1] == '=')
                    {
                        current_token.len = 2;
                        result.emplace_back(current_token);
                        current_token.ptr += 2;
                        continue;
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
            else if(is_ch_in_cstr(current_token[0],"[](){}.:\n\'\",;"))
            {
                result.emplace_back(current_token);
                current_token.ptr++;
                continue;
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
                while(current_token.ptr + current_token.len != code_end && (current_token[current_token.len - 1],"+-*/^%!<>=()[]{},.;&"))
                    current_token.len++;
            }
            else if(is_number(current_token[0])) //数字的解析。这里我改了很久，估计还是有严重的问题
            {
                bool dot{false};//是否有小数点
                while(current_token.ptr + current_token.len != code_end)
                {
                    if(current_token[current_token.len] == '.')
                    {
                        if(dot)     //如果已经有小数点了，那就直接跳出循环，不再向下读取
                            break;
                        else if(current_token.ptr + current_token.len + 1 == code_end ||
                               !is_number(current_token[current_token.len+1]))//如果这个小数点是代码中的最后一个字符,或者小数点后面不是数字,也跳出循环，不再读取
                            break;
                        else//否则(这段数字中之前没有出现过小数点，而且小数点不在代码的尾部出现，小数点的后面一个字符也是数字)
                            dot = true;
                    }
                    else if(!is_number(current_token[current_token.len]))
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
        
        
    }

	//CIRNO的函数
	struct IcyFunction
	{
        std::vector<IcyObject>      m_localobj_table;    //函数的局部变量索引表
        std::map<uint,IcyObject>    default_param_value_table;//参数默认值表
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
        
	protected:
		//icyAstNode  *generate_ast(StrSlice _statement,IcyFunction *_pfunction_context);//第四个参数主要是提供上下文信息。
        icyAstNode  *generate_ast2(TokenList::iterator _begin,TokenList::iterator _end,IcyFunction *_pfunction_context);//上面那个函数的升级版

		IcyObject	execute_ast(icyAstNode* _root,IcyThread &_thread_context,IcyFunction &_func_context);        
        IcyObject  solve_const_ast(icyAstNode *_root);
        IcyFunction *make_function(StrSlice _statement);
        IcyFunction make_function2(TokenList _code);    
        IcyThread   compile_script(char *_source_code, char* _code_file_name,uint _code_len = 0u);

	private:
        char    *m_code;  //源代码
        uint     m_code_len; //代码长度
        char    *m_file_name; //源代码文件名
        //所有的函数也是对象，当完成一个make_function之后make_function的信息也会被存入下面两个表中
		std::map<StrSlice,uint,strslice_cmp>     m_mutualobj_index_table;	//共享对象的索引表
		std::vector<IcyObject>  		         m_mutualobj_table;			//共享对象的地址表

        
        std::map<StrSlice,uint,strslice_cmp>    m_constobj_index_table; //常量索引表
        std::vector<IcyObject>                  m_constobj_table;       //常量地址表

        std::map<StrSlice,uint,strslice_cmp>    m_current_localobj_index_table;  //当前使用的对象索引查询表,在生成AST时使用

        //每次根据对象声明的次序分配索引存入表中，在之后生成AST的过程中如果扫描到相同的对象名，就可以
	};


    icyAstNode *IcyProcess::generate_ast2(TokenList::iterator _begin,TokenList::iterator _end,IcyFunction *_pfunc_context)
    {
        icyAstNode *current_node;
        TokenList::iterator current_token = icy_find_minlevel_token2(_begin,_end);//先找到这一段中执行优先级最低的节点中在表达式最右侧的那个
        current_node = make_ast_node_via_strslice(*current_token);                                        //根据字符生成相应的抽象语法树节点

        uint current_index{0};

        std::map<StrSlice,uint,strslice_cmp>::iterator it;
        if(current_node->node_type == NODETP_CREATE_CONST_OBJ)  //如果是定义常量的节点
        {
            auto obj_name = current_token+1;
            if(obj_name == _end)
                throw"[Syntax Analyse]Exception from Cirno::IcyProcess::generate_ast2: command missing argument.\n";//没有被定义的常量名
            if(!icy_naming_check(*obj_name))
                throw"[Lexical Analyse]Exception from Cirno::IcyProcess::generate_ast2: illegal object name.\n";//命名不合法错误
            //检测该名称是否已经被使用
            bool defined{
                m_constobj_index_table.find(*obj_name) != m_constobj_index_table.end() ||
                m_mutualobj_index_table.find(*obj_name) != m_mutualobj_index_table.end() ||
                m_current_localobj_index_table.find(*obj_name) != m_current_localobj_index_table.end() 
            };
            if(defined)
                throw"[Syntax Analyse]Exception from Cirno::IcyProcess::generate_ast2: multiple definition of an object.\n";//重复定义错误
            
            current_index = m_constobj_table.size();
            m_constobj_index_table.insert(std::map<StrSlice,uint,strslice_cmp>::value_type(*obj_name,current_index));
            m_constobj_table.emplace_back(IcyObject());

            current_node->node_type = NODETP_CONST_OBJECT;  //直接将创建对象的节点转化为对节点本身的引用的节点
            current_node->source = current_index;           
        }
        else if(current_node->node_type == NODETP_CREATE_MUTUAL_OBJ)  //如果是定义常量的节点
        {
            auto obj_name = current_token+1;
            if(obj_name == _end)
                throw"[Syntax Analyse]Exception from Cirno::IcyProcess::generate_ast2: command missing argument.\n";//没有被定义的常量名
            if(!icy_naming_check(*obj_name))
                throw"[Lexical Analyse]Exception from Cirno::IcyProcess::generate_ast2: illegal object name.\n";//命名不合法错误
            //检测该名称是否已经被使用
            bool defined{
                m_constobj_index_table.find(*obj_name) != m_constobj_index_table.end() ||
                m_mutualobj_index_table.find(*obj_name) != m_mutualobj_index_table.end() ||
                m_current_localobj_index_table.find(*obj_name) != m_current_localobj_index_table.end() 
            };
            if(defined)
                throw"[Syntax Analyse]Exception from Cirno::IcyProcess::generate_ast2: multiple definition of an object.\n";//重复定义错误
            
            current_index = m_mutualobj_table.size();
            m_mutualobj_index_table.insert(std::map<StrSlice,uint,strslice_cmp>::value_type(*obj_name,current_index));
            m_mutualobj_table.emplace_back(IcyObject());

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
            //检测该名称是否已经被使用
            bool defined{
                m_constobj_index_table.find(*obj_name) != m_constobj_index_table.end() ||
                m_mutualobj_index_table.find(*obj_name) != m_mutualobj_index_table.end() ||
                m_current_localobj_index_table.find(*obj_name) != m_current_localobj_index_table.end() 
            };
            if(defined)
                throw"[Syntax Analyse]Exception from Cirno::IcyProcess::generate_ast2: multiple definition of an object.\n";//重复定义错误
            
            current_index = _pfunc_context->m_localobj_table.size();
            m_current_localobj_index_table.insert(std::map<StrSlice,uint,strslice_cmp>::value_type(*obj_name,current_index));
            _pfunc_context->m_localobj_table.emplace_back(IcyObject());

            current_node->node_type = NODETP_LOCAL_OBJECT;  //直接将创建对象的节点转化为对节点本身的引用的节点
            current_node->source = current_index;           
        }
        else if(current_node->node_type == _NODETPSEC_OBJECT_SEC_)
        {
            it = m_constobj_index_table.find(*current_token);
            if(it != m_constobj_index_table.end())
            {
                current_node->source = it->second;
                current_node->node_type = NODETP_CONST_OBJECT;
                return current_node;
            }
            it = m_mutualobj_index_table.find(*current_token);
            if(it != m_mutualobj_index_table.end())
            {
                current_node->source = it->second;
                current_node->node_type = NODETP_MUTUAL_OBJECT;
                return current_node;
            }
            it = m_current_localobj_index_table.find(*current_token);
            if(it != m_current_localobj_index_table.end())
            {
                current_node->source = it->second;
                current_node->node_type = NODETP_LOCAL_OBJECT;
                return current_node;
            }
            else
                throw"[Syntax Analyse]Exception from Cirno::IcyProcess::generate_ast2: undefined token.\n";            

        }
        else if(current_node->node_type == NODETP_SHIF_ACCESS)//这个是用]符号表示的
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
            auto sub_node2 = generate_ast2(current_token,_end,_pfunc_context);
            current_node->sub_nodes.emplace_back(sub_node1);//加入两个子节点
            current_node->sub_nodes.emplace_back(sub_node2);
        }
        else if(current_node->node_type == NODETP_CONST_OBJECT)
        {
            it = m_constobj_index_table.find(*current_token);
            if(it == m_constobj_index_table.end())  //如果尚未录入则去录入一下
            {
                current_index = m_constobj_table.size();
                m_constobj_index_table.insert(std::map<StrSlice,uint,strslice_cmp>::value_type(*current_token,current_index));
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
                while(element_end != end_square_bracket && !(*element_end == "," || *element_end == ")"))//另一个迭代器定位到元素末尾
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

        else if(current_node->node_type == NODETP_IF)
        {
            auto condition_begin = current_token;
            auto condition_end = condition_begin;
            while(condition_end != _end && !(*condition_end == ":"))
                condition_end++;
            auto ConditionTreeRoot = generate_ast2(condition_begin,condition_end,_pfunc_context);
            current_node->sub_nodes.emplace_back(ConditionTreeRoot);//获取分支条件
            
            TokenList::iterator exe_if_true_begin = condition_end;
            while(exe_if_true_begin != _end && *exe_if_true_begin == "\n")//获取IF为真将要执行的段落
                exe_if_true_begin++;

            
        }
        else
            throw"[Unfinished part]:currently unsupport type.\n";
        return current_node;

    }

    void execute_ast(icyAstNode* _root,IcyThread &_thread_context,IcyFunction &_func_context)
    {
        if(_root->node_type == NODETP_LOCAL_OBJECT)
        {
            icyobj_t object_type = _func_context.m_localobj_table[_root->source].type;
            switch(object_type)
            {
                case OBJTP_INTEGER:
                    IcyInt value = *((IcyInt*)_func_context.m_localobj_table[_root->source].source_ptr);    //从相应地址读取数据
                    push_icyint(_thread_context.local_swap_stack,value);                                    //将数据入栈
                    break;
                case OBJTP_REALNUM:
                    IcyFloat value = *((IcyFloat*)_func_context.m_localobj_table[_root->source].source_ptr);
                    push_icyfloat(_thread_context.local_swap_stack,value);
                    break;
                //之后在这里添加其他类型
            }

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
                m_current_localobj_index_table.insert(std::map<StrSlice,uint,strslice_cmp>::value_type(*param,local_index));
                ret_func.m_localobj_table.emplace_back(IcyObject());
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
                    IcyObject initial_value = solve_const_ast(initialize_expr->sub_nodes[1]);//获取参数默认值
                    ret_func.default_param_value_table.insert(std::map<uint,IcyObject>::value_type(param_index,initial_value));//记录初始值
                }
                else if(initialize_expr->node_type != NODETP_CREATE_LOCAL_OBJ)
                    throw"[Syntax Error]unsupported operation here.\n";
            }
            param_begin = param_end + 1;
        }
        auto body_begin = param_end+1;
        icyAstNode* root = generate_ast2(body_begin,_code.end(),&ret_func);
        ret_func.function_body = *root;
        delete root;
        return ret_func;
    }

    IcyObject IcyProcess::solve_const_ast(icyAstNode *_root)
    {
        IcyObject result;
        IcyThread temp_thread_context;
        execute_ast(_root,temp_thread_context,temp_thread_context.main_function);   //执行抽象语法树(这里的函数上下文随便用一个就可以，反正这个函数执行完就会销毁函数上下文)
        icyobj_t value_type = pop_objt(temp_thread_context.local_swap_stack);//取出execute_ast运行后在线程上下文的栈里留下的数据
        switch(value_type)//根据返回的类型取出具体的返回值
        {
            case OBJTP_INTEGER:
                result.source_ptr = (byte*)(new IcyInt);
                *((IcyInt*)result.source_ptr) = pop_icyint(temp_thread_context.local_swap_stack);
                break;
            case OBJTP_REALNUM:
                result.source_ptr = (byte*)(new IcyFloat);
                *((IcyFloat*)result.source_ptr) = pop_icyfloat(temp_thread_context.local_swap_stack);
                break;
            default://遇到我还没做的数据类型就直接抛出错误
                throw"Exception from Cirno::IcyProcess::solve_const_expr: currently unsupported data type.\n";
        }
        return result;

    }

    IcyThread IcyProcess::compile_script(char *_source_code, char *_code_file_name,uint _code_len)
    {
        IcyThread main_thread;  //主线程
        if(_source_code && !_code_len)      //如果不是空的指针且未声明代码长度，则自动获取长度
            _code_len = strlen(_source_code);
        TokenList token_list = lexical_analyse(_source_code,_code_len);//完成词法分析
        
        for(auto it = token_list.begin(); it != token_list.end(); it++)
        {
            if(*it == "func")
            {
                auto sk = it;
                
            }
        }

        
    }
}
