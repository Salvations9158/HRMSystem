/*
담당자: 전민규
최근 업데이트: 2025.12.01
진행상태:
1-1 결재 기능 구현 완료 (페이지 적용)
1-2 static 제거 및 외부 참조 가능하도록 변경
1-3 페이지당 표시 개수 5개로 변경 (UI 최적화)
*/

#include "common.h"

#define MAX_LOAD_COUNT 100 // 최대 로드 개수
#define ITEMS_PER_PAGE 5   // 페이지당 표시 개수를 5개로 변경 (UI 박스 크기 고려)

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
            // 페이지 테스트용
            for (int i = 4; i <= 15; i++) fprintf(fp, "%d worker01 테스트%d 내용%d 대기 2025-11-28\n", i, i, i);

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
                    if (strcmp(new_status, "DELETE") == 0) continue; // 삭제
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

    // 힙 메모리 사용 (동적 할당)
    Approval* list = (Approval*)malloc(sizeof(Approval) * MAX_LOAD_COUNT);
    if (list == NULL) {
        printf("메모리 할당 실패\n");
        return;
    }

    int total_count = 0;
    int current_page = 1;
    int total_pages = 1;

    create_dummy_approval_data();

    while (1)
    {
        // 데이터 로드
        FILE* fp = NULL;
        Approval temp;
        total_count = 0;

        if (fopen_s(&fp, "Approval.txt", "r") == 0 && fp != NULL)
        {
            while (fscanf_s(fp, "%d %s %s %s %s %s",
                &temp.no, temp.drafter_id, (unsigned)sizeof(temp.drafter_id),
                temp.type, (unsigned)sizeof(temp.type), temp.content, (unsigned)sizeof(temp.content),
                temp.status, (unsigned)sizeof(temp.status), temp.date, (unsigned)sizeof(temp.date)) != EOF)
            {
                if (total_count < MAX_LOAD_COUNT) list[total_count++] = temp;
            }
            fclose(fp);
        }

        // 페이지 수 계산 (5개 단위)
        if (total_count == 0) total_pages = 1;
        else total_pages = (total_count - 1) / ITEMS_PER_PAGE + 1;

        if (current_page > total_pages) current_page = total_pages;
        if (current_page < 1) current_page = 1;

        // UI 그리기
        system("cls");
        draw_box(5, 3, 70, 18, "관리자 결재 현황 (전체)");
        gotoxy(7, 6); printf("NO  | 기안자   | 종류       | 상태 | 관리");
        gotoxy(7, 6); printf("----------------------------------------------------------");

        int start_idx = (current_page - 1) * ITEMS_PER_PAGE;
        int end_idx = start_idx + ITEMS_PER_PAGE;
        if (end_idx > total_count) end_idx = total_count;

        int display_idx = 0;
        for (int i = start_idx; i < end_idx; i++) {
            gotoxy(7, list_y_start + (display_idx * 2));
            printf("%-3d | %-8s | %-10s | %s", list[i].no, list[i].drafter_id, list[i].type, list[i].status);
            draw_button(50, list_y_start + (display_idx * 2), "삭제", 0);
            display_idx++;
        }

        draw_button(30, 18, "나가기", 0);

        // 페이지 네비게이션 버튼 표시
        if (current_page > 1) draw_button(10, 18, "< 이전", 0);
        if (current_page < total_pages) draw_button(60, 18, "다음 >", 0);

        gotoxy(38, 5); set_color(14, 0); printf("Page %d / %d", current_page, total_pages); set_color(15, 0);

        HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
        FlushConsoleInputBuffer(hInput);
        DWORD mode; GetConsoleMode(hInput, &mode);
        SetConsoleMode(hInput, (mode & ~ENABLE_QUICK_EDIT_MODE) | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);

        int clicked = 0;
        while (!clicked) {
            if (get_mouse_click(&mx, &my)) clicked = 1;
        }

        if (clicked) {
            // 나가기
            if (my == 18 && mx >= 30 && mx <= 40) {
                free(list);
                return;
            }
            // 이전 페이지
            if (current_page > 1 && my == 18 && mx >= 10 && mx <= 20) {
                current_page--;
                continue;
            }
            // 다음 페이지
            if (current_page < total_pages && my == 18 && mx >= 60 && mx <= 70) {
                current_page++;
                continue;
            }

            // 삭제 버튼 클릭
            display_idx = 0;
            for (int i = start_idx; i < end_idx; i++) {
                if (my == list_y_start + (display_idx * 2) && mx >= 50 && mx <= 58) {
                    update_approval_action(list[i].no, "DELETE");
                    gotoxy(20, 16); set_color(12, 0); printf(">> 결재 내역이 삭제되었습니다."); set_color(15, 0);
                    Sleep(800); break;
                }
                display_idx++;
            }
        }
    }
    free(list);
}

