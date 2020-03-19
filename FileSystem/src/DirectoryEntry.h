#ifndef DIRECTORY_ENTRY_H
#define DIRECTORY_ENTRY_H

struct DirectoryEntry 
{
public:
    char    f_name[28];     /* 包含在该目录中的文件名 */
    int     i_no;           /* 外存inode号 */
};

#endif