#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "File.h"
#include "BufferManager.h"
#include "SuperBlockManager.h"
#include "INodeManager.h"
#include "DirectoryEntry.h"
#include <map>
#include <string>

#define ROOT_I_NO 0
typedef map<string, int> FileMap;

class FileManager
{
private:
    SuperBlockManager   *f_SuperBlockManager;
    BufferManager       *f_BufferManager;
    INodeManager        *f_INodeManager;
    File                *f_file;

    File *OpenFile(int i_no);       /* 根据内存inode序号打开文件 */
    void DeleteItem(string f_name); /* 根据文件名删除目录项 */

public:
    FileManager();                                          /* 打开根目录 */
    FileManager(int i_no);                                  /* 打开inode对应的文件 */
    ~FileManager();

    bool IsFolder();                                         /* 判断该路径是否是文件夹 */
    int GetFileSize();                                      /* 获取文件大小 */
    int GetOffset();                                        /* 获取文件指针 */
    void SetOfset(int n_offset);                            /* 更改文件读写指针 */
    bool HasItem(string f_name);                            /* 检查文件夹目录下是否有某文件 */
    int CountItem();                                        /* 统计文件夹下的内容的数量 */
    FileMap *LoadItem();                                    /* 获取该路径下所有项目 */
    int GetDiskINodeNo(string f_name);                      /* 根据文件名查询外存inode编号 */
    int GetFileNo(string addr);                             /* 根据相对地址获取对应文件inode编号 */
    int CreateFile(string f_name, bool is_dir = false);     /* 在当前目录创建文件 */
    void DeleteFile(string f_name);                         /* 删除目录下普通文件 */
    void DeleteFolder(string f_name, bool force = false);   /* 删除目录下的目录 */
    void RenameFile(string f_name, string n_name);          /* 重命名目录下的文件夹 */
    void CopyFile(string f_name, FileManager *pos);         /* 复制文件 */
    void MoveFile(string f_name, FileManager *pos);         /* 移动文件 */
    int Read(char *content, int len);                       /* 读取文件 */
    int Write(char *content, int len);                      /* 写入文件 */
    int Remove(char *content, int len);                     /* 删除部分文件 */
};

#endif