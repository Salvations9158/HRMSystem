/*
담당자: 안도혁
최근 업데이트: 2025.11.30 / 19:50
진행상태:
1-1 직원 등록 기능 구현 완료
1-2 직원 수정 기능 구현 완료
1-3 직원 검색 기능 구현 완료 (employee_info.c 로직 재사용)
*/

/**/
#define _CRT_SECURE_NO_WARNINGS
#include "common.h"

// ------------------------------------------------------------------
// 상수 정의
// ------------------------------------------------------------------
#define BOX_X 5
#define BOX_Y 3
#define BOX_W 70
#define BOX_H 18
#define MENU_X 24

#define BTN_Y_1 6  // 1. 직원 등록
#define BTN_Y_2 8  // 2. 직원 수정
#define BTN_Y_3 10 // 3. 직원 검색
#define BTN_Y_0 12 // 0. 이전 메뉴로
#define BTN_WIDTH 30 

// 등록/수정/검색 UI 박스 좌표 (일반 직원용과 동일한 좌표 사용)
#define INFO_BOX_X 2
#define INFO_BOX_Y 5
#define INFO_BOX_W 76
#define INFO_BOX_H 21

#define LIST_START_Y (INFO_BOX_Y + 7) // 헤더 기준선 (Y=12)

// UI 출력 로직을 재사용하기 위해 매크로 블록으로 정의
#define DRAW_ADMIN_MENU_UI() \
system("cls"); \
set_color(0, 15); \
gotoxy(0, 0); \
for (int i = 0; i < 80; i++) printf(" "); \
gotoxy(2, 0); \
printf("컴공상사 HR System | 관리자: %s %s (%s)", user->name, user->position, user->department); \
set_color(15, 0); \
draw_box(BOX_X, BOX_Y, BOX_W, BOX_H, "1. 직원 데이터 관리"); \
draw_button(MENU_X, BTN_Y_1, "1. 직원 등록", 0); \
draw_button(MENU_X, BTN_Y_2, "2. 직원 수정", 0); \
draw_button(MENU_X, BTN_Y_3, "3. 직원 검색", 0); \
draw_button(MENU_X, BTN_Y_0, "0. 이전 메뉴로", 0); 


// ------------------------------------------------------------------
// 유틸리티 함수: ID 중복 검사
// ------------------------------------------------------------------
// ID가 User.txt에 존재하는지 확인 (1: 존재함, 0: 존재하지 않음)
int is_id_exist(const char* id) {
    FILE* fp;
    User temp;

    if (fopen_s(&fp, "User.txt", "r") != 0 || fp == NULL) {
        return 0; // 파일이 없으면 존재하지 않음으로 간주
    }

    while (fscanf_s(fp, "%s %s %s %s %s %s %s %d",
        temp.id, (unsigned)sizeof(temp.id),
        temp.pw, (unsigned)sizeof(temp.pw),
        temp.name, (unsigned)sizeof(temp.name),
        temp.department, (unsigned)sizeof(temp.department),
        temp.position, (unsigned)sizeof(temp.position),
        temp.phone, (unsigned)sizeof(temp.phone),
        temp.hire_date, (unsigned)sizeof(temp.hire_date),
        &temp.salary) != EOF)
    {
        if (strcmp(temp.id, id) == 0) {
            fclose(fp);
            return 1; // ID 발견
        }
    }
    fclose(fp);
    return 0; // ID 없음
}


