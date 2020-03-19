#include "File.h"
#include <iostream>
using namespace std;

extern BufferManager           os_BufferManager;
extern SuperBlockManager       os_SuperBlockManager;

File::File(MemINode *m_inode)
{
    this->f_BufferManager = &os_BufferManager;
    this->f_SuperBlockManager = &os_SuperBlockManager;

    this->f_type = 0;
    if(m_inode->i_mode & MemINode::IFDIR)
    {
        this->f_type |= File::FDIR;
    }
    this->f_inode = m_inode;
    this->f_offset = 0;
}

File::~File()
{

}

/* 获取文件大小 */
int File::GetFileSize()
{
    return this->f_inode->i_size;
}

/* 获取文件使用的盘块数 */
int File::GetBlkNum()
{
    int blk_num = this->f_inode->i_size / BUFFER_SIZE;
    if(this->f_inode->i_size % BUFFER_SIZE)
    {
        blk_num++;
    }
    return blk_num;
}

/* 获取最后一个盘块使用量 */
int File::GetBlkRest()
{
    return this->f_inode->i_size % BUFFER_SIZE;
}

/* 三级索引，逻辑盘块号到物理盘块号映射 */
int File::TriIndex(int l_blkno)
{
    int p_blkno;
    
    /* 逻辑盘块号不存在 */
    if (l_blkno < 0 || l_blkno > GetBlkNum())
    {
        cout << "ERROR CODE 0005" << endl;
        return -1;
    }

    /* 小型文件直接索引 */
    if (l_blkno < SMALL_FILE_MAX)
    {
        p_blkno = this->f_inode->i_addr[l_blkno];
    }
    /* 大型文件一级间接索引 */
    else if (l_blkno < LARGE_FILE_MAX)
    {
        Buf *index_buf;

        if (l_blkno + 1 - 6 > 128)
            index_buf = this->f_BufferManager->ReadBuf(this->f_inode->i_addr[7]);
        else
            index_buf = this->f_BufferManager->ReadBuf(this->f_inode->i_addr[6]);

        p_blkno = ((int *)index_buf->b_addr)[(l_blkno - 6) % 128];
        this->f_BufferManager->FreeBuf(index_buf);
    }
    /* 巨型文件二级间接索引 */
    else if (l_blkno < HUGE_FILE_MAX)
    {
        Buf *index_buf_1;
        Buf *index_buf_2;

        if (l_blkno + 1 - 6 - 2 * 128 > 128 * 128)
            index_buf_1 = this->f_BufferManager->ReadBuf(this->f_inode->i_addr[9]);
        else
            index_buf_1 = this->f_BufferManager->ReadBuf(this->f_inode->i_addr[8]);

        int index_blkno = ((int *)index_buf_1->b_addr)[(l_blkno - 6 - 128 * 2) / 128 % 128];
        index_buf_2 = this->f_BufferManager->ReadBuf(index_blkno);
        p_blkno = ((int *)index_buf_2->b_addr)[(l_blkno - 6 - 128 * 2) % 128];
        this->f_BufferManager->FreeBuf(index_buf_1);
        this->f_BufferManager->FreeBuf(index_buf_2);
    }
    else
    {
        cout << "ERROR CODE 006" << endl;
        return -1;
    }

    return p_blkno;
}

