#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include "SuperBlock.h"
#include "BufferManager.h"
#include "INodeTable.h"
#include "DirectoryEntry.h"

class FileSystem
{
private:
    void LoadSupBlk();              /* 读取磁盘中的SuperBlock */
    void SaveSupBlk();              /* 将SuperBlock更新到磁盘 */

    void SupBlkInit();              /* 不通过从磁盘读取初始化SuperBlock */
    void DiskINodeInit();           /* 不通过从磁盘读取初始化DiskINode */
    void BlkInit();                 /* 不通过从磁盘读取初始化文件数据区 */

    void ResetDiskINodeInfo();      /* 初始化SuperBlock直接管理的DiskINode */
    void ResetGroupLinkBlkInfo();   /* 使用成组链接法重置全部盘块信息 */

    void CreateRootDir();           /* 格式化时创建根目录 */

public:
    FileSystem(char *root_dev_name);
    ~FileSystem();

    static const int SUPER_BLOCK_START_SECTOR = 0;  /* SuperBlock起始扇区号 */
    static const int SUPER_BLOCK_SECTOR_NUM = 2;    /* SuperBlock块使用的扇区数 */

    static const int INODE_START_SECTOR = 2;        /* inode区起始扇区号 */
    static const int INODE_SIZE = 64;               /* 每个磁盘inode大小 */
    static const int INODE_PER_SECTOR = 8;          /* 每个扇区的磁盘inode节点数 = 512 / 64 */
    static const int INODE_SECTOR_NUM = 1022;       /* inode区扇区数 */

    static const int BLOCK_START_SECTOR = 1024;     /* 文件数据区起始扇区号 */
    static const int BLOCK_END_SECTOR = 65535;      /* 文件数据区最后一个扇区号 */
    static const int BLOCK_SECTOR_NUM = 64512;      /* 文件数据区实际占用的扇区数 */

    SuperBlock *os_SuperBlock;          /* SuperBlock */
    DeviceManager *os_DeviceManager;    /* 设备控制 */
    BufferManager *os_BufferManager;    /* 缓存控制 */
    INodeTable *os_INodeTable;          /* 内存INode控制表 */

    void FormatDisk();                      /* 格式化磁盘 */

    int AllocDiskINode();                   /* 分配一个DiskINode */
    void FreeDiskINode(int i_no);           /* 释放一个DiskINode,之前需要另行读取和保存DiskINode的内容 */

    Buf *AllocBlk();                        /* 分配一个Block */
    void FreeBlk(int b_no);                 /* 释放一个Block,之前需要另行读取和保存DiskINode的内容 */

    MemINode *ReadDiskINode(int i_no);      /* 找到相应编号的DiskINode，读到内存中 */
    void WriteDiskINode(MemINode *i_ptr);   /* 将MemINode写回到外存相应位置 */

    // MemINode *AllocMemINode();              /* 分配一个新的内存INode */
    void FreeMemINode(MemINode *i_ptr);     /* 释放一个新的内存INode */ 
};

#endif