#include "BufferManager.h"

extern BlockDevice os_BlockDevice;

BufferManager::BufferManager()
{
    this->b_BlockManager = &os_BlockDevice;
    Initialize();
}

BufferManager::~BufferManager()
{
    
}

/* 初始化 */
void BufferManager::Initialize()
{
    for (int i = 0; i < BUFFER_NUM;i++)
    {
        /* 第一个块 */
        if (0 == i)
        {
            this->buf[i].b_forw = &this->FreeList;
            this->FreeList.b_back = &this->buf[i];
        }
        /* 前指针 */
        else
        {
            this->buf[i].b_forw = &this->buf[i - 1];
        }

        /* 最后一个块 */
        if (BUFFER_NUM - 1 == i)
        {
            this->buf[i].b_back = &this->FreeList;
            this->FreeList.b_forw = &this->buf[i];
        }
        /* 后指针 */
        else
        {
            this->buf[i].b_back = &this->buf[i + 1];
        }

        /* 每块缓存首地址*/
        this->buf[i].b_addr = this->buffer[i];
    }

    cout << "缓存管理器初始化成功" << endl;

    return;
}

/* 从自由队列获取缓存块 */
void BufferManager::GetBufFromList(Buf *buf)
{
    if (NULL == buf->b_back)
    {
        return;
    }

    buf -> b_forw -> b_back = buf -> b_back;
    buf -> b_back -> b_forw = buf -> b_forw;

    buf -> b_back = NULL;
    buf -> b_forw = NULL;
    return;
}

/* 将缓存块插入自由队列 */
void BufferManager::InsertBufIntoList(Buf *buf)
{
    if (NULL != buf->b_back)
    {
        return;
    }

    buf -> b_forw = this->FreeList.b_forw;
    buf -> b_back = &this->FreeList;

    this->FreeList.b_forw -> b_back = buf;
    this->FreeList.b_forw  = buf;
    return;
}

/* 申请一块缓存，用于读写磁盘上的块 */
Buf *BufferManager::GetBuf(int blkno)
{
    /* 如果之前有buf读过这个盘块 */
    for (int i = 0; i < BUFFER_NUM;i++)
    {
        if (blkno == this->buf[i].b_blkno)
        {
            GetBufFromList(&this->buf[i]);
            return &this->buf[i];
        }
    }

    /* 如果自由队列空 */
    if(this->FreeList.b_back==&this->FreeList)
    {
        cout << "ERROR CODE 001" << endl;
        return NULL;
    }

    /* 获取空闲缓存控制块 */
    Buf *buf = this->FreeList.b_back;
    GetBufFromList(buf);

    /* 处理延迟写 */
    if (Buf::B_DELWRI & buf->b_flags)
    {
        b_BlockManager->Bwrite(buf->b_addr, BUFFER_SIZE, buf->b_blkno * BUFFER_SIZE);
    }

    /* 设置新的盘块信息 */
    buf->b_blkno = blkno;
    buf->b_flags &= ~Buf::B_DELWRI;
    buf->b_flags &= ~Buf::B_DONE;

    return buf;
}

/* 释放缓存控制块buf */
void BufferManager::FreeBuf(Buf *buf)
{
    InsertBufIntoList(buf);
    return;
}

/* 读磁盘的blkno盘块 */
Buf *BufferManager::ReadBuf(int blkno)
{
    Buf *buf = GetBuf(blkno);
    if (Buf::B_DELWRI & buf->b_flags || Buf::B_DONE & buf->b_flags)
    {
        return buf;
    }

    b_BlockManager->Bread(buf->b_addr, BUFFER_SIZE, buf->b_blkno * BUFFER_SIZE);    
    return buf;
}

/* 写一个盘块 */
void BufferManager::WriteBuf(Buf *buf)
{
    /* 清除延迟写 */
    buf->b_flags &= ~Buf::B_DELWRI;
    b_BlockManager->Bwrite(buf->b_addr, BUFFER_SIZE, buf->b_blkno * BUFFER_SIZE);
    /* io完毕 */
    buf->b_flags |= Buf::B_DONE;
    FreeBuf(buf);
    return;
}

/* 延迟写一个盘块 */
void BufferManager::DelWriteBuf(Buf *buf)
{
    buf->b_flags |= Buf::B_DELWRI;
    buf->b_flags |= Buf::B_DONE;
    return;
}

/* 将延迟写的缓存块中的内容全部写入磁盘 */
void BufferManager::FlushBuf()
{
    for (int i = 0; i < BUFFER_NUM; i++)
    {
        /* 设置了延迟写 */
        if (Buf::B_DELWRI & this->buf[i].b_flags)
        {
            /* 清除延迟写 */
            buf->b_flags &= ~Buf::B_DELWRI;
            b_BlockManager->Bwrite(buf->b_addr, BUFFER_SIZE, buf->b_blkno * BUFFER_SIZE);
            /* io完毕 */
            buf->b_flags |= Buf::B_DONE;
            FreeBuf(buf);
        }
    }
    return;
}

/* 清空这个buf中的内容 */
void BufferManager::ClearBuf(Buf *buf)
{
    for (int i = 0; i < BUFFER_SIZE;i++)
    {
        buf->b_addr[i] = 0;
    }
    return;
}