#ifndef DISK_INODE_H
#define DISK_INODE_H

class DiskINode
{
public:
    DiskINode();
    ~DiskINode();

	enum INodeFlag
	{
		ILOCK  = 0x1,		/* 索引节点上锁 */
		IUPD   = 0x2,		/* 内存inode被修改过，需要更新相应外存inode */
		IACC   = 0x4,		/* 内存inode被访问过，需要修改最近一次访问时间 */
		IMOUNT = 0x8,		/* 内存inode用于挂载子文件系统 */
		IWANT  = 0x10,		/* 有进程正在等待该内存inode被解锁，清ILOCK标志时，要唤醒这种进程 */
		ITEXT  = 0x20		/* 内存inode对应进程图像的正文段 */
	};
	
	static const unsigned int IALLOC = 0x8000;		/* 文件被使用 */
	static const unsigned int IFMT   = 0x6000;		/* 文件类型掩码 */
	static const unsigned int IFDIR  = 0x4000;		/* 文件类型：目录文件 */
	static const unsigned int IFCHR  = 0x2000;		/* 字符设备特殊类型文件 */
	static const unsigned int IFBLK  = 0x6000;		/* 块设备特殊类型文件，为0表示常规数据文件 */
	static const unsigned int ILARG  = 0x1000;		/* 文件长度类型：大型或巨型文件 */
	static const unsigned int ISUID  = 0x800;		/* 执行时文件时将用户的有效用户ID修改为文件所有者的User ID */
	static const unsigned int ISGID  = 0x400;		/* 执行时文件时将用户的有效组ID修改为文件所有者的Group ID */
	static const unsigned int ISVTX  = 0x200;		/* 使用后仍然位于交换区上的正文段 */
	static const unsigned int IREAD  = 0x100;		/* 对文件的读权限 */
	static const unsigned int IWRITE = 0x80;		/* 对文件的写权限 */
	static const unsigned int IEXEC  = 0x40;	    /* 对文件的执行权限 */
	static const unsigned int IRWXU  = (IREAD|IWRITE|IEXEC);    /* 文件主对文件的读、写、执行权限 */
	static const unsigned int IRWXG  = ((IRWXU) >> 3);			/* 文件主同组用户对文件的读、写、执行权限 */
	static const unsigned int IRWXO  = ((IRWXU) >> 6);	        /* 其他用户对文件的读、写、执行权限 */

    unsigned int    d_mode;         /* 状态的标志位 */
    int             d_nlink;        /* 文件在目录中不同路径的数量 */
    short           d_uid;          /* 文件所有者的用户标识数 */
    short           d_gid;          /* 文件所有者的组标识数 */
    int             d_size;         /* 文件大小，字节为单位 */
    int             d_addr[10];     /* 文件的盘块的混合索引表 */
    int             d_atime;        /* 最后访问时间 */
    int             d_mtime;        /* 最后修改时间 */
};

#endif