/* 为文件申请新的磁盘盘块 */
Buf *File::AllocNewBlk()
{
    /* 文件原来占有的盘块数 */
    int pre_blkno = GetBlkNum();
    if (pre_blkno > HUGE_FILE_MAX)
    {
        cout << "ERROR CODE 007" << endl;
        return NULL;
    }
    
    /* 需要更新外存inode */
    this->f_inode->i_mode |= MemINode::IUPD;
    /* 新分配盘块 */
    Buf *new_buf = this->f_SuperBlockManager->AllocBlk();

    /* 判断索引情况 */
    if (pre_blkno < SMALL_FILE_MAX)
    {
        this->f_inode->i_addr[pre_blkno] = new_buf->b_blkno;
    }
    else if (pre_blkno < LARGE_FILE_MAX)
    {
        /* 未创建一级索引 */
        if (1 == (pre_blkno + 1 - 6) % 128)
        {
            Buf *index_blk = this->f_SuperBlockManager->AllocBlk();
            
            if (pre_blkno + 1 - 6 > 128)
                this->f_inode->i_addr[7] = index_blk->b_blkno;
            else
                this->f_inode->i_addr[6] = index_blk->b_blkno;

            this->f_BufferManager->ClearBuf(index_blk);
            this->f_BufferManager->FreeBuf(index_blk);
        }

        /* 读取索引块 */
        Buf *index_blk;
        if (pre_blkno + 1 - 6 > 128)
            index_blk = this->f_BufferManager->ReadBuf(this->f_inode->i_addr[7]);
        else
            index_blk = this->f_BufferManager->ReadBuf(this->f_inode->i_addr[6]);

        ((int *)(index_blk->b_addr))[(pre_blkno - 6) % 128] = new_buf->b_blkno;
        this->f_BufferManager->DelWriteBuf(index_blk);
    }
    else if (pre_blkno < HUGE_FILE_MAX)
    {
        /* 未创建一级索引 */
        if (1 == (pre_blkno + 1 - 6 - 2 * 128) % (128 * 128))
        {
            Buf *index_blk_1 = this->f_SuperBlockManager->AllocBlk();
            
            if (pre_blkno + 1 - 6 - 2 * 128 > 128 * 128)
                this->f_inode->i_addr[9] = index_blk_1->b_blkno;
            else
                this->f_inode->i_addr[8] = index_blk_1->b_blkno;

            this->f_BufferManager->ClearBuf(index_blk_1);
            this->f_BufferManager->FreeBuf(index_blk_1);
        }

        /* 未创建二级索引 */
        if (1 == (pre_blkno + 1 - 6 - 2 * 128) % 128)
        {
            Buf *index_blk_2 = this->f_SuperBlockManager->AllocBlk();

            Buf *index_blk_1;
            if (pre_blkno + 1 - 6 - 2 * 128 > 128 * 128)
                index_blk_1 = this->f_BufferManager->ReadBuf(this->f_inode->i_addr[9]);
            else
                index_blk_1 = this->f_BufferManager->ReadBuf(this->f_inode->i_addr[8]);

            ((int *)(index_blk_1->b_addr))[(pre_blkno - 6 - 2 * 128) / 128 % 128] = index_blk_2->b_blkno;
            this->f_BufferManager->DelWriteBuf(index_blk_1);
            this->f_BufferManager->ClearBuf(index_blk_2);
            this->f_BufferManager->FreeBuf(index_blk_2);
        }

        Buf *index_blk_1;
        Buf *index_blk_2;
        if (pre_blkno + 1 - 6 - 2 * 128 > 128 * 128)
            index_blk_1 = this->f_BufferManager->ReadBuf(this->f_inode->i_addr[9]);
        else
            index_blk_1 = this->f_BufferManager->ReadBuf(this->f_inode->i_addr[8]);

        index_blk_2 = this->f_BufferManager->ReadBuf(((int *)(index_blk_1->b_addr))[(pre_blkno - 6 - 2 * 128) / 128 % 128]);
        ((int *)(index_blk_2->b_addr))[(pre_blkno - 6 - 2 * 128) % 128] = new_buf->b_blkno;

        this->f_BufferManager->FreeBuf(index_blk_1);
        this->f_BufferManager->DelWriteBuf(index_blk_2);
    }
    else
    {
        cout << "ERROR CODE 008" << endl;
        return NULL;
    }

    return new_buf;
}

