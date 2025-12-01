/*
  파일명: evaluation.c
  수정사항: User.txt (텍스트 파일) 입출력 방식에 맞춰 읽기/쓰기 로직 변경
  + 급여 조회 및 수정 기능 추가
  + 관리자 평가 조회 기능 추가
*/

#define _CRT_SECURE_NO_WARNINGS
#include "common.h"


// ==========================================================
// [상수 정의]
// ==========================================================
#define BTN_X 24             
#define BOX_WIDTH 30         

// 관리자 메뉴 좌표
#define ADM_BTN_Y_1  8       // 급여 수정
#define ADM_BTN_Y_2  10      // 평가 조회 (New!)
#define ADM_BTN_Y_0  14      // 뒤로 가기

// 직원 메뉴 좌표
#define EMP_EVAL_BTN_X  45   
#define EMP_EVAL_BTN_Y  17   
#define EMP_BACK_BTN_X  15   
#define EMP_BACK_BTN_Y  20   

#define USER_DATA_FILE "User.txt" 
#define EVAL_DATA_FILE "Evaluation.txt"
#define MAX_USERS 100 

// ==========================================================
// [유틸] 입력 모드 제어
// ==========================================================
void set_mouse_mode() {
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode;

    GetConsoleMode(hIn, &mode);
    mode |= (ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);
    mode &= ~ENABLE_QUICK_EDIT_MODE;
    mode &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
    SetConsoleMode(hIn, mode);

    CONSOLE_CURSOR_INFO cursorInfo = { 100, FALSE };
    SetConsoleCursorInfo(hOut, &cursorInfo);
}

