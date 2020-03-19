#include "UserInterface.h"

extern SuperBlockManager os_SuperBlockManager;

UserInterface::UserInterface()
{
    this->u_SuperBlockManager = &os_SuperBlockManager;
    this->cur_dir = "/";
    this->cd_FileManager = new FileManager;
    this->open_FileManager = NULL;
}

UserInterface::~UserInterface()
{
    if (NULL != this->cd_FileManager)
    {
        delete this->cd_FileManager;
    }

    if (NULL != this->open_FileManager)
    {
        delete this->open_FileManager;
    }
}

/* 格式化磁盘 */
void UserInterface::format()
{
    /* 释放已经打开或者控制的文件 */
    if (NULL != this->cd_FileManager)
    {
        delete this->cd_FileManager;
    }
    if (NULL != this->open_FileManager)
    {
        delete this->open_FileManager;
    }

    /* 格式化磁盘信息 */
    this->u_SuperBlockManager->FormatDisk();
    cur_dir = "/";
    this->cd_FileManager = new FileManager;
    this->open_FileManager = NULL;

    return;
}

/* 返回当前路径 */
string UserInterface::pwd()
{
    return this->cur_dir;
}

/* 列出当前路径下所有文件和文件夹 */
set<string> UserInterface::ls()
{
    /* 获取当前目录下的所有项 */
    FileMap *fm = this->cd_FileManager->LoadItem();

    /* 分析挑选各个项 */
    FileMap::iterator it;
    set<string> result;
    for (it = fm->begin(); it != fm->end(); it++)
    {
        result.insert(it->first);
    }
    delete fm;
    return result;
}

/* 创建文件夹 */
int UserInterface::mkdir(string f_name)
{
    if (this->cd_FileManager->HasItem(f_name))
    {
        cout << "已存在同名文件" << endl;
        return -1;
    }

    this->cd_FileManager->CreateFile(f_name, true);
    return 0;
}

/* 创建文件 */
int UserInterface::touch(string f_name)
{
    if (this->cd_FileManager->HasItem(f_name))
    {
        cout << "已存在同名文件" << endl;
        return -1;
    }

    this->cd_FileManager->CreateFile(f_name);
    return 0;
}

/* 打开文件 */
int UserInterface::fopen(string f_name)
{
    if (NULL != this->open_FileManager)
    {
        delete this->open_FileManager;
    }
    
    if (!this->cd_FileManager->HasItem(f_name))
    {
        cout << "该文件不存在" << endl;
        return - 1;
    }

    int d_no = this->cd_FileManager->GetDiskINodeNo(f_name);
    this->open_FileManager = new FileManager(d_no);
    if (this->open_FileManager->IsFolder())
    {
        delete this->open_FileManager;
        this->open_FileManager = NULL;
        return 2;
    }

    return 1;
}

/* 关闭文件 */
void UserInterface::fclose(string f_name)
{
    if (NULL != this->open_FileManager)
    {
        delete this->open_FileManager;
    }
    this->open_FileManager = NULL;
}

/* 读取文件 */
int UserInterface::fread(char* buf, int length)
{
    if (NULL == this->open_FileManager)
    {
        return -1;
    }

    return this->open_FileManager->Read(buf, length);
}

/* 写入文件 */
int UserInterface::fwrite(char *buf, int length)
{
    if (NULL == this->open_FileManager)
    {
        return -1;
    }

    return this->open_FileManager->Write(buf, length);
}

/* 更改文件指针 */
void UserInterface::fseek(int offset)
{
    if (NULL == this->open_FileManager)
    {
        return;
    }

    this->open_FileManager->SetOfset(offset);
}

/* 获取当前文件读写指针 */
int UserInterface::curseek()
{
    if (NULL == this->open_FileManager)
    {
        return -1;
    }

    return this->open_FileManager->GetOffset();
}

/* 获取文件大小 */
int UserInterface::fsize()
{
    if (NULL == this->open_FileManager)
    {
        return -1;
    }

    return this->open_FileManager->GetFileSize();
}

/* 删除文件 */
int UserInterface::fdelete(string f_name)
{
    if (NULL == this->open_FileManager)
    {
        return -1;
    }

    int d_no = this->cd_FileManager->GetDiskINodeNo(f_name);
    FileManager *fm = new FileManager(d_no);
    if (fm->IsFolder())
    {
        delete fm;
        return 2;
    }

    delete fm;
    this->cd_FileManager->DeleteFile(f_name);
    return 1;
}

/* 删除目录 */
int UserInterface::dirdelete(string f_name)
{
    if (NULL == this->open_FileManager)
    {
        return -1;
    }

    int d_no = this->cd_FileManager->GetDiskINodeNo(f_name);
    FileManager *fm = new FileManager(d_no);
    if (!fm->IsFolder())
    {
        delete fm;
        return 1;
    }

    delete fm;
    this->cd_FileManager->DeleteFolder(f_name);
    return 2;    
}