/* 根据f_offset和len读取内容 */
int File::Read(char *content, int len)
{
    /* 文件剩余字节 */
    int rest_byte_size = max(GetFileSize() - this->f_offset, 0);
    /* 目标待读取字节 */
    int to_read_byte_size = min(rest_byte_size, len);
    /* 已经读过的字节计数器 */
    int read_byte_count = 0;
    /* 已经读过的盘块数 */
    int has_read_blk_num = this->f_offset / BUFFER_SIZE;
    /* 当前盘块内剩余的字节数 */
    int blk_rest_byte_size = BUFFER_SIZE - (this->f_offset - has_read_blk_num * BUFFER_SIZE);

    while (read_byte_count < to_read_byte_size)
    {
        /* 当前盘块读完 */
        if (0 == blk_rest_byte_size)
        {
            blk_rest_byte_size = BUFFER_SIZE;
            has_read_blk_num++;
        }

        /* 剩余待读取字节 */
        int rest_to_read_byte_size = to_read_byte_size - read_byte_count;
        /* 本盘块剩下的内容够读 */
        if (blk_rest_byte_size >= rest_to_read_byte_size)
        {
            int blkno = TriIndex(has_read_blk_num);
            Buf *buf = this->f_BufferManager->ReadBuf(blkno);

            for (int i = 0; i < rest_to_read_byte_size; i++)
            {
                content[read_byte_count] = buf->b_addr[this->f_offset % BUFFER_SIZE + i];
                read_byte_count++;
            }

            this->f_offset += rest_to_read_byte_size;
            break;
        }
        /* 本盘块剩下的内容不够读 */
        else
        {
            int blkno = TriIndex(has_read_blk_num);
            Buf *buf = this->f_BufferManager->ReadBuf(blkno);

            for (int i = 0; i < blk_rest_byte_size; i++)
            {
                content[read_byte_count] = buf->b_addr[this->f_offset % BUFFER_SIZE + i];
                read_byte_count++;
            }

            this->f_offset += blk_rest_byte_size;
            blk_rest_byte_size = 0;
        }
    }

    return to_read_byte_size;
}

