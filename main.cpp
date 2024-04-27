#include"icy.hpp"

int main()
{
    Cirno::IcyProcess cirno;
    try{
    cirno.load_script("./test.icy");
    cirno.compile();
    }
    catch(const char *_msg)
    {
        std::cout << _msg;
    }
 
 


    /*
   fstream fs("test.icy",ios::in|ios::binary);
   struct stat file_info;
   
   if(stat("test.icy",&file_info))
   {
        cout << "Open file failed.\n";
        exit(0);
   }

    char *pCodes = new char[file_info.st_size];

    fs.read(pCodes,file_info.st_size);

   TokenList token_list = Cirno::lexical_analyse(pCodes,file_info.st_size);

    bool in_str{false};
    //var v = 1 + 2
    // 0  1 4 1 3 1
   for(auto it:token_list)
   {
        if(is_strslice_number(it) || it == "true" || it == "false")
            cout << "\033[32m";
        else if(Cirno::is_icy_keywd(it))
            cout << "\033[33m";
        else if(icy_naming_check(it))
            cout << "\033[34m";
        else if(Cirno::is_icy_operator(it))
            cout << "\033[31m";
        else if(it[0] == '\"')
            cout << "\033[32m";
        else
            cout << "\033[37m";

        cout.put(' ');
        for(uint i=0;i<it.len;i++)
            cout.put(it[i]);


   }
   cout << "\n\n";
    fs.close();
    delete[] pCodes;
    */





}