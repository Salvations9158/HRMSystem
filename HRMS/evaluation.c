/*
담당자: 박지훈
최근 업데이트: 2025.11.30 / 17:00
진행상태: 급여 입력 및 저장, 급여 조회 및 인사 평가 진입 기능 구현 완료
*/

#include "common.h"

// ================================================================
// 1. [공통/내부] 팀원 인사 평가 입력 로직 (별도 함수로 분리)
// ================================================================
void input_team_evaluation(User* evaluator) {
    system("cls");
    draw_box(15, 5, 50, 18, " 팀원 인사 평가 입력 ");

    // 1. 평가 대상 입력
    // (실제로는 직원 목록에서 선택해야 하지만, 지금은 ID 입력 방식으로 구현)
    char target_id[20];
    gotoxy(20, 8); printf("평가할 직원 ID : ");
    scanf("%s", target_id);

    // 2. 점수 입력
    int score_work, score_attitude;
    gotoxy(20, 10); printf("업무 성과 점수 (0~50): ");
    scanf("%d", &score_work);

    gotoxy(20, 12); printf("근무 태도 점수 (0~50): ");
    scanf("%d", &score_attitude);

    // 버퍼 비우기 (엔터 키 제거)
    while (getchar() != '\n');

    // 3. 코멘트 입력
    char comment[100];
    gotoxy(20, 14); printf("한줄 코멘트: ");
    fgets(comment, sizeof(comment), stdin);
    comment[strcspn(comment, "\n")] = 0; // 줄바꿈 문자 제거

    // 4. 저장 처리 (여기서 파일 저장이나 구조체 저장을 수행)
    // 현재는 저장되었다고 가정하고 메시지만 출력
    gotoxy(20, 17); printf(">> %s님에 대한 평가 저장 완료!", target_id);
    Sleep(1500);
}

// ================================================================
// 2. [관리자용] 급여 관리 (수동 입력 모드) - 메뉴 3번
// ================================================================
void run_salary_eval_admin(User* admin) {
    while (1) {
        system("cls");
        draw_box(15, 5, 50, 16, " [관리자] 급여 수동 관리 ");

        draw_button(20, 8, "1. 급여 정보 수동 입력/수정", 0);
        draw_button(20, 10, "2. 급여 데이터 파일 저장   ", 0);
        draw_button(20, 14, "0. 뒤로 가기               ", 0);

        int x, y;
        if (get_mouse_click(&x, &y)) {
            if (x >= 20 && x <= 50) {
                if (y == 8) {
                    // [수동 입력 로직]
                    // 테스트를 위해 '현재 로그인한 관리자'의 급여를 수정해봅니다.
                    // 실제 완성 시에는 '직원 검색' -> '선택' -> '수정' 순서가 되어야 합니다.

                    system("cls");
                    draw_box(15, 8, 50, 10, " 급여 입력 ");
                    gotoxy(20, 12); printf("현재 설정된 급여: %d 만원", admin->salary);
                    gotoxy(20, 14); printf("수정할 금액 입력: ");

                    int input_salary = 0;
                    scanf("%d", &input_salary);
                    while (getchar() != '\n'); // 버퍼 비우기

                    admin->salary = input_salary; // 입력값 바로 저장

                    gotoxy(20, 16); printf(">> 급여가 %d만원으로 설정되었습니다.", admin->salary);
                    Sleep(1000);
                }
                else if (y == 10) {
                    gotoxy(20, 17); printf(">> 파일 저장 기능 (구현 예정)      ");
                    Sleep(1000);
                }
                else if (y == 14) return; // 메뉴로 복귀
            }
        }
    }
}

// ================================================================
// 3. [직원용] 급여 조회 및 인사 평가 진입 - 메뉴 3번
// ================================================================
void run_salary_eval_employee(User* my_user) {
    while (1) {
        system("cls");
        draw_box(10, 4, 60, 20, " 급여 조회 / 인사 평가 ");

        // [급여 명세서 출력]
        // 관리자가 입력해둔 my_user->salary 값을 그대로 보여줍니다.
        set_color(11, 0);
        gotoxy(15, 7); printf("=== %s %s님의 급여 정보 ===", my_user->name, my_user->position);
        set_color(15, 0);

        gotoxy(15, 10); printf("----------------------------------------");

        set_color(14, 0); // 노란색 강조
        gotoxy(15, 12); printf("▶ 이번 달 급여 : %d 만원", my_user->salary);
        set_color(15, 0);

        gotoxy(15, 14); printf("----------------------------------------");

        // [인사 평가 버튼]
        // 상사(부장, 차장, 과장)인 경우에만 평가하기 버튼을 보여줍니다.
        int is_boss = (strcmp(my_user->position, "부장") == 0 ||
            strcmp(my_user->position, "차장") == 0 ||
            strcmp(my_user->position, "과장") == 0);

        if (is_boss) {
            draw_button(45, 17, " 팀원 평가하기 ", 0);
        }

        gotoxy(15, 20); printf("[ESC] 뒤로 가기");

        int x, y;
        // 마우스 클릭 처리
        if (get_mouse_click(&x, &y)) {
            // '팀원 평가하기' 버튼 클릭 시 (좌표 범위 체크)
            if (is_boss && y == 17 && x >= 45 && x <= 60) {
                // 여기서 위에 만들어둔 평가 입력 함수를 호출합니다.
                input_team_evaluation(my_user);
            }
        }

        // 키보드(ESC) 처리
        if (kbhit()) {
            if (getch() == 27) return; // ESC 누르면 종료
        }
    }
}