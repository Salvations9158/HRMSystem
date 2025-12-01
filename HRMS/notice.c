#define _CRT_SECURE_NO_WARNINGS
#include "common.h"

#define BOX_X 5
#define BOX_Y 3
#define BOX_W 80
#define BOX_H 20
#define MENU_X 22
#define BTN_WIDTH 35

#define BTN_Y_1 7
#define BTN_Y_2 9
#define BTN_Y_3 11
#define BTN_Y_4 13
#define BTN_Y_0 17

Notice notice_list[500];
int notice_count = 0;
int notice_next_id = 1;

//-----------------------------------------------------------
// 오늘 날짜 반환
//-----------------------------------------------------------
void get_today2(char* dst) {
    time_t t = time(NULL);
    struct tm* tm = localtime(&t);
    sprintf(dst, "%04d-%02d-%02d",
        tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday);
}

//-----------------------------------------------------------
// 파일 불러오기 Notice.txt
//-----------------------------------------------------------
void load_notice() {
    FILE* fp = fopen("Notice.txt", "r");
    if (!fp) return;

    notice_count = 0;
    notice_next_id = 1;

    while (notice_count < 500) {
        Notice* n = &notice_list[notice_count];
        if (fscanf(fp, "%d|%49[^|]|%499[^|]|%19[^|]|%14[^\n]\n",
            &n->id, n->title, n->content, n->writer_id, n->date) != 5) {
            break;
        }

        if (n->id >= notice_next_id)
            notice_next_id = n->id + 1;

        notice_count++;
    }
    fclose(fp);
}

//-----------------------------------------------------------
// 파일 저장
//-----------------------------------------------------------
void save_notice() {
    FILE* fp = fopen("Notice.txt", "w");
    if (!fp) return;

    for (int i = 0; i < notice_count; i++) {
        Notice* n = &notice_list[i];
        fprintf(fp, "%d|%s|%s|%s|%s\n",
            n->id, n->title, n->content, n->writer_id, n->date);
    }
    fclose(fp);
}

//-----------------------------------------------------------
// 문자열 입력 (fgets 후 개행 제거)
//-----------------------------------------------------------
void input_string(char* buf, int size) {
    if (fgets(buf, size, stdin)) {
        buf[strcspn(buf, "\n")] = 0;
    }
}

//-----------------------------------------------------------
// 공지 등록
//-----------------------------------------------------------
void notice_add(User* user) {
    system("cls");
    draw_box(BOX_X, BOX_Y, BOX_W, BOX_H, "공지사항 등록");

    Notice* n = &notice_list[notice_count];

    n->id = notice_next_id++;
    strcpy(n->writer_id, user->id);
    get_today2(n->date);

    gotoxy(BOX_X + 2, BOX_Y + 2);
    printf("제목: ");
    input_string(n->title, sizeof(n->title));

    gotoxy(BOX_X + 2, BOX_Y + 4);
    printf("내용: ");
    input_string(n->content, sizeof(n->content));

    notice_count++;
    save_notice();

    gotoxy(BOX_X, BOX_Y + BOX_H + 3);
    printf("등록 완료! 아무 키나 누르세요.");
    _getch();
}

//-----------------------------------------------------------
// [수정] 공지 목록 출력 (UI 깨짐 방지 적용)
//-----------------------------------------------------------
void print_notice_list(const char* title) {
    draw_box(BOX_X, BOX_Y, BOX_W, BOX_H, (char*)title);

    int line = BOX_Y + 2;
    // 박스 하단 테두리를 침범하지 않도록 최대 줄 수 계산
    int max_line = BOX_Y + BOX_H - 2;

    // 헤더 출력
    gotoxy(BOX_X + 2, line++);
    printf("%-5s %-45s %-12s", "NO", "제목", "날짜");
    gotoxy(BOX_X + 2, line++);
    printf("----------------------------------------------------------------");

    for (int i = 0; i < notice_count; i++) {
        // 박스를 넘어가면 출력 중단
        if (line >= max_line) break;

        gotoxy(BOX_X + 2, line++);

        // [중요] %-45.45s : 45칸을 차지하되, 글자가 더 길면 45자에서 자름 (UI 보호)
        printf("%-5d %-45.45s %-12s",
            notice_list[i].id,
            notice_list[i].title,
            notice_list[i].date);
    }

    gotoxy(BOX_X, BOX_Y + BOX_H + 3);
    printf("상세보기: 공지 번호 입력 → ");
}

