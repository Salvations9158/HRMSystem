/*
담당자: 안도혁
최근 업데이트: 2025.11.30 / 03:00
진행상태:
1-1 개인 정보 조회 기능 구현 완료
1-2 직원 조회 기능 구현 완료
1-0 이전 메뉴로 기능 구현 완료
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

// 직원 조회 화면 박스 좌표
#define LIST_BOX_X 1
#define LIST_BOX_Y 5
#define LIST_BOX_W 78
#define LIST_BOX_H 18 
#define SEARCH_Y 7
#define LIST_START_Y (LIST_BOX_Y + 7)

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
draw_button(MENU_X, BTN_Y_2, "2. 직원 조회", 0); \
draw_button(MENU_X, BTN_Y_0, "0. 이전 메뉴로", 0); 



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
    set_color(0, 15);
    gotoxy(0, 0);
    for (int i = 0; i < 80; i++) printf(" ");
    gotoxy(2, 0);
    printf("컴공상사 HR System | 사용자: %s %s (%s)", user->name, user->position, user->department);
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
// 1-2. 직원 조회 기능 (이름/부서/직급 검색)
// ------------------------------------------------------------------
void list_employees(User* user)
{
    char search_term[50];
    // *수정*: LIST_START_Y 상수를 사용
    int line_y = LIST_START_Y;
    int result_count = 0;

    // *수정*: 함수 진입 시 전체 화면 지우기
    system("cls");

    // UI 박스 그리기
    set_color(0, 15);
    gotoxy(0, 0);
    for (int i = 0; i < 80; i++) printf(" ");
    gotoxy(2, 0);
    printf("컴공상사 HR System | 사용자: %s %s (%s)", user->name, user->position, user->department);
    draw_box(LIST_BOX_X, LIST_BOX_Y, LIST_BOX_W, LIST_BOX_H, "1-2. 직원 조회 (이름/부서/직급 검색)");

    // 검색 안내 메시지
    gotoxy(LIST_BOX_X + 2, SEARCH_Y);
    set_color(14, 0); // 노란색
    printf("검색어 입력 (이름/부서/직급)          # '.'(온점) 입력 시 이전 메뉴로 이동");

    // 헤더 출력
    set_color(15, 0);
    gotoxy(LIST_BOX_X + 2, line_y - 2);
    printf("--------------------------------------------------------------------------");
    set_color(10, 0); // 연한 녹색
    gotoxy(LIST_BOX_X + 5, line_y - 1);  printf("이름");
    gotoxy(LIST_BOX_X + 20, line_y - 1); printf("부서");
    gotoxy(LIST_BOX_X + 35, line_y - 1); printf("직급");
    gotoxy(LIST_BOX_X + 50, line_y - 1); printf("연락처");
    set_color(15, 0);
    gotoxy(LIST_BOX_X + 2, line_y - 0);
    printf("--------------------------------------------------------------------------");

    while (1)
    {
        // 1. 검색어 입력 받기
        gotoxy(LIST_BOX_X + 7, SEARCH_Y + 2);
        printf("검색어:                                        "); // 기존 내용 지우기

        gotoxy(LIST_BOX_X + 15, SEARCH_Y + 2);
        set_color(14, 0); // 노란색 입력 필드

        // 커서 보이기 설정
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_CURSOR_INFO cursorInfo = { 100, TRUE };
        SetConsoleCursorInfo(hConsole, &cursorInfo);

        scanf_s("%s", search_term, (unsigned)sizeof(search_term));
        while (getchar() != '\n'); // 엔터 버퍼 제거

        // 커서 숨기기 설정
        cursorInfo.bVisible = FALSE;
        SetConsoleCursorInfo(hConsole, &cursorInfo);
        set_color(15, 0);

        // 2. 종료 조건 확인
        if (strcmp(search_term, ".") == 0)
        {
            // 종료 메시지 출력 후 복귀
            gotoxy(LIST_BOX_X + 2, LIST_BOX_Y + LIST_BOX_H + 1);
            printf("                                                                         ");
            gotoxy(LIST_BOX_X + 2, LIST_BOX_Y + LIST_BOX_H + 1);
            set_color(11, 0);
            printf(">> 이전 메뉴로 돌아갑니다.");
            Sleep(1000);
            return;
        }

        // 3. 이전 검색 결과 지우기
        // 이전 결과의 최대 라인만큼 반복하여 목록 영역을 공백으로 덮어씀
        for (int i = 0; i < (LIST_BOX_H - 8); i++) // LIST_BOX_H - 8: 목록이 출력되는 실제 라인 수
        {
            gotoxy(LIST_BOX_X + 2, LIST_START_Y + 1 + i); // LIST_START_Y + 1 부터 시작
            printf("                                                                        ");
        }
        result_count = 0;

        // 4. 파일 검색 및 결과 출력
        FILE* fp = NULL;
        User temp;

        if (fopen_s(&fp, "User.txt", "r") == 0 && fp != NULL)
        {
            int current_y = LIST_START_Y + 1; // 목록은 LIST_START_Y 다음 줄부터 시작

            while (fscanf_s(fp, "%s %s %s %s %s %s %s %d",
                temp.id, (unsigned)sizeof(temp.id),
                temp.pw, (unsigned)sizeof(temp.pw),
                temp.name, (unsigned)sizeof(temp.name),
                temp.department, (unsigned)sizeof(temp.department),
                temp.position, (unsigned)sizeof(temp.position),
                temp.phone, (unsigned)sizeof(temp.phone),
                temp.hire_date, (unsigned)sizeof(temp.hire_date),
                &temp.salary) != EOF)
            {
                // strstr을 사용하여 부분 문자열 일치 검사
                if (strstr(temp.name, search_term) != NULL ||
                    strstr(temp.department, search_term) != NULL ||
                    strstr(temp.position, search_term) != NULL)
                {
                    // 목록 출력 줄이 박스 범위를 넘어가지 않도록 방지
                    if (current_y >= LIST_BOX_Y + LIST_BOX_H - 1) {
                        break;
                    }

                    // 이름, 부서, 직책, 연락처 출력
                    set_color(15, 0);
                    gotoxy(LIST_BOX_X + 5, current_y);  printf("%-10s", temp.name);
                    gotoxy(LIST_BOX_X + 20, current_y); printf("%-10s", temp.department);
                    gotoxy(LIST_BOX_X + 35, current_y); printf("%-10s", temp.position);
                    gotoxy(LIST_BOX_X + 50, current_y); printf("%s", temp.phone);

                    current_y++;
                    result_count++;
                }
            }
            fclose(fp); //파일 닫기
        }

        // 5. 검색 결과 메시지 출력
        gotoxy(LIST_BOX_X + 2, LIST_BOX_Y + LIST_BOX_H + 1);
        printf("                                                                         "); // 기존 메시지 지우기
        gotoxy(LIST_BOX_X + 2, LIST_BOX_Y + LIST_BOX_H + 1);
        set_color(11, 0);
        printf(">> [알림] 총 %d건의 검색 결과를 찾았습니다. 새로운 검색어를 입력하세요.", result_count);
        set_color(15, 0);
    }
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

                Sleep(500);
                show_personal_info(user);

                DRAW_SUB_MENU_UI(); //UI 재출력
            }
            // 2. 직원 조회
            else if (my == BTN_Y_2 && mx >= MENU_X && mx <= MENU_X + BTN_WIDTH)
            {
                gotoxy(MENU_X, BOX_Y + 12);
                set_color(11, 0);
                printf(">> 2. 직원 조회                    ");
                set_color(15, 0);

                Sleep(500);
                list_employees(user);

                DRAW_SUB_MENU_UI(); //UI 재출력
            }
            // 0. 이전 메뉴로
            else if (my == BTN_Y_0 && mx >= MENU_X && mx <= MENU_X + BTN_WIDTH)
            {

                gotoxy(MENU_X, BOX_Y + 12);
                set_color(11, 0);
                printf(">> 0. 이전 메뉴로                   ");
                set_color(15, 0);

                Sleep(500); // 0.5초 대기 후 복귀
                return; //메인화면 UI 재출력
            }
        }
    }
}