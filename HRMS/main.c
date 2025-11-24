#include "common.h"

int main() 
{
    User current_user; //ºó±×¸© ¸¸µé¾îµÎ±â

    init_console();
    create_dummy_data();

    while (1) 
    {
        if (login_process(&current_user)) 
        {
            show_main_menu(&current_user);
        }
    }

    return 0;
}