/* 计算新路径 */
string UserInterface::GetNewDir(string path)
{
    vector<string> paths = split(path, "/");
    vector<string>::const_iterator it;
    for (it = paths.begin(); it != paths.end(); it++)
    {
        const string temp = *it;
        if ("." == temp)
        {
            continue;
        }
        else if (".." == temp)
        {
            if ("/" == this->cur_dir)
            {
                continue;
            }

            int count = 0;
            int length = this->cur_dir.size();
            int i = 0;
            for (i = 0; i < length; i++)
            {
                if (this->cur_dir[i] == '/')
                {
                    count++;
                }
            }

            string to_return = "";
            int met_count = 0;
            i = 0;
            while(true) 
            {
                if(this->cur_dir[i] == '/') 
                {
                    met_count++;
                }
                if(met_count == count) 
                {
                    break;
                }

                to_return += this->cur_dir[i];
                i++;
            }
            this->cur_dir = to_return;
            continue;
        }

        this->cur_dir += (this->cur_dir == "/" ? "" : "/") + temp;

    }
    
    if ("" == this->cur_dir)
        this->cur_dir = "/";
    return this->cur_dir;
}

/* 更换路径 */
int UserInterface::cd(string f_name)
{
    if ("" == f_name || "/" == f_name)
    {
        this->cur_dir = "/";
        delete this->cd_FileManager;
        this->cd_FileManager = new FileManager(ROOT_I_NO);
        return 0;
    }
    else
    {
        int d_no = this->cd_FileManager->GetFileNo(f_name);
        if (-1 == d_no)
        {
            // cout << "该文件不存在" << endl;
            return -1;
        }

        FileManager *fm = new FileManager;
        if(!fm->IsFolder())
        {
            delete fm;
            // cout << "不是文件夹" << endl;
            return -2;
        }
        delete fm;

        string n_dir = GetNewDir(f_name);
        this->cur_dir = n_dir;
        delete this->cd_FileManager;
        this->cd_FileManager = new FileManager(d_no);

        return 0;
    }
}

/* 复制文件 */
int UserInterface::cp(string f_name,string d_name)
{
    int old_no = this->cd_FileManager->GetFileNo(f_name);
    int new_no = this->cd_FileManager->GetFileNo(d_name);

    if (-1 == old_no)
    {
        cout << "该文件不存在" << endl;
        return -1;
    }

    if (-1 == new_no)
    {
        cout << "目标文件不存在" << endl;
        return -2;
    }

    FileManager fm(new_no);
    if (!fm.IsFolder())
    {
        cout << "目标文件不是文件夹" << endl;
        return -3;
    }

    this->cd_FileManager->CopyFile(f_name, &fm);
    return 0;
}

/* 移动文件 */
int UserInterface::mv(string f_name,string d_name)
{
    int old_no = this->cd_FileManager->GetFileNo(f_name);
    int new_no = this->cd_FileManager->GetFileNo(d_name);

    if (-1 == old_no)
    {
        cout << "该文件不存在" << endl;
        return -1;
    }

    if (-1 == new_no)
    {
        cout << "目标文件不存在" << endl;
        return -2;
    }

    FileManager fm(new_no);
    if (!fm.IsFolder())
    {
        cout << "目标文件不是文件夹" << endl;
        return -3;
    }

    this->cd_FileManager->MoveFile(f_name, &fm);
    return 0;
}

/* 是否有文件正在打开 */
bool UserInterface::IsOpenFolder()
{
    return this->open_FileManager != NULL;
}

/* 等待用户输入命令 */
void UserInterface::GetCmd(string user_name)
{
    while(true) 
    {
        string dir = pwd();

        cout << user_name << "@unix " << dir;
        if (IsOpenFolder())
        {
            cout << " $ ";
        }
        else
        {
            cout << " % ";
        }

        string input = "";
        char c;
        while ((c = getchar()) != '\n')
        {
            input += c;
        }
        if(input.size() == 0) 
        {
            continue;
        }

        if("exit"==input)
        {
            return;
        }

        ProcessCmd(input);
    }
}

