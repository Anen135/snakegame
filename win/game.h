#pragma once
#ifdef _WIN32
#include <conio.h>
#include <windows.h>

inline void setCursorVisible(bool visible) {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;    
    GetConsoleCursorInfo(consoleHandle, &info); 
    info.bVisible = visible;                    
    SetConsoleCursorInfo(consoleHandle, &info); 
}

void sleep_ms(int ms) { Sleep(ms); }
int kbhit_wrapper() { return _kbhit(); }
int getch_wrapper() { int c = _getch(); return (c == 0 || c == 224) ? _getch() : c; }
inline void clearscreen() { system("cls");}
inline void setCursorPos(int x, int y) { COORD coord = { (SHORT)x, (SHORT)y }; SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord); }
inline void initterminal() { setCursorVisible(false); }
inline void restoreterminal() { setCursorVisible(true); }
void setColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}
#endif