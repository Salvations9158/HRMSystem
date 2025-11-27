/*
담당자: 이은수
최근 업데이트: 2025.11.26 / 22:02
진행상태: 근태 관리 기능 구현 완료
*/

#define _CRT_SECURE_NO_WARNINGS
#include "common.h"

/*
=========================================================
            근태 관리 시스템 (TUI 기반)
=========================================================
- 출근 체크 / 퇴근 체크 / 내 근태 조회 / 전체 조회
- 관리자 여부에 따른 메뉴 표시 제어
- 파일 저장 및 시간 계산 포함
=========================================================
*/


// ------------------------------------------------------------------
// 상수 정의 (TUI UI 구성 요소 위치 및 크기)
// ------------------------------------------------------------------
#define BOX_X 5
#define BOX_Y 3
#define BOX_W 70
#define BOX_H 18
#define MENU_X 24

#define BTN_Y_1 6   // 출근 버튼 위치
#define BTN_Y_2 8   // 퇴근 버튼 위치
#define BTN_Y_3 10  // 내 근태 조회 버튼
#define BTN_Y_4 12  // 전체 근태 조회(관리자)
#define BTN_Y_0 16  // 이전 메뉴로
#define BTN_WIDTH 30


// ------------------------------------------------------------------
// 근태 데이터 저장 공간 (전역 변수)
// - 최대 500개까지 기록 저장 가능
// ------------------------------------------------------------------
Attendance attendance_list[500];
int attendance_count = 0;


// ------------------------------------------------------------------
// 현재 날짜 자동 생성: YYYY-MM-DD 형식 반환
// ------------------------------------------------------------------
void get_today(char* buffer) {
    time_t t = time(NULL);
    struct tm* tm = localtime(&t);
    sprintf(buffer, "%04d-%02d-%02d",
        tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday);
}


// ------------------------------------------------------------------
// 현재 시간 자동 생성: HH:MM:SS 형식 반환
// ------------------------------------------------------------------
void get_time_now(char* buffer) {
    time_t t = time(NULL);
    struct tm* tm = localtime(&t);
    sprintf(buffer, "%02d:%02d:%02d",
        tm->tm_hour, tm->tm_min, tm->tm_sec);
}


// ------------------------------------------------------------------
// 화면 하단 안내 메시지 표시 → 일정 시간 후 자동 삭제
// ------------------------------------------------------------------
void show_message(const char* msg, int x, int y, int color, int duration_ms) {
    gotoxy(x, y);
    set_color(color, 0);
    printf("%s", msg);

    set_color(15, 0);
    Sleep(duration_ms);

    // 출력 영역 지우기
    gotoxy(x, y);
    printf("%*s", (int)strlen(msg), " ");
}


// ------------------------------------------------------------------
// 파일에서 근태 기록 불러오기
// - work_hours는 파일에 저장하지 않으므로 제외
// ------------------------------------------------------------------
void load_attendance() {
    FILE* fp = fopen("Attendance.txt", "r");
    if (!fp) return;

    attendance_count = 0;

    while (fscanf(fp, "%s %s %s %s %[^\n]\n",
        attendance_list[attendance_count].user_id,
        attendance_list[attendance_count].date,
        attendance_list[attendance_count].check_in,
        attendance_list[attendance_count].check_out,
        attendance_list[attendance_count].status) == 5) {

        attendance_count++;
        if (attendance_count >= 500) break;
    }
    fclose(fp);
}


// ------------------------------------------------------------------
// 파일 저장: 모든 근태 기록을 텍스트 파일에 저장
// ------------------------------------------------------------------
void save_attendance() {
    FILE* fp = fopen("Attendance.txt", "w");
    if (!fp) return;

    for (int i = 0; i < attendance_count; i++) {
        fprintf(fp, "%s %s %s %s %s\n",
            attendance_list[i].user_id,
            attendance_list[i].date,
            attendance_list[i].check_in,
            attendance_list[i].check_out,
            attendance_list[i].status);
    }
    fclose(fp);
}


// ------------------------------------------------------------------
// 출근 체크 로직
// - 동일 날짜에 출근 기록이 있는지 검사
// - 문제 없을 시 새 기록 저장
// ------------------------------------------------------------------
void check_in(User* user) {
    char today[20];
    get_today(today);

    // 출근 중복 체크
    for (int i = 0; i < attendance_count; i++) {
        if (strcmp(attendance_list[i].user_id, user->id) == 0 &&
            strcmp(attendance_list[i].date, today) == 0 &&
            strcmp(attendance_list[i].check_out, "-") == 0) {

            show_message("이미 오늘 출근 체크한 기록이 있습니다!",
                MENU_X, BOX_Y + BOX_H - 2, 11, 1000);
            return;
        }
    }

    // 신규 출근 기록 생성
    Attendance* a = &attendance_list[attendance_count];
    strcpy(a->user_id, user->id);
    strcpy(a->date, today);

    get_time_now(a->check_in);
    strcpy(a->check_out, "-");    // 아직 퇴근 전
    a->work_hours = 0;            // 내부 계산 전 초기화
    strcpy(a->status, "출근");    // 상태 표시

    attendance_count++;
    save_attendance();

    show_message("출근 기록 완료!", MENU_X, BOX_Y + BOX_H - 2, 10, 1000);
}


