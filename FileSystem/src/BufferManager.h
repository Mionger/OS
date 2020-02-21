#ifndef BUFFER_MANAGER_H
#define BUFFER_MANAGER_H

#include "Buf.h"
#include "DeviceManager.h"

class BufferManager
{
public:
	static const int NBUF = 15;			/* 缓存控制块、缓冲区的数量 */
	static const int BUFFER_SIZE = 512;	/* 缓冲区大小。 以字节为单位 */

	Buf bFreeList;								/* 自由缓存队列控制块 */
	unsigned char Buffer[NBUF][BUFFER_SIZE];	/* 缓冲区数组 */

	BufferManager();
	~BufferManager();
	
	//待测
	void SetDevMngr(DeviceManager *DevMngr);	/* 设置设备管理器 */
	Buf *GetBuf(short dev, int blkno);  		/* 为ß指定盘块号的块申请一个高速缓存 */
	
	void FreeBuf(Buf *buf);						/* 释放指定缓存控制块 */
	Buf *ReadBuf(int blkno);					/* 读磁盘的指定盘块到缓存块 */
	void WriteBuf(Buf *buf);					/* 将一个缓存块的内容写入磁盘对应盘块 */
	void DelWriteBuf(Buf *buf);					/* 将一个缓存块的内容延迟写入磁盘对应盘块 */
	void FlushBuf(Buf *buf);					/* 将延迟写的缓存块的内容写入对应的磁盘盘块 */
	void ClearBuf(Buf *buf);					/* 清除指定的缓存块的全部内容 */

private:
	
	Buf m_Buf[NBUF]; 							/* 缓存控制块数组 */
	DeviceManager *m_DeviceManager;				/* 指向设备驱动器 */

	void Initialize();				/* 缓存控制块队列的初始化。将缓存控制块中b_addr指向相应缓冲区首地址。 */

	void BufGetFree(Buf *buf);		/* 从自由队列获得一个缓存块 */
	void BufInsertFree(Buf *buf);   /* 将一个缓存块插入到自由队列队尾 */	
};

#endif