/* 处理用户输入的命令 */
void UserInterface::ProcessCmd(string cmd)
{
    
    if (0 == cmd.size())
    {
        cout << endl;
    }

    /* 处理命令 */
    vector<string> cmds = split(cmd, " ");
    /* 命令类型 */
    string type = cmds[0];
    /* 参数数量 */
    int para_count = cmds.size() - 1;
    for (int i = 1; i < cmds.size();i++)
    {
        if (cmds[i].find("/") != string::npos)
        {
            cout << "嵌套路径请多次使用cd指令" << endl;
            return;
        }
    }

    if ("format" == type)
    {
        if (0 == para_count)
        {
            format();
            cout << "格式化成功" << endl;
        }
        else 
        {
            cout << "format命令不接受参数" << endl;
        }
        return;
    }
    else if ("pwd" == type)
    {
        if (0 == para_count)
        {
            cout << pwd() << endl;
        }
        else 
        {
            cout << "pwd命令不接受参数" << endl;
        }
        return;
    }
    else if("ls"==type)
    {
        if (0 == para_count)
        {
            set<string> result = ls();
            set<string>::iterator it;
            for (it = result.begin(); it != result.end(); it++)
            {
                cout << *it << endl;
            }
        }
        else 
        {
            cout << "ls命令不接受参数" << endl;
        }
        return;
    }
    else if ("mkdir" == type)
    {
        if (1 == para_count)
        {
            mkdir(cmds[1]);
        }
        else 
        {
            cout << "mkdir命令只接受1个参数" << endl;
        }
        return;
    }
    else if ("touch" == type)
    {
        if (1 == para_count)
        {
            touch(cmds[1]);
        }
        else
        {
            cout << "touch命令只接受1个参数" << endl;
        }
    }
    else if ("fopen" == type)
    {
        if (1 == para_count)
        {
            int res = fopen(cmds[1]);
            if (-1 == res)
            {
                cout << "该文件不存在" << endl;
            }
            else if (2 == res)
            {
                cout << "该文件是文件夹" << endl;
            }
        }
        else
        {
            cout << "fopen命令只接受1个参数" << endl;
        }
    }
    else if ("fclose" == type)
    {
        if (0 == para_count)
        {
            fclose(cmds[1]);
        }
        else
        {
            cout << "fclose命令不接受参数" << endl;
        }
    }
    else if ("fread" == type)
    {
        if (1 == para_count)
        {
            int f_size = stoi(cmds[1]);
            char *buf = new char[f_size];
            int result = fread(buf, f_size);
            for (int k = 0; k < result; k++)
            {
                cout << buf[k];
            }
            cout << endl;
            delete[] buf;
            cout << "共读取" << result << "个字节" << endl;
        }
        else
        {
            cout << "fread命令只接受1个参数" << endl;
        } 
    }
    else if ("fwrite" == type)
    {
        if (0 == para_count)
        {
            string data = "";
            char c;
            while ((c = getchar()) != '\n')
            {
                data += c;
            }
            char *buf = new char[data.length()];
            for (int k = 0; k < data.length(); k++)
            {
                buf[k] = data[k];
            }
            int result = fwrite(buf, data.length());
            delete[] buf;
            cout << "共写入" << result << "个字节" << endl;
        }
        else
        {
            cout << "fwrite命令不接受参数" << endl;
        }
    }
    else if ("fseek" == type)
    {
        if (1 == para_count)
        {
            fseek(stoi(cmds[1]));
        }
        else
        {
            cout << "fseek命令只接受1个参数" << endl;
        }
    }
    else if ("curseek" == type)
    {
        if (0 == para_count)
        {
            cout << curseek() << endl;
        }
        else
        {
            cout << "curseek命令不接受参数" << endl;
        }
    }
    else if ("fsize" == type)
    {
        if (0 == para_count)
        {
            cout << fsize() << endl;
        }
        else
        {
            cout << "fsize命令不接受参数" << endl;
        }
    }
    else if ("fdelete" == type)
    {
        if (1 == para_count)
        {
            int result = fdelete(cmds[1]);
            if (-1 == result)
            {
                cout << "该文件不存在" << endl;
            }
            else if (2 == result)
            {
                cout << "该文件是文件夹" << endl;
            }
        }
        else
        {
            cout << "fdelete命令只接受1个参数" << endl;
        }
    }
    else if ("dirdelete" == type)
    {
        if (1 == para_count)
        {
            int result = fdelete(cmds[1]);
            if (-1 == result)
            {
                cout << "该文件夹不存在" << endl;
            }
            else if (1 == result)
            {
                cout << "删除目标是文件" << endl;
            }
        }
        else
        {
            cout << "dirdelete命令只接受1个参数" << endl;
        }
    }
    else if ("cd" == type)
    {
        if(0 == para_count)
        {
            cd();
        }
        else if (1 == para_count)
        {
            int result = cd(cmds[1]);
            if (-1 == result)
            {
                cout << "该文件夹不存在" << endl;
            }
            else if (-2 == result)
            {
                cout << "目标路径不是文件夹" << endl;
            }
        }
        else
        {
            cout << "cd命令接受1个参数或者没有参数" << endl;
        }
    }
    else if ("cp" == type)
    {
        if (2 == para_count)
        {
            int result = cp(cmds[1], cmds[2]);
            if (-1 == result)
            {
                cout << "原文件不存在" << endl;
            }
            else if(-2==result)
            {
                cout << "目的路径不存在" << endl;
            }
        }
        else
        {
            cout << "cd命令只接受2个参数" << endl;
        }   
    }
    else if ("mv" == type)
    {
        if (2 == para_count)
        {
            int result = mv(cmds[1], cmds[2]);
            if (-1 == result)
            {
                cout << "原文件不存在" << endl;
            }
            else if(-2==result)
            {
                cout << "目的路径不存在" << endl;
            }
        }
        else
        {
            cout << "mv命令只接受2个参数" << endl;
        }   
    }
    else
    {
        cout << "暂时不支持命令" << type << endl;
    }

    return;
}