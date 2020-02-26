#ifndef SUPER_BLOCK_H
#define SUPER_BLOCK_H

class SuperBlock
{
public:
    int s_isize;        /* 外存Inode区占用的盘块数 */
    int s_fsize;        /* 盘块总数 */

    int s_ninode;       /* 直接管理的空闲外存Inode数量 */
    int s_inode[100];   /* 直接管理的空闲外存Inode索引表 */

    int s_nfree;        /* 直接管理的空闲盘块数量 */
    int s_free[100];    /* 直接管理的空闲盘块索引表 */

    int	s_fmod;			/* 内存中super block副本被修改标志，意味着需要更新外存对应的SuperBlock */
};

struct GroupLink
{
    int s_nfree;
    int s_free[100];
};

#endif