// ------------------------------------------------------------------
// 1. 직원 등록 기능
// ------------------------------------------------------------------
void register_employee(User* user) {
    User new_user;
    FILE* fp;
    char input_buffer[50];
    int y_pos = INFO_BOX_Y + 2;

    // --- 커서 제어를 위한 변수 선언 ---
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo = { 100, TRUE }; // 커서 보이기 설정 (dwSize, TRUE)
    CONSOLE_CURSOR_INFO cursorHide = { 100, FALSE }; // 커서 숨기기 설정 (dwSize, FALSE)

    // UI박스 그리기
    system("cls");
    set_color(0, 15);
    gotoxy(0, 0);
    for (int i = 0; i < 80; i++) printf(" ");
    gotoxy(2, 0);
    printf("컴공상사 HR System | 사용자: %s %s (%s)", user->name, user->position, user->department);
    draw_box(INFO_BOX_X, INFO_BOX_Y, INFO_BOX_W, INFO_BOX_H, "1-1. 직원 등록");

    set_color(15, 0);
    gotoxy(INFO_BOX_X + 2, y_pos); printf("ID: ");
    gotoxy(INFO_BOX_X + 2, y_pos + 2); printf("PW: ");
    gotoxy(INFO_BOX_X + 2, y_pos + 4); printf("이름: ");
    gotoxy(INFO_BOX_X + 2, y_pos + 6); printf("부서: ");
    gotoxy(INFO_BOX_X + 2, y_pos + 8); printf("직급: ");
    gotoxy(INFO_BOX_X + 2, y_pos + 10); printf("연락처: ");
    gotoxy(INFO_BOX_X + 2, y_pos + 12); printf("입사일 (YYYY-MM-DD): ");

    set_color(14, 0); // 입력 필드 색상 변경

    // ID 입력 및 중복 검사 루프
    while (1) {
        gotoxy(INFO_BOX_X + 25, y_pos); printf("                          ");
        SetConsoleCursorInfo(hConsole, &cursorInfo); // 커서 보이게
        gotoxy(INFO_BOX_X + 25, y_pos); scanf_s("%s", new_user.id, (unsigned)sizeof(new_user.id));
        while (getchar() != '\n');
        SetConsoleCursorInfo(hConsole, &cursorHide); // 커서 다시 안 보이게

        if (is_id_exist(new_user.id)) {
            gotoxy(INFO_BOX_X + 55, y_pos); set_color(12, 0); printf("중복된 ID입니다."); set_color(14, 0);
        }
        else {
            gotoxy(INFO_BOX_X + 40, y_pos); printf("                                "); // 메시지 지우기
            break;
        }
    }

    // PW 입력
    gotoxy(INFO_BOX_X + 25, y_pos + 2);
    SetConsoleCursorInfo(hConsole, &cursorInfo); // 커서 보이기
    scanf_s("%s", new_user.pw, (unsigned)sizeof(new_user.pw));
    while (getchar() != '\n');
    SetConsoleCursorInfo(hConsole, &cursorHide); // 커서 숨기기

    // 이름 입력
    gotoxy(INFO_BOX_X + 25, y_pos + 4);
    SetConsoleCursorInfo(hConsole, &cursorInfo); 
    scanf_s("%s", new_user.name, (unsigned)sizeof(new_user.name));
    while (getchar() != '\n');
    SetConsoleCursorInfo(hConsole, &cursorHide); 

    // 부서 입력
    gotoxy(INFO_BOX_X + 25, y_pos + 6);
    SetConsoleCursorInfo(hConsole, &cursorInfo); 
    scanf_s("%s", new_user.department, (unsigned)sizeof(new_user.department));
    while (getchar() != '\n');
    SetConsoleCursorInfo(hConsole, &cursorHide); 

    // 직급 입력
    gotoxy(INFO_BOX_X + 25, y_pos + 8);
    SetConsoleCursorInfo(hConsole, &cursorInfo); 
    scanf_s("%s", new_user.position, (unsigned)sizeof(new_user.position));
    while (getchar() != '\n');
    SetConsoleCursorInfo(hConsole, &cursorHide); 

    // 연락처 입력
    gotoxy(INFO_BOX_X + 25, y_pos + 10);
    SetConsoleCursorInfo(hConsole, &cursorInfo); 
    scanf_s("%s", new_user.phone, (unsigned)sizeof(new_user.phone));
    while (getchar() != '\n');
    SetConsoleCursorInfo(hConsole, &cursorHide); 

    // 입사일 입력
    gotoxy(INFO_BOX_X + 25, y_pos + 12);
    SetConsoleCursorInfo(hConsole, &cursorInfo); 
    scanf_s("%s", new_user.hire_date, (unsigned)sizeof(new_user.hire_date));
    while (getchar() != '\n');
    SetConsoleCursorInfo(hConsole, &cursorHide); 

    // 연봉 초기화
    new_user.salary = 0; // 급여는 급여관리에서 따로 다루므로, 일단 0원으로 초기화

    set_color(15, 0);

    // User.txt 파일에 추가 (Append 모드)
    if (fopen_s(&fp, "User.txt", "a") == 0 && fp != NULL) {
        fprintf(fp, "%s %s %s %s %s %s %s %d\n",
            new_user.id, new_user.pw, new_user.name, new_user.department,
            new_user.position, new_user.phone, new_user.hire_date, new_user.salary);
        fclose(fp);

        gotoxy(INFO_BOX_X + 2, INFO_BOX_Y + INFO_BOX_H + 1);
        set_color(11, 0);
        printf("[완료] 직원 (%s, %s) 등록이 완료되었습니다.", new_user.name, new_user.id);
    }
    else {
        gotoxy(INFO_BOX_X + 2, INFO_BOX_Y + INFO_BOX_H + 1);
        set_color(12, 0);
        printf("[오류] User.txt 파일을 열 수 없습니다. 등록 실패.");
    }

    // 키 입력 대기
    gotoxy(INFO_BOX_X + 2, INFO_BOX_Y + INFO_BOX_H + 3);
    set_color(11, 0);
    printf(">> 아무 키나 누르시면 이전 메뉴로 돌아갑니다.");
    _getch();
}



