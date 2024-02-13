#include<vector>
#include<map>
#include<unordered_map>
#include"icy_ast/icy_ast.hpp"
#include"stack.hpp"



using ushort = unsigned short;
using uint   = unsigned int;
using byte   = char;

using IcyInt   = int;
using IcyFloat = double;
using IcyChar =  wchar_t;
//std::size_t _strslice_hash(StrSlice &_slicce);

struct strslice_cmp
{
    bool operator()(StrSlice sliceA,StrSlice sliceB)
    {
        return sliceA == sliceB;
    }
};

//012345678
//^   ^
void push_icyint(Stack &_stack,IcyInt _value)
{
    if(STACK_MAXSIZE - (_stack.top-_stack.base) < sizeof(IcyInt))
        throw"Exception from \"push_icyint\": no enough space to push in data.\n";
    *((IcyInt*)(_stack.top)) = _value;
    _stack.top += sizeof(IcyInt); 
}

void push_icyfloat(Stack &_stack,IcyFloat _value)
{
    if(STACK_MAXSIZE - (_stack.top-_stack.base) < sizeof(IcyFloat))
        throw"Exception from \"push_icyflooat\": no enough space to push in data.\n";
    *((IcyFloat*)(_stack.top)) = _value;
    _stack.top += sizeof(IcyFloat); 
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

	struct IcyObject
	{		
		uint  type;
		byte* source_ptr;
        IcyObject(uint _type = OBJTP_NIL,byte *_source = nullptr);
        ~IcyObject();
        void operator = (IcyObject &_icyobj);
	};
    IcyObject::IcyObject(uint _type = OBJTP_NIL,byte *_source = nullptr)//注意！IcyObject在被创建后不是随机值，而是默认的空值
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
		icyAstNode *generate_ast(StrSlice _statement,IcyFunction *_pfunction_context);//第四个参数主要是提供上下文信息。
        
        IcyObject  *solve_const_expr(StrSlice _statement);
        IcyFunction *make_function(StrSlice _statement);
	private:
        //所有的函数也是对象，当完成一个make_function之后make_function的信息也会被存入下面两个表中
		std::map<StrSlice,uint,strslice_cmp>     m_mutualobj_index_table;	//共享对象的索引表
		std::vector<IcyObject>  		         m_mutualobj_table;			//共享对象的地址表

        
        std::map<StrSlice,uint,strslice_cmp>    m_constobj_index_table; //常量索引表
        std::vector<IcyObject>                  m_constobj_table;       //常量地址表

        std::map<StrSlice,uint,strslice_cmp>    m_current_localobj_index_table;  //当前使用的对象索引查询表,在生成AST时使用

        //每次根据对象声明的次序分配索引存入表中，在之后生成AST的过程中如果扫描到相同的对象名，就可以
	};

    icyAstNode *IcyProcess::generate_ast(StrSlice _statement,IcyFunction *_pfunction_context)
    {
        icyAstNode *pAstNode;
        StrSlice    current_operator = icy_find_minlevel_token(_statement);                //获取当前一级的操作符
        icyAstNode *current_node = make_ast_node_via_strslice(current_operator);    //根据当前操作符生成相应的节点
        uint        current_local_index;    //本地对象当前应该使用的索引值
        uint        current_mutual_index;   //共享对象当前应该使用的索引值
        uint        current_const_index;    //常量当前应该使用的索引值
        StrSlice    param1,param2;
        //创建对象的指令节点不会被添加到抽象语法树中。相反，对象在编译阶段就被创建，创建对象的操作最终只产生一个对象引用节点。
        if(current_node->node_type == NODETP_CREATE_LOCAL_OBJ)  //如果是创建局部对象的指令
        {
            //显然var指令后面必须是对象名，直接尝试读取
            StrSlice object_name = jump_space(current_operator.ptr + 3);//这里可以直接赋值是因为没有禁用隐式转换，注意看一下数据的类型
            //对象名之后的符号有可能是:1.空格 2.换行符号（未初始化的对象）3.赋值符号=
            //下面根据这个规律读取完整的对象名
            while(object_name[object_name.len] != ' ' && object_name[object_name.len] != '\n' && object_name[object_name.len] != '=')
                object_name.len++;
            //下面检查这个对象名是否已经被声明
            std::map<StrSlice,uint,strslice_cmp>::iterator it;
            it = m_mutualobj_index_table.find(object_name); //全局找一下
            if(it != m_mutualobj_index_table.end())
                throw"Exception from function \"Cirno::IcyProcess::generate_ast\": multiple definition of an object(defined in mutual scope)";
            it = m_current_localobj_index_table.find(object_name);//局部找一下
            if(it != m_current_localobj_index_table.end())
                throw"Exception from function \"Cirno::IcyProcess::generate_ast\": multiple definition of an object(defined in local scope)";
            //将新对象插入表中
            current_local_index = _pfunction_context->m_localobj_table.size();  //  
            m_current_localobj_index_table.insert(std::map<StrSlice,uint,strslice_cmp>::value_type(object_name,current_local_index));
            _pfunction_context->m_localobj_table.push_back(IcyObject());

            //我们不销毁current_node，而是直接改变它的属性。这样会更快吧
            current_node->node_type = NODETP_LOCAL_OBJECT;
            current_node->source    = current_local_index;

            return current_node;//返回这个对对象引用的节点
        }
        //你怎么知道我这一段代码是上一段直接copy&paste过来再修改的
        else if(current_node->node_type == NODETP_CREATE_MUTUAL_OBJ)  //如果是创建局部对象的指令
        {
            StrSlice object_name = jump_space(current_operator.ptr + 6);
            while(object_name[object_name.len] != ' ' && object_name[object_name.len] != '\n' && object_name[object_name.len] != '=')
                object_name.len++;
            std::map<StrSlice,uint,strslice_cmp>::iterator it;
            it = m_mutualobj_index_table.find(object_name); //全局找一下
            if(it != m_mutualobj_index_table.end())
                throw"Exception from function \"Cirno::IcyProcess::generate_ast\": multiple definition of an object(defined in mutual scope)";
            it = m_current_localobj_index_table.find(object_name);//局部找一下
            if(it != m_current_localobj_index_table.end())
                throw"Exception from function \"Cirno::IcyProcess::generate_ast\": multiple definition of an object(defined in local scope)";
            //将新对象插入表中
            current_mutual_index = m_mutualobj_table.size();  //  
            m_mutualobj_index_table.insert(std::map<StrSlice,uint,strslice_cmp>::value_type(object_name,current_mutual_index));
            m_mutualobj_table.push_back(IcyObject());

            //我们不销毁current_node，而是直接改变它的属性。这样会更快吧
            current_node->node_type = NODETP_MUTUAL_OBJECT;
            current_node->source    = current_mutual_index;

            return current_node;//返回这个对对象引用的节点
        }
        //对象类节点
        else if(current_node->node_type == _NODETPSEC_OBJECT_SEC_)
        {
        //下面在各个作用域查找这个对象
            std::map<StrSlice,uint,strslice_cmp>::iterator it;
            it = m_mutualobj_index_table.find(current_operator);
            if(it != m_mutualobj_index_table.end())//如果在全局共享对象中可以找到
            {
                current_node->source    = it->second;  //将索引值置入节点
                current_node->node_type = NODETP_MUTUAL_OBJECT; //  设置节点类型为共享对象引用
                return current_node;
            }
            it = m_current_localobj_index_table.find(current_operator);
            if(it != m_current_localobj_index_table.end())
            {
                current_node->source = it->second;
                current_node->node_type = NODETP_LOCAL_OBJECT;
                return current_node;
            }
            it = m_constobj_index_table.find(current_operator);
            if(it != m_constobj_index_table.end())
            {
                current_node->source = it->second;
                current_node->node_type = NODETP_CONST_OBJECT;
                return current_node;
            } 
            else//如果哪里都找不到定义，那么只好抛出错误了
                throw"Exception from function \"Cirno::IcyProcess::generate_ast\": undefined identifier";          
        }
        //常量类节点，包括常量数字、字符和字符串
        else if(current_node->node_type == NODETP_CONST_OBJECT)
        {
            std::map<StrSlice,uint,strslice_cmp>::iterator it;
            it = m_constobj_index_table.find(current_operator);
            if(it == m_constobj_index_table.end())
            {
                current_const_index = m_constobj_index_table.size();
                m_constobj_index_table.insert(std::map<StrSlice,uint,strslice_cmp>::value_type(current_operator,current_const_index));
                m_constobj_table.push_back(read_icy_constant_val(current_operator));
                current_node->source = current_const_index;
            }
            else
            {
                current_node->source = it->second;
            }
            return current_node;

        }
        else if(current_node->node_type == NODETP_SHIF_ACCESS)
        {
            //获取第一个参数
            param1.ptr = _statement.ptr;
            param1.len = current_operator.ptr - _statement.ptr;
            //获取第二个参数
            param2.ptr = current_operator.ptr+1;
            char* end_pos = find_pair_sign(_statement.ptr,_statement.len - (current_operator.ptr - _statement.ptr));
            param2.len = end_pos - param2.ptr;
            //向下迭代生成sub_ast并添加为当前节点的子节点
            auto sub_node1 = generate_ast(param1,_pfunction_context);
            auto sub_node2 = generate_ast(param2,_pfunction_context);
            current_node->sub_nodes.push_back(sub_node1);
            current_node->sub_nodes.push_back(sub_node2);
            return current_node;


        }
        //这里统一处理[]之外所有的双目运算符
        else if(current_node->node_type > _NODETPSEC_BIN_BEGIN_ && current_node->node_type < _NODETPSEC_BIN_END_)
        {
            param1.ptr = _statement.ptr;
            param1.len = current_operator.ptr - _statement.ptr;

            param2.ptr = current_operator.ptr + current_operator.len;
            param2.len = _statement.ptr + _statement.len - current_operator.ptr -1;

            auto sub_node1 = generate_ast(param1,_pfunction_context);
            auto sub_node2 = generate_ast(param2,_pfunction_context);
            current_node->sub_nodes.push_back(sub_node1);
            current_node->sub_nodes.push_back(sub_node2);
            return current_node;
        }
        else if(current_node->node_type == NODETP_NOT)
        {
            // ! success
            //0123456789
            param1.ptr = current_operator.ptr + 1;
            param1.len = _statement.ptr + _statement.len - current_operator.ptr - 1;
            auto sub_node = generate_ast(param1,_pfunction_context);
            current_node->sub_nodes.push_back(sub_node);
            return current_node;
        }
        else
            throw"Exception from function \"Cirno::IcyProcess::generate_ast\":unknown type";



    }


