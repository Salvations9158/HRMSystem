/*
담당자: 전민규
최근 업데이트: 2025.11.24 / 13:23
진행상태:
1-0 메인 메뉴 진입 로직 구현 완료
2 기능 메뉴 별 클릭 상호작용 구현 완료
2-1 로그아웃 기능 구현 완료
2-2 나머지 기능 미구현 (추후 작업 예정)
*/
#include "common.h"

// 메인 메뉴 화면 (기존과 동일)
void show_main_menu(User* user)
{
    int mx, my;
    int is_admin = (strcmp(user->position, "관리자") == 0 || strcmp(user->id, "admin") == 0);

    int box_x = 5, box_y = 3;
    int menu_x = 24;

    system("cls");

    set_color(0, 15);
    gotoxy(0, 0);
    for (int i = 0; i < 80; i++) printf(" ");
    gotoxy(2, 0);
    printf("컴공상사 HR System | 사용자: %s %s (%s)", user->name, user->position, user->department);
    set_color(15, 0);
    draw_box(box_x, box_y, 70, 18, is_admin ? "관리자 메인 메뉴" : "일반직원 메인 메뉴");

    if (is_admin)
    {
        draw_button(menu_x, 6, "1. 직원 데이터 관리", 0);
        draw_button(menu_x, 8, "2. 근태 관리       ", 0);
        draw_button(menu_x, 10, "3. 급여 관리       ", 0);
        draw_button(menu_x, 12, "4. 공지사항 관리   ", 0);
        draw_button(menu_x, 14, "5. 결재 관리       ", 0);
        draw_button(menu_x, 18, "0. 로그아웃        ", 0);
    }
    else
    {
        draw_button(menu_x, 6, "1. 내 정보 / 직원 조회", 0);
        draw_button(menu_x, 8, "2. 근태 관리          ", 0);
        draw_button(menu_x, 10, "3. 급여 / 평가        ", 0);
        draw_button(menu_x, 12, "4. 결재 처리          ", 0);
        draw_button(menu_x, 14, "5. 공지사항           ", 0);
        draw_button(menu_x, 18, "0. 로그아웃           ", 0);
    }

    //재설정 안해주면 클릭 이벤트 작동안함
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);

    DWORD mode;
    GetConsoleMode(hInput, &mode);
    SetConsoleMode(hInput, (mode & ~ENABLE_QUICK_EDIT_MODE) | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);

    while (1) {
        if (get_mouse_click(&mx, &my)) {
            // [0] 로그아웃 버튼 (Y: 18) - 버튼 크기 고려하여 범위(mx)를 넉넉하게 잡음
            if (my == 18 && mx >= menu_x && mx <= menu_x + 30) {
                gotoxy(menu_x, 16);
                set_color(14, 0); // 노란색
                printf(">> 로그아웃 중입니다...               ");
                set_color(15, 0);

                Sleep(1000); // 1초 대기 후 복귀
                return; // 함수가 종료 시 main의 루프에 의해 login_process가 다시 호출
            }

            // 메뉴 버튼 클릭 (Y: 6, 8, 10, 12, 14)
            if (mx >= menu_x && mx <= menu_x + 30) {

                // 안내 메시지 출력을 위한 좌표 설정
                gotoxy(menu_x, 16);
                set_color(11, 0); // 하늘색


                //주의!!!!관리자와 직원의 메뉴 순서가 다름
                switch (my) {
                case 6: // 관리자: 1. 직원 데이터 관리
                    if (is_admin)
                    {
                        printf(">> [관리자] 직원 데이터 관리 기능 실행   ");
                        //여기다가 작업
                    }
                    else // 직원: 1. 내 정보 / 직원 조회
                    {
                        printf(">> [직원] 내 정보 / 직원 조회 기능 실행  ");
                        Sleep(1000);
                        show_employee_info_menu(user); //작업자 안도혁
                    }
                    break;

                case 8: // 공통: 2. 근태 관리
                    printf(">> [공통] 근태 관리 기능 실행            ");
                    Sleep(700);
                    attendance_menu(user); // 근태 관리 연결
					system("cls");
                    break;

                case 10: // 관리자: 3. 급여 관리
                    if (is_admin)
                    {
                        printf(">> [관리자] 급여 관리 기능 실행          ");
                        //여기다가 작업
                    }
                    else // 직원: 3. 급여 / 급여 평가
                    {
                        printf(">> [직원] 급여 / 평가 기능 실행          ");
                        //여기다가 작업
                    }
                    break;

                case 12: // 관리자: 4. 공지사항 관리
                    if (is_admin)
                    {
                        printf(">> [관리자] 공지사항 관리 기능 실행      ");
                        //여기다가 작업
                    }
                    else // 직원: 4. 결재 처리
                    {
                        printf(">> [직원] 결재 처리 기능 실행            ");
                        //여기다가 작업
                    }
                    break;

                case 14: // 관리자: 5. 결재 관리
                    if (is_admin)
                    {
                        printf(">> [관리자] 결재 관리 기능 실행          ");
                        //여기다가 작업
                    }
                    else // 직원: 5. 공지사항
                    {
                        printf(">> [직원] 공지사항 조회 기능 실행        ");
                        //여기다가 작업
                    }
                    break;
                }
                set_color(15, 0); // 색상 복구
            }
        }
    }
}