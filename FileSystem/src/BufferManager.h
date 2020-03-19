#ifndef BUFFER_MANAGER_H
#define BUFFER_MANAGER_H

#include "Buf.h"
#include "BlockDevice.h"

#define BUFFER_NUM  100
#define BUFFER_SIZE 512

class BufferManager
{
private:
    BlockDevice    *b_BlockManager;                     /* 块设备 */
    Buf             FreeList;                           /* 自由队列 */
    Buf             buf[BUFFER_NUM];                    /* 缓存控制块 */
    char            buffer[BUFFER_NUM][BUFFER_SIZE];    /* 缓冲区 */

    void Initialize();                  /* 初始化 */
    void GetBufFromList(Buf *buf);      /* 从自由队列获取缓存块 */
    void InsertBufIntoList(Buf *buf);   /* 将缓存块插入自由队列 */

public:
    BufferManager();
    ~BufferManager();

    Buf *GetBuf(int blkno);         /* 申请一块缓存，用于读写磁盘上的块 */
    void FreeBuf(Buf *buf);         /* 释放缓存控制块buf */
    Buf *ReadBuf(int blkno);        /* 读磁盘的blkno盘块 */
    void WriteBuf(Buf *buf);        /* 写一个盘块 */
    void DelWriteBuf(Buf *buf);     /* 延迟写一个盘块 */
    void FlushBuf();                /* 将延迟写的缓存块中的内容全部写入磁盘 */
    void ClearBuf(Buf *buf);        /* 清空这个buf中的内容 */
};

#endif