/* 根据f_offset和len写入内容 */
int File::Write(char *content, int len)
{
    /* 文件剩余字节 */
    int rest_byte_size = max(GetFileSize() - this->f_offset, 0);
    /* 目标待写入字节 */
    int to_write_byte_size = min(rest_byte_size, len);
    /* 需要新写入的大小 */
    int append_byte_size = (len - rest_byte_size) > 0 ? (len - rest_byte_size) : 0;
    /* 已经写过的字节计数器 */
    int write_byte_count = 0;
    /* 已经写过的盘块数 */
    int has_write_blk_num = this->f_offset / BUFFER_SIZE;
    /* 当前盘块内剩余的字节数 */
    int blk_rest_byte_size = BUFFER_SIZE - (this->f_offset - has_write_blk_num * BUFFER_SIZE);

    while (write_byte_count < to_write_byte_size)
    {
        /* 当前盘块读完 */
        if (0 == blk_rest_byte_size)
        {
            blk_rest_byte_size = BUFFER_SIZE;
            has_write_blk_num++;
        }

        /* 剩余待写入字节 */
        int rest_to_write_byte_size = to_write_byte_size - write_byte_count;
        /* 本盘块剩下的内容够写 */
        if (blk_rest_byte_size >= rest_to_write_byte_size)
        {
            int blkno = TriIndex(has_write_blk_num);
            Buf *buf = this->f_BufferManager->ReadBuf(blkno);

            for (int i = 0; i < rest_to_write_byte_size; i++)
            {
                buf->b_addr[this->f_offset % BUFFER_SIZE + i] = content[write_byte_count];
                write_byte_count++;
            }

            this->f_offset += rest_to_write_byte_size;
            this->f_BufferManager->DelWriteBuf(buf);
            break;
        }
        /* 本盘块剩下的内容不够写 */
        else
        {
            int blkno = TriIndex(has_write_blk_num);
            Buf *buf = this->f_BufferManager->ReadBuf(blkno);

            for (int i = 0; i < blk_rest_byte_size; i++)
            {
                buf->b_addr[this->f_offset % BUFFER_SIZE + i] = content[write_byte_count];
                write_byte_count++;
            }

            this->f_offset += blk_rest_byte_size;
            this->f_BufferManager->DelWriteBuf(buf);
            blk_rest_byte_size = 0;
        }
    }

    has_write_blk_num = this->f_offset / BUFFER_SIZE;
    blk_rest_byte_size = BUFFER_SIZE - (this->f_offset - has_write_blk_num * BUFFER_SIZE);

    /* 最后一个盘块需要写入的字节数 */
    int write_pre_byte_num = (append_byte_size > blk_rest_byte_size) ? blk_rest_byte_size : append_byte_size;
    /* 需要新增的盘块数 */
    int write_add_blk_num = (append_byte_size - write_pre_byte_num) / BUFFER_SIZE;
    /* 写完完整盘块仍然需要写入的字节数 */
    int write_last_byte_num = append_byte_size - write_pre_byte_num - BUFFER_SIZE * write_add_blk_num;

    int pre_blkno;
    Buf *pre_buf;
    /* 空文件 */
    if (0 == GetFileSize())
    {
        pre_buf = AllocNewBlk();
        pre_blkno = TriIndex(0);
    }
    else
    {
        pre_blkno = TriIndex(has_write_blk_num);
        pre_buf = this->f_BufferManager->ReadBuf(pre_blkno);
    }

    for (int i = 0; i < write_pre_byte_num; i++)
    {
        pre_buf->b_addr[this->f_offset % BUFFER_SIZE + i] = content[write_byte_count];
        write_byte_count++;
    }
    this->f_BufferManager->DelWriteBuf(pre_buf);
    this->f_inode->i_size += write_pre_byte_num;

    /* 写完整的新盘块 */
    for (int i = 0; i < write_add_blk_num; i++)
    {
        /* 每次分配一个新盘块 */
        Buf *buf = AllocNewBlk();
        for (int j = 0; j < BUFFER_SIZE;j++)
        {
            buf->b_addr[j] = content[write_byte_count];
            write_byte_count++;
        }
        this->f_BufferManager->DelWriteBuf(buf);
        this->f_inode->i_size += BUFFER_SIZE;
    }

    /* 最后一个盘块 */
    Buf *last_buf = AllocNewBlk();
    for (int i = 0; i < write_last_byte_num; i++)
    {
        last_buf->b_addr[i] = content[write_byte_count];
        write_byte_count++;
    }
    this->f_BufferManager->DelWriteBuf(last_buf);
    this->f_inode->i_size += write_last_byte_num;

    /* 待更新inode节点 */
    if(write_add_blk_num+write_last_byte_num!=0)
    {
        this->f_inode->i_mode |= MemINode::IUPD;
    }

    this->f_offset += write_pre_byte_num + write_add_blk_num * BUFFER_SIZE + write_last_byte_num;

    return write_byte_count;
}

