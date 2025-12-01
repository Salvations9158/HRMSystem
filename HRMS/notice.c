/*
담당자: 이은수
최근 업데이트: 2025.12.01
진행상태: 공지사항 관리 기능 구현 완료 (뒤로가기/취소 기능 추가됨)
*/

#define _CRT_SECURE_NO_WARNINGS
#include "common.h"

/*
=========================================================
            공지사항 관리 시스템 (TUI 기반)
=========================================================
- 공지 목록 조회 / 상세 보기 / 등록 / 수정 / 삭제
- 관리자 및 직원 메뉴 분리
- [Update] 등록 중 취소 기능 추가 (제목에서 엔터)
- [Update] 조회/수정/삭제 선택 시 '0' 입력으로 뒤로가기 추가
=========================================================
*/

// ------------------------------------------------------------------
// 상수 정의 (TUI UI 구성 요소 위치 및 크기)
// ------------------------------------------------------------------
#define BOX_X 5
#define BOX_Y 3
#define BOX_W 80
#define BOX_H 20
#define MENU_X 22
#define BTN_WIDTH 35

// 버튼 Y 좌표
#define BTN_Y_1 7   // 목록 보기
#define BTN_Y_2 9   // 등록
#define BTN_Y_3 11  // 수정
#define BTN_Y_4 13  // 삭제
#define BTN_Y_0 17  // 이전 메뉴

// ------------------------------------------------------------------
// 전역 변수: 공지사항 데이터 저장
// ------------------------------------------------------------------
Notice notice_list[500];
int notice_count = 0;
int notice_next_id = 1;

// ------------------------------------------------------------------
// 유틸리티: 오늘 날짜 반환 (YYYY-MM-DD)
// ------------------------------------------------------------------
void get_today2(char* dst) {
    time_t t = time(NULL);
    struct tm* tm = localtime(&t);
    sprintf(dst, "%04d-%02d-%02d",
        tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday);
}

// ------------------------------------------------------------------
// 파일 불러오기: Notice.txt
// ------------------------------------------------------------------
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

// ------------------------------------------------------------------
// 파일 저장
// ------------------------------------------------------------------
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

// ------------------------------------------------------------------
// 유틸리티: 문자열 입력
// ------------------------------------------------------------------
void input_string(char* buf, int size) {
    fseek(stdin, 0, SEEK_END);
    if (fgets(buf, size, stdin)) {
        buf[strcspn(buf, "\n")] = 0;
    }
}

// ------------------------------------------------------------------
// 공지 등록 기능 (수정됨: 취소 기능 추가)
// ------------------------------------------------------------------
void notice_add(User* user) {
    system("cls"); // 화면 초기화
    draw_box(BOX_X, BOX_Y, BOX_W, BOX_H, "공지사항 등록");

    // 안전장치: user 정보가 제대로 넘어왔는지 확인
    if (user == NULL) {
        gotoxy(BOX_X + 2, BOX_Y + 2);
        set_color(12, 0); // [색상] 빨간색 (오류)
        printf("오류: 로그인 정보가 없습니다.");
        set_color(15, 0); // [색상] 복구
        _getch();
        return;
    }

    Notice* n = &notice_list[notice_count];

    n->id = notice_next_id++;
    strcpy(n->writer_id, user->id);
    get_today2(n->date);

    // 제목 입력 (빈칸 입력 시 취소)
    gotoxy(BOX_X + 2, BOX_Y + 2);
    printf("제목 (빈칸 입력 시 취소): ");
    input_string(n->title, sizeof(n->title));

    // [수정] 제목이 비어있으면 등록 취소
    if (strlen(n->title) == 0) {
        notice_next_id--; // ID 증가 되돌리기
        gotoxy(BOX_X, BOX_Y + BOX_H + 3);
        printf("등록이 취소되었습니다. 아무 키나 누르세요.");
        _getch();
        return;
    }

    // 내용 입력
    gotoxy(BOX_X + 2, BOX_Y + 4);
    printf("내용: ");
    input_string(n->content, sizeof(n->content));

    // 저장 및 종료
    notice_count++;
    save_notice();

    gotoxy(BOX_X, BOX_Y + BOX_H + 3);
    set_color(10, 0); // [색상] 초록색 (성공)
    printf("등록 완료! 아무 키나 누르세요.");
    set_color(15, 0); // [색상] 복구

    _getch();
}

