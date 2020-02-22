#include<iostream>
#include "BufferManager.h"

using namespace std;

BufferManager::BufferManager()
{
    Initialize();
}

BufferManager::~BufferManager()
{

}

/* 缓存控制块队列的初始化。将缓存控制块中b_addr指向相应缓冲区首地址。 */
void BufferManager::Initialize()
{
    cout << "Starting initialize buffer manager..." << endl;

    /* 初始化自由队列和NODEV队列 */
    this->bFreeList.b_forw = &(this->bFreeList);
    this->bFreeList.b_back = &(this->bFreeList);
    this->bFreeList.av_forw = &(this->bFreeList);
    this->bFreeList.av_back = &(this->bFreeList);

    Buf *buf_ptr;
    for (int i = 0; i < BufferManager::NBUF; i++)
    {
        buf_ptr = &(this->m_Buf[i]);

        /* 缓冲区定位 */
        buf_ptr->b_addr = this->Buffer[i];
        Buffer[i][0] = 'a' + i;

        Buf *temp = this->bFreeList.b_forw;
        
        /* 初始化NODEV队列 */
        buf_ptr->b_forw = temp;
        temp->b_back = buf_ptr;
        this->bFreeList.b_forw = buf_ptr;
        buf_ptr->b_back = &(this->bFreeList);
        
        /* 初始化自由队列 */
        buf_ptr->av_back = temp;
        temp->av_forw = buf_ptr;
        this->bFreeList.av_back = buf_ptr;
        buf_ptr->av_forw = &(this->bFreeList);

        cout << "Insert one buffer succeed!" << endl;
    }

    return;
}

void BufferManager::BufGetFree(Buf*buf)
{
    /* 没有在NODEV队列 */
    if (NULL == buf->b_back)
        return;
    else
    {
        /* 从NODEV队列取出缓存块 */
        buf->b_forw->b_back = buf->b_back;
        buf->b_back->b_forw = buf->b_forw;
        buf->b_forw = NULL;
        buf->b_back = NULL;

        /* 从自由队列取出缓存块 */
        buf->av_forw->av_back = buf->av_back;
        buf->av_back->av_forw = buf->av_forw;
        buf->av_forw = NULL;
        buf->av_back = NULL;
        return;
    }
}

void BufferManager::BufInsertFree(Buf *buf)
{
    /* 在NODEV队列中 */
    if (NULL != buf->b_back)
        return;
    else
    {
        /* 只插入自由队列 */
        this->bFreeList.av_back->av_forw = buf;
        buf->av_forw = &(this->bFreeList);
        buf->av_back = this->bFreeList.av_back;
        this->bFreeList.av_back = buf;
        return;
    }
}

/* 设置设备管理器 */
void BufferManager::SetDevMngr(DeviceManager *DevMngr)
{
    this->m_DeviceManager = DevMngr;
    return;
}

/* 为指定盘块号的块申请一个高速缓存 */
Buf *BufferManager::GetBuf(short dev, int blkno)
{
    Buf *new_buf;
    DevTab *dev_ptr;

    /* NODEV队列 */
    if (-1 == dev)
    {
        dev_ptr = (DevTab *)(&this->bFreeList);
    }
    /* 设备队列中寻找对应缓存块 */
    else
    {
        dev_ptr = this->m_DeviceManager->GetBlockDevice(dev).d_tab;
        for (new_buf = dev_ptr->b_forw; new_buf != (Buf *)dev_ptr; new_buf = new_buf->b_forw)
        {
            if (new_buf->b_blkno != blkno || new_buf->b_dev != dev)
                continue;
            BufGetFree(new_buf);
            return new_buf;
        }
    }

    /* 自由队列空 */
    if (this->bFreeList.av_forw == &(this->bFreeList))
    {
        cout << "ERROR CODE 001" << endl;
        return NULL;
    }
    else
    {
        /* 自由队列非空，取第一个(最早插入的)空闲块 */
        new_buf = this->bFreeList.av_forw;
        BufGetFree(new_buf);

        /* 原缓存块设置延迟写，需要立刻同步到磁盘 */
        if (new_buf->b_flags & Buf::B_DELWRI)
        {
            m_DeviceManager->GetBlockDevice(new_buf->b_dev).write((char *)new_buf->b_addr, BUFFER_SIZE, new_buf->b_blkno * 512);
        }

        /* 从原有的设备队列取出 */
        new_buf->b_forw->b_back = new_buf->b_back;
        new_buf->b_back->b_forw = new_buf->b_forw;

        /* 放入新的设备队列 */
        new_buf->b_forw = dev_ptr->b_forw;
        new_buf->b_back = (Buf *)dev_ptr;
        dev_ptr->b_forw->b_back = new_buf;
        dev_ptr->b_forw = new_buf;
        
        /* 设置缓存块新参数信息 */
        new_buf->b_dev = dev;
        new_buf->b_blkno = blkno;

        /* 复位缓存块标志位 */
        new_buf->b_flags &= ~(Buf::B_DELWRI | Buf::B_DONE);

        return new_buf;
    } 
}

/* 释放指定缓存控制块 */
void BufferManager::FreeBuf(Buf *buf)
{
    /* 单用户单进程的话不用考虑等待等问题 */
    /* 只需要从IO请求队列移出，拿到自由队列 
     * 两者共用同一对指针，因此只考虑这对指针的变化就可以
     */
    BufInsertFree(buf);
    return;
}

/* 清除指定的缓存块的全部内容 */
void BufferManager::ClearBuf(Buf *buf)
{
    int *int_ptr = (int *)buf->b_addr;
    for (int i = 0; i < BufferManager::BUFFER_SIZE / sizeof(int); i++)
    {
        int_ptr[i] = 0;
    }
    return;
}

/* 将一个缓存块的内容设置延迟写 */
void BufferManager::DelWriteBuf(Buf *buf)
{
    /* 设置标记位 */
    buf->b_flags |= Buf::B_DELWRI;
    buf->b_flags |= Buf::B_DONE;

    /* 释放缓存块 */
    FreeBuf(buf);

    return;
}