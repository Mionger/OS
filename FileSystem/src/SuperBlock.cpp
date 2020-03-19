#include "SuperBlock.h"

SuperBlock::SuperBlock()
{
    /* 外存inode数 */
    this->s_isize = DISK_INODE_MAX;
    /* 盘块总数 */
    this->s_fsize = DISK_BLOCK_MAX;

    /* 初始化外存inode位图 */
    for (int i = 0; i < DISK_INODE_BLOCK; i++)
    {
        this->s_ibitmap[i] = 0;
    }

    /* 初始化直接管理的盘块 */
    this->s_nfree = 100;
    for (int i = 0; i < this->s_nfree; i++)
    {
        this->s_free[i] = 0;
    }

    /* 初始化脏页标记 */
    this->s_dirty = 0;

    cout << "SuperBlock初始化成功" << endl;
}

SuperBlock::~SuperBlock()
{
    
}

void SuperBlock::GetValue(SuperBlock *op)
{
    /* 防止自赋值 */
    if(this != op)
    {
        this->s_isize = op->s_isize;
        this->s_fsize = op->s_fsize;
        for (int i = 0; i < DISK_INODE_BLOCK; i++)
        {
            this->s_ibitmap[i] = op->s_ibitmap[i];
        }
        this->s_nfree = op->s_nfree;
        for (int i = 0; i < this->s_nfree; i++)
        {
            this->s_free[i] = op->s_free[i];
        }
        this->s_dirty = op->s_dirty;
    }

    return;
}