//-----------------------------------------------------------
// [수정] 공지 상세보기
//-----------------------------------------------------------
void notice_view_detail() {
    int sel;
    // 입력 커서 위치 잡아주기
    gotoxy(MENU_X + 8, BOX_Y + BOX_H + 3);
    scanf("%d", &sel);
    // 입력 버퍼 비우기 (엔터 제거)
    while (getchar() != '\n');

    system("cls");
    draw_box(BOX_X, BOX_Y, BOX_W, BOX_H, "공지 상세보기");

    for (int i = 0; i < notice_count; i++) {
        if (notice_list[i].id == sel) {
            gotoxy(BOX_X + 2, BOX_Y + 2);
            printf("제목: %.50s", notice_list[i].title); // 제목도 너무 길면 자름

            gotoxy(BOX_X + 2, BOX_Y + 4);
            printf("작성자: %s", notice_list[i].writer_id);

            gotoxy(BOX_X + 40, BOX_Y + 4);
            printf("작성일: %s", notice_list[i].date);

            gotoxy(BOX_X + 2, BOX_Y + 6);
            printf("--------------------------------------------------------------------------");

            gotoxy(BOX_X + 2, BOX_Y + 8);
            printf("내용:");

            // 내용 출력 (박스 밖으로 나가는 것 방지 - 단순 구현)
            gotoxy(BOX_X + 2, BOX_Y + 9);
            // 내용이 너무 길면 잘라서 출력하거나 줄바꿈 처리가 필요하지만
            // 여기서는 UI 보호를 위해 일정 길이까지만 출력하도록 함
            printf("%.70s", notice_list[i].content);
            if (strlen(notice_list[i].content) > 70) printf("...");

            gotoxy(BOX_X, BOX_Y + BOX_H + 3);
            printf("아무 키나 누르면 돌아갑니다.");
            _getch();
            return;
        }
    }

    gotoxy(BOX_X, BOX_Y + BOX_H + 3);
    printf("해당 번호의 공지 없음. 아무 키나 누르세요.");
    _getch();
}

//-----------------------------------------------------------
// [수정] 공지 수정 (제목 입력 스킵 해결)
//-----------------------------------------------------------
void notice_edit() {
    int sel;
    // 입력 커서 위치
    gotoxy(MENU_X + 8, BOX_Y + BOX_H + 3);
    scanf("%d", &sel);

    // [중요] scanf 후 남은 엔터키 제거 (이게 없으면 제목 입력이 스킵됨)
    while (getchar() != '\n');

    for (int i = 0; i < notice_count; i++) {
        if (notice_list[i].id == sel) {
            system("cls");
            draw_box(BOX_X, BOX_Y, BOX_W, BOX_H, "공지 수정");

            gotoxy(BOX_X + 2, BOX_Y + 2);
            // 기존 제목 보여주기
            printf("기존 제목: %.30s...", notice_list[i].title);

            gotoxy(BOX_X + 2, BOX_Y + 4);
            printf("새 제목: ");
            input_string(notice_list[i].title, sizeof(notice_list[i].title));

            gotoxy(BOX_X + 2, BOX_Y + 6);
            printf("새 내용: ");
            input_string(notice_list[i].content, sizeof(notice_list[i].content));

            save_notice();

            gotoxy(BOX_X, BOX_Y + BOX_H + 3);
            printf("수정 완료! 아무 키나 누르세요.");
            _getch();
            return;
        }
    }

    gotoxy(BOX_X, BOX_Y + BOX_H + 3);
    printf("해당 번호 없음. 아무 키나 누르세요.");
    _getch();
}

