#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include<string>
#include<vector>
#include<set>
using namespace std;

class UserInterface
{
private:
    vector<string> DealInput(const string s, const string devider); /* 处理输入的字符串，分析命令和参数 */

public:
    /* 构造函数 */
    UserInterface();
    /* 析构函数 */
    ~UserInterface();

    void GetCmd(string user_name);  /* 等待用户输入命令 */
    void ProcessCmd(string input);  /* 处理用户输入的命令 */

    void fformat();                     /* 格式化文件卷
                                         * 命令格式 : fformat */
    set<string> ls();                   /* 列出当前目录下的所有文件和文件夹
                                         * 命令格式 : ls */
    int mkdir(string dir_name);         /* 在当前目录下创建文件夹
                                         * 命令格式 : mkdir <dir_name> */
    int fcreat(string f_name);          /* 创建文件
                                         * 命令格式 : fcreat <f_name> */
    int fopen(string f_name);           /* 打开文件
                                         * 命令格式 : fopen <f_name> */
    void fclose();                      /* 关闭文件
                                         * 命令格式 : fclose <f_name> */
    int fdelete(string f_name);         /* 删除文件
                                         * 命令格式 : fdelete <f_name> */
    int fread(char *buf, int length);   /* 读取文件
                                         * 命令格式 : fread <buf><length> */
    int fwrite(char *buf, int length);  /* 写入文件
                                         * 命令格式 : fwrite <buf><length> */
    void flseek(int offset);            /* 定位文件读写指针
                                         * 命令格式 : flseek <offset> */
    
    void help();                        /* 获取帮助
                                         * 命令格式 : help */
    int cd(string dir_name);            /* 更换路径
                                         * 命令格式 : cd <sir_name>*/
};

#endif