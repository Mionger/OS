#include "FileManager.h"
#include "tool.h"

extern SuperBlockManager       os_SuperBlockManager;
extern BufferManager           os_BufferManager;
extern INodeManager            os_INodeManager;

/* 打开根目录 */
FileManager::FileManager()
{
    this->f_SuperBlockManager = &os_SuperBlockManager;
    this->f_BufferManager = &os_BufferManager;
    this->f_INodeManager = &os_INodeManager;
    int i_no = ROOT_I_NO;
    this->f_file = OpenFile(i_no);
}

/* 打开inode对应的文件 */
FileManager::FileManager(int i_no)
{
    this->f_SuperBlockManager = &os_SuperBlockManager;
    this->f_BufferManager = &os_BufferManager;
    this->f_INodeManager = &os_INodeManager;
    this->f_file = OpenFile(i_no);
}

FileManager::~FileManager()
{
    MemINode *i_node = this->f_file->f_inode;
    delete this->f_file;
}

/* 根据内存inode序号打开文件 */
File *FileManager::OpenFile(int i_no)
{
    MemINode *inode = this->f_INodeManager->ReadDiskINode(i_no);
    File *file = new File(inode);
    return file;
}

/* 判断该路径是否是文件夹 */
bool FileManager::IsFolder()
{
    return (this->f_file->f_type & File::FDIR) != 0;
}

/* 获取文件大小 */
int FileManager::GetFileSize()
{
    return this->f_file->GetFileSize();
}

/* 获取文件指针 */
int FileManager::GetOffset()
{
    return this->f_file->f_offset;
}

/* 更改文件读写指针 */
void FileManager::SetOfset(int n_offset)
{
    this->f_file->f_offset = n_offset;
}

/* 检查文件夹目录下是否有某文件 */
bool FileManager::HasItem(string f_name)
{
    char *read_buf = new char[GetFileSize()];
    this->f_file->f_offset = 0;
    this->f_file->Read(read_buf, GetFileSize());

    DirectoryEntry *items = (DirectoryEntry *)(read_buf);
    for (int i = 0; i < CountItem(); i++)
    {
        if (f_name == items[i].f_name)
        {
            delete[] read_buf;
            return true;
        }
    }

    delete[] read_buf;
    return false;
}

/* 统计文件夹下的内容的数量 */
int FileManager::CountItem()
{
    return GetFileSize() / sizeof(DirectoryEntry);
}

/* 根据文件名查询外存inode编号 */
int FileManager::GetDiskINodeNo(string f_name)
{
    char *read_buf = new char[GetFileSize()];
    this->f_file->f_offset = 0;
    this->f_file->Read(read_buf, GetFileSize());

    DirectoryEntry *items = (DirectoryEntry *)(read_buf);
    for (int i = 0; i < CountItem(); i++)
    {
        if (f_name == items[i].f_name)
        {
            return items[i].i_no;
        }
    }

    return -1;
}

/* 获取该路径下所有项目 */
FileMap *FileManager::LoadItem()
{
    char *read_buf = new char[GetFileSize()];
    this->f_file->f_offset = 0;
    this->f_file->Read(read_buf, GetFileSize());

    FileMap *fm = new FileMap;
    DirectoryEntry *items = (DirectoryEntry *)(read_buf);
    for (int i = 0; i < CountItem(); i++)
    {
        string f_name = items[i].f_name;
        (*fm)[f_name] = items[i].i_no;
    }

    return fm;
}

/* 根据相对地址获取对应文件inode编号 */
int FileManager::GetFileNo(string addr)
{
    vector<string> path;
    int i_no = this->f_file->f_inode->i_number;
    path = split(addr, "/");
    for (int i = 0; i < path.size(); i++)
    {
        FileManager fm(i_no);
        if (fm.IsFolder() && fm.HasItem(path[i]))
        {
            i_no = fm.GetDiskINodeNo(path[i]);
        }
        else
        {
            return -1;
        }
    }

    return i_no;
}