void set_text_mode() {
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode;

    GetConsoleMode(hIn, &mode);
    mode |= (ENABLE_PROCESSED_INPUT | ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
    SetConsoleMode(hIn, mode);

    CONSOLE_CURSOR_INFO cursorInfo = { 100, TRUE };
    SetConsoleCursorInfo(hOut, &cursorInfo);
}

void force_input_string(int x, int y, char* prompt, char* buffer) {
    gotoxy(x, y); printf("%s", prompt);
    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
    set_text_mode();
    scanf("%s", buffer);
    while (getchar() != '\n');
    set_mouse_mode();
}

void force_input_int(int x, int y, char* prompt, int* value) {
    gotoxy(x, y); printf("%s", prompt);
    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
    set_text_mode();
    scanf("%d", value);
    while (getchar() != '\n');
    set_mouse_mode();
}


// ==========================================================
// 1. [직원용] 팀원 인사 평가 입력 로직
// ==========================================================
void input_team_evaluation(User* evaluator) {
    system("cls");
    draw_box(15, 5, 50, 18, " 팀원 인사 평가 입력 ");

    char target_id[20];
    force_input_string(20, 8, "평가할 직원 ID : ", target_id);

    int score_work, score_attitude;
    force_input_int(20, 10, "업무 성과 점수 (0~50): ", &score_work);
    force_input_int(20, 12, "근무 태도 점수 (0~50): ", &score_attitude);

    char comment[100];
    gotoxy(20, 14); printf("한줄 코멘트: ");

    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
    set_text_mode();
    fgets(comment, sizeof(comment), stdin);
    comment[strcspn(comment, "\n")] = 0;
    set_mouse_mode();

    // 파일 저장 (추가 모드 'a')
    FILE* fp = fopen(EVAL_DATA_FILE, "a");
    if (fp != NULL) {
        fprintf(fp, "%s %s %d %d %s\n",
            evaluator->id, target_id, score_work, score_attitude, comment);
        fclose(fp);
    }

    gotoxy(20, 17);
    set_color(11, 0);
    printf(">> %s님 평가 저장 완료!", target_id);
    set_color(15, 0);
    Sleep(1500);
}


// ==========================================================
// 2. [관리자용] 급여 및 평가 조회 메뉴
// ==========================================================
void run_salary_eval_admin(User* admin) {
    int x, y;

    while (1) {
        system("cls");
        draw_box(15, 5, 50, 16, " [관리자] 급여 데이터 관리 ");

        draw_button(BTN_X, ADM_BTN_Y_1, "1. 직원 급여 검색 및 수정  ", 0);
        draw_button(BTN_X, ADM_BTN_Y_2, "2. 직원 인사평가 조회      ", 0); // 이름 변경
        draw_button(BTN_X, ADM_BTN_Y_0, "0. 뒤로 가기               ", 0);

        // 마우스 모드 강제 활성화
        HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
        DWORD mode;
        GetConsoleMode(hInput, &mode);
        SetConsoleMode(hInput, (mode & ~ENABLE_QUICK_EDIT_MODE) | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);

        CONSOLE_CURSOR_INFO cursorInfo = { 100, FALSE };
        SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);

        // 클릭 대기
        while (!get_mouse_click(&x, &y)) {}

        // =====================================================
        // 1. 직원 급여 수정
        // =====================================================
        if (y == ADM_BTN_Y_1 && x >= BTN_X && x <= BTN_X + BOX_WIDTH) {
            system("cls");
            draw_box(15, 5, 50, 15, " 직원 급여 수정 ");

            char target_id[20];
            gotoxy(20, 8); printf("수정할 직원 ID: ");

            FlushConsoleInputBuffer(hInput);
            set_text_mode();
            scanf("%s", target_id);
            set_mouse_mode();

            FILE* fp = fopen(USER_DATA_FILE, "r");
            if (fp == NULL) {
                gotoxy(20, 10); printf(">> 오류: %s 파일 없음", USER_DATA_FILE);
                Sleep(1500);
            }
            else {
                User users[MAX_USERS];
                int count = 0;
                int found_idx = -1;

                while (fscanf(fp, "%s %s %s %s %s %s %s %d",
                    users[count].id, users[count].pw, users[count].name,
                    users[count].department, users[count].position,
                    users[count].phone, users[count].hire_date,
                    &users[count].salary) != EOF)
                {
                    if (strcmp(users[count].id, target_id) == 0) {
                        found_idx = count;
                    }
                    count++;
                    if (count >= MAX_USERS) break;
                }
                fclose(fp);

                if (found_idx != -1) {
                    gotoxy(20, 10); printf("이름: %s / 직급: %s", users[found_idx].name, users[found_idx].position);
                    gotoxy(20, 11); printf("현재 급여: %d", users[found_idx].salary);

                    int new_pay;
                    gotoxy(20, 13); printf("변경할 급여: ");
                    set_text_mode();
                    scanf("%d", &new_pay);
                    while (getchar() != '\n');
                    set_mouse_mode();

                    users[found_idx].salary = new_pay;

                    fp = fopen(USER_DATA_FILE, "w");
                    for (int i = 0; i < count; i++) {
                        fprintf(fp, "%s %s %s %s %s %s %s %d\n",
                            users[i].id, users[i].pw, users[i].name,
                            users[i].department, users[i].position,
                            users[i].phone, users[i].hire_date,
                            users[i].salary);
                    }
                    fclose(fp);

                    gotoxy(20, 15);
                    set_color(10, 0);
                    printf(">> 수정 완료되었습니다!");
                    set_color(15, 0);
                }
                else {
                    gotoxy(20, 10); printf(">> ID '%s'를 찾을 수 없습니다.", target_id);
                }
                Sleep(1500);
            }
        }

        // =====================================================
        // 2. 직원 인사평가 조회 (새로 추가된 기능)
        // =====================================================
        else if (y == ADM_BTN_Y_2 && x >= BTN_X && x <= BTN_X + BOX_WIDTH) {
            system("cls");
            draw_box(10, 5, 75, 18, " 직원 인사평가 조회 ");

            char search_id[20];
            gotoxy(20, 8); printf("조회할 직원 ID: ");

            FlushConsoleInputBuffer(hInput);
            set_text_mode();
            scanf("%s", search_id);
            set_mouse_mode();

            FILE* fp = fopen(EVAL_DATA_FILE, "r");
            if (fp == NULL) {
                gotoxy(20, 10); printf(">> 평가 데이터가 없습니다.");
            }
            else {
                char eval_id[20], emp_id[20], comment[100];
                int s1, s2;
                int found = 0;

                // 파일에서 한 줄씩 읽어서 검색
                // 형식: 평가자 피평가자 점수1 점수2 코멘트
                while (fscanf(fp, "%s %s %d %d", eval_id, emp_id, &s1, &s2) != EOF) {
                    // 코멘트는 공백 포함일 수 있으니 fgets로 남은 줄 읽기
                    fgets(comment, sizeof(comment), fp);
                    // 앞뒤 공백 제거 등 처리가 필요할 수 있으나 단순 출력

                    if (strcmp(emp_id, search_id) == 0) {
                        found = 1;
                        gotoxy(20, 10); printf("--- [검색 결과] ---");
                        gotoxy(20, 12); printf("평가자 ID : %s", eval_id);
                        gotoxy(20, 13); printf("업무 점수 : %d점", s1);
                        gotoxy(20, 14); printf("태도 점수 : %d점", s2);
                        gotoxy(20, 15); printf("코멘트    :%s", comment); // comment에 이미 줄바꿈 포함됨
                        break; // 가장 최근(또는 첫번째) 기록 하나만 보여주고 멈춤
                    }
                }
                fclose(fp);

                if (!found) {
                    gotoxy(20, 10); printf(">> 해당 직원의 평가 기록이 없습니다.");
                }
            }

            gotoxy(20, 17); printf(">> 아무 키나 누르면 돌아갑니다.");
            _getch(); // 확인 대기
        }

        // 0. 뒤로 가기
        else if (y == ADM_BTN_Y_0 && x >= BTN_X && x <= BTN_X + BOX_WIDTH) {
            return;
        }
    }
}


