#include "FileSystem.h"

FileSystem::FileSystem(char *root_dev_name)
{
    /* 申请SuperBlock */
    this->os_SuperBlock = new SuperBlock;
    /* 申请设备管理 */
    this->os_DeviceManager = new DeviceManager(root_dev_name);
    /* 申请缓存管理 */
    this->os_BufferManager = new BufferManager;
    /* 申请内存INode控制表 */
    this->os_INodeTable = new INodeTable;

    /* 把设备管理模块挂载到缓存管理模块 */
    os_BufferManager->SetDevMngr(this->os_DeviceManager);
}

FileSystem::~FileSystem()
{
    delete this->os_SuperBlock;
    delete this->os_DeviceManager;
    delete this->os_BufferManager;
    delete this->os_INodeTable;
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

    /* 设置SuperBlock被修改标记 */
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
        //TODO:
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

/* 释放一个DiskINode,之前需要另行读取和保存inode的内容 */
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

/* 分配一个Block */
Buf *FileSystem::AllocBlk()
{
    Buf *buf;
    int b_no = this->os_SuperBlock->s_free[--this->os_SuperBlock->s_nfree];
    if (0 == this->os_SuperBlock->s_nfree)
    {
        /* 读取下一组空闲块 */
        buf = this->os_BufferManager->ReadBuf(b_no);

        /* 赋值下一组空闲块 */
        this->os_SuperBlock->s_nfree = ((int *)buf->b_addr)[0];
        for (int i = 0; i < 100; i++)
        {
            this->os_SuperBlock->s_free[i] = ((int *)buf->b_addr)[i + 1];
        }

        this->os_BufferManager->FreeBuf(buf);
    }

    buf = this->os_BufferManager->GetBuf(DeviceManager::ROOTDEV, b_no);

    /* 设置SuperBlock被修改标记 */
    this->os_SuperBlock->s_fmod = 1;

    return buf;
}

/* 释放一个Block,之前需要另行读取和保存DiskINode的内容 */
void FileSystem::FreeBlk(int b_no)
{
    Buf *buf;
    
    /* 直接管理的数块已满 */
    if (100 == this->os_SuperBlock->s_nfree)
    {
        buf = this->os_BufferManager->GetBuf(DeviceManager::ROOTDEV, b_no);

        GroupLink temp_blk;
        temp_blk.s_nfree = this->os_SuperBlock->s_nfree;
        for (int i = 0; i < 100;i++)
        {
            temp_blk.s_free[i] = this->os_SuperBlock->s_free[i];
        }
        this->os_BufferManager->WriteBuf(buf);

        this->os_SuperBlock->s_nfree = 0;
    }

    this->os_SuperBlock->s_free[this->os_SuperBlock->s_nfree++] = b_no;

    /* 设置SuperBlock被修改标记 */
    this->os_SuperBlock->s_fmod = 1;

    return;
}

/* 格式化时创建根目录 */
void FileSystem::CreateRootDir()
{
    /* 申请DiskINode以及文件块 */
    int i_no = AllocDiskINode();
    Buf *blk = AllocBlk();
    int b_no = blk->b_blkno;
    char *buf = blk->b_addr;

    /* 修改DiskINode节点信息并保存 */
    DiskINode d_inode;
    this->os_DeviceManager->GetBlockDevice(DeviceManager::ROOTDEV).read((char *)&d_inode, FileSystem::INODE_SIZE, FileSystem::INODE_START_SECTOR * BufferManager::BUFFER_SIZE + FileSystem::INODE_SIZE * i_no);
    d_inode.d_addr[0] = b_no;
    d_inode.d_size = 2 * sizeof(DirectoryEntry);
    d_inode.d_mode |= FILE_DIR;
    this->os_DeviceManager->GetBlockDevice(DeviceManager::ROOTDEV).write((char *)&d_inode, FileSystem::INODE_SIZE, FileSystem::INODE_START_SECTOR * BufferManager::BUFFER_SIZE + FileSystem::INODE_SIZE * i_no);

    /* 修改目录文件*/
    DirectoryEntry dir[16];
    memcpy(dir[0].m_name, ".", 2);
    memcpy(dir[0].m_name, "..", 3);
    /* 根目录的本级和上一级都是自身 */
    dir[0].m_ino = i_no;
    dir[1].m_ino = i_no;
    /* 保存 */
    memcpy(buf, (char *)dir, sizeof(dir));
    this->os_BufferManager->WriteBuf(blk);
    
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

    /* 格式化时创建根目录 */
    CreateRootDir();

    return;
}

/* 找到相应编号的DiskINode，读到内存中 */
MemINode *FileSystem::ReadDiskINode(int i_no)
{
    MemINode *m_inode;
    /* 从磁盘读取对应的DiskINode */
    DiskINode d_inode;
    this->os_DeviceManager->GetBlockDevice(DeviceManager::ROOTDEV).read((char *)&d_inode, FileSystem::INODE_SIZE, FileSystem::INODE_START_SECTOR * BufferManager::BUFFER_SIZE + FileSystem::INODE_SIZE * i_no);

    /* DiskINode未被分配 */
    if(!(d_inode.d_mode & ALLOCED))
    {
        //TODO:
        cout << "ERROR CODE 003" << endl;
        return NULL;
    }

    /* 未加载到内存中 */
    if (-1 == this->os_INodeTable->IsLoaded(DeviceManager::ROOTDEV, i_no))
    {
        m_inode = this->os_INodeTable->GetMemINode();
        m_inode->FromDiskINode(d_inode);
        m_inode->i_number = i_no;
        return m_inode;
    }
    /* 已经加载到内存中 */
    else
    {
        m_inode = this->os_INodeTable->GetLoadedINode(i_no);
        return m_inode;
    }
}

/* 将MemINode写回到外存相应位置 */
void FileSystem::WriteDiskINode(MemINode *i_ptr)
{
    i_ptr->i_mode &= ~UPDATE;
    int i_no = i_ptr->i_number;
    DiskINode d_inode = i_ptr->ToDiskINode();
    this->os_DeviceManager->GetBlockDevice(DeviceManager::ROOTDEV).write((char *)&d_inode, FileSystem::INODE_SIZE, FileSystem::INODE_START_SECTOR * BufferManager::BUFFER_SIZE + FileSystem::INODE_SIZE * i_no);
    return;
}