/* 在当前目录创建文件 */
int FileManager::CreateFile(string f_name, bool is_dir)
{
    if (f_name.size() > 27)
    {
        cout << "文件名过长" << endl;
    }

    if (HasItem(f_name))
    {
        cout << "文件名已存在" << endl;
    }

    MemINode *new_inode = this->f_INodeManager->AllocMemINode();
    if(is_dir)
    {
        /* 设置文件夹 */
        new_inode->i_mode |= MemINode::IFDIR;

        /* 新建相对路径相关 */
        DirectoryEntry dir[2];
        memcpy(dir[0].f_name, ".", 2);
        dir[0].i_no = new_inode->i_number;
        memcpy(dir[1].f_name, "..", 3);
        dir[1].i_no = this->f_file->f_inode->i_number;

        /* 设置文件信息 */
        File f(new_inode);
        f.f_offset = 0;
        f.Write((char *)dir, sizeof(dir));
    }
    int i_no = new_inode->i_number;
    this->f_INodeManager->FreeMemINode(new_inode);
    

    DirectoryEntry new_item;
    new_item.i_no = i_no;
    for (int i = 0; i <= f_name.size(); i++)
    {
        if(f_name.size()==i)
        {
            new_item.f_name[i] = '\0';
        }
        else
        {
            new_item.f_name[i] = f_name[i];
        }
    }

    this->f_file->f_offset = GetFileSize();
    this->Write((char *)&new_item, sizeof(DirectoryEntry));
    return i_no;
}

/* 根据文件名删除目录项 */
void FileManager::DeleteItem(string f_name)
{
    char *read_buf = new char[GetFileSize()];
    this->f_file->f_offset = 0;
    this->f_file->Read(read_buf, GetFileSize());

    int index;
    DirectoryEntry *items = (DirectoryEntry *)(read_buf);
    for (int i = 0; i < CountItem(); i++)
    {
        if (f_name == items[i].f_name)
        {
            index = i;
            break;
        }

        if (CountItem() - 1 == i)
        {
            cout << "未找到目录中的此文件" << endl;
            return;
        }
    }

    this->f_file->f_offset = sizeof(DirectoryEntry) * index;
    this->f_file->Remove(NULL, sizeof(DirectoryEntry));
    return;
}

/* 删除目录下普通文件 */
void FileManager::DeleteFile(string f_name)
{
    if (!HasItem(f_name))
    {
        cout << "当前目录下没有该文件" << endl;
        return;
    }

    int d_no = GetDiskINodeNo(f_name);
    MemINode *inode = this->f_INodeManager->ReadDiskINode(d_no);
    if (inode->i_mode & MemINode::IFDIR)
    {
        this->f_INodeManager->FreeMemINode(inode);
        cout << "待删除文件是文件夹，不是普通文件" << endl;
        return;
    }

    /* 删除目录项 */
    DeleteItem(f_name);
    
    /* 处理文件控制块 */
    File *f = new File(inode);
    f->Delete();
    delete f;

    if (inode->i_nlink > 1)
    {
        inode->i_nlink -= 1;
        inode->i_mode |= MemINode::IUPD;
        this->f_INodeManager->FreeMemINode(inode);
        return;
    }

    this->f_INodeManager->FreeMemINode(inode);
    this->f_SuperBlockManager->FreeDiskINode(d_no);
    return;
}

/* 删除目录下的目录 */
void FileManager::DeleteFolder(string f_name, bool force)
{
    int d_no = GetDiskINodeNo(f_name);
    MemINode *inode = this->f_INodeManager->ReadDiskINode(d_no);
    if (!(inode->i_mode & MemINode::IFDIR))
    {
        this->f_INodeManager->FreeMemINode(inode);
        cout << "待删除文件是普通文件，不是文件夹" << endl;
        return;
    }

    if (inode->i_nlink > 1)
    {
        inode->i_nlink -= 1;
        inode->i_mode |= MemINode::IUPD;
        this->f_INodeManager->FreeMemINode(inode);
        DeleteItem(f_name);
        return;
    }
    else
    {
        FileManager fm(d_no);
        FileMap *item_map = fm.LoadItem();

        /* 如果只包含相对路径相关的两项 */
        if (2 == item_map->size())
        {
            /* 释放外存inode */
            this->f_SuperBlockManager->FreeDiskINode(d_no);
            
            /* 释放内存inode */
            this->f_INodeManager->FreeMemINode(inode);
            delete item_map;
            DeleteItem(f_name);
            return;
        }
        else
        {
            /* 不强制删除 */
            if (!force)
            {
                cout << " 文件夹不为空，不能删除" << endl;
                /* 释放内存inode */
                this->f_INodeManager->FreeMemINode(inode);
                delete item_map;
                return;
            }

            FileMap::iterator it;
            for (it = item_map->begin(); it != item_map->end();it++)
            {
                const string subf_name = it -> first;
                const int subf_no = it -> second;

                if(subf_name == "." || subf_name == "..") 
                {
                    continue;
                }
                MemINode *subf_inode = this->f_INodeManager->ReadDiskINode(subf_no);
                /* 删除文件夹 */
                if (subf_inode->i_mode & MemINode::IFDIR)
                {
                    cout << "删除了文件夹 " << subf_name << endl;
                    fm.DeleteFolder(subf_name, true);
                }
                /* 删除文件 */ 
                else 
                {
                    cout << "删除了文件 " << subf_name << endl;
                    fm.DeleteFile(subf_name);
                }
            }
            /* 释放内存inode */
            this->f_INodeManager->FreeMemINode(inode);
            delete item_map;
            DeleteItem(f_name);
            return;
        }
    }
}

