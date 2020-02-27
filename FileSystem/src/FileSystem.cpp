#include "FileSystem.h"

FileSystem::FileSystem(char *root_dev_name)
{
    /* 申请SuperBlock */
    this->os_SuperBlock = new SuperBlock;
    /* 申请设备管理 */
    this->os_DeviceManager = new DeviceManager(root_dev_name);
    /* 申请缓存管理 */
    this->os_BufferManager = new BufferManager;

    /* 把设备管理模块挂载到缓存管理模块 */
    os_BufferManager->SetDevMngr(this->os_DeviceManager);
}

FileSystem::~FileSystem()
{
    delete this->os_SuperBlock;
    delete this->os_DeviceManager;
    delete this->os_BufferManager;   
}

/* 读取磁盘中的SuperBlock */
void FileSystem::LoadSupBlk ()
{
    /* 从内存读取 */
    os_DeviceManager->GetBlockDevice(DeviceManager::ROOTDEV).read((char *)os_SuperBlock, BufferManager::BUFFER_SIZE * 2, FileSystem::SUPER_BLOCK_START_SECTOR);

    return;
}

/* 将SuperBlock更新到磁盘 */
void FileSystem::SaveSupBlk()
{
    if(this->os_SuperBlock->s_fmod)
    {
        /* 复位SuperBlock更新标记 */
        this->os_SuperBlock->s_fmod = 0;

        /* 写入内存 */
        os_DeviceManager->GetBlockDevice(DeviceManager::ROOTDEV).write((char *)os_SuperBlock, BufferManager::BUFFER_SIZE * 2, FileSystem::SUPER_BLOCK_START_SECTOR);
    }
    return;
}

/* 不通过从磁盘读取初始化SuperBlock */
void FileSystem::SupBlkInit()
{
    /* inode区盘块数 */
    this->os_SuperBlock->s_isize = FileSystem::BLOCK_START_SECTOR - FileSystem::INODE_START_SECTOR;
    /* 总盘块数 */
    this->os_SuperBlock->s_fsize = FileSystem::BLOCK_END_SECTOR + 1;

    /* 直接管理的空闲盘块 */
    this->os_SuperBlock->s_nfree = 100;
    for (int i = 0; i < this->os_SuperBlock->s_nfree; i++)
    {
        //TODO:设置具体盘块号？
        this->os_SuperBlock->s_free[i] = 0;
    }

    /* 直接管理的空闲inode */
    this->os_SuperBlock->s_ninode = 100;
    for (int i = 0; i < this->os_SuperBlock->s_ninode; i++)
    {
        // TODO:设置具体inode号？
        this->os_SuperBlock->s_inode[i] = 0;
    }

    /* 复位SuperBlock更新标记 */
    this->os_SuperBlock->s_fmod = 0;

    SaveSupBlk();

    return;
}

/* 不通过从磁盘读取初始化DiskINode */
void FileSystem::DiskINodeInit()
{
    DiskINode d_inode;
    int inode_num = FileSystem::INODE_SECTOR_NUM * FileSystem::INODE_PER_SECTOR;
    for (int i = 0; i < inode_num; i++)
    {
        this->os_DeviceManager->GetBlockDevice(DeviceManager::ROOTDEV).write((char *)&d_inode, FileSystem::INODE_SIZE, FileSystem::INODE_START_SECTOR * BufferManager::BUFFER_SIZE + FileSystem::INODE_SIZE * i);
    }
    return;
}

/* 不通过从磁盘读取初始化文件数据区 */
void FileSystem::BlkInit()
{
    char blk[512] = {0};
    for (int i = 0; i < FileSystem::BLOCK_SECTOR_NUM; i++)
    {
        this->os_DeviceManager->GetBlockDevice(DeviceManager::ROOTDEV).write(blk, BufferManager::BUFFER_SIZE, BufferManager::BUFFER_SIZE * (i + FileSystem::BLOCK_START_SECTOR));
    }
    return;
}

