#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <conio.h>

// User 구조체
typedef struct
{
    char id[20];         // 아이디
    char pw[20];         // 패스워드
    char name[20];       // 이름
    char department[20]; // 부서
    char position[20];   // 직급
    char phone[20];      // 전화번호
    char hire_date[15];  // 입사 날짜
    int salary;          // 봉급
} User;

// Evaluation(인사평가) 구조체
typedef struct
{
    char evaluator_id[20]; // 평가자 ID 
    char employee_id[20];  // 피평가자 ID 
	int teamwork_score;    // 팀워크 점수(0~100)
	int attitude_score;    // 태도 점수(0~100)
    char comments[100];    // 코멘트
} Evaluation;

// --- 함수 원형 선언 ---

// ui.c (화면 및 입력 제어)
void init_console();                // 콘솔 초기화 (커서 숨김, 마우스 활성화)
void gotoxy(int x, int y);          // 좌표 이동
void set_color(int color, int bg);  // 색상 변경
void draw_box(int x, int y, int w, int h, char* title); // 박스 그리기
void draw_button(int x, int y, char* text, int is_highlight); // 버튼 그리기
int get_mouse_click(int* x, int* y); // 마우스 클릭 감지

// main.c (로직)

// login.c (로그인 처리)
void create_dummy_data();           // 테스트용 파일 생성
int login_process(User* login_user); // 로그인 처리

// menu.c (메인 메뉴)
void show_main_menu(User* user);    // 메인 메뉴 (직원/관리자 분기)

#endif
