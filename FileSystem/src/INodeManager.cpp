#include "INodeManager.h"


extern BlockDevice          os_BlockDevice;
extern BufferManager        os_BufferManager;
extern SuperBlockManager    os_SuperBlockManager;

INodeManager::INodeManager()
{
    this->i_BlockDevice = &os_BlockDevice;
    this->i_BufferManager = &os_BufferManager;
    this->i_SuperBlockManager = &os_SuperBlockManager;
}

INodeManager::~INodeManager()
{
    for (int i = 0; i < INodeManager::MEM_INODE_MAX; i++)
    {
        if (this->m_inode[i].i_mode & MemINode::IUPD)
        {
            WriteDiskINode(&this->m_inode[i]);
        }
    }
}

/* 分配新内存inode */
MemINode *INodeManager::AllocMemINode()
{
    for (int i = 0; i < INodeManager::MEM_INODE_MAX; i++)
    {
        if (!(this->m_inode[i].i_mode & MemINode::IALLOC))
        {
            this->m_inode[i].i_mode |= MemINode::IALLOC;
            this->m_inode[i].i_mode |= MemINode::IUPD;
            return &this->m_inode[i];
        }
    }

    cout << "ERROR CODE 004" << endl;
    return NULL;
}

/* 释放内存inode */
void INodeManager::FreeMemINode(MemINode *m_inode)
{
    /* 还有其他File使用这个内存inode */
    if (m_inode->i_count > 1)
    {
        m_inode->i_count--;
    }
    /* 没有其他File使用这个内存inode */
    else
    {
        if (m_inode->i_mode & MemINode::IUPD)
        {
            WriteDiskINode(m_inode);
        }
        m_inode->i_mode &= ~(MemINode::IALLOC);
        m_inode->i_number = 0;
        m_inode->i_count = 0;
    }

    return;
}

/* 判断外存inode1是否在内存 */
bool INodeManager::InMem(int d_no)
{
    for (int i = 0; i < INodeManager::MEM_INODE_MAX; i++)
    {
        if (d_no == this->m_inode[i].i_number)
        {
            return true;
        }
    }

    return false;
}

/* 获取已经读取到内存的外存inode */
MemINode *INodeManager::GetLoadedINode(int d_no)
{
    for (int i = 0; i < INodeManager::MEM_INODE_MAX;i++)
    {
        if(d_no == this->m_inode[i].i_number)
        {
            return &this->m_inode[i];
        }
    }

    return NULL;
}

/* 根据外存inode编号读取到内存 */
MemINode *INodeManager::ReadDiskINode(int d_no)
{
    MemINode *m_inode;

    /* 要找的的外存inode未分配 */
    if(!this->i_SuperBlockManager->IfAllocedDiskINode(d_no))
    {
        cout << "ERROR CODE 003" << endl;
        return NULL;
    }

    /* 已经读入到内存 */
    if (InMem(d_no))
    {
        m_inode = GetLoadedINode(d_no);
        m_inode->i_count++;
        return m_inode;
    }

    /* 从磁盘读取外存inode */
    DiskINode d_inode;
    this->i_BlockDevice->Bread((char *)&d_inode, sizeof(d_inode), sizeof(SuperBlock) + d_no * sizeof(DiskINode));
    
    /* 申请内存inode */
    m_inode = AllocMemINode();
    m_inode->i_number = d_no;
    m_inode->i_count = 1;
    m_inode->FromDiskINode(d_inode);

    return m_inode;
}

/* 将内存inode写回到外存 */
void INodeManager::WriteDiskINode(MemINode *m_inode)
{
    m_inode->i_mode &= ~MemINode::IUPD;
    int d_no = m_inode->i_number;
    DiskINode d_inode = m_inode->ToDiskINode();
    this->i_BlockDevice->Bwrite((char *)&d_inode, sizeof(DiskINode), sizeof(SuperBlock) + d_no * sizeof(DiskINode));
    return;
}