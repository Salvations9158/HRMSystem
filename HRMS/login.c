/*
담당자: 전민규
최근 업데이트: 2025.12.01 / 12:11
진행상태:
1-0 'User.txt' 파일이 없을 경우 더미 데이터 생성 기능 구현 완료
1-1 로그인 프로세스 기능 구현 완료
1-2 ID를 통한 패스워드 찾기 기능 구현 완료
*/
#include "common.h"

// 테스트를 위한 더미 데이터 생성
void create_dummy_data()
{
    FILE* fp = NULL;
    errno_t err;

    err = fopen_s(&fp, "User.txt", "r");

    if (err != 0 || fp == NULL)
    {
        err = fopen_s(&fp, "User.txt", "w");
        if (err == 0 && fp != NULL)
        {
            // 연봉 데이터 포함하여 저장
            fprintf(fp, "admin 1234 전민규 관리팀 관리자 010-1111-2222 2010-01-01 4000000\n");
            fprintf(fp, "worker01 1234 안도혁 개발팀 대리 010-1234-5678 2025-11-01 3000000\n");
            printf("초기 데이터 파일(User.txt)을 생성했습니다.\n");
            fclose(fp);
        }
    }
    else
    {
        fclose(fp);
    }
}

// 로그인 프로세스
int login_process(User* login_user)
{
    int mx, my;
    char input_id[20] = "";
    char input_pw[20] = "";
    int is_running = 1;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE); // 입력 핸들 추가
    CONSOLE_CURSOR_INFO cursorInfo;

    // UI 좌표 설정
    int box_x = 20, box_y = 5;
    int input_label_x = 26;
    int input_field_x = 32;
    int btn_start_x = 24;

    system("cls");

    // 1. 기본 UI 그리기
    draw_box(box_x, box_y, 40, 14, "로그인");
    gotoxy(25, box_y + 3); printf(" 컴공상사 인사관리 프로그램 ");

    gotoxy(input_label_x, box_y + 6); printf("ID  : ");
    gotoxy(input_label_x, box_y + 8); printf("PW  : ");

    draw_button(btn_start_x, box_y + 11, "로그인", 0);
    draw_button(btn_start_x + 11, box_y + 11, "PW찾기", 0);
    draw_button(btn_start_x + 22, box_y + 11, " 종 료 ", 0);

    // ---------------------------------------------------------
    // [초기 자동 입력 모드임]
    // ---------------------------------------------------------

    // (1) ID 입력 자동 진입
    cursorInfo.dwSize = 100;
    cursorInfo.bVisible = TRUE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);

    gotoxy(input_field_x, box_y + 6);
    set_color(14, 0); // 노란색
    scanf_s("%s", input_id, (unsigned)sizeof(input_id));
    while (getchar() != '\n'); // 엔터 버퍼 제거 버퍼 제거 함수 만들까 고민중

    set_color(15, 0);
    gotoxy(input_label_x, box_y + 6);
    printf("ID  : %-15s", input_id);

    // (2) PW 입력 자동 진입
    gotoxy(input_field_x, box_y + 8);
    set_color(14, 0);
    scanf_s("%s", input_pw, (unsigned)sizeof(input_pw));
    while (getchar() != '\n');

    set_color(15, 0);
    gotoxy(input_label_x, box_y + 8);

    // 커서 끄기
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);

    gotoxy(22, box_y + 13);
    set_color(11, 0);
    printf(">> [로그인] 버튼을 눌러주세요.");
    set_color(15, 0);

    // [중요******] 입력 버퍼 비우기 & 마우스 모드 재설정
    // scanf 사용 후 남아있을 수 있는 엔터 키 이벤트 등을 제거합니다.
    FlushConsoleInputBuffer(hInput);

    //모드 풀리니까 다시 설정해줘야됨 안해주면 마우스 클릭이벤트 마비
    DWORD mode;
    GetConsoleMode(hInput, &mode);
    SetConsoleMode(hInput, (mode & ~ENABLE_QUICK_EDIT_MODE) | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);

    // ---------------------------------------------------------
    // [마우스 이벤트 루프 (아이디나 비밀번호 재입력)]
    // ---------------------------------------------------------
    while (is_running)
    {
        if (get_mouse_click(&mx, &my))
        {

            // 1. ID 입력칸 클릭 (수정 시)
            if (my == box_y + 6 && mx >= input_label_x && mx <= input_label_x + 25)
            {
                cursorInfo.bVisible = TRUE; SetConsoleCursorInfo(hConsole, &cursorInfo);

                gotoxy(input_field_x, box_y + 6); printf("                    ");
                gotoxy(input_field_x, box_y + 6); set_color(14, 0);

                scanf_s("%s", input_id, (unsigned)sizeof(input_id));
                while (getchar() != '\n');

                cursorInfo.bVisible = FALSE; SetConsoleCursorInfo(hConsole, &cursorInfo);
                set_color(15, 0);
                gotoxy(input_label_x, box_y + 6); printf("ID  : %-15s", input_id);
                FlushConsoleInputBuffer(hInput); // 수정 후에도 버퍼 비우기
            }
            // 2. PW 입력칸 클릭 (수정 시)
            else if (my == box_y + 8 && mx >= input_label_x && mx <= input_label_x + 25)
            {
                cursorInfo.bVisible = TRUE; SetConsoleCursorInfo(hConsole, &cursorInfo);

                gotoxy(input_field_x, box_y + 8); printf("                    ");
                gotoxy(input_field_x, box_y + 8); set_color(14, 0);

                scanf_s("%s", input_pw, (unsigned)sizeof(input_pw));
                while (getchar() != '\n');

                cursorInfo.bVisible = FALSE; SetConsoleCursorInfo(hConsole, &cursorInfo);
                set_color(15, 0);
                gotoxy(input_label_x, box_y + 8);
                FlushConsoleInputBuffer(hInput); // 수정 후에도 버퍼 비우기
            }
            // 3. 로그인 버튼 클릭 (범위 확장 했으 +8 -> +10)
            else if (my == box_y + 11 && mx >= btn_start_x && mx <= btn_start_x + 10)
            {
                gotoxy(22, box_y + 13); printf("                            "); // 메시지 지움

                if (strlen(input_id) == 0 || strlen(input_pw) == 0)
                {
                    gotoxy(25, box_y + 13); printf("ID와 PW를 모두 입력하세요!");
                    continue;
                }

                FILE* fp = NULL;
                User temp;
                int found = 0;

                if (fopen_s(&fp, "User.txt", "r") == 0 && fp != NULL)
                {
                    // 연봉(%d) 추가하여 읽기 형식 맞춤
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

                        if (strcmp(temp.id, input_id) == 0 && strcmp(temp.pw, input_pw) == 0)
                        {
                            *login_user = temp;
                            found = 1;
                            break;
                        }
                    }
                    fclose(fp);
                }

                if (found)
                {
                    gotoxy(24, box_y + 13);
                    set_color(11, 0);
                    printf("[성공] %s %s님 환영합니다!", login_user->name, login_user->position);
                    set_color(15, 0);
                    Sleep(1500);
                    return 1;
                }
                else
                {
                    gotoxy(24, box_y + 13);
                    set_color(12, 0);
                    printf("[실패] 정보를 확인해주세요.      ");
                    set_color(15, 0);
                }
            }
            // 4. PW찾기 버튼
            else if (my == box_y + 11 && mx >= btn_start_x + 11 && mx <= btn_start_x + 20)
            {
                find_account_process();

                system("cls");
                draw_box(box_x, box_y, 40, 14, "로그인");
                gotoxy(25, box_y + 3); printf(" 컴공상사 인사관리 프로그램 ");
                gotoxy(input_label_x, box_y + 6); printf("ID  : %-15s", input_id);
                gotoxy(input_label_x, box_y + 8); printf("PW  : ");
                draw_button(btn_start_x, box_y + 11, "로그인", 0);
                draw_button(btn_start_x + 11, box_y + 11, "PW찾기", 0);
                draw_button(btn_start_x + 22, box_y + 11, " 종 료 ", 0);

                gotoxy(22, box_y + 13);
                printf(">> [로그인] 버튼을 눌러주세요.");

                DWORD mode;
                GetConsoleMode(hInput, &mode);
                SetConsoleMode(hInput, (mode & ~ENABLE_QUICK_EDIT_MODE) | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);
            }
            // 5. 종료 버튼
            else if (my == box_y + 11 && mx >= btn_start_x + 22 && mx <= btn_start_x + 32)
            {
                exit(0);
            }
        }
    }
    return 0;
}


