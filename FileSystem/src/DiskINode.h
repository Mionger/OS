#ifndef DISK_INODE_H
#define DISK_INODE_H

#define ALLOCED  	0x8000
#define FILE_DIR 	0x4000
#define UPDATE	 	0x2

class DiskINode
{
public:
	DiskINode();
	~DiskINode();

	unsigned int 	d_mode;			/* 状态的标志位，定义见enum INodeFlag */
	int				d_nlink;		/* 文件联结计数，即该文件在目录树中不同路径名的数量 */
	
	short			d_uid;			/* 文件所有者的用户标识数 */
	short			d_gid;			/* 文件所有者的组标识数 */
	
	int				d_size;			/* 文件大小，字节为单位 */
	int				d_addr[10];		/* 用于文件逻辑块好和物理块好转换的基本索引表 */
	
	int				d_atime;		/* 最后访问时间 */
	int				d_mtime;		/* 最后修改时间 */
};

#endif