/* 重命名目录下的文件夹 */
void FileManager::RenameFile(string f_name, string n_name)
{
    if (!HasItem(f_name) )
    {
        cout << "文件不存在" << endl;
    }

    if(f_name.size()>27)
    {
        cout << "文件名过长" << endl;
    }

    char *read_buf = new char[GetFileSize()];
    this->f_file->f_offset = 0;
    this->f_file->Read(read_buf, GetFileSize());
    
    /* 查找对应文件的外存inode节点 */
    int index;
    DirectoryEntry *items = (DirectoryEntry *)(read_buf);
    for (int i = 0; i < CountItem(); i++)
    {
        /* 找到对应文件 */
        if (items->f_name == f_name)
        {
            index = items->i_no;
            break;
        }

        /* 没找到 */
        if (i == CountItem() - 1)
        {
            cout << "没有找到目录中的该文件" << endl;
            return;
        }
    }

    DirectoryEntry n_item;
    /* 调整读写指针 */
    this->f_file->f_offset = sizeof(DirectoryEntry) * index;
    /* 读取文件目录项 */
    this->f_file->Read((char *)&n_item, sizeof(DirectoryEntry));
    /* 调整文件目录项 */
    for (int i = 0; i <= f_name.size(); i++)
    {
        if (i == f_name.size())
        {
            n_item.f_name[i] = '\0';
        }
        else
        {
            n_item.f_name[i] = f_name[i];
        }
    }
    /* 调整读写指针 */
    this->f_file->f_offset = sizeof(DirectoryEntry) * index;
    /* 写会文件目录项 */
    this->f_file->Write((char *)&n_item, sizeof(DirectoryEntry));

    return;
}

/* 复制文件 */
void FileManager::CopyFile(string f_name, FileManager *pos)
{
    if (!HasItem(f_name))
    {
        cout << "目标文件不存在" << endl;
        return;
    }
    
    if (!pos->IsFolder())
    {
        cout << "目标地址不是文件夹" << endl;
        return;
    }

    int d_no = GetDiskINodeNo(f_name);
    DirectoryEntry n_item;
    n_item.i_no = d_no;
    for (int i = 0; i <= f_name.size(); i++)
    {
        if (i == f_name.size())
        {
            n_item.f_name[i] = '\0';
        }
        else
        {
            n_item.f_name[i] = f_name[i];
        }
    }

    pos->f_file->f_offset = pos->GetFileSize();
    pos->f_file->Write((char *)&n_item, sizeof(DirectoryEntry));

    MemINode *n_inode = this->f_INodeManager->ReadDiskINode(d_no);
    n_inode->i_nlink += 1;
    n_inode->i_mode |= MemINode::IUPD;
    this->f_INodeManager->FreeMemINode(n_inode);
    return;
}

/* 移动文件 */
void FileManager::MoveFile(string f_name, FileManager *pos)
{
    if (!HasItem(f_name))
    {
        cout << "目标文件不存在" << endl;
        return;
    }
    
    if (!pos->IsFolder())
    {
        cout << "目标地址不是文件夹" << endl;
        return;
    }

    int d_no = GetDiskINodeNo(f_name);
    DirectoryEntry n_item;
    n_item.i_no = d_no;
    for (int i = 0; i <= f_name.size(); i++)
    {
        if (i == f_name.size())
        {
            n_item.f_name[i] = '\0';
        }
        else
        {
            n_item.f_name[i] = f_name[i];
        }
    }

    pos->f_file->f_offset = pos->GetFileSize();
    pos->f_file->Write((char *)&n_item, sizeof(DirectoryEntry));

    DeleteItem(f_name);

    return;
}

/* 读取文件 */
int FileManager::Read(char* content, int len)
{
    return this->f_file->Read(content, len);
}

/* 写入文件 */
int FileManager::Write(char* content, int len)
{
    return this->f_file->Write(content, len);
}

/* 删除部分文件 */
int FileManager::Remove(char* content, int len)
{
    return this->f_file->Remove(content, len);
}