/* 截断文件，保留若干字节 */
void File::CutOff(int size)
{
    /* 原文件的字节大小 */
    int old_type_size = GetFileSize();
    /* 原文件使用的盘块数量 */
    int old_blk_num = GetBlkNum();

    /* 截断后剩余的盘块数 */
    int new_blk_num = size / BUFFER_SIZE + (size % BUFFER_SIZE == 0 ? 0 : 1);
    /* 需要删除的盘块数量 */
    int remove_blk_num = old_blk_num - new_blk_num;

    int i, j, k;
    for (i = 0; i < remove_blk_num; i++)
    {
        if (0 != this->f_inode->i_addr[9] || 0 != this->f_inode->i_addr[8])
        {
            /* 读取一级索引块 */
            Buf *index_blk_1 = this->f_BufferManager->ReadBuf(0 != this->f_inode->i_addr[9] ? this->f_inode->i_addr[9] : this->f_inode->i_addr[8]);

            for (j = 127; j >= 0; j--)
            {
                if (0 != ((int *)index_blk_1->b_addr)[j])
                {
                    /* 读取二级索引块 */
                    Buf *index_blk_2 = this->f_BufferManager->ReadBuf(((int *)index_blk_1->b_addr)[j]);
                    for (k = 127; k >= 0; k--)
                    {
                        if (0 != ((int *)index_blk_2->b_addr)[k])
                        {
                            this->f_SuperBlockManager->FreeBlk(((int *)index_blk_2->b_addr)[k]);
                            ((int *)index_blk_2->b_addr)[k] = 0;
                            break;
                        }
                    }

                    if (0 == k)
                    {
                        this->f_SuperBlockManager->FreeBlk(((int *)index_blk_1->b_addr)[j]);
                        ((int *)index_blk_1->b_addr)[j] = 0;
                        this->f_BufferManager->DelWriteBuf(index_blk_2);
                        break;
                    }

                    this->f_BufferManager->FreeBuf(index_blk_2);
                    break;
                }   
            }

            if (0 == j)
            {
                this->f_SuperBlockManager->FreeBlk(0 != this->f_inode->i_addr[9] ? this->f_inode->i_addr[9] : this->f_inode->i_addr[8]);
                this->f_inode->i_mode |= MemINode::IUPD;
                if(0 != this->f_inode->i_addr[9])
                {
                    this->f_inode->i_addr[9] = 0;
                }
                else
                {
                    this->f_inode->i_addr[8] = 0;
                }
                this->f_BufferManager->DelWriteBuf(index_blk_1);
                continue;
            }

            this->f_BufferManager->FreeBuf(index_blk_1);
            continue;
        }

        if (0 != this->f_inode->i_addr[7] || 0 != this->f_inode->i_addr[6])
        {
            /* 读取一级索引块 */
            Buf *index_blk_1 = this->f_BufferManager->ReadBuf(0 != this->f_inode->i_addr[7] ? this->f_inode->i_addr[7] : this->f_inode->i_addr[6]);

            for (j = 127; j >= 0;i--)
            {
                if (0 != ((int *)index_blk_1->b_addr)[j])
                {
                    this->f_SuperBlockManager->FreeBlk(((int *)index_blk_1->b_addr)[j]);
                    ((int *)index_blk_1->b_addr)[j] = 0;
                    break;
                }
            }

            if (0 == j)
            {
                this->f_SuperBlockManager->FreeBlk(0 != this->f_inode->i_addr[7] ? this->f_inode->i_addr[7] : this->f_inode->i_addr[6]);
                this->f_inode->i_mode |= MemINode::IUPD;
                if(0 != this->f_inode->i_addr[7])
                {
                    this->f_inode->i_addr[7] = 0;
                }
                else
                {
                    this->f_inode->i_addr[6] = 0;
                }
                this->f_BufferManager->DelWriteBuf(index_blk_1);
                continue;
            }

            this->f_BufferManager->FreeBuf(index_blk_1);
            continue;
        }

        for (j = 5; j >= 0; j--)
        {
            if(0!=this->f_inode->i_addr[j])
            {
                this->f_SuperBlockManager->FreeBlk(this->f_inode->i_addr[j]);
                this->f_inode->i_mode |= MemINode::IUPD;
                this->f_inode->i_addr[j] = 0;
                break;
            }
        }
    }

    /* 调整文件结果的最终大小 */
    this->f_inode->i_size = size;

    return;
}

/* 根据f_offset和len删除内容 */
int File::Remove(char *content, int len)
{
    /* 文件剩余字节 */
    int rest_byte_size = max(GetFileSize() - this->f_offset, 0);
    /* 目标待删除字节 */
    int to_remove_byte_size = min(rest_byte_size, len);

    if (0 == to_remove_byte_size)
    {
        return 0;
    }

    /* 暂存文件读写位置指针 */
    int offset = this->f_offset;

    char *rest_saver = new char[rest_byte_size];
    Read(rest_saver, rest_byte_size);
    CutOff(GetFileSize() - rest_byte_size);
    if(rest_byte_size>to_remove_byte_size)
    {
        this->f_offset = offset;
        Write(&rest_saver[to_remove_byte_size], rest_byte_size - to_remove_byte_size);
    }

    if (NULL != content)
    {
        for (int i = 0; i < to_remove_byte_size;i++)
        {
            content[i] = rest_saver[i];
        }
    }
    delete[] rest_saver;

    this->f_offset = offset;
    this->f_inode->i_mode |= MemINode::IUPD;

    return to_remove_byte_size;
}

