#ifndef INODE_MANAGER_H
#define INODE_MANAGER_H

#include "BlockDevice.h"
#include "BufferManager.h"
#include "SuperBlockManager.h"
#include "MemINode.h"
#include <iostream>
using namespace std;

class INodeManager
{
private:
    static const int MEM_INODE_MAX = 100;               /* 内存inode表大小 */

    BlockDevice *i_BlockDevice;
    BufferManager *i_BufferManager;
    SuperBlockManager *i_SuperBlockManager;
    MemINode m_inode[INodeManager::MEM_INODE_MAX]; /* 内存inode表 */

public:
    INodeManager();
    ~INodeManager();

    MemINode *AllocMemINode();              /* 分配新内存inode */
    void FreeMemINode(MemINode *m_inode);   /* 释放内存inode */
    bool InMem(int d_no);                   /* 判断外存inode1是否在内存 */
    MemINode *GetLoadedINode(int d_no);     /* 获取已经读取到内存的外存inode */
    MemINode *ReadDiskINode(int d_no);      /* 根据外存inode编号读取到内存 */
    void WriteDiskINode(MemINode *m_inode); /* 将内存inode写回到外存 */
};

#endif