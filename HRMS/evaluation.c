/*
담당자: 박지훈
최근 업데이트: 2025.11.30 / 17:00
진행상태: 급여 입력 및 저장, 급여 조회 및 인사 평가 진입 기능 구현 완료
*/
# define _CRT_SECURE_NO_WARNINGS
#include "common.h"

// ================================================================
// 1. [공통/내부] 팀원 인사 평가 입력 로직 (별도 함수로 분리)
// ================================================================
void input_team_evaluation(User* evaluator) {
    system("cls");
    draw_box(15, 5, 50, 18, " 팀원 인사 평가 입력 ");

    char target_id[20];
    gotoxy(20, 8); printf("평가할 직원 ID : ");
    // 입력 받을 때 커서 위치 등 처리가 필요하지만 간단히 scanf 사용
    scanf("%s", target_id);

    int score_work, score_attitude;
    gotoxy(20, 10); printf("업무 성과 점수 (0~50): ");
    scanf("%d", &score_work);

    gotoxy(20, 12); printf("근무 태도 점수 (0~50): ");
    scanf("%d", &score_attitude);

    while (getchar() != '\n'); // 버퍼 비우기

    char comment[100];
    gotoxy(20, 14); printf("한줄 코멘트: ");
    fgets(comment, sizeof(comment), stdin);
    comment[strcspn(comment, "\n")] = 0;

    gotoxy(20, 17); printf(">> %s님에 대한 평가 저장 완료!", target_id);
    Sleep(1500);
}

// ================================================================
// 2. [관리자용] 급여 관리 (수동 입력 모드)
// ================================================================
void run_salary_eval_admin(User* admin) {
    // [수정 포인트 1] 화면 그리는 코드를 while문 밖으로 뺐습니다. (최초 1회만 그림)
    system("cls");
    draw_box(15, 5, 50, 16, " [관리자] 급여 수동 관리 ");
    draw_button(20, 8, "1. 직원 급여 검색 및 수정  ", 0);
    draw_button(20, 10, "2. 급여 데이터 파일 저장   ", 0);
    draw_button(20, 14, "0. 뒤로 가기               ", 0);

    while (1) {
        int x, y;
        if (get_mouse_click(&x, &y)) {
            if (x >= 15 && x <= 65) {
                if (y == 8) {
                    // --- [기능 실행: 급여 수정] ---
                    system("cls");
                    draw_box(15, 5, 50, 15, " 직원 급여 수정 ");

                    char target_id[20];
                    gotoxy(20, 8); printf("수정할 직원 ID: ");
                    scanf("%s", target_id);

                    // 파일 처리 
                    FILE* fp = fopen("User.txt", "rb+");
                    if (fp == NULL) {
                        gotoxy(20, 10); printf(">> 데이터 파일을 열 수 없습니다!");
                        Sleep(1000);
                    }
                    else {
                        User temp_user;
                        int found = 0;
                        while (fread(&temp_user, sizeof(User), 1, fp) > 0) {
                            if (strcmp(temp_user.id, target_id) == 0) {
                                found = 1;
                                gotoxy(20, 10); printf("이름: %s / 직급: %s", temp_user.name, temp_user.position);
                                gotoxy(20, 11); printf("현재 급여: %d 만원", temp_user.salary);

                                gotoxy(20, 13); printf("변경할 급여(만원): ");
                                int new_salary;
                                scanf("%d", &new_salary);
                                while (getchar() != '\n');

                                temp_user.salary = new_salary;
                                fseek(fp, -(long)sizeof(User), SEEK_CUR);
                                fwrite(&temp_user, sizeof(User), 1, fp);

                                gotoxy(20, 15); printf(">> 수정 완료되었습니다!");
                                break;
                            }
                        }
                        fclose(fp);
                        if (!found) { gotoxy(20, 10); printf(">> 해당 ID의 직원이 없습니다."); }
                        Sleep(1500);
                    }

                    // --- [복귀] 작업이 끝나고 돌아왔으니 UI를 다시 그려줍니다 ---
                    system("cls");
                    draw_box(15, 5, 50, 16, " [관리자] 급여 수동 관리 ");
                    draw_button(20, 8, "1. 직원 급여 검색 및 수정  ", 0);
                    draw_button(20, 10, "2. 급여 데이터 파일 저장   ", 0);
                    draw_button(20, 14, "0. 뒤로 가기               ", 0);
                }
                else if (y == 10) {
                    gotoxy(20, 17); printf(">> 변경 사항은 즉시 파일에 반영됩니다.");
                    Sleep(1000);
                    // 메시지 출력 후 지워진 부분 등 복구가 필요할 수 있으나, 간단히 넘어감
                }
                else if (y == 14) return; // 뒤로 가기
            }
        }
        Sleep(50); 
    }
}

// ================================================================
// 3. [직원용] 급여 조회 및 인사 평가 진입
// ================================================================
void run_salary_eval_employee(User* my_user) {
    // [수정 포인트] 화면 그리기를 while 밖으로 이동
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

    if (is_boss) {
        draw_button(45, 17, " 팀원 평가하기 ", 0);
    }
    gotoxy(15, 20); printf("[ESC] 뒤로 가기");

    while (1) {
        int x, y;
        if (get_mouse_click(&x, &y)) {
            if (is_boss && y == 17 && x >= 45 && x <= 60) {
                // 평가 화면으로 이동
                input_team_evaluation(my_user);

                // [복귀] 평가하고 돌아왔으니 내 화면 다시 그리기
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
                if (is_boss) draw_button(45, 17, " 팀원 평가하기 ", 0);
                gotoxy(15, 20); printf("[ESC] 뒤로 가기");
            }
        }

        if (_kbhit()) {
            if (_getch() == 27) return;
        }
        Sleep(50); // CPU 쉬게 해주기
    }
}