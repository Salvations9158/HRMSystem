/*
담당자: 전민규
최근 업데이트: 2025.12.01 / 02:25
진행상태:
1-1 결재 기능 구현 완료
1-2 아직 전체적으로 결재가 늘어났을때 유동적인 ui처리가 미흡함 (추후 개선 예정)
*/

#include "common.h"

// 결재 더미 데이터 생성
void create_dummy_approval_data()
{
    FILE* fp = NULL;
    if (fopen_s(&fp, "Approval.txt", "r") != 0 || fp == NULL)
    {
        if (fopen_s(&fp, "Approval.txt", "w") == 0 && fp != NULL)
        {
            fprintf(fp, "1 worker01 연차신청 개인사정 대기 2025-11-25\n");
            fprintf(fp, "2 worker01 비품구매 모니터구매 대기 2025-11-26\n");
            fprintf(fp, "3 admin 출장신청 본사출장 대기 2025-11-27\n");
            printf("시스템: 초기 결재 데이터를 생성했습니다.\n");
            fclose(fp);
        }
    }
    else fclose(fp);
}

// 결재 번호 생성: 한 줄씩 읽어서 맨 앞의 정수만 추출
int get_next_approval_no()
{
    FILE* fp = NULL;
    char line[256];
    int max_no = 0;

    if (fopen_s(&fp, "Approval.txt", "r") == 0 && fp != NULL)
    {
        while (fgets(line, sizeof(line), fp) != NULL)
        {
            int no = 0;
            // 한 줄의 맨 앞에서 정수 하나만 읽어옴
            if (sscanf_s(line, "%d", &no) == 1)
            {
                if (no > max_no) max_no = no;
            }
        }
        fclose(fp);
    }

    return max_no + 1;
}

// 사용자 ID로 부서 찾기
void get_user_dept_approval(char* user_id, char* dept_buf)
{
    FILE* fp = NULL;
    User temp;
    strcpy_s(dept_buf, 20, "");

    if (fopen_s(&fp, "User.txt", "r") == 0 && fp != NULL)
    {
        while (fscanf_s(fp, "%s %s %s %s %s %s %s %d",
            temp.id, (unsigned)sizeof(temp.id), temp.pw, (unsigned)sizeof(temp.pw),
            temp.name, (unsigned)sizeof(temp.name), temp.department, (unsigned)sizeof(temp.department),
            temp.position, (unsigned)sizeof(temp.position), temp.phone, (unsigned)sizeof(temp.phone),
            temp.hire_date, (unsigned)sizeof(temp.hire_date),
            &temp.salary) != EOF)
        {
            if (strcmp(temp.id, user_id) == 0)
            {
                strcpy_s(dept_buf, 20, temp.department);
                break;
            }
        }
        fclose(fp);
    }
}

// 결재 상태 변경 및 삭제 공통 함수
// new_status가 "DELETE"면 삭제, 아니면 상태값 변경
void update_approval_action(int target_no, char* new_status)
{
    FILE* fp = NULL;
    FILE* tfp = NULL;
    Approval temp;

    if (fopen_s(&fp, "Approval.txt", "r") == 0 && fp != NULL)
    {
        if (fopen_s(&tfp, "Approval_temp.txt", "w") == 0 && tfp != NULL)
        {
            while (fscanf_s(fp, "%d %s %s %s %s %s",
                &temp.no, temp.drafter_id, (unsigned)sizeof(temp.drafter_id),
                temp.type, (unsigned)sizeof(temp.type), temp.content, (unsigned)sizeof(temp.content),
                temp.status, (unsigned)sizeof(temp.status), temp.date, (unsigned)sizeof(temp.date)) != EOF)
            {
                if (temp.no == target_no) {
                    if (strcmp(new_status, "DELETE") == 0) continue; // 삭제 시 파일에 쓰지 않고 건너뜀
                    fprintf(tfp, "%d %s %s %s %s %s\n", temp.no, temp.drafter_id, temp.type, temp.content, new_status, temp.date);
                }
                else {
                    fprintf(tfp, "%d %s %s %s %s %s\n", temp.no, temp.drafter_id, temp.type, temp.content, temp.status, temp.date);
                }
            }
            fclose(tfp);
        }
        fclose(fp);
        remove("Approval.txt");
        rename("Approval_temp.txt", "Approval.txt");
    }
}