// ------------------------------------------------------------------
// 공지 목록 출력 (수정됨: 안내 문구 변경)
// ------------------------------------------------------------------
void print_notice_list(const char* title) {
    draw_box(BOX_X, BOX_Y, BOX_W, BOX_H, (char*)title);

    int line = BOX_Y + 2;
    int max_line = BOX_Y + BOX_H - 2;

    // 헤더 출력
    gotoxy(BOX_X + 2, line++);
    printf("%-5s %-45s %-12s", "NO", "제목", "날짜");
    gotoxy(BOX_X + 2, line++);
    printf("----------------------------------------------------------------");

    // 리스트 출력
    for (int i = 0; i < notice_count; i++) {
        if (line >= max_line) break;

        gotoxy(BOX_X + 2, line++);
        printf("%-5d %-45.45s %-12s",
            notice_list[i].id,
            notice_list[i].title,
            notice_list[i].date);
    }

    // [수정] 안내 문구 변경
    gotoxy(BOX_X, BOX_Y + BOX_H + 3);
    printf("번호 입력 (0: 뒤로가기) → ");
}

// ------------------------------------------------------------------
// 공지 상세보기 (수정됨: 0 입력 시 뒤로가기)
// ------------------------------------------------------------------
void notice_view_detail() {
    int sel;
    gotoxy(MENU_X + 10, BOX_Y + BOX_H + 3);
    scanf("%d", &sel);
    while (getchar() != '\n');

    // [수정] 0 입력 시 뒤로가기
    if (sel == 0) return;

    system("cls");
    draw_box(BOX_X, BOX_Y, BOX_W, BOX_H, "공지 상세보기");

    for (int i = 0; i < notice_count; i++) {
        if (notice_list[i].id == sel) {
            // 상세 내용 출력
            gotoxy(BOX_X + 2, BOX_Y + 2);
            printf("제목: %.50s", notice_list[i].title);

            gotoxy(BOX_X + 2, BOX_Y + 4);
            printf("작성자: %s", notice_list[i].writer_id);

            gotoxy(BOX_X + 40, BOX_Y + 4);
            printf("작성일: %s", notice_list[i].date);

            gotoxy(BOX_X + 2, BOX_Y + 6);
            printf("--------------------------------------------------------------------------");

            gotoxy(BOX_X + 2, BOX_Y + 8);
            printf("내용:");
            gotoxy(BOX_X + 2, BOX_Y + 9);
            // 내용 길이 제한
            printf("%.70s", notice_list[i].content);
            if (strlen(notice_list[i].content) > 70) printf("...");

            gotoxy(BOX_X, BOX_Y + BOX_H + 3);
            printf("아무 키나 누르면 돌아갑니다.");
            _getch();
            return;
        }
    }

    // 찾는 번호 없을 때
    gotoxy(BOX_X, BOX_Y + BOX_H + 3);
    set_color(12, 0); // [색상] 빨간색 (경고)
    printf("해당 번호의 공지 없음. 아무 키나 누르세요.");
    set_color(15, 0); // [색상] 복구

    _getch();
}

