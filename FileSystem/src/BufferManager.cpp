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
    // cout << "Starting initialize buffer manager..." << endl;

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
        // Buffer[i][0] = 'a' + i;

        Buf *temp = this->bFreeList.b_forw;
        /* 初始化自由队列 */
        buf_ptr->b_forw = temp;
        temp->b_back = buf_ptr;
        this->bFreeList.b_forw = buf_ptr;
        buf_ptr->b_back = &(this->bFreeList);
        /* 初始化NODEV队列 */
        buf_ptr->av_back = temp;
        temp->av_forw = buf_ptr;
        this->bFreeList.av_back = buf_ptr;
        buf_ptr->av_forw = &(this->bFreeList);

        // cout << "Insert one buffer succeed!" << endl;
    }
}