// -----------------------------------------------------------
// 1. 관리자 모드: 전체 조회 및 삭제
// -----------------------------------------------------------
void admin_approval_main()
{
    int mx, my;
    int list_y_start = 7;
    int shown_nos[10] = { 0, };
    int shown_count = 0;

    create_dummy_approval_data();

    // 화면 그리기 (루프 밖으로 이동)
    system("cls");
    draw_box(5, 3, 70, 18, "관리자 결재 현황 (전체)");
    gotoxy(7, 6); printf("NO  | 기안자   | 종류       | 상태 | 관리");
    gotoxy(7, 6); printf("----------------------------------------------------------");

    // 파일 읽기 및 출력 (한 번만 실행)
    FILE* fp = NULL;
    Approval app;
    shown_count = 0;

    if (fopen_s(&fp, "Approval.txt", "r") == 0 && fp != NULL)
    {
        while (fscanf_s(fp, "%d %s %s %s %s %s",
            &app.no, app.drafter_id, (unsigned)sizeof(app.drafter_id),
            app.type, (unsigned)sizeof(app.type), app.content, (unsigned)sizeof(app.content),
            app.status, (unsigned)sizeof(app.status), app.date, (unsigned)sizeof(app.date)) != EOF)
        {
            if (shown_count < 10) {
                gotoxy(7, list_y_start + (shown_count * 2));
                printf("%-3d | %-8s | %-10s | %s", app.no, app.drafter_id, app.type, app.status);

                // 관리자는 '삭제'만 가능
                draw_button(50, list_y_start + (shown_count * 2), "삭제", 0);

                shown_nos[shown_count++] = app.no;
            }
        }
        fclose(fp);
    }

    draw_button(30, 18, "나가기", 0);

    // 마우스 모드 재설정 (루프 진입 전)
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    FlushConsoleInputBuffer(hInput);
    DWORD mode;
    GetConsoleMode(hInput, &mode);
    SetConsoleMode(hInput, (mode & ~ENABLE_QUICK_EDIT_MODE) | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);

    int loop = 1;
    while (loop) {
        if (get_mouse_click(&mx, &my)) {
            if (my == 18 && mx >= 30 && mx <= 40) return;

            for (int i = 0; i < shown_count; i++) {
                if (my == list_y_start + (i * 2) && mx >= 50 && mx <= 58) {
                    update_approval_action(shown_nos[i], "DELETE");
                    gotoxy(20, 16); set_color(12, 0); printf(">> 결재 내역이 삭제되었습니다."); set_color(15, 0);
                    Sleep(800);
                    // 삭제 후 화면 갱신을 위해 재귀 호출 또는 루프 재시작 필요하나, 
                    // 구조상 간단히 함수 종료 후 재진입 유도 (또는 화면 지우고 다시 그리기)
                    loop = 0; // 루프 종료 -> 함수 종료 -> 메뉴에서 다시 호출됨 (구조에 따라 다름)
                    // 여기서는 함수를 끝내고 메뉴로 돌아가게 처리 (사용자가 다시 들어오도록)
                    break;
                }
            }
        }
    }
    // 삭제 후 바로 갱신된 화면을 보여주고 싶다면 admin_approval_main()을 다시 호출
    if (loop == 0) admin_approval_main();
}

// -----------------------------------------------------------
// 2. 직원 모드 (부장 포함): 신청, 조회, (부장)승인
// -----------------------------------------------------------