/* 使用成组链接法重置全部盘块信息 */
void FileSystem::ResetGroupLinkBlkInfo()
{
    /* 计算组数 */
    int g_num = FileSystem::BLOCK_SECTOR_NUM / 100;
    /* 计算剩余的盘块数 */
    int r_num = FileSystem::BLOCK_SECTOR_NUM % 100;
    
    /* 文件数据区计数器 */
    int blk_cnt = FileSystem::BLOCK_START_SECTOR;

    /* 修正组数 */
    if (0 != r_num)
    {
        g_num++;
    }

    /* SuperBlock直接管理的空闲块 */
    this->os_SuperBlock->s_nfree = 100;
    int next_group_pos = blk_cnt;
    for (int i = 0; i < this->os_SuperBlock->s_nfree; i++)
    {
        this->os_SuperBlock->s_free[i] = blk_cnt;
        blk_cnt++;
    }
    this->os_SuperBlock->s_fmod = 1;

    /* 满的空闲块 */
    for (int i = 0; i < g_num - 1; i++)
    {
        GroupLink temp_blk;
        temp_blk.s_nfree = 100;
        for (int j = 0; j < temp_blk.s_nfree; j++)
        {
            temp_blk.s_free[j] = blk_cnt;
            blk_cnt++;
        }

        os_DeviceManager->GetBlockDevice(DeviceManager::ROOTDEV).write((char *)&temp_blk, sizeof(temp_blk), BufferManager::BUFFER_SIZE * next_group_pos);

        next_group_pos = temp_blk.s_free[0];
    }

    /* 包含剩余块的不满空闲块 */
    GroupLink temp_blk;
    temp_blk.s_nfree = r_num;
    /* 空闲盘块结束标志 */
    temp_blk.s_free[0] = 0;
    for (int i = 1; i <= r_num; i++)
    {
        temp_blk.s_free[i] = blk_cnt;
        blk_cnt++;
    }
    os_DeviceManager->GetBlockDevice(DeviceManager::ROOTDEV).write((char *)&temp_blk, sizeof(temp_blk), BufferManager::BUFFER_SIZE * next_group_pos);

    this->os_SuperBlock->s_fmod = 1;

    return;
}

/* 初始化SuperBlock直接管理的DiskINode */
void FileSystem::ResetDiskINodeInfo()
{
    /* SuperBlock直接管理的磁盘INode */
    os_SuperBlock->s_ninode = 0;
    int inode_num = FileSystem::INODE_SECTOR_NUM * FileSystem::INODE_PER_SECTOR;
    int inode_cnt = 0;
    DiskINode d_inode;
    while (os_SuperBlock->s_ninode < 100 && inode_cnt < inode_num)
    {
        while(true)
        {
            /* 从磁盘中读取序号为inode_cnt的DiskINode */
            this->os_DeviceManager->GetBlockDevice(DeviceManager::ROOTDEV).read((char *)&d_inode, FileSystem::INODE_SIZE, FileSystem::INODE_START_SECTOR * BufferManager::BUFFER_SIZE + FileSystem::INODE_SIZE * inode_cnt);
            /* 如果这个外存inode没被分配 */
            if (!(d_inode.d_mode & ALLOCED))
            {
                os_SuperBlock->s_inode[os_SuperBlock->s_ninode] = inode_cnt;
                inode_cnt++;
                os_SuperBlock->s_ninode++;
                break;
            }
            /* 如果被分配 */
            else
            {
                inode_cnt++;   
            }
        }
    }

    /* 设置SuperBlock被修改标记 */
    this->os_SuperBlock->s_fmod = 1;

    return;
}

