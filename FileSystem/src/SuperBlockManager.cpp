#include "SuperBlockManager.h"
#include <iostream>

using namespace std;

extern BlockDevice     os_BlockDevice;
extern BufferManager   os_BufferManager;
extern SuperBlock      os_SuperBlock;

SuperBlockManager::SuperBlockManager()
{
    this->s_BlockDevice = &os_BlockDevice;
    this->s_BufferManager = &os_BufferManager;
    this->s_SuperBlock = &os_SuperBlock;
    LoadSuperBlock();
    SaveSuperBlock();
}

SuperBlockManager::~SuperBlockManager()
{
    SaveSuperBlock();
}

/* 从磁盘中读取SuperBlock */
void SuperBlockManager::LoadSuperBlock()
{
    this->s_BlockDevice->Bread((char *)s_SuperBlock, BUFFER_SIZE * 2, 0);
    cout << "从磁盘获取SuperBlock" << endl;
    return;
}

/* 将SuperBlock保存到磁盘 */
void SuperBlockManager::SaveSuperBlock()
{
    if(s_SuperBlock->s_dirty)
    {
        s_SuperBlock->s_dirty = 0;
        this->s_BlockDevice->Bwrite((char *)s_SuperBlock, BUFFER_SIZE * 2, 0);
    }
    cout << "SuperBlock写入磁盘" << endl;
    return;
}

/* 空闲盘块成组链接 */
void SuperBlockManager::ResetFreeBlkInfo()
{
    int disk_num = DISK_BLOCK_MAX - (2 + DISK_INODE_BLOCK);
    int group_n = disk_num / 100;
    int group_r = disk_num % 100;

    if (group_r)
    {
        group_n += 1;
    }

    /* 直接管理的盘块 */
    this->s_SuperBlock->s_nfree = 100;
    int blk_count = 2 + DISK_INODE_BLOCK;
    int group_blk = blk_count;
    for (int i = 0; i < 100; i++)
    {
        this->s_SuperBlock->s_free[i] = blk_count;
        blk_count++;
    }

    /* 剩余的装满的组 */
    for (int i = 1; i < group_n - 1; i++)
    {
        LinkGroup group;
        group.s_nfree = 100;

        for(int j = 0; j < 100; j++) 
        {
            group.s_free[j] = blk_count;
            blk_count++;
        }

        /* 将本组内容写入磁盘 */
        this->s_BlockDevice->Bwrite((char *)&group, sizeof(group), group_blk * BUFFER_SIZE);
        /* 记录下一组内容应该写在磁盘哪块 */
        group_blk = group.s_free[0];
    }

    /* 最后一块磁盘，可能未满 */
    LinkGroup group;
    group.s_nfree = 0;
    /* 放置结束标记 */
    group.s_free[0] = 0;
    for (int i = 1; i <= group_r; i++)
    {
        group.s_free[i] = blk_count;
        blk_count++;
    }
    /* 将本组内容写入磁盘 */
    this->s_BlockDevice->Bwrite((char *)&group, sizeof(group), group_blk * BUFFER_SIZE);

    return;
}

/* 创建根目录 */
void SuperBlockManager::CreateRootDir()
{
    /* 申请盘块 */
    Buf *blk = AllocBlk();
    int b_no = blk->b_blkno;
    char *buf = blk->b_addr;

    /* 申请外存inode */
    DiskINode d_inode;
    int i_no = AllocDiskINode();
    d_inode.d_addr[0] = b_no;
    d_inode.d_size = 2 * sizeof(DirectoryEntry);
    d_inode.d_mode |= DiskINode::IALLOC;
    d_inode.d_mode |= DiskINode::IFDIR;
    this->s_BlockDevice->Bwrite((char *)&d_inode, sizeof(DiskINode), sizeof(SuperBlock) + i_no * sizeof(DiskINode));

    /* 创建根目录 */
    DirectoryEntry dir[2];
    /* 本级路径 */
    dir[0].i_no = i_no;
    memcpy(dir[0].f_name, ".", 2);
    /* 上级路径 */
    dir[0].i_no = i_no;
    memcpy(dir[1].f_name, "..", 3);

    /* 写入磁盘 */
    memcpy(buf, (char *)dir, sizeof(dir));
    this->s_BufferManager->WriteBuf(blk);

    return;
}