// [내부 함수] 결재 신청 화면
void process_request(User* user) {
    int mx, my;
    char input_type[20] = "", input_content[100] = "", input_date[15] = "";
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    int box_x = 15, box_y = 5;

    system("cls");
    draw_box(box_x, box_y, 50, 16, "결재 신청 작성");
    gotoxy(20, box_y + 4); printf("종  류 : ");
    gotoxy(20, box_y + 6); printf("내  용 : ");
    gotoxy(20, box_y + 8); printf("날  짜 : ");
    draw_button(box_x + 10, box_y + 13, "신청하기", 0);
    draw_button(box_x + 25, box_y + 13, "취  소", 0);

    cursorInfo.dwSize = 100; cursorInfo.bVisible = TRUE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);

    gotoxy(30, box_y + 4); set_color(14, 0); scanf_s("%s", input_type, (unsigned)sizeof(input_type)); while (getchar() != '\n');
    set_color(15, 0); gotoxy(20, box_y + 4); printf("종  류 : %-15s", input_type);

    gotoxy(30, box_y + 6); set_color(14, 0); scanf_s("%s", input_content, (unsigned)sizeof(input_content)); while (getchar() != '\n');
    set_color(15, 0); gotoxy(20, box_y + 6); printf("내  용 : %-15s", input_content);

    gotoxy(30, box_y + 8); set_color(14, 0); scanf_s("%s", input_date, (unsigned)sizeof(input_date)); while (getchar() != '\n');
    set_color(15, 0); gotoxy(20, box_y + 8); printf("날  짜 : %-15s", input_date);

    cursorInfo.bVisible = FALSE; SetConsoleCursorInfo(hConsole, &cursorInfo);

    FlushConsoleInputBuffer(hInput);
    DWORD mode; GetConsoleMode(hInput, &mode);
    SetConsoleMode(hInput, (mode & ~ENABLE_QUICK_EDIT_MODE) | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);

    while (1) {
        if (get_mouse_click(&mx, &my)) {
            if (my == box_y + 13) {
                // [신청하기] 버튼 클릭 시
                if (mx >= box_x + 10 && mx <= box_x + 20) {

                    // [수정 핵심] 파일을 열기 전에 먼저 번호를 생성해서 가져옵니다.
                    int next_no = get_next_approval_no();

                    FILE* fp = NULL;
                    // 그 다음 파일을 열어 저장합니다.
                    if (fopen_s(&fp, "Approval.txt", "a") == 0 && fp != NULL) {
                        fprintf(fp, "%d %s %s %s 대기 %s\n", next_no, user->id, input_type, input_content, input_date);
                        fclose(fp);
                        gotoxy(20, box_y + 15); printf("신청 완료! (NO.%d)", next_no); Sleep(1000); return;
                    }
                }
                else if (mx >= box_x + 25 && mx <= box_x + 35) return;
            }
        }
    }
}

// [내부 함수] 본인 결재 현황 조회
void employee_view_status(User* user) {
    int mx, my;
    int list_y_start = 7, shown_count = 0;

    // 화면 그리기 (루프 밖)
    system("cls");
    draw_box(5, 3, 70, 18, "내 결재 신청 현황");
    gotoxy(7, 6); printf("NO  | 종류       | 날짜       | 상태");
    gotoxy(7, 6); printf("----------------------------------------------------------");

    FILE* fp = NULL; Approval app; shown_count = 0;
    if (fopen_s(&fp, "Approval.txt", "r") == 0 && fp != NULL) {
        while (fscanf_s(fp, "%d %s %s %s %s %s", &app.no, app.drafter_id, (unsigned)sizeof(app.drafter_id), app.type, (unsigned)sizeof(app.type), app.content, (unsigned)sizeof(app.content), app.status, (unsigned)sizeof(app.status), app.date, (unsigned)sizeof(app.date)) != EOF) {
            if (strcmp(app.drafter_id, user->id) == 0 && shown_count < 10) {
                gotoxy(7, list_y_start + (shown_count * 2));
                if (strcmp(app.status, "승인") == 0) set_color(11, 0);
                else if (strcmp(app.status, "반려") == 0) set_color(12, 0);
                printf("%-3d | %-10s | %-10s | %s", app.no, app.type, app.date, app.status);
                set_color(15, 0);
                shown_count++;
            }
        }
        fclose(fp);
    }
    draw_button(30, 18, "뒤로가기", 0);

    // 마우스 모드 설정
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    FlushConsoleInputBuffer(hInput);
    DWORD mode; GetConsoleMode(hInput, &mode);
    SetConsoleMode(hInput, (mode & ~ENABLE_QUICK_EDIT_MODE) | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);

    while (1) {
        if (get_mouse_click(&mx, &my)) {
            if (my == 18 && mx >= 30 && mx <= 40) return;
        }
    }
}