/*
func myfunc ( 
            param1,
             param2
             )
        :
        param1 ("something"),
        param2
        (48)
        {

        }
*/
	IcyFunction *IcyProcess::make_function(StrSlice _code)
    {
        IcyFunction* p_this_function = new IcyFunction;

        char *new_pos = jump_space(_code.ptr+4,_code.len-4);    // 跳过关键字func以及紧随其后的空格，直接定位到函数名的开头
        _code.len -= (new_pos - _code.ptr);
        _code.ptr = new_pos;


        //获取函数名称
        StrSlice function_name(_code.ptr);
        uint i;
        for(i=0; i<_code.len;i++)//一直走到左括号为止.这里可以看出i=0时标识的位置是函数名称的第一个字母
            if(_code[i] == '(' || _code[i] == ' ')
                break;
        function_name.len = i;
        //至此完成了对函数名称的收集
        if(!icy_naming_check(function_name))
            throw"Exception from function\"Cirno::IcyProcess::make_function\":illegal function name\n";

        //获取参数
        //v
        char* bracket_begin = jump_space(_code.ptr+i);
        //^
        char* bracket_end = find_pair_sign(bracket_begin);
        StrSlice param_name;//长度默认为0
        param_name.ptr = bracket_begin+1;//定位到左括号之后的位置
        param_name.ptr = jump_space_et_linefd(param_name.ptr);//跳过可能存在的空格和换行符号

        char* param_begin_pos = param_name.ptr; //获得参数列表的起始位置

        uint current_index{0};
        uint num_params{0};//参数的数量
        for(i=0; param_begin_pos + i != bracket_end; i++)//i在这里被重置了
        {
            if(function_name[i] == ',')
            {
                current_index = m_current_localobj_index_table.size();//当前对象被分配到的索引
                if(!icy_naming_check(param_name))//如果该参数名不合法则抛出错误
                    throw"Exception from function\"Cirno::IcyProcess::make_function\":illegal param name\n";
                m_current_localobj_index_table.insert(std::map<StrSlice,uint,strslice_cmp>::value_type(param_name,current_index));//将参数信息插入表中
                p_this_function->m_localobj_table.push_back(IcyObject());//创建对象的索引。尽管是空的
                //至此，对当前参数信息的录入已经完成
                if(param_begin_pos + i + 1 == bracket_end)    // 如果逗号后面的位置就是右括号，抛出错误
                    throw"Exception from function \"Cirno::IcyProcess::make_function\": lose parameter after comma\n";
                
                param_name.ptr = jump_space_et_linefd(function_name.ptr + i + 1);//跳过可能存在的空格和换行，直接定位到下一个参数名的首字母
                param_name.len = 0; //重置长度
                continue;       //进入下一轮循环
                
            }
            param_name.len++;
            
        }
        //定义最后一个参数
        current_index = m_current_localobj_index_table.size();
        m_current_localobj_index_table.insert(std::map<StrSlice,uint,strslice_cmp>::value_type(param_name,current_index));
        p_this_function->m_localobj_table.push_back(IcyObject());
        //所有参数的定义完成了

        //下面对参数的初始化列表进行读取
        char *initialize_list_begin = bracket_end + 1;
        while(initialize_list_begin != _code.ptr + _code.len && *initialize_list_begin != '{')
        {
            if(*initialize_list_begin == ':')
                break;
            initialize_list_begin++;
        }
        param_begin_pos = jump_space_et_linefd(initialize_list_begin + 1);
        param_name.ptr = param_begin_pos;
        while(*param_begin_pos != ' ' && *param_begin_pos != '(')
        {
            param_begin_pos++;
            param_name.len++;
        }
        if(!icy_naming_check(param_name))
            throw"Exception from function\"Cirno::IcyProcess::make_function\": illegal indentifier.\n";
        //找到需要被初始化的对象
        std::map<StrSlice,uint,strslice_cmp>::iterator it;
        it = m_current_localobj_index_table.find(param_name);
        if(it == m_current_localobj_index_table.end())
            throw"Exception from function\"Cirno::IcyProcess::make_function\": undefined object name.\n";

        //func myfunction(v1,value2)
        //012345678      0123456789
        //^    ^
        //     0123456789
        
        /*
        func myfunc(p1,p2)
        :p1(12),p2("Hello")
        {

        }
        */



        return p_this_function;
    }


    

    IcyObject *IcyProcess::solve_const_expr(StrSlice _statement)
    {
        icyAstNode *pRootNode = generate_ast(_statement,nullptr);//第二个参数是空指针，因为被计算的全部是常量，不需参考函数上下文
        if(!is_ast_const_expr(pRootNode))//如果不是常量表达式，那么抛出错误
            throw"Exception from \"Cirno::IcyProcess::solve_const_expr\": initial value should be a constant expression.\n";
        
    }


}
