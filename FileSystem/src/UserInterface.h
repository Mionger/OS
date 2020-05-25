#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H


#include "SuperBlockManager.h"
#include "FileManager.h"
#include "tool.h"
#include <iostream>
#include<string>
#include<vector>
#include<set>
using namespace std;

class UserInterface
{
private:
    SuperBlockManager   *u_SuperBlockManager;
    string               cur_dir;               /* 当前路径 */
    FileManager         *cd_FileManager;        /* 当前路径 */
    FileManager         *open_FileManager;      /* 打开路径 */

    string GetNewDir(string path);      /* 计算新路径 */
    bool IsOpenFolder();                /* 是否有文件正在打开 */

public:
    UserInterface();
    ~UserInterface();

    void format();                      /* 格式化磁盘 */
    string pwd();                       /* 返回当前路径 */
    set<string> ls();                   /* 列出当前路径下所有文件和文件夹 */
    int mkdir(string f_name);           /* 创建文件夹 */
    int touch(string f_name);           /* 创建文件 */
    int fopen(string f_name);           /* 打开文件 */
    void fclose();                      /* 关闭文件 */
    int fread(char* buf, int length);   /* 读取文件 */
    int fwrite(char *buf, int length);  /* 写入文件 */
    void fseek(int offset);             /* 更改文件指针 */
    int curseek();                      /* 获取当前文件读写指针 */
    int fsize();                        /* 获取文件大小 */
    int fdelete(string f_name);         /* 删除文件 */
    int dirdelete(string f_name);       /* 删除目录 */
    int cd(string f_name = "");         /* 更换路径 */
    int cp(string f_name,string d_name);/* 复制文件 */
    int mv(string f_name,string d_name);/* 移动文件 */

    void GetCmd(string user_name);      /* 等待用户输入命令 */
    void ProcessCmd(string cmd);        /* 处理用户输入的命令 */
};

#endif