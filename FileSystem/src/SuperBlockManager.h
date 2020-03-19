#ifndef SUPER_BLOCK_MANAGER_H
#define SUPER_BLOCK_MANAGER_H

#include "BlockDevice.h"
#include "BufferManager.h"
#include "SuperBlock.h"
#include "DiskINode.h"
#include "DirectoryEntry.h"

class SuperBlockManager
{
private:
    BlockDevice     *s_BlockDevice;
    BufferManager   *s_BufferManager;
    SuperBlock      *s_SuperBlock;

    void LoadSuperBlock();          /* 从磁盘中读取SuperBlock */
    void SaveSuperBlock();          /* 将SuperBlock保存到磁盘 */
    void ResetFreeBlkInfo();        /* 空闲盘块成组链接 */
    void CreateRootDir();           /* 创建根目录 */

public:
    SuperBlockManager();
    ~SuperBlockManager();

    void FormatDisk();                  /* 格式化磁盘 */
    int AllocDiskINode();               /* 分配外存inode节点 */
    void FreeDiskINode(int d_no);       /* 释放外存inode节点 */
    bool IfAllocedDiskINode(int d_no);  /* 判断外存inode节点是否被分配 */
    Buf *AllocBlk();                    /* 分配外存盘块 */
    void FreeBlk(int b_no);             /* 释放一个盘块 */
};

#endif