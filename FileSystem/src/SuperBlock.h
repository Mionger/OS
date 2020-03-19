#ifndef SUPER_BLOCK_H
#define SUPER_BLOCK_H

#define DISK_INODE_MAX      1024
#define DISK_INODE_BLOCK    128
#define DISK_BLOCK_MAX      65536

#include <iostream>
using namespace std;

class SuperBlock
{
public:
    SuperBlock();
    ~SuperBlock();

    void GetValue(SuperBlock *op);          /* 赋值 */

    int     s_isize;                        /* 外存inode数 */
    int     s_fsize;                        /* 盘块总数 */

    char    s_ibitmap[DISK_INODE_BLOCK];    /* SuperBlock通过位图管理外存inode */

    int     s_nfree;                        /* 直接管理的空闲盘块数量 */
    int     s_free[100];                    /* 直接管理的空闲盘块索引表 */

    int	    s_dirty;			            /* SuperBlock脏标记 */

    char    s_padding[480];                 /* 填充至1024字节（两个盘块大小） */
};

struct LinkGroup
{
    int s_nfree;                            /* 直接管理的空闲盘块数量 */
    int s_free[100];                        /* 直接管理的空闲盘块索引表 */
};

#endif