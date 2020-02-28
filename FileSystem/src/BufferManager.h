#ifndef BUFFER_MANAGER_H
#define BUFFER_MANAGER_H

#include "Buf.h"
#include "DeviceManager.h"

class BufferManager
{
public:
	static const int NBUF = 15;			/* 缓存控制块、缓冲区的数量 */
	static const int BUFFER_SIZE = 512;	/* 缓冲区大小。 以字节为单位 */

	BufferManager();
	~BufferManager();
	
	void SetDevMngr(DeviceManager *DevMngr);	/* 设置设备管理器 */
	Buf *GetBuf(short dev, int blkno);  		/* 为ß指定盘块号的块申请一个高速缓存 */
	void FreeBuf(Buf *buf);						/* 释放指定缓存控制块 */
	void ClearBuf(Buf *buf);					/* 清除指定的缓存块的全部内容 */
	void DelWriteBuf(Buf *buf);					/* 将一个缓存块的内容设置延迟写 */
	Buf *ReadBuf(int blkno);					/* 读磁盘的指定盘块到缓存块 */
	void WriteBuf(Buf *buf);					/* 将一个缓存块的内容写入磁盘对应盘块 */
	void FlushBuf(short dev);					/* 将dev指定的设备队列的延迟写的缓存块的内容写入对应的磁盘盘块 */

	Buf &GetBufFreeList();						/* 获取缓存的自由队列链表头 */
	char *GetBuf(int blkno);					/* 获取指定序号的缓存块的首地址 */

private:
	Buf bFreeList;								/* 自由缓存队列控制块 */
	Buf m_Buf[NBUF]; 							/* 缓存控制块数组 */
	char Buffer[NBUF][BUFFER_SIZE];	/* 缓冲区数组 */
	DeviceManager *m_DeviceManager;				/* 指向设备驱动器 */

	void Initialize();				/* 缓存控制块队列的初始化。将缓存控制块中b_addr指向相应缓冲区首地址。 */

	void BufGetFree(Buf *buf);		/* 从自由队列获得一个缓存块 */
	void BufInsertFree(Buf *buf);   /* 将一个缓存块插入到自由队列队尾 */	
};

#endif
