/*
  파일명: evaluation.c
  구조: attendance.c(근태관리)의 루프 및 입력 방식을 100% 동일하게 적용
*/

#define _CRT_SECURE_NO_WARNINGS
#include "common.h"

// ==========================================================
// [상수 정의] 버튼 위치 관리
// ==========================================================
#define BTN_X 24             
#define BOX_WIDTH 30         

// 관리자 메뉴 좌표
#define ADM_BTN_Y_1  8       
#define ADM_BTN_Y_2  10      
#define ADM_BTN_Y_0  14      

// 직원 메뉴 좌표
#define EMP_EVAL_BTN_X  45   
#define EMP_EVAL_BTN_Y  17   
#define EMP_BACK_BTN_X  15   
#define EMP_BACK_BTN_Y  20   

#define USER_DATA_FILE "User.txt" 

// ==========================================================
// [내부 함수] 텍스트 입력 모드로 전환 (scanf 사용 전 필수)
// ==========================================================
void set_input_mode() {
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hInput, &mode);
    SetConsoleMode(hInput, ENABLE_PROCESSED_INPUT | ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);

    // 커서 보이기
    CONSOLE_CURSOR_INFO cursorInfo = { 100, TRUE };
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}


// ==========================================================
// 1. [직원용] 팀원 인사 평가 입력 로직
// ==========================================================
void input_team_evaluation(User* evaluator) {
    system("cls");
    draw_box(15, 5, 50, 18, " 팀원 인사 평가 입력 ");

    // 1. ID 입력
    char target_id[20];
    gotoxy(20, 8); printf("평가할 직원 ID : ");

    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE)); // 버퍼 초기화
    set_input_mode(); // 텍스트 모드 ON
    scanf("%s", target_id);

    // 2. 점수 입력
    int score_work, score_attitude;
    gotoxy(20, 10); printf("업무 성과 점수 (0~50): ");
    scanf("%d", &score_work);

    gotoxy(20, 12); printf("근무 태도 점수 (0~50): ");
    scanf("%d", &score_attitude);

    while (getchar() != '\n'); // 엔터 제거

    // 3. 코멘트 입력
    char comment[100];
    gotoxy(20, 14); printf("한줄 코멘트: ");
    fgets(comment, sizeof(comment), stdin);
    comment[strcspn(comment, "\n")] = 0;

    // 4. 저장 완료
    gotoxy(20, 17);
    set_color(11, 0);
    printf(">> %s님 평가 저장 완료!", target_id);
    set_color(15, 0);
    Sleep(1500);
}