/* 分配一个DiskINode */
int FileSystem::AllocDiskINode()
{
    DiskINode d_inode;

    /* 没有SuperBlock直接管理的DiskInode */
    if (0 == this->os_SuperBlock->s_ninode)
    {
        int inode_num = FileSystem::INODE_SECTOR_NUM * FileSystem::INODE_PER_SECTOR;
        int inode_cnt = 0;
        while (os_SuperBlock->s_ninode < 100 && inode_cnt < inode_num)
        {
            while(true)
            {
                /* 从磁盘中读取序号为inode_cnt的DiskINode */
                this->os_DeviceManager->GetBlockDevice(DeviceManager::ROOTDEV).read((char *)&d_inode, FileSystem::INODE_SIZE, FileSystem::INODE_START_SECTOR * BufferManager::BUFFER_SIZE + FileSystem::INODE_SIZE * inode_cnt);
                /* 如果这个外存inode没被分配 */
                if (!(d_inode.d_mode & ALLOCED))
                {
                    os_SuperBlock->s_inode[os_SuperBlock->s_ninode] = inode_cnt;
                    inode_cnt++;
                    os_SuperBlock->s_ninode++;
                    break;
                }
                /* 如果被分配 */
                else
                {
                    inode_cnt++;   
                }
            }
        }
    }

    /* DiskINode用尽 */
    if(0 == this->os_SuperBlock->s_ninode)
    {
        cout << "ERROR CODE 002" << endl;
    }

    int inode_no = this->os_SuperBlock->s_inode[--this->os_SuperBlock->s_ninode];
    /* 从磁盘中读取序号为inode_cnt的DiskINode */
    this->os_DeviceManager->GetBlockDevice(DeviceManager::ROOTDEV).read((char *)&d_inode, FileSystem::INODE_SIZE, FileSystem::INODE_START_SECTOR * BufferManager::BUFFER_SIZE + FileSystem::INODE_SIZE * inode_no);
    /* 对刚刚分配的DiskINode的处理 */
    d_inode.d_mode |= ALLOCED;
    /* 占用信息更新到磁盘 */
    this->os_DeviceManager->GetBlockDevice(DeviceManager::ROOTDEV).write((char *)&d_inode, FileSystem::INODE_SIZE, FileSystem::INODE_START_SECTOR * BufferManager::BUFFER_SIZE + FileSystem::INODE_SIZE * inode_no);

    /* 设置SuperBlock被修改标记 */
    this->os_SuperBlock->s_fmod = 1;

    return inode_no;
}

/* 释放一个DiskINode */
void FileSystem::FreeDiskINode(int i_no)
{
    DiskINode d_inode;

    /* SuperBlock直接管理的DiskINode有剩余位置 */
    if (this->os_SuperBlock->s_ninode < 100)
    {
        this->os_SuperBlock->s_inode[this->os_SuperBlock->s_ninode++] = i_no;
        /* 从磁盘中读取序号为inode_cnt的DiskINode */
        this->os_DeviceManager->GetBlockDevice(DeviceManager::ROOTDEV).read((char *)&d_inode, FileSystem::INODE_SIZE, FileSystem::INODE_START_SECTOR * BufferManager::BUFFER_SIZE + FileSystem::INODE_SIZE * i_no);
        /* 对刚刚分配的DiskINode的处理 */
        d_inode.d_mode &= ~(ALLOCED);
        /* 占用信息更新到磁盘 */
        this->os_DeviceManager->GetBlockDevice(DeviceManager::ROOTDEV).write((char *)&d_inode, FileSystem::INODE_SIZE, FileSystem::INODE_START_SECTOR * BufferManager::BUFFER_SIZE + FileSystem::INODE_SIZE * i_no);

        /* 设置SuperBlock被修改标记 */
        this->os_SuperBlock->s_fmod = 1;
    }
    /* SuperBlock直接管理的DiskINode没有剩余位置 */
    else
    {
        /* 从磁盘中读取序号为inode_cnt的DiskINode */
        this->os_DeviceManager->GetBlockDevice(DeviceManager::ROOTDEV).read((char *)&d_inode, FileSystem::INODE_SIZE, FileSystem::INODE_START_SECTOR * BufferManager::BUFFER_SIZE + FileSystem::INODE_SIZE * i_no);
        /* 对刚刚分配的DiskINode的处理 */
        d_inode.d_mode &= ~(ALLOCED);
        /* 占用信息更新到磁盘 */
        this->os_DeviceManager->GetBlockDevice(DeviceManager::ROOTDEV).write((char *)&d_inode, FileSystem::INODE_SIZE, FileSystem::INODE_START_SECTOR * BufferManager::BUFFER_SIZE + FileSystem::INODE_SIZE * i_no);
    }

    return;
}

/* 格式化磁盘 */
void FileSystem::FormatDisk()
{
    /* 格式化并保存SuperBlock */
    SupBlkInit();

    /* 格式化并保存磁盘inode */
    DiskINodeInit();

    /* 格式化并保存文件数据区 */
    BlkInit();

    /* 初始化SuperBlock直接管理的DiskINode */
    ResetDiskINodeInfo();

    /* 将空闲盘块成组链接 */
    ResetGroupLinkBlkInfo();

    /* 保存整理的SuperBlock直接管理的DiskINode和磁盘盘块的信息 */
    SaveSupBlk();

    return;
}
