#include <iostream>
#include "UserInterface.h"
#include "BufferManager.h"

int main()
{
    BufferManager os_BufferManager;
    // cout << "Initialized buffer manager..." << endl;
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

    string user_name = "root";
    UserInterface UI;
    // UI.GetCmd(user_name);

    
    return 0;
}