// ------------------------------------------------------------------
// 퇴근 체크 로직
// - 출근 기록 확인 후 퇴근 시간 저장
// - 근무 시간(시:분) 계산하여 status에 저장
// ------------------------------------------------------------------
void check_out(User* user) {
    char today[20];
    get_today(today);

    for (int i = 0; i < attendance_count; i++) {
        if (strcmp(attendance_list[i].user_id, user->id) == 0 &&
            strcmp(attendance_list[i].date, today) == 0 &&
            strcmp(attendance_list[i].check_out, "-") == 0) {

            // 퇴근 시간 기록
            get_time_now(attendance_list[i].check_out);

            // 시간 차이 계산 (초 단위)
            int in_h, in_m, in_s;
            int out_h, out_m, out_s;
            sscanf(attendance_list[i].check_in, "%d:%d:%d", &in_h, &in_m, &in_s);
            sscanf(attendance_list[i].check_out, "%d:%d:%d", &out_h, &out_m, &out_s);

            int diff_sec =
                (out_h * 3600 + out_m * 60 + out_s)
                - (in_h * 3600 + in_m * 60 + in_s);

            if (diff_sec < 0) diff_sec = 0; // 안전 처리

            int wh = diff_sec / 3600;
            int wm = (diff_sec % 3600) / 60;

            attendance_list[i].work_hours = wh + wm / 60.0f;

            // HH:MM 형태로 상태 저장
            sprintf(attendance_list[i].status, "%02d:%02d", wh, wm);

            save_attendance();
            show_message("퇴근 기록 완료!", MENU_X, BOX_Y + BOX_H - 2, 10, 1200);
            return;
        }
    }

    // 출근 기록 없는 경우
    show_message("오늘 출근 기록이 없습니다!",
        MENU_X, BOX_Y + BOX_H - 2, 12, 1000);
}


// ------------------------------------------------------------------
// 내 근태 내역 조회 (현재 로그인된 사용자 기준)
// ------------------------------------------------------------------
void my_attendance(User* user) {
    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
    system("cls");

    draw_box(BOX_X, BOX_Y, BOX_W, BOX_H, "내 근태 기록");

    int line = BOX_Y + 2;
    for (int i = 0; i < attendance_count && line < BOX_Y + BOX_H - 2; i++) {
        if (strcmp(attendance_list[i].user_id, user->id) == 0) {
            gotoxy(BOX_X + 2, line++);
            printf("%s | 출근:%s | 퇴근:%s | %s",
                attendance_list[i].date,
                attendance_list[i].check_in,
                attendance_list[i].check_out,
                attendance_list[i].status);
        }
    }

    gotoxy(BOX_X, BOX_Y + BOX_H + 3);
    printf(">> 아무 키나 누르면 이전 메뉴로 돌아갑니다.");
    _getch();
}


// ------------------------------------------------------------------
// 전체 근태 내역 조회 (관리자 전용)
// ------------------------------------------------------------------
void all_attendance() {
    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
    system("cls");

    draw_box(BOX_X, BOX_Y, BOX_W, BOX_H, "전체 근태 기록");

    int line = BOX_Y + 2;
    for (int i = 0; i < attendance_count && line < BOX_Y + BOX_H - 2; i++) {
        gotoxy(BOX_X + 2, line++);
        printf("%s (%s) | 출근:%s | 퇴근:%s | %s",
            attendance_list[i].user_id,
            attendance_list[i].date,
            attendance_list[i].check_in,
            attendance_list[i].check_out,
            attendance_list[i].status);
    }

    gotoxy(BOX_X, BOX_Y + BOX_H + 3);
    printf(">> 아무 키나 누르면 이전 메뉴로 돌아갑니다.");
    _getch();
}


// ------------------------------------------------------------------
// 근태 메뉴 메인 UI
// - 사용자 관리자 여부에 따라 메뉴 다르게 표시
// - 마우스 입력을 통한 버튼 클릭 처리
// ------------------------------------------------------------------
void attendance_menu(User* user) {
    int mx, my;
    int is_admin = (strcmp(user->position, "관리자") == 0);

    load_attendance();

    system("cls");
    draw_box(BOX_X, BOX_Y, BOX_W, BOX_H, "근태 관리");

    // 버튼 표시
    draw_button(MENU_X, BTN_Y_1, "1. 출근 체크", 0);
    draw_button(MENU_X, BTN_Y_2, "2. 퇴근 체크", 0);
    draw_button(MENU_X, BTN_Y_3, "3. 내 근태 조회", 0);
    if (is_admin)
        draw_button(MENU_X, BTN_Y_4, "4. 전체 근태 조회", 0);
    draw_button(MENU_X, BTN_Y_0, "0. 이전 메뉴로", 0);


    while (1) {
        // 마우스 입력 활성화 (QuickEdit 비활성)
        HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
        DWORD mode;
        GetConsoleMode(hInput, &mode);
        SetConsoleMode(hInput,
            (mode & ~ENABLE_QUICK_EDIT_MODE) | ENABLE_MOUSE_INPUT);

        // 버튼 클릭 이벤트 처리
        if (get_mouse_click(&mx, &my)) {

            if (my == BTN_Y_1 && mx >= MENU_X && mx <= MENU_X + BTN_WIDTH)
                check_in(user);

            else if (my == BTN_Y_2 && mx >= MENU_X && mx <= MENU_X + BTN_WIDTH)
                check_out(user);

            else if (my == BTN_Y_3 && mx >= MENU_X && mx <= MENU_X + BTN_WIDTH) {
                my_attendance(user);

                // 돌아오면 UI 재출력
                attendance_menu(user);
                return;
            }

            else if (is_admin && my == BTN_Y_4 &&
                mx >= MENU_X && mx <= MENU_X + BTN_WIDTH) {
                all_attendance();
                attendance_menu(user);
                return;
            }

            else if (my == BTN_Y_0 && mx >= MENU_X && mx <= MENU_X + BTN_WIDTH) {
                show_main_menu(user);
                return;
            }
        }
    }
}