// ==========================================================
// 3. [직원용] 급여 조회 및 평가 대시보드
// ==========================================================
void run_salary_eval_employee(User* my_user) {
    int x, y;

    while (1) {
        system("cls");
        draw_box(10, 4, 60, 20, " 급여 조회 / 인사 평가 ");

        set_color(11, 0);
        gotoxy(15, 7); printf("=== %s %s님의 급여 정보 ===", my_user->name, my_user->position);
        set_color(15, 0);

        gotoxy(15, 10); printf("----------------------------------------");
        set_color(14, 0);
        gotoxy(15, 12); printf("▶ 이번 달 급여 : %d", my_user->salary);
        set_color(15, 0);
        gotoxy(15, 14); printf("----------------------------------------");

        int is_boss = (strcmp(my_user->position, "부장") == 0 ||
            strcmp(my_user->position, "차장") == 0 ||
            strcmp(my_user->position, "과장") == 0);

        if (is_boss) draw_button(EMP_EVAL_BTN_X, EMP_EVAL_BTN_Y, " 팀원 평가하기 ", 0);
        draw_button(EMP_BACK_BTN_X, EMP_BACK_BTN_Y, " 0. 뒤로 가기 ", 0);

        HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
        DWORD mode;
        GetConsoleMode(hInput, &mode);
        SetConsoleMode(hInput, (mode & ~ENABLE_QUICK_EDIT_MODE) | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);

        while (!get_mouse_click(&x, &y)) {}

        if (is_boss && y == EMP_EVAL_BTN_Y && x >= EMP_EVAL_BTN_X - 2 && x <= EMP_EVAL_BTN_X + 20) {
            input_team_evaluation(my_user);
        }

        if (y == EMP_BACK_BTN_Y && x >= EMP_BACK_BTN_X - 2 && x <= EMP_BACK_BTN_X + 20) {
            return;
        }
    }
}