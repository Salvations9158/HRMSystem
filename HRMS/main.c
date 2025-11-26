/*
담당자: 전민규
최근 업데이트: 2025.11.24 / 13:23
진행상태:
1-0 로그인 기능 루프 구현 완료
*/
#include "common.h"

int main() 
{
    User current_user; //빈그릇 만들어두기

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