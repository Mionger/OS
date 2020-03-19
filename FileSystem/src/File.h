#ifndef FILE_H
#define FILE_H

#include "MemINode.h"
#include "BufferManager.h"
#include "SuperBlockManager.h"

#define SMALL_FILE_MAX  6
#define LARGE_FILE_MAX  262
#define HUGE_FILE_MAX   33030

class File
{
public:
    File(MemINode *m_inode);
    ~File();

    static const int FDIR = 0x1;

    BufferManager       *f_BufferManager;
    SuperBlockManager   *f_SuperBlockManager;

    int         f_type;     /* 文件类型 */
    MemINode   *f_inode;    /* 系统文件打开控制项指向的内存inode */
    int         f_offset;   /* 文件读写位置指针 */

    int GetFileSize();                  /* 获取文件大小 */
    int GetBlkNum();                    /* 获取文件使用的盘块数 */
    int GetBlkRest();                   /* 获取最后一个盘块使用量 */
    int TriIndex(int l_blkno);          /* 三级索引，逻辑盘块号到物理盘块号映射 */
    Buf *AllocNewBlk();                 /* 为文件申请新的磁盘盘块 */
    int Read(char *content, int len);   /* 根据f_offset和len读取内容 */
    int Write(char *content, int len);  /* 根据f_offset和len写入内容 */
    void CutOff(int size);              /* 截断文件，保留若干字节 */
    int Remove(char *content, int len); /* 根据f_offset和len删除内容 */
    void Delete();                      /* 删除文件 */
};

#endif