/* 格式化磁盘 */
void SuperBlockManager::FormatDisk()
{
    cout << "开始格式化磁盘" << endl;

    /* 初始化外存inode */
    DiskINode d_inode;
    for (int i = 0; i < DISK_INODE_MAX; i++)
    {
        this->s_BlockDevice->Bwrite((char *)&d_inode, sizeof(DiskINode), sizeof(SuperBlock) + i * sizeof(DiskINode));
    }
    cout << "初始化外存inode成功" << endl;

    /* 初始化文件数据区 */
    char blk[512] = {0};
    for (int i = 0; i < DISK_BLOCK_MAX - 2 - DISK_INODE_BLOCK; i++)
    {
        this->s_BlockDevice->Bwrite((char *)&d_inode, BUFFER_SIZE, (2 + DISK_INODE_BLOCK + i) * BUFFER_SIZE);
    }
    cout << "初始化文件数据区成功" << endl;

    SuperBlock superblock;
    this->s_SuperBlock->GetValue(&superblock);

    /* 空闲盘块成组链接 */
    ResetFreeBlkInfo();
    /* 创建根目录 */
    CreateRootDir();
    /* 将SuperBlock保存到磁盘 */
    SaveSuperBlock();

    cout << "格式化磁盘完成" << endl;

    return;
}

/* 分配外存inode节点 */
int SuperBlockManager::AllocDiskINode()
{
    const unsigned int l = 1;

    bool find = false;
    int i_no = -1;
    char blk;
    for (int i = 0; i < DISK_INODE_BLOCK && !find; i++)
    {
        blk = this->s_SuperBlock->s_ibitmap[i];
        for (int j = 0; j < 8; j++)
        {
            /* 每一位代表一个外存inode节点 */
            if (!((l << j) & blk))
            {
                find = true;
                i_no = i * 8 + j;
                this->s_SuperBlock->s_dirty = 1;
                this->s_SuperBlock->s_ibitmap[i] |= (l << j);
                break;
            }
        }
    }

    /* 外存inode节点用尽 */
    if(-1==i_no)
    {
        cout << "ERROR CODE 002" << endl;
    }

    return i_no;
}

/* 释放外存inode节点 */
void SuperBlockManager::FreeDiskINode(int d_no)
{
    const unsigned int l = 1;

    int i = d_no / 8;
    int j = d_no % 8;

    this->s_SuperBlock->s_dirty = 1;
    this->s_SuperBlock->s_ibitmap[i] &= ~(l << j);

    return;
}

/* 判断外存inode节点是否被分配 */
bool SuperBlockManager::IfAllocedDiskINode(int d_no)
{
    const unsigned int l = 1;

    int i = d_no / 8;
    int j = d_no % 8;
    unsigned char result = (l << j) & this->s_SuperBlock->s_ibitmap[i];

    return result != 0;
}

/* 分配外存盘块 */
Buf *SuperBlockManager::AllocBlk()
{
    Buf *buf;

    this->s_SuperBlock->s_dirty = 1;
    int blkno = this->s_SuperBlock->s_free[--this->s_SuperBlock->s_nfree];

    /* 直接管理的盘块为0 */
    if (0 == this->s_SuperBlock->s_nfree)
    {
        buf = this->s_BufferManager->ReadBuf(blkno);
        LinkGroup group;

        group.s_nfree = buf->b_addr[0];
        for (int i = 0; i < group.s_nfree; i++)
        {
            group.s_free[i] = buf->b_addr[i + 1];
        }

        this->s_BufferManager->FreeBuf(buf);
    }

    buf = this->s_BufferManager->GetBuf(blkno);

    return buf;
}

/* 释放一个盘块 */
void SuperBlockManager::FreeBlk(int b_no)
{
    Buf *buf;
    this->s_SuperBlock->s_dirty = 1;

    /* 直接控制的盘块满 */
    if (100 == this->s_SuperBlock->s_nfree)
    {
        buf = this->s_BufferManager->GetBuf(b_no);
        LinkGroup group;

        group.s_nfree = this->s_SuperBlock->s_nfree;
        for (int i = 0; i < group.s_nfree; i++)
        {
            group.s_free[i] = this->s_SuperBlock->s_free[i];
        }

        this->s_BufferManager->WriteBuf(buf);
        this->s_SuperBlock->s_nfree = 0;
    }

    this->s_SuperBlock->s_free[this->s_SuperBlock->s_nfree++] = b_no;

    return;
}