//-----------------------------------------------------------
// 공지 삭제 (관리자 전용)
//-----------------------------------------------------------
void notice_delete() {
    int sel;
    // 입력 커서 위치
    gotoxy(MENU_X + 8, BOX_Y + BOX_H + 3);
    scanf("%d", &sel);
    while (getchar() != '\n'); // 버퍼 비우기

    for (int i = 0; i < notice_count; i++) {
        if (notice_list[i].id == sel) {
            for (int j = i; j < notice_count - 1; j++)
                notice_list[j] = notice_list[j + 1];
            notice_count--;
            save_notice();

            gotoxy(BOX_X, BOX_Y + BOX_H + 3);
            printf("삭제 완료! 아무 키나 누르세요.");
            _getch();
            return;
        }
    }

    gotoxy(BOX_X, BOX_Y + BOX_H + 3);
    printf("해당 번호 없음. 아무 키나 누르세요.");
    _getch();
}

//-----------------------------------------------------------
// 관리자 공지 메뉴
//-----------------------------------------------------------
void notice_admin_menu(User* user) {
    int mx, my;

    // 메뉴 진입 시 잔여 클릭 제거
    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));

    load_notice();

    while (1) {
        system("cls");
        init_console();

        draw_box(BOX_X, BOX_Y, BOX_W, BOX_H, "공지사항 관리 (관리자)");

        draw_button(MENU_X, BTN_Y_1, "1. 공지 목록 보기", 0);
        draw_button(MENU_X, BTN_Y_2, "2. 공지 등록", 0);
        draw_button(MENU_X, BTN_Y_3, "3. 공지 수정", 0);
        draw_button(MENU_X, BTN_Y_4, "4. 공지 삭제", 0);
        draw_button(MENU_X, BTN_Y_0, "0. 이전 메뉴", 0);

        while (!get_mouse_click(&mx, &my)) {
            Sleep(10);
        }

        if (my == BTN_Y_1 && mx >= MENU_X && mx <= MENU_X + BTN_WIDTH) {
            system("cls");
            print_notice_list("공지사항 목록 (관리자)");
            notice_view_detail();
        }
        else if (my == BTN_Y_2 && mx >= MENU_X && mx <= MENU_X + BTN_WIDTH) {
            notice_add(user);
        }
        else if (my == BTN_Y_3 && mx >= MENU_X && mx <= MENU_X + BTN_WIDTH) {
            system("cls");
            print_notice_list("공지사항 목록 (관리자)");
            notice_edit();
        }
        else if (my == BTN_Y_4 && mx >= MENU_X && mx <= MENU_X + BTN_WIDTH) {
            system("cls");
            print_notice_list("공지사항 목록 (관리자)");
            notice_delete();
        }
        else if (my == BTN_Y_0 && mx >= MENU_X && mx <= MENU_X + BTN_WIDTH) {
            system("cls");
            return;
        }
    }
}

//-----------------------------------------------------------
// 직원 공지 메뉴
//-----------------------------------------------------------
void notice_employee_menu() {
    int mx, my;

    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
    load_notice();

    while (1) {
        system("cls");
        init_console();

        draw_box(BOX_X, BOX_Y, BOX_W, BOX_H, "공지사항");

        draw_button(MENU_X, BTN_Y_1, "1. 공지 목록 보기", 0);
        draw_button(MENU_X, BTN_Y_0, "0. 이전 메뉴", 0);

        while (!get_mouse_click(&mx, &my)) {
            Sleep(10);
        }

        if (my == BTN_Y_1 && mx >= MENU_X && mx <= MENU_X + BTN_WIDTH) {
            system("cls");
            print_notice_list("공지사항 목록");
            notice_view_detail();
        }
        else if (my == BTN_Y_0 && mx >= MENU_X && mx <= MENU_X + BTN_WIDTH) {
            system("cls");
            return;
        }
    }
}