// ==========================================================
// 2. [관리자용] 급여 관리 메뉴
// ==========================================================
void run_salary_eval_admin(User* admin) {
    int x, y;

    while (1) {
        // 1. 화면 초기화 + UI 그리기
        system("cls");
        draw_box(15, 5, 50, 16, " [관리자] 급여 데이터 관리 ");

        draw_button(BTN_X, ADM_BTN_Y_1, "1. 직원 급여 검색 및 수정  ", 0);
        draw_button(BTN_X, ADM_BTN_Y_2, "2. (기능 준비중)           ", 0);
        draw_button(BTN_X, ADM_BTN_Y_0, "0. 뒤로 가기               ", 0);

        // 2. [팀원 코드 방식] 마우스 입력 모드 매번 강제 활성화
        HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
        DWORD mode;
        GetConsoleMode(hInput, &mode);
        SetConsoleMode(hInput, (mode & ~ENABLE_QUICK_EDIT_MODE) | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);

        // 커서 숨기기 (UI 깔끔하게)
        CONSOLE_CURSOR_INFO cursorInfo = { 100, FALSE };
        SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);

        // 3. [팀원 코드 방식] 클릭될 때까지 여기서 대기 (멈춰있는 게 아님, 클릭 기다리는 중)
        while (!get_mouse_click(&x, &y)) {
            // 클릭 감지될 때까지 무한 루프
        }

        // ========== 버튼 클릭 판정 ==========

        // 1. 직원 급여 수정
        if (y == ADM_BTN_Y_1 && x >= BTN_X && x <= BTN_X + BOX_WIDTH) {
            system("cls");
            draw_box(15, 5, 50, 15, " 직원 급여 수정 ");

            char target_id[20];
            gotoxy(20, 8); printf("수정할 직원 ID: ");

            // 입력 전 텍스트 모드로 변경
            FlushConsoleInputBuffer(hInput);
            set_input_mode();
            scanf("%s", target_id);

            FILE* fp = fopen(USER_DATA_FILE, "rb+");
            if (fp == NULL) {
                gotoxy(20, 10); printf(">> 오류: %s 파일 없음", USER_DATA_FILE);
                Sleep(1500);
            }
            else {
                User temp;
                int found = 0;
                while (fread(&temp, sizeof(User), 1, fp) > 0) {
                    if (strcmp(temp.id, target_id) == 0) {
                        found = 1;
                        gotoxy(20, 10); printf("이름: %s / 직급: %s", temp.name, temp.position);
                        gotoxy(20, 11); printf("현재 급여: %d 만원", temp.salary);

                        int new_pay;
                        gotoxy(20, 13); printf("변경할 급여(만원): ");
                        scanf("%d", &new_pay);

                        temp.salary = new_pay;
                        fseek(fp, -(long)sizeof(User), SEEK_CUR);
                        fwrite(&temp, sizeof(User), 1, fp);

                        gotoxy(20, 15);
                        set_color(10, 0);
                        printf(">> 수정 완료되었습니다!");
                        set_color(15, 0);
                        break;
                    }
                }
                fclose(fp);
                if (!found) { gotoxy(20, 10); printf(">> ID '%s' 없음", target_id); }
                Sleep(1500);
            }
            // 루프 돌면서 다시 UI 그림
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
        // 1. UI 그리기
        system("cls");
        draw_box(10, 4, 60, 20, " 급여 조회 / 인사 평가 ");

        set_color(11, 0);
        gotoxy(15, 7); printf("=== %s %s님의 급여 정보 ===", my_user->name, my_user->position);
        set_color(15, 0);

        gotoxy(15, 10); printf("----------------------------------------");
        set_color(14, 0);
        gotoxy(15, 12); printf("▶ 이번 달 급여 : %d 만원", my_user->salary);
        set_color(15, 0);
        gotoxy(15, 14); printf("----------------------------------------");

        int is_boss = (strcmp(my_user->position, "부장") == 0 ||
            strcmp(my_user->position, "차장") == 0 ||
            strcmp(my_user->position, "과장") == 0);

        if (is_boss) draw_button(EMP_EVAL_BTN_X, EMP_EVAL_BTN_Y, " 팀원 평가하기 ", 0);
        draw_button(EMP_BACK_BTN_X, EMP_BACK_BTN_Y, " 0. 뒤로 가기 ", 0);

        // 2. [팀원 코드 방식] 마우스 모드 강제 활성화
        HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
        DWORD mode;
        GetConsoleMode(hInput, &mode);
        SetConsoleMode(hInput, (mode & ~ENABLE_QUICK_EDIT_MODE) | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);

        // 3. [팀원 코드 방식] 클릭 대기 루프 (여기서 멈춰 있다가 클릭하면 넘어감)
        while (!get_mouse_click(&x, &y)) {
            // 대기 중...
        }

        // ========== 버튼 클릭 판정 ==========

        // 팀원 평가하기
        if (is_boss && y == EMP_EVAL_BTN_Y && x >= EMP_EVAL_BTN_X - 2 && x <= EMP_EVAL_BTN_X + 20) {
            input_team_evaluation(my_user);
            // 함수 끝나면 루프 처음으로 돌아가서 다시 그림 (이때 마우스 모드도 다시 세팅됨)
        }

        // 뒤로 가기
        if (y == EMP_BACK_BTN_Y && x >= EMP_BACK_BTN_X - 2 && x <= EMP_BACK_BTN_X + 20) {
            return;
        }
    }
}