// ------------------------------------------------------------------
// 2. 직원 수정 기능
// ------------------------------------------------------------------
void edit_employee(User* user) {
    char target_id[20];
    User user_list[100]; // 최대 100명 가정
    int user_count = 0;
    int target_index = -1;
    FILE* fp, * temp_fp;
    int y_pos = INFO_BOX_Y + 2;

    // --- 커서 제어를 위한 변수 선언 ---
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo = { 100, TRUE }; // 커서 보이기 설정 (bVisible = TRUE)
    CONSOLE_CURSOR_INFO cursorHide = { 100, FALSE }; // 커서 숨기기 설정 (bVisible = FALSE)

    // UI박스 그리기
    system("cls");
    set_color(0, 15);
    gotoxy(0, 0);
    for (int i = 0; i < 80; i++) printf(" ");
    gotoxy(2, 0);
    printf("컴공상사 HR System | 사용자: %s %s (%s)", user->name, user->position, user->department);
    draw_box(INFO_BOX_X, INFO_BOX_Y, INFO_BOX_W, INFO_BOX_H, "1-2. 직원 수정");
    set_color(15, 0);

    // 1. 수정할 ID 입력 받기
    gotoxy(INFO_BOX_X + 2, y_pos); printf("수정할 직원의 ID를 입력하세요: ");
    set_color(14, 0);
    gotoxy(INFO_BOX_X + 35, y_pos); 
    SetConsoleCursorInfo(hConsole, &cursorInfo); // 커서 보이게
    scanf_s("%s", target_id, (unsigned)sizeof(target_id));
    while (getchar() != '\n');
    SetConsoleCursorInfo(hConsole, &cursorHide); // 다시 커서 안 보이게
    set_color(15, 0);

    // 2. User.txt 파일을 메모리로 로드하고 ID 검색
    if (fopen_s(&fp, "User.txt", "r") == 0 && fp != NULL) {
        while (fscanf_s(fp, "%s %s %s %s %s %s %s %d",
            user_list[user_count].id, (unsigned)sizeof(user_list[user_count].id),
            user_list[user_count].pw, (unsigned)sizeof(user_list[user_count].pw),
            user_list[user_count].name, (unsigned)sizeof(user_list[user_count].name),
            user_list[user_count].department, (unsigned)sizeof(user_list[user_count].department),
            user_list[user_count].position, (unsigned)sizeof(user_list[user_count].position),
            user_list[user_count].phone, (unsigned)sizeof(user_list[user_count].phone),
            user_list[user_count].hire_date, (unsigned)sizeof(user_list[user_count].hire_date),
            &user_list[user_count].salary) != EOF)
        {
            if (strcmp(user_list[user_count].id, target_id) == 0) {
                target_index = user_count;
            }
            user_count++;
        }
        fclose(fp);
    }

    if (target_index == -1) {
        gotoxy(INFO_BOX_X + 2, y_pos + 2); set_color(12, 0); printf("[실패] 해당 ID를 가진 직원을 찾을 수 없습니다.");
    }
    else {

        // 3. 수정 또는 삭제 선택
        char choice;

        // 선택지 UI
        gotoxy(INFO_BOX_X + 2, y_pos + 2); printf("직원 ID: %s (%s)", target_id, user_list[target_index].name);
        gotoxy(INFO_BOX_X + 2, y_pos + 3); printf("작업을 선택하세요: [1] 수정, [2] 삭제 : ");
        set_color(14, 0);
        gotoxy(INFO_BOX_X + 50, y_pos + 3);

        SetConsoleCursorInfo(hConsole, &cursorInfo);
        scanf_s(" %c", &choice, 1); // 공백을 넣어 이전 입력 버퍼의 개행 문자를 건너뜁니다.
        while (getchar() != '\n');
        SetConsoleCursorInfo(hConsole, &cursorHide);
        set_color(15, 0);

        // 4. 선택 분기 처리
        if (choice == '1') {
            // 4.1 직원 데이터 수정

            // 4.1.1 수정할 필드 입력 받기
            User* target_user = &user_list[target_index];

            gotoxy(INFO_BOX_X + 2, y_pos + 3); set_color(11, 0); printf("[%s] 수정할 항목의 새 값을 입력하세요 :", target_user->name);
            set_color(15, 0);

            char input_buffer[50];

            // PW 수정
            gotoxy(INFO_BOX_X + 2, y_pos + 5); printf("                                                              ");
            gotoxy(INFO_BOX_X + 2, y_pos + 5); printf("PW (현재: %s): ", target_user->pw);
            set_color(14, 0);
            gotoxy(INFO_BOX_X + 35, y_pos + 5); printf("                                      ");
            gotoxy(INFO_BOX_X + 35, y_pos + 5);
            SetConsoleCursorInfo(hConsole, &cursorInfo); // 커서 보이게
            scanf_s("%s", input_buffer, (unsigned)sizeof(input_buffer));
            while (getchar() != '\n');
            SetConsoleCursorInfo(hConsole, &cursorHide); // 커서 안보이게
            if (strlen(input_buffer) > 0) {
                // strcpy_s 사용 (대상, 대상 크기, 소스)
                strcpy_s(target_user->pw, sizeof(target_user->pw), input_buffer);
            }
            set_color(15, 0);
            y_pos += 2;

            // 이름 수정
            gotoxy(INFO_BOX_X + 2, y_pos + 5); printf("                                                              ");
            gotoxy(INFO_BOX_X + 2, y_pos + 5); printf("이름 (현재: %s): ", target_user->name);
            set_color(14, 0);
            gotoxy(INFO_BOX_X + 35, y_pos + 5); printf("                                      ");
            gotoxy(INFO_BOX_X + 35, y_pos + 5);
            SetConsoleCursorInfo(hConsole, &cursorInfo); // 커서 보이게
            scanf_s("%s", input_buffer, (unsigned)sizeof(input_buffer));
            while (getchar() != '\n');
            SetConsoleCursorInfo(hConsole, &cursorHide); // 커서 안보이게
            if (strlen(input_buffer) > 0) {
                // strcpy_s 사용 (대상, 대상 크기, 소스)
                strcpy_s(target_user->name, sizeof(target_user->name), input_buffer);
            }
            set_color(15, 0);
            y_pos += 2;

            // 부서 수정
            gotoxy(INFO_BOX_X + 2, y_pos + 5); printf("                                                              ");
            gotoxy(INFO_BOX_X + 2, y_pos + 5); printf("부서 (현재: %s): ", target_user->department);
            set_color(14, 0);
            gotoxy(INFO_BOX_X + 35, y_pos + 5); printf("                                      ");
            gotoxy(INFO_BOX_X + 35, y_pos + 5);
            SetConsoleCursorInfo(hConsole, &cursorInfo); // 커서 보이게
            scanf_s("%s", input_buffer, (unsigned)sizeof(input_buffer));
            while (getchar() != '\n');
            SetConsoleCursorInfo(hConsole, &cursorHide); // 커서 안보이게
            if (strlen(input_buffer) > 0) {
                // strcpy_s 사용 (대상, 대상 크기, 소스)
                strcpy_s(target_user->department, sizeof(target_user->department), input_buffer);
            }
            set_color(15, 0);
            y_pos += 2;

            // 직급 수정
            gotoxy(INFO_BOX_X + 2, y_pos + 5); printf("                                                              ");
            gotoxy(INFO_BOX_X + 2, y_pos + 5); printf("직급 (현재: %s): ", target_user->position);
            set_color(14, 0);
            gotoxy(INFO_BOX_X + 35, y_pos + 5); printf("                                      ");
            gotoxy(INFO_BOX_X + 35, y_pos + 5);
            SetConsoleCursorInfo(hConsole, &cursorInfo); // 커서 보이게
            scanf_s("%s", input_buffer, (unsigned)sizeof(input_buffer));
            while (getchar() != '\n');
            SetConsoleCursorInfo(hConsole, &cursorHide); // 커서 안보이게
            if (strlen(input_buffer) > 0) {
                strcpy_s(target_user->position, sizeof(target_user->position), input_buffer);
            }
            set_color(15, 0);
            y_pos += 2;

            // 연락처 수정
            gotoxy(INFO_BOX_X + 2, y_pos + 5); printf("                                                              ");
            gotoxy(INFO_BOX_X + 2, y_pos + 5); printf("연락처 (현재: %s): ", target_user->phone);
            set_color(14, 0);
            gotoxy(INFO_BOX_X + 35, y_pos + 5); printf("                                      ");
            gotoxy(INFO_BOX_X + 35, y_pos + 5);
            SetConsoleCursorInfo(hConsole, &cursorInfo); // 커서 보이게
            scanf_s("%s", input_buffer, (unsigned)sizeof(input_buffer));
            while (getchar() != '\n');
            SetConsoleCursorInfo(hConsole, &cursorHide); // 커서 안보이게
            if (strlen(input_buffer) > 0) {
                strcpy_s(target_user->phone, sizeof(target_user->phone), input_buffer);
            }
            set_color(15, 0);
            y_pos += 2;

            // 입사일 수정
            gotoxy(INFO_BOX_X + 2, y_pos + 5); printf("                                                              ");
            gotoxy(INFO_BOX_X + 2, y_pos + 5); printf("입사일 (현재: %s): ", target_user->hire_date);
            set_color(14, 0);
            gotoxy(INFO_BOX_X + 35, y_pos + 5); printf("                                      ");
            gotoxy(INFO_BOX_X + 35, y_pos + 5);
            SetConsoleCursorInfo(hConsole, &cursorInfo); // 커서 보이게
            scanf_s("%s", input_buffer, (unsigned)sizeof(input_buffer));
            while (getchar() != '\n');
            SetConsoleCursorInfo(hConsole, &cursorHide); // 커서 안보이게
            if (strlen(input_buffer) > 0) {
                strcpy_s(target_user->hire_date, sizeof(target_user->hire_date), input_buffer);
            }
            set_color(15, 0);

            // 4.1.2 임시 파일에 수정된 전체 데이터 쓰기
            if (fopen_s(&temp_fp, "User_temp.txt", "w") == 0 && temp_fp != NULL) {
                for (int i = 0; i < user_count; i++) {
                    fprintf(temp_fp, "%s %s %s %s %s %s %s %d\n",
                        user_list[i].id, user_list[i].pw, user_list[i].name, user_list[i].department,
                        user_list[i].position, user_list[i].phone, user_list[i].hire_date, user_list[i].salary);
                }
                fclose(temp_fp);

                // 4.1.3 파일 교체
                remove("User.txt");
                rename("User_temp.txt", "User.txt");

                gotoxy(INFO_BOX_X + 2, INFO_BOX_Y + INFO_BOX_H + 1);
                set_color(11, 0);
                printf("[완료] 직원 (%s, %s) 정보 수정이 완료되었습니다.", target_user->name, target_user->id);
            }
            else {
                gotoxy(INFO_BOX_X + 2, INFO_BOX_Y + INFO_BOX_H + 1);
                set_color(12, 0);
                printf("[오류] 임시 파일을 생성할 수 없습니다. 수정 실패.");
            }
        }
        else if (choice == '2') {
            // 4.2 직원 데이터 삭제
            
            // UI 정리
            gotoxy(INFO_BOX_X + 2, y_pos + 4); printf("                                                                           "); // 선택지 지우기

            // 4. 임시 파일에 삭제된 직원을 제외한 전체 데이터 쓰기
            if (fopen_s(&temp_fp, "User_temp.txt", "w") == 0 && temp_fp != NULL) {
                for (int i = 0; i < user_count; i++) {
                    // 삭제 대상인 target_index를 건너뜁니다.
                    if (i != target_index) {
                        fprintf(temp_fp, "%s %s %s %s %s %s %s %d\n",
                            user_list[i].id, user_list[i].pw, user_list[i].name,
                            user_list[i].department, user_list[i].position,
                            user_list[i].phone, user_list[i].hire_date, user_list[i].salary);
                    }
                }
                fclose(temp_fp);
                remove("User.txt");
                rename("User_temp.txt", "User.txt");

                gotoxy(INFO_BOX_X + 2, INFO_BOX_Y + INFO_BOX_H + 1);
                set_color(11, 0);
                printf("[완료] 직원 ID: %s (%s) 정보가 성공적으로 삭제되었습니다.", target_id, user_list[target_index].name);
            }
            else {
                gotoxy(INFO_BOX_X + 2, INFO_BOX_Y + INFO_BOX_H + 1);
                set_color(12, 0);
                printf("[오류] 임시 파일을 생성할 수 없습니다. 삭제 실패.");
            }
        }
        else {
            // 잘못된 선택 처리
            gotoxy(INFO_BOX_X + 2, INFO_BOX_Y + INFO_BOX_H + 1);
            set_color(12, 0);
            printf("[경고] 잘못된 선택입니다. (1 또는 2만 입력 가능)");
        }
    }

    // 키 입력 대기
    gotoxy(INFO_BOX_X + 2, INFO_BOX_Y + INFO_BOX_H + 3);
    set_color(11, 0);
    printf(">> 아무 키나 누르시면 이전 메뉴로 돌아갑니다.");
    _getch();
}