//PW 찾기 프로세스
void find_account_process()
{
    int mx, my;
    char input_search_id[20] = ""; // 검색할 ID
    int is_finding = 1;

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;

    // UI 좌표 설정
    int box_x = 20, box_y = 5;
    int label_x = 26;
    int field_x = 32;
    int btn_x = 35; // '뒤로가기' 버튼 위치

    system("cls");

    // 1. UI 그리기
    draw_box(box_x, box_y, 40, 14, "비밀번호 찾기");
    gotoxy(25, box_y + 3); printf("* 찾으실 ID를 입력하세요 *");

    gotoxy(label_x, box_y + 7); printf("ID  : ");

    // ---------------------------------------------------------
    // [초기 자동 입력 모드]
    // ---------------------------------------------------------
    cursorInfo.dwSize = 100;
    cursorInfo.bVisible = TRUE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);

    gotoxy(field_x, box_y + 7);
    set_color(14, 0); // 노란색
    scanf_s("%s", input_search_id, (unsigned)sizeof(input_search_id));
    while (getchar() != '\n'); // 엔터 버퍼 제거

    set_color(15, 0);
    gotoxy(label_x, box_y + 7);
    printf("ID  : %-15s", input_search_id);

    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);

    // ---------------------------------------------------------
    // [초기 검색 수행]
    // ---------------------------------------------------------
    FILE* fp = NULL;
    User temp;
    int found = 0;
    int is_admin_account = 0;

    if (fopen_s(&fp, "User.txt", "r") == 0 && fp != NULL)
    {
        // [수정] 연봉(%d) 추가하여 읽기 형식 맞춤
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
            if (strcmp(temp.id, input_search_id) == 0)
            {
                found = 1;
                if (strcmp(temp.id, "admin") == 0 || strcmp(temp.position, "사장") == 0)
                {
                    is_admin_account = 1;
                }
                break;
            }
        }
        fclose(fp);
    }

    // 초기 결과 표시
    gotoxy(22, box_y + 11);
    if (found)
    {
        if (is_admin_account)
        {
            set_color(12, 0);
            printf("[경고] 관리자 계정은 조회 불가합니다.");
        }
        else
        {
            set_color(11, 0);
            printf("[알림] 해당 계정의 PW: %s", temp.pw);
        }
    }
    else
    {
        set_color(12, 0);
        printf("[실패] 존재하지 않는 ID입니다.");
    }
    set_color(15, 0);

    draw_button(btn_x, box_y + 13, "뒤로가기", 0);

    FlushConsoleInputBuffer(hInput);
    DWORD mode;
    GetConsoleMode(hInput, &mode);
    SetConsoleMode(hInput, (mode & ~ENABLE_QUICK_EDIT_MODE) | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);

    // ---------------------------------------------------------
    // [마우스 이벤트 루프] - 재검색 기능 추가
    // ---------------------------------------------------------
    while (is_finding)
    {
        if (get_mouse_click(&mx, &my))
        {
            // 1. ID 입력칸 클릭 (재검색 시도)
            if (my == box_y + 7 && mx >= label_x && mx <= label_x + 25)
            {
                // 입력 모드 전환
                cursorInfo.bVisible = TRUE; SetConsoleCursorInfo(hConsole, &cursorInfo);

                // 기존 입력 및 결과 지우기
                gotoxy(field_x, box_y + 7); printf("                    "); // ID칸 지움
                gotoxy(22, box_y + 11);     printf("                                     "); // 결과칸 지움

                // 입력 받기
                gotoxy(field_x, box_y + 7); set_color(14, 0);
                scanf_s("%s", input_search_id, (unsigned)sizeof(input_search_id));
                while (getchar() != '\n');

                // 입력 종료 처리
                cursorInfo.bVisible = FALSE; SetConsoleCursorInfo(hConsole, &cursorInfo);
                set_color(15, 0);
                gotoxy(label_x, box_y + 7); printf("ID  : %-15s", input_search_id);

                // --- 재검색 로직 시작 ---
                found = 0;
                is_admin_account = 0;

                if (fopen_s(&fp, "User.txt", "r") == 0 && fp != NULL)
                {
                    // [수정] 연봉(%d) 추가하여 읽기 형식 맞춤
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
                        if (strcmp(temp.id, input_search_id) == 0)
                        {
                            found = 1;
                            if (strcmp(temp.id, "admin") == 0 || strcmp(temp.position, "사장") == 0)
                            {
                                is_admin_account = 1;
                            }
                            break;
                        }
                    }
                    fclose(fp);
                }

                // 결과 다시 표시
                gotoxy(22, box_y + 11);
                if (found)
                {
                    if (is_admin_account)
                    {
                        set_color(12, 0);
                        printf("[경고] 관리자 계정은 조회 불가합니다.");
                    }
                    else
                    {
                        set_color(11, 0);
                        printf("[알림] 해당 계정의 PW: %s", temp.pw);
                    }
                }
                else
                {
                    set_color(12, 0);
                    printf("[실패] 존재하지 않는 ID입니다.");
                }
                set_color(15, 0);

                FlushConsoleInputBuffer(hInput); // 버퍼 비우기
            }

            // 2. 뒤로가기 버튼 클릭
            else if (my == box_y + 13 && mx >= btn_x && mx <= btn_x + 12)
            {
                return; // 함수 종료
            }
        }
    }
}