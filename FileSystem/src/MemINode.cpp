#include "MemINode.h"

MemINode::MemINode()
{
    this->i_mode = 0;

    this->i_count = 0;
    this->i_nlink = 0;

    this->i_dev = 0;
    this->i_number = -1;

    this->i_uid = 0;
    this->i_gid = 0;

    this->i_size = 0;
    for (int i = 0; i < 10; i++)
    {
        this->i_addr[i] = 0;
    }

    this->i_lastr = 0;
}

MemINode::~MemINode()
{
    
}

/* 将外存inode转化成内存inode */
void MemINode::FromDiskINode(DiskINode d_inode)
{
    this->i_mode = d_inode.d_mode; 
    this->i_nlink = d_inode.d_nlink; 
    this->i_uid = d_inode.d_uid; 
    this->i_gid = d_inode.d_gid; 
    this->i_size = d_inode.d_size; 
    
    for (int i = 0; i < 10; i++)
    {
        this->i_addr[i] = d_inode.d_addr[i];
    }

    this->i_count = 0;
    return;
}

/* 将内存inode转化成外存inode */
DiskINode MemINode::ToDiskINode()
{
    DiskINode d_inode;
    d_inode.d_mode = this->i_mode;
    d_inode.d_nlink = this->i_nlink;
    d_inode.d_uid = this->i_uid;
    d_inode.d_gid = this->i_gid;
    d_inode.d_size = this->i_size;
    for (int i = 0; i < 10; i++)
    {
        d_inode.d_addr[i] = this->i_addr[i];
    }

    return d_inode;
}