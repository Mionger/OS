#include <iostream>
#include "UserInterface.h"

UserInterface::UserInterface()
{

}

UserInterface::~UserInterface()
{
    
}

void UserInterface::GetCmd(string user_name)
{
    while(true)
    {
        string cur_path = "/";
        cout << user_name<<" " << cur_path << " % ";

        string input = "";
        char get_c;
        while ((get_c = getchar()) != 10)
        {
            input += get_c;
        }

        if (0 == input.size())
        {
            continue;
        }
    }
}