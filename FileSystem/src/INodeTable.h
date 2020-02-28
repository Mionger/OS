#ifndef INODE_TABLE_H
#define INODE_TABLE_H

#include "SuperBlock.h"
#include "MemINode.h"

class INodeTable
{
public:
    INodeTable();
    ~INodeTable();

    static const int MEM_INODE_MAX = 100;   /* 内存INode最大数量 */

    MemINode m_inode[INodeTable::MEM_INODE_MAX];    /* 内存Inode数组，每个打开文件都会占用一个内存Inode */

    
};

#endif