// ------------------------------------------------------------------
// 3. 직원 검색 기능 (employee_info.c 로직 재사용)
// ------------------------------------------------------------------

// 주의: 이 함수는 employee_info.c에서 복사해 온 것이며,
//       User* user 인수는 사용되지 않을 수 있습니다.
void admin_list_employees(User* user)
{
    char search_term[50];
    // 이 좌표는 employee_info.c와 동일한 상수를 사용해야 일관성이 유지됨
    int line_y = LIST_START_Y;
    int result_count = 0;

    // UI박스 그리기
    system("cls");
    set_color(0, 15);
    gotoxy(0, 0);
    for (int i = 0; i < 80; i++) printf(" ");
    gotoxy(2, 0);
    printf("컴공상사 HR System | 사용자: %s %s (%s)", user->name, user->position, user->department);
    draw_box(INFO_BOX_X, INFO_BOX_Y, INFO_BOX_W, INFO_BOX_H, "1-3. 직원 검색 (이름/부서/직급 검색)");

    // 검색 안내 메시지 (SEARCH_Y = 7)
    gotoxy(INFO_BOX_X + 2, INFO_BOX_Y + 2); // LIST_BOX_X 대신 INFO_BOX_X 사용
    set_color(14, 0); // 노란색
    printf("검색어 입력 (이름/부서/직급)        # '.'(온점) 입력 시 이전 메뉴로 이동");

    // 헤더 출력 (LIST_START_Y = 12 기준)
    set_color(15, 0);
    gotoxy(INFO_BOX_X + 2, line_y - 2); // Y=10
    printf("-------------------------------------------------------------------------");
    set_color(10, 0); // 연한 녹색
    gotoxy(INFO_BOX_X + 5, line_y - 1);  printf("이름"); // Y=11
    gotoxy(INFO_BOX_X + 20, line_y - 1); printf("부서");
    gotoxy(INFO_BOX_X + 35, line_y - 1); printf("직급");
    gotoxy(INFO_BOX_X + 50, line_y - 1); printf("연락처");
    set_color(15, 0);
    gotoxy(INFO_BOX_X + 2, line_y - 0); // Y=12
    printf("-------------------------------------------------------------------------");

    while (1)
    {
        // 1. 검색어 입력 받기
        gotoxy(INFO_BOX_X + 7, INFO_BOX_Y + 4); // Y=9 대신 Y=9에 해당하는 INFO_BOX_Y + 4 사용
        printf("검색어:                                        ");

        gotoxy(INFO_BOX_X + 15, INFO_BOX_Y + 4);
        set_color(14, 0);

        // 커서 보이기 설정
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_CURSOR_INFO cursorInfo = { 100, TRUE };
        SetConsoleCursorInfo(hConsole, &cursorInfo);

        scanf_s("%s", search_term, (unsigned)sizeof(search_term));
        while (getchar() != '\n'); // 엔터 버퍼 제거

        // 커서 숨기기 설정
        cursorInfo.bVisible = FALSE;
        SetConsoleCursorInfo(hConsole, &cursorInfo);
        set_color(15, 0);

        // 2. 종료 조건 확인
        if (strcmp(search_term, ".") == 0)
        {
            gotoxy(INFO_BOX_X + 2, INFO_BOX_Y + INFO_BOX_H + 1);
            printf("                                                                         ");
            gotoxy(INFO_BOX_X + 2, INFO_BOX_Y + INFO_BOX_H + 1);
            set_color(11, 0);
            printf(">> 이전 메뉴로 돌아갑니다.");
            Sleep(1000);
            return;
        }

        // 3. 이전 검색 결과 지우기
        for (int i = 0; i < (INFO_BOX_H - 8); i++) // 10줄 정도 지움
        {
            gotoxy(INFO_BOX_X + 2, LIST_START_Y + 1 + i); // Y=13 부터 시작
            printf("                                                                        ");
        }
        result_count = 0;

        // 4. 파일 검색 및 결과 출력
        FILE* fp = NULL;
        User temp;

        if (fopen_s(&fp, "User.txt", "r") == 0 && fp != NULL)
        {
            int current_y = LIST_START_Y + 1; // Y=13

            while (fscanf_s(fp, "%s %s %s %s %s %s %s %d",
                temp.id, (unsigned)sizeof(temp.id),
                temp.pw, (unsigned)sizeof(temp.pw),
                temp.name, (unsigned)sizeof(temp.name),
                temp.department, (unsigned)sizeof(temp.department),
                temp.position, (unsigned)sizeof(temp.position),
                temp.phone, (unsigned)sizeof(temp.phone),
                temp.hire_date, (unsigned)sizeof(temp.hire_date),
                &temp.salary) != EOF)
            {
                if (strstr(temp.name, search_term) != NULL ||
                    strstr(temp.department, search_term) != NULL ||
                    strstr(temp.position, search_term) != NULL)
                {
                    if (current_y >= INFO_BOX_Y + INFO_BOX_H - 1) {
                        break;
                    }

                    set_color(15, 0);
                    gotoxy(INFO_BOX_X + 5, current_y);  printf("%-10s", temp.name);
                    gotoxy(INFO_BOX_X + 20, current_y); printf("%-10s", temp.department);
                    gotoxy(INFO_BOX_X + 35, current_y); printf("%-10s", temp.position);
                    gotoxy(INFO_BOX_X + 50, current_y); printf("%s", temp.phone);

                    current_y++;
                    result_count++;
                }
            }
            fclose(fp);
        }

        // 5. 검색 결과 메시지 출력
        gotoxy(INFO_BOX_X + 2, INFO_BOX_Y + INFO_BOX_H + 1);
        printf("                                                                         ");
        gotoxy(INFO_BOX_X + 2, INFO_BOX_Y + INFO_BOX_H + 1);
        set_color(11, 0);
        printf(">> [알림] 총 %d건의 검색 결과를 찾았습니다. 새로운 검색어를 입력하세요.", result_count);
        set_color(15, 0);
    }
}


