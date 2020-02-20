#ifndef BUFFER_MANAGER_H
#define BUFFER_MANAGER_H

#include "Buf.h"

class BufferManager
{
public:
	static const int NBUF = 15;			/* 缓存控制块、缓冲区的数量 */
	static const int BUFFER_SIZE = 512;	/* 缓冲区大小。 以字节为单位 */

	
	
public:
	BufferManager();
	~BufferManager();

	Buf *GetBlk(int blkno);			/* 位指定盘块号的块申请一个高速缓存 */

private:
	Buf bFreeList;								/* 自由缓存队列控制块 */
	Buf m_Buf[NBUF];							/* 缓存控制块数组 */
	unsigned char Buffer[NBUF][BUFFER_SIZE];	/* 缓冲区数组 */

	void Initialize();				/* 缓存控制块队列的初始化。将缓存控制块中b_addr指向相应缓冲区首地址。 */

	void BufGetFree(Buf *buf);		/* 从自由队列获得一个缓存块 */
	void BufInsertFree(Buf *buf);   /* 将一个缓存块插入到自由队列队尾 */	
};

#endif
