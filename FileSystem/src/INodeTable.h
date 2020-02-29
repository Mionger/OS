#ifndef INODE_TABLE_H
#define INODE_TABLE_H

#include <iostream>
#include "MemINode.h"

class INodeTable
{
public:
    static const int MEM_INODE_MAX = 100;   /* 内存INode最大数量 */

    MemINode m_inode[INodeTable::MEM_INODE_MAX];    /* 内存Inode数组，每个打开文件都会占用一个内存Inode */

    
    int IsLoaded(short dev, int i_no);      /* 是否加载到内存INode表中 */
    MemINode* GetMemINode();                /* 申请空闲的内存INode */
    MemINode *GetLoadedINode(int i_no);     /* 根据DiskINode编号获得内存INode */
};

#endif