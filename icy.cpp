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
		OBJTP_NIL = 0,
		OBJTP_FUNCTION,
	
	};

	struct IcyObject
	{		
		uint  type;
		byte* source_ptr;
        IcyObject(uint _type = OBJTP_NIL,byte *_source = nullptr);
        ~IcyObject();
	};
    IcyObject::IcyObject(uint _type = OBJTP_NIL,byte *_source = nullptr)
        :type(_type),source_ptr(_source){}
    IcyObject::~IcyObject()
    {
        if(source_ptr)
            delete[] source_ptr;//这里日后要改一下，具体怎么释放内存
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

	};
	class IcyProcess
	{
    public:
        
	protected:
		icyAstNode *generate_ast(StrSlice _statement,IcyFunction *_pfunction_context);//第四个参数主要是提供上下文信息。
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
            //向下迭代生成sub_ast并添加为当前节点的子节点
            auto sub_node1 = generate_ast(param1,_pfunction_context);
            auto sub_node2 = generate_ast(param2,_pfunction_context);
            current_node->sub_nodes.push_back(sub_node1);
            current_node->sub_nodes.push_back(sub_node2);
            return current_node;


        }


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

	IcyObject icy_execute_ast();


}