/* 删除文件 */
void File::Delete()
{;
    this->f_inode->i_mode |= MemINode::IUPD;
    int blk_num = GetBlkNum();

    int i, j, k;

    /* 释放使用的盘块 */
    for (i = 0; i < blk_num;i++)
    {
        this->f_SuperBlockManager->FreeBlk(TriIndex(i));
    }

    /* 清除索引节点 */
    if (0 != this->f_inode->i_addr[6])
    {
        Buf *index_blk_1 = this->f_BufferManager->ReadBuf(this->f_inode->i_addr[6]);
        for (j = 0; j < BUFFER_SIZE / sizeof(int); j++)
        {
            if (0 == ((int *)index_blk_1->b_addr)[j])
            {
                break;
            }
            this->f_SuperBlockManager->FreeBlk(((int *)index_blk_1->b_addr)[j]);
        }
        this->f_BufferManager->FreeBuf(index_blk_1);
        this->f_SuperBlockManager->FreeBlk(this->f_inode->i_addr[6]);
    }

    if (0 != this->f_inode->i_addr[7])
    {
        Buf *index_blk_1 = this->f_BufferManager->ReadBuf(this->f_inode->i_addr[7]);
        for (j = 0; j < BUFFER_SIZE / sizeof(int); j++)
        {
            if (0 == ((int *)index_blk_1->b_addr)[j])
            {
                break;
            }
            this->f_SuperBlockManager->FreeBlk(((int *)index_blk_1->b_addr)[j]);
        }
        this->f_BufferManager->FreeBuf(index_blk_1);
        this->f_SuperBlockManager->FreeBlk(this->f_inode->i_addr[7]);
    }

    if (0 != this->f_inode->i_addr[8])
    {
        Buf *index_blk_1 = this->f_BufferManager->ReadBuf(this->f_inode->i_addr[8]);
        for (j = 0; j < BUFFER_SIZE / sizeof(int); j++)
        {
            if (0 == ((int *)index_blk_1->b_addr)[j])
            {
                break;
            }

            Buf *index_blk_2 = this->f_BufferManager->ReadBuf(((int *)index_blk_1->b_addr)[j]);
            for (k = 0; k < BUFFER_SIZE / sizeof(int); k++)
            {
                if (0 == ((int *)index_blk_2->b_addr)[k])
                {
                    break;
                }
                this->f_SuperBlockManager->FreeBlk(((int *)index_blk_2->b_addr)[k]);
            }

            this->f_BufferManager->FreeBuf(index_blk_2);
            this->f_SuperBlockManager->FreeBlk(((int *)index_blk_1->b_addr)[j]);
        }
        this->f_BufferManager->FreeBuf(index_blk_1);
        this->f_SuperBlockManager->FreeBlk(this->f_inode->i_addr[8]);
    }

    if (0 != this->f_inode->i_addr[9])
    {
        Buf *index_blk_1 = this->f_BufferManager->ReadBuf(this->f_inode->i_addr[9]);
        for (j = 0; j < BUFFER_SIZE / sizeof(int); j++)
        {
            if (0 == ((int *)index_blk_1->b_addr)[j])
            {
                break;
            }

            Buf *index_blk_2 = this->f_BufferManager->ReadBuf(((int *)index_blk_1->b_addr)[j]);
            for (k = 0; k < BUFFER_SIZE / sizeof(int); k++)
            {
                if (0 == ((int *)index_blk_2->b_addr)[k])
                {
                    break;
                }
                this->f_SuperBlockManager->FreeBlk(((int *)index_blk_2->b_addr)[k]);
            }

            this->f_BufferManager->FreeBuf(index_blk_2);
            this->f_SuperBlockManager->FreeBlk(((int *)index_blk_1->b_addr)[j]);
        }
        this->f_BufferManager->FreeBuf(index_blk_1);
        this->f_SuperBlockManager->FreeBlk(this->f_inode->i_addr[9]);
    }

    /* 修改inode节点 */
    this->f_inode->i_size = 0;
    for (i = 0; i < 10; i++)
    {
        this->f_inode->i_addr[i] = 0;
    }
    return;
}
