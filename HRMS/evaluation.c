/*
담당자: 박지훈
최근 업데이트: 2025.11.30 / 17:00
진행상태: 급여 입력 및 저장, 급여 조회 및 인사 평가 진입 기능 구현 완료
*/
# define _CRT_SECURE_NO_WARNINGS

#include "common.h"

// ==========================================================
// [상수 정의] UI 위치 및 크기 관리 
// ==========================================================
// 1. 공통 버튼 X 좌표
#define BTN_X 20

// 2. 관리자 메뉴 버튼 Y 좌표
#define ADM_BTN_Y_1  8   // 급여 수정
#define ADM_BTN_Y_2  10  // 파일 저장
#define ADM_BTN_Y_0  14  // 뒤로 가기

// 3. 직원 메뉴 평가 버튼 좌표
#define EMP_EVAL_BTN_X  45
#define EMP_EVAL_BTN_Y  17

// ==========================================================
// [필수] 마우스 입력 모드 강제 활성화 함수
// ==========================================================
void enable_mouse_input() {
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hIn, &mode);
    SetConsoleMode(hIn, (mode & ~ENABLE_QUICK_EDIT_MODE) | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);
}

// ==========================================================
// 1. [공통/내부] 팀원 인사 평가 입력 로직
// ==========================================================
void input_team_evaluation(User* evaluator) {
    enable_mouse_input();

    system("cls");
    draw_box(15, 5, 50, 18, " 팀원 인사 평가 입력 ");

    char target_id[20];
    gotoxy(BTN_X, 8); printf("평가할 직원 ID : ");

    // 입력 모드 전환
    SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), ENABLE_PROCESSED_INPUT | ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
    scanf("%s", target_id);
    enable_mouse_input(); // 복구

    int score_work, score_attitude;
    gotoxy(BTN_X, 10); printf("업무 성과 점수 (0~50): ");
    scanf("%d", &score_work);

    gotoxy(BTN_X, 12); printf("근무 태도 점수 (0~50): ");
    scanf("%d", &score_attitude);

    while (getchar() != '\n');

    char comment[100];
    gotoxy(BTN_X, 14); printf("한줄 코멘트: ");
    fgets(comment, sizeof(comment), stdin);
    comment[strcspn(comment, "\n")] = 0;

    gotoxy(BTN_X, 17); printf(">> %s님 평가 저장 완료!", target_id);
    Sleep(1500);
}

// ==========================================================
// 2. [관리자용] 급여 관리 (수정본)
// ==========================================================
void run_salary_eval_admin(User* admin) {
    enable_mouse_input(); // 시작할 때 마우스 켜기
    system("cls");
    draw_box(15, 5, 50, 16, " [관리자] 급여 수동 관리 ");

    draw_button(BTN_X, ADM_BTN_Y_1, "1. 직원 급여 검색 및 수정  ", 0);
    draw_button(BTN_X, ADM_BTN_Y_2, "2. 급여 데이터 파일 저장   ", 0);
    draw_button(BTN_X, ADM_BTN_Y_0, "0. 뒤로 가기               ", 0);

    while (1) {
        int x, y;

        if (get_mouse_click(&x, &y)) {
            // 클릭 범위 체크
            if (x >= BTN_X - 5 && x <= BTN_X + 40) {

                if (y == ADM_BTN_Y_1) { // 1. 급여 수정
                    // --- 기능 실행 ---
                    system("cls");
                    draw_box(15, 5, 50, 15, " 직원 급여 수정 ");

                    char target_id[20];
                    gotoxy(BTN_X, 8); printf("수정할 직원 ID: ");

                    // [핵심 수정 1] 첫 번째 입력(ID) 전에 텍스트 모드로 변경
                    SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), ENABLE_PROCESSED_INPUT | ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
                    scanf("%s", target_id);
                    // (주의: 여기서 마우스 모드로 돌아가지 않고 계속 텍스트 모드 유지)

                    FILE* fp = fopen("users.dat", "rb+");
                    if (fp == NULL) {
                        gotoxy(BTN_X, 10); printf(">> 파일을 찾을 수 없습니다.");
                        Sleep(1000);
                    }
                    else {
                        User temp;
                        int found = 0;
                        while (fread(&temp, sizeof(User), 1, fp) > 0) {
                            if (strcmp(temp.id, target_id) == 0) {
                                found = 1;
                                gotoxy(BTN_X, 10); printf("이름: %s / 급여: %d", temp.name, temp.salary);

                                // [핵심 수정 2] 두 번째 입력(금액) 받기
                                // 아까 텍스트 모드로 바꿔뒀으니 여기선 그냥 scanf 쓰면 됨 (멈춤 해결!)
                                gotoxy(BTN_X, 12); printf("변경할 금액: ");
                                int new_pay;
                                scanf("%d", &new_pay);
                                while (getchar() != '\n'); // 버퍼 비우기

                                temp.salary = new_pay;
                                fseek(fp, -(long)sizeof(User), SEEK_CUR);
                                fwrite(&temp, sizeof(User), 1, fp);
                                gotoxy(BTN_X, 14); printf(">> 수정 완료!");
                                break;
                            }
                        }
                        fclose(fp);
                        if (!found) { gotoxy(BTN_X, 10); printf(">> 해당 ID 없음"); }
                        Sleep(1500);
                    }

                    // [핵심 수정 3] 모든 입력이 끝났으니 이제 마우스 모드로 복구
                    enable_mouse_input();

                    // --- UI 복구 ---
                    system("cls");
                    draw_box(15, 5, 50, 16, " [관리자] 급여 수동 관리 ");
                    draw_button(BTN_X, ADM_BTN_Y_1, "1. 직원 급여 검색 및 수정  ", 0);
                    draw_button(BTN_X, ADM_BTN_Y_2, "2. 급여 데이터 파일 저장   ", 0);
                    draw_button(BTN_X, ADM_BTN_Y_0, "0. 뒤로 가기               ", 0);
                }
                else if (y == ADM_BTN_Y_0) return; // 뒤로 가기
            }
        }

        if (_kbhit()) {
            if (_getch() == 27) return;
        }
    }
}

// ==========================================================
// 3. [직원용] 급여 조회 및 평가
// ==========================================================
void run_salary_eval_employee(User* my_user) {
    enable_mouse_input();
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

    // 상수로 버튼 위치 지정
    if (is_boss) draw_button(EMP_EVAL_BTN_X, EMP_EVAL_BTN_Y, " 팀원 평가하기 ", 0);

    gotoxy(15, 20); printf("[ESC] 뒤로 가기");

    while (1) {
        int x, y;
        if (get_mouse_click(&x, &y)) {
            // 상수를 사용하여 클릭 범위 체크
            if (is_boss && y == EMP_EVAL_BTN_Y && x >= EMP_EVAL_BTN_X - 2 && x <= EMP_EVAL_BTN_X + 20) {
                input_team_evaluation(my_user);

                enable_mouse_input();
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

                if (is_boss) draw_button(EMP_EVAL_BTN_X, EMP_EVAL_BTN_Y, " 팀원 평가하기 ", 0);

                gotoxy(15, 20); printf("[ESC] 뒤로 가기");
            }
        }

        if (_kbhit()) {
            if (_getch() == 27) return;
        }
    }
}