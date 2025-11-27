#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <conio.h>
#include <time.h>

// =====================================================
// 구조체 정의
// =====================================================

// -----------------------------------------------------
// User 구조체: 직원 기본 정보 저장
// - 로그인 인증, 직원 조회, 인사 관리 등에 사용됨
// -----------------------------------------------------
typedef struct
{
    char id[20];         // 아이디
    char pw[20];         // 비밀번호
    char name[20];       // 이름
    char department[20]; // 부서명
    char position[20];   // 직급
    char phone[20];      // 연락처
    char hire_date[15];  // 입사일 (YYYY-MM-DD)
    int salary;          // 봉급
} User;


// -----------------------------------------------------
// Evaluation 구조체: 인사 평가 데이터
// - 평가자 / 피평가자 / 점수 / 코멘트 저장
// -----------------------------------------------------
typedef struct
{
    char evaluator_id[20]; // 평가자 ID
    char employee_id[20];  // 피평가자 ID
    int teamwork_score;    // 팀워크 점수 (0~100)
    int attitude_score;    // 태도 점수 (0~100)
    char comments[100];    // 평가 코멘트
} Evaluation;


// -----------------------------------------------------
// Attendance 구조체: 근태 기록 저장
// - 출근/퇴근 시간 및 상태 관리
// -----------------------------------------------------
typedef struct {
    char user_id[20];     // 사용자(직원) ID
    char date[15];        // 날짜 (YYYY-MM-DD)
    char check_in[10];    // 출근 시간 HH:MM:SS
    char check_out[10];   // 퇴근 시간 HH:MM:SS
    float work_hours;     // 실제 근무 시간 (시 단위, 파일 저장 X)
    char status[10];      // 근태 상태 또는 근무 시간 표시 (예: "출근", "08:12")
} Attendance;


// -----------------------------------------------------
// Notice 구조체: 공지사항 저장
// - 관리자가 작성 / 직원이 열람
// -----------------------------------------------------
typedef struct {
    int id;               // 공지 글 번호
    char title[50];       // 제목
    char content[500];    // 내용
    char writer_id[20];   // 작성자 ID
    char date[15];        // 작성일
} Notice;


// =====================================================
// 근태 관리 함수 (TUI)
// =====================================================

void check_in(User* user);         // 출근 체크 처리
void check_out(User* user);        // 퇴근 체크 처리
void my_attendance(User* user);    // 로그인한 직원의 근태 조회 화면
void all_attendance();             // 전체 직원 근태 조회 (관리자 전용)
void attendance_menu(User* user);  // 근태 관리 메인 메뉴 (마우스 기반 TUI)


// =====================================================
// 공지사항 관련
// =====================================================

void notice_admin_menu(User* user);   // 관리자용 공지사항 메뉴
void notice_add(User* user);          // 공지 작성
void notice_list_admin();             // 관리자용 공지 목록 조회
void notice_edit();                   // 기존 공지 수정
void notice_employee_menu();          // 직원용 공지 메뉴
void notice_list_employee();          // 직원용 공지 목록 조회
void notice_view_detail();            // 공지 상세보기


// =====================================================
// UI 관련 함수
// - 콘솔 TUI 구현에 사용
// =====================================================

void init_console();                            // 콘솔 초기 설정(커서 숨김/마우스 활성화)
void gotoxy(int x, int y);                      // 콘솔 커서 이동
void set_color(int text, int bg);               // 텍스트/배경 색상 변경
void draw_box(int x, int y, int w, int h, char* title);    // 박스 그리기
void draw_button(int x, int y, char* text, int is_highlight); // 버튼 출력
int get_mouse_click(int* x, int* y);            // 마우스 클릭 이벤트 감지


// =====================================================
// 로그인 및 사용자 데이터 관리
// =====================================================

void create_dummy_data();              // 테스트용 임시 계정 데이터 생성
int login_process(User* login_user);   // 로그인 프로세스 수행


// =====================================================
// 메뉴 연결
// =====================================================

void show_main_menu(User* user);            // 로그인 이후 메인 메뉴 (관리자/직원 분기)
void show_employee_info_menu(User* user);   // 직원 정보 조회 메뉴


#endif
