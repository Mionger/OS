#include <iostream>
#include "UserInterface.h"
#include "DeviceManager.h"
#include "BufferManager.h"

int main()
{
    DeviceManager os_DeviceManager;
    cout << os_DeviceManager.GetBlockDevice(0).GetName()<<" mounted..." << endl;

    BufferManager os_BufferManager;
    cout << "Initialize buffer manager secceed!" << endl;
    os_BufferManager.SetDevMngr(&os_DeviceManager);
    // Buf *buf_ptr = os_BufferManager.bFreeList.av_back;
    // while (buf_ptr != &(os_BufferManager.bFreeList))
    // {
    //     cout << char(buf_ptr->b_addr[0]) << endl;
    //     buf_ptr = buf_ptr->av_back;
    // }

    // Buf *temp = os_BufferManager.bFreeList.av_back;
    // os_BufferManager.BufGetFree(temp);

    // buf_ptr = os_BufferManager.bFreeList.av_back;
    // while (buf_ptr != &(os_BufferManager.bFreeList))
    // {
    //     cout << char(buf_ptr->b_addr[0]) << endl;
    //     buf_ptr = buf_ptr->av_back;
    // }

    // os_BufferManager.BufInsertFree(temp);
    // buf_ptr = os_BufferManager.bFreeList.av_back;
    // while (buf_ptr != &(os_BufferManager.bFreeList))
    // {
    //     cout << char(buf_ptr->b_addr[0]) << endl;
    //     buf_ptr = buf_ptr->av_back;
    // }

    // os_DeviceManager.GetBlockDevice(0).write((char *)(os_BufferManager.Buffer[0]), os_BufferManager.BUFFER_SIZE, 0);
    // os_DeviceManager.GetBlockDevice(0).read((char *)(os_BufferManager.Buffer[14]), os_BufferManager.BUFFER_SIZE, 0);
    // buf_ptr = os_BufferManager.bFreeList.av_back;
    // while (buf_ptr != &(os_BufferManager.bFreeList))
    // {
    //     cout << char(buf_ptr->b_addr[0]) << endl;
    //     buf_ptr = buf_ptr->av_back;
    // }

    string user_name = "root";
    UserInterface UI;
    // UI.GetCmd(user_name);

    
    return 0;
}