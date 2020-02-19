#ifndef INODE_TABLE_H
#define INODE_TABLE_H

#include "INode.h"
#include "FileSystem.h"

/* 
 * 内存Inode表(class InodeTable)
 * 负责内存Inode的分配和释放。
 */
class InodeTable
{
	/* 静态常属性 */
public:
	static const int NINODE	= 100;	/* 内存Inode的数量 */
	
	/* 方法 */
public:
	/* 构造函数 */
	InodeTable();
	/* 析构函数 */
	~InodeTable();
	
	void Initialize();                      /* 初始化对g_FileSystem对象的引用 */
	Inode* IGet(short dev, int inumber);    /* 根据指定设备号dev，外存Inode编号获取对应
	                                         * Inode。如果该Inode已经在内存中，对其上锁并返回该内存Inode，
	                                         * 如果不在内存中，则将其读入内存后上锁并返回该内存Inode */
	void IPut(Inode* pNode);                /* 减少该内存Inode的引用计数，如果此Inode已经没有目录项指向它，
	                                         * 且无进程引用该Inode，则释放此文件占用的磁盘块。 */
	void UpdateInodeTable();                /* 将所有被修改过的内存Inode更新到对应外存Inode中 */
	int IsLoaded(short dev, int inumber);   /* 检查设备dev上编号为inumber的外存inode是否有内存拷贝，
	                                         * 如果有则返回该内存Inode在内存Inode表中的索引 */
	Inode* GetFreeInode();                  /* 在内存Inode表中寻找一个空闲的内存Inode */
	
	/* 属性 */
public:
	Inode m_Inode[NINODE];		/* 内存Inode数组，每个打开文件都会占用一个内存Inode */
	FileSystem* m_FileSystem;	/* 对全局对象g_FileSystem的引用 */
};

#endif