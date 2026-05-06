#pragma once
#ifndef _WIN32
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
void sleep_ms(int ms) { usleep(ms * 1000); }

void init_termios() {
    static struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}

inline void hideCursor() { 
    std::cout << "\033[?25l" << std::flush; 
}

inline void showCursor() { 
    std::cout << "\033[?25h" << std::flush; 
}

void reset_termios() {
    static struct termios oldt;
    tcgetattr(STDIN_FILENO, &oldt);
    oldt.c_lflag |= (ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

int kbhit_wrapper() {
    struct timeval tv = {0, 0};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    return select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
}

int getch_wrapper() {
    return getchar();
}

inline void setCursorPos(int row, int col) { std::cout << "\033[" << row << ";" << col << "H" << std::flush; }

inline void clearscreen() { std::cout << "\033[H\033[J"; }
inline void initterminal() { 
    init_termios(); 
    hideCursor(); 
    atexit(restoreterminal); 
}
inline void restoreterminal() { 
    showCursor();   
    reset_termios(); 
}

void setColor(int color) {
    static const char* ansi_colors[] = {
        "\033[0m",  // 0: Reset
        "\033[31m", // 1: Red
        "\033[32m", // 2: Green
        "\033[33m", // 3: Yellow
        "\033[36m"  // 4: Cyan
    };
    if (color >= 0 && color <= 4) std::cout << ansi_colors[color];
}

#endif