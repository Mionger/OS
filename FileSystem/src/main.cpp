#include <iostream>
#include "UserInterface.h"

int main()
{
    string user_name = "root";
    UserInterface UI;
    UI.GetCmd(user_name);
    return 0;
}