// ------------------------------------------------------------------
// 공지 수정 (수정됨: 0 입력 시 뒤로가기)
// ------------------------------------------------------------------
void notice_edit() {
    int sel;

    // 수정할 번호 입력
    gotoxy(MENU_X + 10, BOX_Y + BOX_H + 3);
    scanf("%d", &sel);
    while (getchar() != '\n'); // 버퍼 비우기

    // [수정] 0 입력 시 뒤로가기
    if (sel == 0) return;

    for (int i = 0; i < notice_count; i++) {
        if (notice_list[i].id == sel) {
            system("cls");
            draw_box(BOX_X, BOX_Y, BOX_W, BOX_H, "공지 수정");

            // 기존 정보 표시
            gotoxy(BOX_X + 2, BOX_Y + 2);
            printf("기존 제목: %.30s...", notice_list[i].title);

            // 새 제목 입력
            gotoxy(BOX_X + 2, BOX_Y + 4);
            printf("새 제목: ");
            input_string(notice_list[i].title, sizeof(notice_list[i].title));

            // 새 내용 입력
            gotoxy(BOX_X + 2, BOX_Y + 6);
            printf("새 내용: ");
            input_string(notice_list[i].content, sizeof(notice_list[i].content));

            save_notice();

            gotoxy(BOX_X, BOX_Y + BOX_H + 3);
            set_color(10, 0); // [색상] 초록색 (성공)
            printf("수정 완료! 아무 키나 누르세요.");
            set_color(15, 0); // [색상] 복구

            _getch();
            return;
        }
    }

    gotoxy(BOX_X, BOX_Y + BOX_H + 3);
    set_color(12, 0); // [색상] 빨간색 (경고)
    printf("해당 번호 없음. 아무 키나 누르세요.");
    set_color(15, 0); // [색상] 복구

    _getch();
}

// ------------------------------------------------------------------
// 공지 삭제 (수정됨: 0 입력 시 뒤로가기)
// ------------------------------------------------------------------
void notice_delete() {
    int sel;

    // 삭제할 번호 입력
    gotoxy(MENU_X + 10, BOX_Y + BOX_H + 3);
    scanf("%d", &sel);
    while (getchar() != '\n'); // 버퍼 비우기

    // [수정] 0 입력 시 뒤로가기
    if (sel == 0) return;

    for (int i = 0; i < notice_count; i++) {
        if (notice_list[i].id == sel) {
            for (int j = i; j < notice_count - 1; j++)
                notice_list[j] = notice_list[j + 1];

            notice_count--;
            save_notice();

            gotoxy(BOX_X, BOX_Y + BOX_H + 3);
            set_color(10, 0); // [색상] 초록색 (성공)
            printf("삭제 완료! 아무 키나 누르세요.");
            set_color(15, 0); // [색상] 복구

            _getch();
            return;
        }
    }

    gotoxy(BOX_X, BOX_Y + BOX_H + 3);
    set_color(12, 0); // [색상] 빨간색 (경고)
    printf("해당 번호 없음. 아무 키나 누르세요.");
    set_color(15, 0); // [색상] 복구

    _getch();
}

// ------------------------------------------------------------------
// 관리자 공지 메뉴
// ------------------------------------------------------------------
void notice_admin_menu(User* user) {
    int mx, my;

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

// ------------------------------------------------------------------
// 직원 공지 메뉴
// ------------------------------------------------------------------
void notice_employee_menu(User* user) {
    int mx, my;

    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
    load_notice();

    while (1) {
        system("cls");
        init_console();

        draw_box(BOX_X, BOX_Y, BOX_W, BOX_H, "공지사항 (직원)");
        draw_button(MENU_X, BTN_Y_1, "1. 공지 목록 보기", 0);
        draw_button(MENU_X, BTN_Y_0, "0. 이전 메뉴", 0);

        while (!get_mouse_click(&mx, &my)) {
            Sleep(10);
        }
        if (my == BTN_Y_1 && mx >= MENU_X && mx <= MENU_X + BTN_WIDTH) {
            system("cls");
            print_notice_list("공지사항 목록 (읽기 전용)");
            notice_view_detail();
        }
        else if (my == BTN_Y_0 && mx >= MENU_X && mx <= MENU_X + BTN_WIDTH) {
            system("cls");
            return;
        }
    }
}