// ------------------------------------------------------------------
// admin_show_info_menu: 관리자 메뉴 UI 및 입력 처리
// ------------------------------------------------------------------
void show_admin_info_menu(User* user)
{
    int mx, my; // 마우스 좌표
    gotoxy(0, 0);

    // 1. 초기 UI 출력
    DRAW_ADMIN_MENU_UI();

    // 2. 입력 루프
    while (1)
    {
        HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
        DWORD mode;
        GetConsoleMode(hInput, &mode);
        SetConsoleMode(hInput, (mode & ~ENABLE_QUICK_EDIT_MODE) | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);

        if (get_mouse_click(&mx, &my))
        {
            // 1. 직원 등록
            if (my == BTN_Y_1 && mx >= MENU_X && mx <= MENU_X + BTN_WIDTH)
            {
                gotoxy(MENU_X, BOX_Y + 14);
                set_color(11, 0);
                printf(">> 1. 직원 등록 실행                 ");
                Sleep(500);
                register_employee(user);
                DRAW_ADMIN_MENU_UI();
            }
            // 2. 직원 수정
            else if (my == BTN_Y_2 && mx >= MENU_X && mx <= MENU_X + BTN_WIDTH)
            {
                gotoxy(MENU_X, BOX_Y + 14);
                set_color(11, 0);
                printf(">> 2. 직원 수정 실행                 ");
                Sleep(500);
                edit_employee(user);
                DRAW_ADMIN_MENU_UI();
            }
            // 3. 직원 검색
            else if (my == BTN_Y_3 && mx >= MENU_X && mx <= MENU_X + BTN_WIDTH)
            {
                gotoxy(MENU_X, BOX_Y + 14);
                set_color(11, 0);
                printf(">> 3. 직원 검색 실행                 ");
                Sleep(500);
                admin_list_employees(user);
                DRAW_ADMIN_MENU_UI();
            }
            // 0. 이전 메뉴로
            else if (my == BTN_Y_0 && mx >= MENU_X && mx <= MENU_X + BTN_WIDTH)
            {
                gotoxy(MENU_X, BOX_Y + 14);
                set_color(11, 0);
                printf(">> 0. 이전 메뉴로 복귀               ");
                Sleep(500);
                return;
            }
        }
    }
}