// -----------------------------------------------------------
// 2. 직원/부장 모드 함수들 
// -----------------------------------------------------------

// 결재 신청 화면
void process_request(User* user)
{
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
                if (mx >= box_x + 10 && mx <= box_x + 20) {
                    //파일을 열기 전에 번호를 먼저 받아옴
                    int next_no = get_next_approval_no();

                    FILE* fp = NULL;
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

// 본인 결재 현황 조회
void employee_view_status(User* user)
{
    int mx, my;
    int list_y_start = 7;

    Approval* list = (Approval*)malloc(sizeof(Approval) * MAX_LOAD_COUNT);
    if (list == NULL) {
        printf("메모리 할당 실패\n");
        return;
    }

    int total_count = 0;
    int current_page = 1;
    int total_pages = 1;

    while (1) {
        FILE* fp = NULL; Approval temp; total_count = 0;
        if (fopen_s(&fp, "Approval.txt", "r") == 0 && fp != NULL) {
            while (fscanf_s(fp, "%d %s %s %s %s %s", &temp.no, temp.drafter_id, (unsigned)sizeof(temp.drafter_id), temp.type, (unsigned)sizeof(temp.type), temp.content, (unsigned)sizeof(temp.content), temp.status, (unsigned)sizeof(temp.status), temp.date, (unsigned)sizeof(temp.date)) != EOF) {
                if (strcmp(temp.drafter_id, user->id) == 0 && total_count < MAX_LOAD_COUNT) {
                    list[total_count++] = temp;
                }
            }
            fclose(fp);
        }

        if (total_count == 0) total_pages = 1;
        else total_pages = (total_count - 1) / ITEMS_PER_PAGE + 1;
        if (current_page > total_pages) current_page = total_pages;
        if (current_page < 1) current_page = 1;

        system("cls");
        draw_box(5, 3, 70, 18, "내 결재 신청 현황");
        gotoxy(7, 6); printf("NO  | 종류       | 날짜       | 상태");
        gotoxy(7, 6); printf("----------------------------------------------------------");

        int start_idx = (current_page - 1) * ITEMS_PER_PAGE;
        int end_idx = start_idx + ITEMS_PER_PAGE;
        if (end_idx > total_count) end_idx = total_count;

        int display_idx = 0;
        for (int i = start_idx; i < end_idx; i++) {
            gotoxy(7, list_y_start + (display_idx * 2));
            if (strcmp(list[i].status, "승인") == 0) set_color(11, 0);
            else if (strcmp(list[i].status, "반려") == 0) set_color(12, 0);
            printf("%-3d | %-10s | %-10s | %s", list[i].no, list[i].type, list[i].date, list[i].status);
            set_color(15, 0);
            display_idx++;
        }

        draw_button(30, 18, "뒤로가기", 0);
        if (current_page > 1) draw_button(10, 18, "< 이전", 0);
        if (current_page < total_pages) draw_button(60, 18, "다음 >", 0);
        gotoxy(38, 5); set_color(14, 0); printf("Page %d / %d", current_page, total_pages); set_color(15, 0);

        HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
        FlushConsoleInputBuffer(hInput);
        DWORD mode; GetConsoleMode(hInput, &mode);
        SetConsoleMode(hInput, (mode & ~ENABLE_QUICK_EDIT_MODE) | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);

        int clicked = 0;
        while (!clicked) {
            if (get_mouse_click(&mx, &my)) clicked = 1;
        }

        if (clicked) {
            if (my == 18 && mx >= 30 && mx <= 40) {
                free(list);
                return;
            }
            if (current_page > 1 && my == 18 && mx >= 10 && mx <= 20) { current_page--; continue; }
            if (current_page < total_pages && my == 18 && mx >= 60 && mx <= 70) { current_page++; continue; }
        }
    }
    free(list);
}

// 부장 전용 승인/반려 프로세스
void manager_approve_process(User* manager)
{
    int mx, my;
    int list_y_start = 7;

    Approval* list = (Approval*)malloc(sizeof(Approval) * MAX_LOAD_COUNT);
    if (list == NULL) {
        printf("메모리 할당 실패\n");
        return;
    }

    int total_count = 0;
    int current_page = 1;
    int total_pages = 1;

    while (1) {
        FILE* fp = NULL; Approval temp; char drafter_dept[20];
        total_count = 0;

        if (fopen_s(&fp, "Approval.txt", "r") == 0 && fp != NULL) {
            while (fscanf_s(fp, "%d %s %s %s %s %s", &temp.no, temp.drafter_id, (unsigned)sizeof(temp.drafter_id), temp.type, (unsigned)sizeof(temp.type), temp.content, (unsigned)sizeof(temp.content), temp.status, (unsigned)sizeof(temp.status), temp.date, (unsigned)sizeof(temp.date)) != EOF) {
                if (strcmp(temp.status, "대기") == 0) {
                    get_user_dept_approval(temp.drafter_id, drafter_dept);
                    if (strcmp(manager->department, drafter_dept) == 0 && total_count < MAX_LOAD_COUNT) {
                        list[total_count++] = temp;
                    }
                }
            }
            fclose(fp);
        }

        if (total_count == 0) total_pages = 1;
        else total_pages = (total_count - 1) / ITEMS_PER_PAGE + 1;
        if (current_page > total_pages) current_page = total_pages;
        if (current_page < 1) current_page = 1;

        system("cls");
        draw_box(5, 3, 70, 18, "부서 결재 승인/반려");
        gotoxy(7, 5); set_color(11, 0); printf("[%s] 대기 상태 결재 목록", manager->department); set_color(15, 0);
        gotoxy(7, 6); printf("NO  | 기안자   | 종류       | 상태");
        gotoxy(7, 6); printf("----------------------------------------------------------");

        int start_idx = (current_page - 1) * ITEMS_PER_PAGE;
        int end_idx = start_idx + ITEMS_PER_PAGE;
        if (end_idx > total_count) end_idx = total_count;

        int display_idx = 0;
        for (int i = start_idx; i < end_idx; i++) {
            gotoxy(7, list_y_start + (display_idx * 2));
            printf("%-3d | %-8s | %-10s | %s", list[i].no, list[i].drafter_id, list[i].type, list[i].status);
            draw_button(50, list_y_start + (display_idx * 2), "승인", 0);
            draw_button(58, list_y_start + (display_idx * 2), "반려", 0);
            display_idx++;
        }

        if (total_count == 0) { gotoxy(20, 10); printf("처리할 대기 결재가 없습니다."); }

        draw_button(30, 18, "나가기", 0);
        if (current_page > 1) draw_button(10, 18, "< 이전", 0);
        if (current_page < total_pages) draw_button(60, 18, "다음 >", 0);
        gotoxy(38, 5); set_color(14, 0); printf("Page %d / %d", current_page, total_pages); set_color(15, 0);

        HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
        FlushConsoleInputBuffer(hInput);
        DWORD mode; GetConsoleMode(hInput, &mode);
        SetConsoleMode(hInput, (mode & ~ENABLE_QUICK_EDIT_MODE) | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);

        int clicked = 0;
        while (!clicked) {
            if (get_mouse_click(&mx, &my)) clicked = 1;
        }

        if (clicked) {
            if (my == 18 && mx >= 30 && mx <= 40) {
                free(list);
                return;
            }
            if (current_page > 1 && my == 18 && mx >= 10 && mx <= 20) { current_page--; continue; }
            if (current_page < total_pages && my == 18 && mx >= 60 && mx <= 70) { current_page++; continue; }

            display_idx = 0;
            for (int i = start_idx; i < end_idx; i++) {
                int y = list_y_start + (display_idx * 2);
                if (my == y) {
                    if (mx >= 50 && mx <= 56) { update_approval_action(list[i].no, "승인"); Sleep(500); break; }
                    else if (mx >= 58 && mx <= 64) { update_approval_action(list[i].no, "반려"); Sleep(500); break; }
                }
                display_idx++;
            }
        }
    }
    free(list);
}

// [메인 진입점] 직원 결재 메뉴
void employee_approval_main(User* user)
{
    int mx, my, box_x = 20, box_y = 8;
    int is_manager = (strcmp(user->position, "부장") == 0); // 부장 확인

    while (1) {
        system("cls");
        draw_box(box_x, box_y, 40, 12, "결재 처리 메뉴");
        draw_button(box_x + 10, box_y + 3, "1. 결재 신청", 0);
        draw_button(box_x + 10, box_y + 5, "2. 결재 현황", 0);

        if (is_manager) {
            set_color(11, 0);
            draw_button(box_x + 10, box_y + 7, "3. 부서 결재 관리", 0);
            set_color(15, 0);
            draw_button(box_x + 10, box_y + 9, "0. 뒤로 가기", 0);
        }
        else {
            draw_button(box_x + 10, box_y + 7, "0. 뒤로 가기", 0);
        }

        HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
        FlushConsoleInputBuffer(hInput);
        DWORD mode; GetConsoleMode(hInput, &mode);
        SetConsoleMode(hInput, (mode & ~ENABLE_QUICK_EDIT_MODE) | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);

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