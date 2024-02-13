#include<iostream>
#include"icy_ast.hpp"

using namespace std;

void print_strslice(StrSlice _slice)
{
    for(uint i=0;i<_slice.len;i++)
        cout.put(_slice[i]);
    //cout.put(';');
    cout.put('\n');
}

void print_functon_msg(StrSlice _code)
{


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
        //if(!icy_naming_check(function_name))
        //    throw"Exception from function\"Cirno::IcyProcess::make_function\":illegal function name";
        
        cout << "Function name:";
        print_strslice(function_name);

        //获取参数
        //v
        char* bracket_begin = jump_space(_code.ptr+i);
        //^
        char* bracket_end = find_pair_sign(bracket_begin);
        StrSlice param_name;//长度默认为0
        param_name.ptr = bracket_begin+1;//定位到左括号之后的位置
        param_name.ptr = jump_space_et_linefd(param_name.ptr);//跳过可能存在的空格和换行符号

        char* param_begin_pos = param_name.ptr; //获得参数列表的起始位置


        uint num_params{0};//参数的数量
        for(i=0; param_begin_pos + i != bracket_end; i++)//i在这里被重置了
        {
            if(param_begin_pos[i] == ',')
            {

                cout << "parameter " << num_params+1 << ':';
                print_strslice(param_name);

                if(!icy_naming_check(param_name))//如果该参数名不合法则抛出错误
                    throw"Exception from function\"Cirno::IcyProcess::make_function\":illegal param name";

                //至此，对当前参数信息的录入已经完成
                if(param_begin_pos + i + 1 == bracket_end)    // 如果逗号后面的位置就是右括号，抛出错误
                    throw"Exception from function \"Cirno::IcyProcess::make_function\": lose parameter after comma";

                num_params++;

                
                param_name.ptr = jump_space_et_linefd(param_begin_pos + i + 1);//跳过可能存在的空格和换行，直接定位到下一个参数名的首字母
                param_name.len = 0; //重置长度
                continue;       //进入下一轮循环
                
            }
            param_name.len++;
            
        }
        cout << "parameter " << num_params+1 << ':';
        //下面对参数的初始化列表进行读取
        char *initialize_list_begin = bracket_end + 1;
        char *pvalue_str{nullptr};
        while(initialize_list_begin != _code.ptr + _code.len && *initialize_list_begin != '{')
        {
            if(*initialize_list_begin == ':')
                break;
            initialize_list_begin++;
        }
        param_begin_pos = jump_space_et_linefd(initialize_list_begin + 1);
        while(*param_begin_pos != '{' && *param_begin_pos != ';')
        {
            param_name.ptr = param_begin_pos;
            param_name.len = 0;
            while(*param_begin_pos != ' ' && *param_begin_pos != '(')
            {
                param_begin_pos++;
                param_name.len++;
            }
            cout << "initialized parameter:";
            print_strslice(param_name);
            param_begin_pos = jump_space(param_begin_pos);//运行到这一步，param_begin_pos指向的字符必然是左括号(

            char* &pvalue_str = param_begin_pos;//这里是一个引用，其实还是param_begin_pos，只不过我现在想用它读取值，所以换个名字
            pvalue_str++;//走到括号之后
            pvalue_str = jump_space(pvalue_str);
            
            
            
        }

        

        //if(!icy_naming_check(param_name))
          //  throw"Exception from function\"Cirno::IcyProcess::make_function\": illegal indentifier.\n";

}

int main()
{
    StrSlice slice;
    char buff[64];

    char* pos{nullptr};

    while(!compair_strslice_with_cstr(slice,"quit"))
    {
        cin.getline(buff,63);

        pos = jump_space(buff);

        slice.ptr = pos;
        slice.len = strlen(pos);
/*
        if(slice.ptr)
            for(uint i=0; i<slice.len; i++)
            {
                cout.put(slice[i]);
            }       
        cout.put('\n');
        try{
        slice = Cirno::icy_find_minlevel_token(slice);
        }
*/
        try{
            print_functon_msg(slice);
            //cout << strslice_to_integer(slice) <<'\n';
        }

        catch(const char* _msg)
        {
            cerr << _msg;
        }
        if(slice.ptr)
            for(uint i=0; i<slice.len; i++)
            {
                cout.put(slice[i]);
            }
        cout.put('\n');
        //cout << (icy_naming_check(slice)?"Correct\n":"Incorrect\n");
    }
    return 0;
}

int main2()
{
    StrSlice slice;
    char buff[64];
    while(!compair_strslice_with_cstr(slice,"quit"))
    {
        cin.getline(buff,63);
        slice.ptr = buff;
        slice.len = strlen(buff);
        cout << (Cirno::is_icy_keywd(slice) ? "Yes\n" : "No\n");
    }
    return 0;
}