// [내부 함수] 부장 전용 승인/반려 프로세스
void manager_approve_process(User* manager) {
    int mx, my;
    int list_y_start = 7, shown_nos[10] = { 0, }, shown_count = 0;

    system("cls");
    draw_box(5, 3, 70, 18, "부서 결재 승인/반려");
    gotoxy(7, 5); set_color(11, 0); printf("[%s] 대기 상태 결재 목록", manager->department); set_color(15, 0);
    gotoxy(7, 6); printf("NO  | 기안자   | 종류       | 상태");
    gotoxy(7, 6); printf("----------------------------------------------------------");

    FILE* fp = NULL; Approval app; char drafter_dept[20]; shown_count = 0;
    if (fopen_s(&fp, "Approval.txt", "r") == 0 && fp != NULL) {
        while (fscanf_s(fp, "%d %s %s %s %s %s", &app.no, app.drafter_id, (unsigned)sizeof(app.drafter_id), app.type, (unsigned)sizeof(app.type), app.content, (unsigned)sizeof(app.content), app.status, (unsigned)sizeof(app.status), app.date, (unsigned)sizeof(app.date)) != EOF) {
            if (strcmp(app.status, "대기") == 0) {
                get_user_dept_approval(app.drafter_id, drafter_dept);
                if (strcmp(manager->department, drafter_dept) == 0 && shown_count < 10) {
                    gotoxy(7, list_y_start + (shown_count * 2));
                    printf("%-3d | %-8s | %-10s | %s", app.no, app.drafter_id, app.type, app.status);
                    draw_button(50, list_y_start + (shown_count * 2), "승인", 0);
                    draw_button(58, list_y_start + (shown_count * 2), "반려", 0);
                    shown_nos[shown_count++] = app.no;
                }
            }
        }
        fclose(fp);
    }
    if (shown_count == 0) { gotoxy(20, 10); printf("처리할 대기 결재가 없습니다."); }
    draw_button(30, 18, "나가기", 0);

    // 마우스 모드 설정
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    FlushConsoleInputBuffer(hInput);
    DWORD mode; GetConsoleMode(hInput, &mode);
    SetConsoleMode(hInput, (mode & ~ENABLE_QUICK_EDIT_MODE) | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);

    int loop = 1;
    while (loop) {
        if (get_mouse_click(&mx, &my)) {
            if (my == 18 && mx >= 30 && mx <= 40) return;
            for (int i = 0; i < shown_count; i++) {
                if (my == list_y_start + (i * 2)) {
                    if (mx >= 50 && mx <= 56) { update_approval_action(shown_nos[i], "승인"); loop = 0; break; }
                    else if (mx >= 58 && mx <= 64) { update_approval_action(shown_nos[i], "반려"); loop = 0; break; }
                }
            }
        }
    }
    if (loop == 0) manager_approve_process(manager); // 처리 후 화면 갱신
}

// [메인 진입점] 직원 결재 메뉴 (부장 기능 포함)
void employee_approval_main(User* user) {
    int mx, my, box_x = 20, box_y = 8;
    int is_manager = (strcmp(user->position, "부장") == 0); // 부장 확인

    while (1) {
        system("cls");
        draw_box(box_x, box_y, 40, 12, "결재 처리 메뉴");
        draw_button(box_x + 10, box_y + 3, "1. 결재 신청", 0);
        draw_button(box_x + 10, box_y + 5, "2. 결재 현황", 0);

        // 부장만 보이는 히든 메뉴
        if (is_manager) {
            set_color(11, 0);
            draw_button(box_x + 10, box_y + 7, "3. 부서 결재 관리", 0);
            set_color(15, 0);
            draw_button(box_x + 10, box_y + 9, "0. 뒤로 가기", 0);
        }
        else {
            draw_button(box_x + 10, box_y + 7, "0. 뒤로 가기", 0);
        }

        // 마우스 모드 설정
        HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
        FlushConsoleInputBuffer(hInput);
        DWORD mode; GetConsoleMode(hInput, &mode);
        SetConsoleMode(hInput, (mode & ~ENABLE_QUICK_EDIT_MODE) | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);

        // 루프 내에서 클릭 대기
        int clicked = 0;
        while (!clicked) {
            if (get_mouse_click(&mx, &my)) clicked = 1;
        }

        if (clicked) {
            if (my == box_y + 3 && mx >= box_x + 10 && mx <= box_x + 25) process_request(user);
            else if (my == box_y + 5 && mx >= box_x + 10 && mx <= box_x + 25) employee_view_status(user);
            else if (is_manager && my == box_y + 7 && mx >= box_x + 10 && mx <= box_x + 25) manager_approve_process(user);
            else if ((is_manager && my == box_y + 9) || (!is_manager && my == box_y + 7)) {
                if (mx >= box_x + 10 && mx <= box_x + 25) return;
            }
        }
    }
}