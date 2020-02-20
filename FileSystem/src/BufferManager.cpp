#include "BufferManager.h"
#include<iostream>
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
    for (int i = 0; i < this->NBUF; i++)
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