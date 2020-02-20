#include <iostream>
#include "UserInterface.h"
#include "BufferManager.h"

int main()
{
    BufferManager os_BufferManager;
    // cout << "Initialized buffer manager..." << endl;
    // Buf *buf_ptr = os_BufferManager.bFreeList.b_back;
    // while (buf_ptr != &(os_BufferManager.bFreeList))
    // {
    //     cout << char(buf_ptr->b_addr[0]) << endl;
    //     buf_ptr = buf_ptr->b_back;
    // }

    string user_name = "root";
    UserInterface UI;
    // UI.GetCmd(user_name);

    
    return 0;
}