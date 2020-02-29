#include "INodeTable.h"

/* 是否加载到内存INode表中 */
int INodeTable::IsLoaded(short dev, int i_no)
{
    for (int i = 0; i < INodeTable::MEM_INODE_MAX;i++)
    {
        /* 加载到内存中必须要有文件加载，i_count不等于0 */
        if (dev == this->m_inode[i].i_dev && i_no == this->m_inode[i].i_number && 0 != this->m_inode[i].i_count)
        {
            return i;
        }
    }
    /* 没找到 */
    return -1;
}

/* 申请空闲的内存INode */
MemINode* INodeTable::GetMemINode()
{
    for (int i = 0; i < INodeTable::MEM_INODE_MAX; i++)
    {
        /* 引用计数为0表示空闲 */
        if (0 == this->m_inode[i].i_count)
        {
            return &(this->m_inode[i]);
        }
    }
    return NULL;
}

/* 根据DiskINode编号获得内存INode */
MemINode *INodeTable::GetLoadedINode(int i_no)
{
    for (int i = 0; i < INodeTable::MEM_INODE_MAX; i++)
    {
        if (i_no == this->m_inode[i].i_number)
        {
            return &(this->m_inode[i]);
        }
    }

    return NULL;
}