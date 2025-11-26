/*
담당자: 전민규
최근 업데이트: 2025.11.24 / 13:23
진행상태:
1 각종 유틸 함수 구현 완료
1-2 UI 박스 그리기, 버튼 그리기, 마우스 클릭 감지 기능 구현 완료
*/
#include "common.h"

// 콘솔 초기화 - 커서 숨기기, 마우스 활성화, 빠른 편집 모드 해제
void init_console()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);

    // 커서 숨기기
    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.dwSize = 1;
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);

    // 마우스 입력 활성화 및 빠른 편집 모드(QuickEdit Mode) 비활성화
    DWORD mode;
    GetConsoleMode(hInput, &mode);
    SetConsoleMode(hInput, (mode & ~ENABLE_QUICK_EDIT_MODE) | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);

}

// 좌표 이동 함수
void gotoxy(int x, int y)
{
    COORD pos = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

// 색상 설정 함수 (텍스트, 배경)
void set_color(int text, int bg)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), text | (bg << 4));
}

// UI 박스 그리기
void draw_box(int x, int y, int w, int h, char* title)
{
    set_color(15, 0); // 흰색 글씨

    // 상단 테두리
    gotoxy(x, y);
    printf("┌");
    for (int i = 0; i < w - 2; i++) printf("─");
    printf("┐");

    // 타이틀 출력
    if (title != NULL)
    {
        gotoxy(x + 2, y);
        printf(" %s ", title);
    }

    // 몸통
    for (int i = 1; i < h - 1; i++)
    {
        gotoxy(x, y + i);
        printf("│");
        gotoxy(x + w - 1, y + i);
        printf("│");
    }

    // 하단 테두리
    gotoxy(x, y + h - 1);
    printf("└");
    for (int i = 0; i < w - 2; i++) printf("─");
    printf("┘");
}

// 버튼 그리기 (텍스트 박스 형태)
void draw_button(int x, int y, char* text, int is_highlight)
{
    if (is_highlight) set_color(0, 15); // 반전 색상 설정시 사용 하셈
    else set_color(15, 0);

    gotoxy(x, y);
    printf("[ %s ]", text);
    set_color(15, 0); // 색상 복구
}

// 마우스 클릭 감지 함수
int get_mouse_click(int* x, int* y)
{
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    INPUT_RECORD ir;
    DWORD read;

    // 이벤트 읽기
    ReadConsoleInput(hInput, &ir, 1, &read);

    if (ir.EventType == MOUSE_EVENT)
    {
        // 더블 클릭 등의 플래그가 섞여 있어도 클릭으로 인식하도록함
        if (ir.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED) {
            *x = ir.Event.MouseEvent.dwMousePosition.X;
            *y = ir.Event.MouseEvent.dwMousePosition.Y;
            return 1; // 클릭 감지 시 리턴
        }
    }
    return 0;
}