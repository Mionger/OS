#include <iostream>
#include "UserInterface.h"
#include "FileSystem.h"

int main()
{
    /* 块设备读写测试 */
    // BlockDevice *os_BlockDevice = new BlockDevice("disk_a.img");
    // Buf temp;
    // temp.b_addr = new unsigned char[BufferManager::BUFFER_SIZE];
    // for (int j = 0; j < BufferManager::BUFFER_SIZE; j++)
    // {
    //     if (0 == j % 2)
    //         temp.b_addr[j] = 'a';
    //     else
    //         temp.b_addr[j] = 'b';
    // }
    // for (int j = 0; j < BufferManager::BUFFER_SIZE;j++)
    // {
    //     if (0 == j % 16)
    //         cout << endl;
    //     cout << temp.b_addr[j];
    // }
    // cout << endl;
    // os_BlockDevice->write(temp.b_addr, BufferManager::BUFFER_SIZE, 512);
    // delete temp.b_addr;

    /* 设置系统磁盘名称 */
    char d_name[10] = "a.img";

    /* 加载文件系统 */
    FileSystem *os_FileSystem = new FileSystem(d_name);
    
    /* 格式化文件系统 */
    os_FileSystem->FormatDisk();


    /*******************以下为缓存控制部分的读写测试 *********************/
    // Buf *buf_ptr;

    // /* 遍历输出初始化的自由队列 */
    // cout << "初始自由队列 : " << endl;
    // buf_ptr = os_BufferManager.GetBufFreeList().av_back;
    // Buf *freelist = buf_ptr->av_forw;
    // while (buf_ptr != freelist)
    // {
    //     cout << buf_ptr->b_addr[0] << endl;
    //     buf_ptr = buf_ptr->av_back;
    // }

    // buf_ptr = os_BufferManager.GetBufFreeList().av_back;
    // freelist = buf_ptr->av_forw;
    // while (buf_ptr != freelist)
    // {
    //     for (int j = 0; j < BufferManager::BUFFER_SIZE;j++)
    //     {
    //         cout << buf_ptr->b_addr[j];
    //     }
    //     cout << endl;
    //     buf_ptr = buf_ptr->av_back;
    // }   

    // /* 把缓存都移到磁盘的设备队列，并写入磁盘 */
    // cout << "Start to write all buffers to the disk..." << endl;
    // for (int i = 0; i <= BufferManager::NBUF; i++)
    // {
    //     buf_ptr = os_BufferManager.GetBuf(DeviceManager::ROOTDEV, i);
    //     cout << "Get the " << buf_ptr->b_blkno << " buffer done!" << endl;
    //     os_BufferManager.WriteBuf(buf_ptr);
    //     cout << "Write the " << buf_ptr->b_addr[0] << " buffer done!" << endl;
    //     os_BufferManager.FreeBuf(buf_ptr);
    // }

    return 0;
}