/*
담당자: 안도혁
최근 업데이트: 2025.11.25 / 17:25
진행상태: 
1-1 개인 정보 조회 기능 구현 완료
1-2 직원 조회 기능 미구현 (추후 작업 예정)
1-0 이전 메뉴로 기능 미구현 (추후 작업 예정)
*/

#include "common.h"

// ------------------------------------------------------------------
// 상수 정의
// ------------------------------------------------------------------
// UI 박스 및 버튼 좌표
#define BOX_X 5
#define BOX_Y 3
#define BOX_W 70
#define BOX_H 18
#define MENU_X 24

#define BTN_Y_1 6  // 1. 개인 정보 조회
#define BTN_Y_2 8  // 2. 직원 조회
#define BTN_Y_0 10 // 0. 이전 메뉴로
#define BTN_WIDTH 30 

// 개인 정보 조회 화면 박스 좌표
#define INFO_BOX_X 22
#define INFO_BOX_Y 5
#define INFO_BOX_W 50
#define INFO_BOX_H 15

// UI 출력 로직을 재사용하기 위해 매크로 블록으로 정의
#define DRAW_SUB_MENU_UI() \
system("cls"); \
set_color(0, 15); \
gotoxy(0, 0); \
for (int i = 0; i < 80; i++) printf(" "); \
gotoxy(2, 0); \
printf("컴공상사 HR System | 사용자: %s %s (%s)", user->name, user->position, user->department); \
set_color(15, 0); \
draw_box(BOX_X, BOX_Y, BOX_W, BOX_H, "1. 내 정보 / 직원 조회"); \
draw_button(MENU_X, BTN_Y_1, "1. 개인 정보 조회", 0); \
draw_button(MENU_X, BTN_Y_2, "2. 직원 조회 (미구현)", 0); \
draw_button(MENU_X, BTN_Y_0, "0. 이전 메뉴로 (미구현)", 0); 



// ------------------------------------------------------------------
// 1-1. 개인 정보 조회 기능 
// ------------------------------------------------------------------
void show_personal_info(User* user)
{
    int i;
    int label_x = INFO_BOX_X + 2;
    int data_x = INFO_BOX_X + 20;
    int start_y = INFO_BOX_Y + 2;

    // 출력할 라벨 및 사용자 데이터 배열 (User 구조체 기반)
    char* labels[] = { "사번 (ID)", "이름", "부서", "직급", "전화번호", "입사일" };
    char* data[] = {
        user->id,
        user->name,
        user->department,
        user->position,
        user->phone,
        user->hire_date
    };

    // 화면 초기화 및 정보 출력 박스 그리기
    system("cls");
    draw_box(INFO_BOX_X - 1, INFO_BOX_Y - 1, INFO_BOX_W + 2, INFO_BOX_H + 2, "1-1. 개인 정보 조회");

    // 사용자 데이터 출력
    set_color(15, 0);
    for (i = 0; i < 6; i++)
    {
        // 라벨 출력
        gotoxy(label_x, start_y + i * 2);
        set_color(10, 0); // 연한 녹색
        printf("%s:", labels[i]);

        // 데이터 출력
        gotoxy(data_x, start_y + i * 2);
        set_color(15, 0); // 흰색
        printf("%s", data[i]);
    }

    // 완료 메시지 출력 및 키 입력 대기
    gotoxy(INFO_BOX_X, INFO_BOX_Y + INFO_BOX_H + 3);
    set_color(11, 0);
    printf(">> [완료] 아무 키나 누르시면 이전 메뉴로 돌아갑니다.");

    // 키 입력 대기
    _getch();
}


// ------------------------------------------------------------------
// show_employee_info_menu: 1. 내 정보 / 직원 조회 UI 및 입력 처리
// ------------------------------------------------------------------
void show_employee_info_menu(User* user)
{
    int mx, my; // 마우스 좌표
    gotoxy(0, 0); // 커서 위치 초기화

    // 1. 초기 UI 출력
    DRAW_SUB_MENU_UI();

    // 2. 입력 루프
    while (1)
    {
        // 1. 마우스 입력 활성화 
        HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
        DWORD mode;
        GetConsoleMode(hInput, &mode);
        SetConsoleMode(hInput, (mode & ~ENABLE_QUICK_EDIT_MODE) | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);

        if (get_mouse_click(&mx, &my))
        {
            // 1. 개인 정보 조회 기능 활성화
            if (my == BTN_Y_1 && mx >= MENU_X && mx <= MENU_X + BTN_WIDTH)
            {
                gotoxy(MENU_X, BOX_Y + 12);
                set_color(11, 0);
                printf(">> 1. 개인 정보 조회                 ");
                set_color(15, 0);

                Sleep(1000);
                show_personal_info(user);

                DRAW_SUB_MENU_UI(); //UI 재출력
            }
            // 2. 직원 조회 (기능 미구현)
            else if (my == BTN_Y_2 && mx >= MENU_X && mx <= MENU_X + BTN_WIDTH)
            {
                gotoxy(MENU_X, BOX_Y + 12);
                set_color(11, 0);
                printf(">> 2. 직원 조회                    ");
                set_color(15, 0);
            }
            // 0. 이전 메뉴로 (기능 미구현)
            else if (my == BTN_Y_0 && mx >= MENU_X && mx <= MENU_X + BTN_WIDTH)
            {

                gotoxy(MENU_X, BOX_Y + 12);
                set_color(11, 0);
                printf(">> 0. 이전 메뉴로                   ");
                set_color(15, 0);
                /*
                Sleep(1000); // 1초 대기 후 복귀
                DRAW_MAIN_MENU_UI(); //메인화면 UI 재출력
                return;
                */
            }
        }
    }
}