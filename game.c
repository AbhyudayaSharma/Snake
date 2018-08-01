/*
 * Copyright (c) Abhyudaya Sharma, 2018
 */
#include <stdio.h>
#include <stdbool.h>
#include <curses.h>
#include <string.h>

#define MY_CURSOR ACS_DIAMOND

inline bool isInBounds(int y, int x) {
    int maxX, maxY;
    getmaxyx(stdscr, maxY, maxX);
    if (y >= 0 && y < maxY && x >= 0 && x < maxX) {
        return true;
    } else {
        return false;
    }
}

/*
 * The y and x are the current locations of the pointer.
 * dy and dx are the change in y and x respectively.
 */
void moveCursor(int* y, int* x, int dy, int dx) {
    if (isInBounds(*y + dy, *x + dx)) {
        clear();
        *y += dy;
        *x += dx;
        mvaddch(*y, *x, MY_CURSOR);
    }
}

void resetCursor(int* y, int* x) {
    clear();
    int maxX, maxY;
    getmaxyx(stdscr, maxY, maxX);

    int curX = maxX / 2;
    int curY = maxY / 2;

    mvaddch(curY, curX, MY_CURSOR); 

    // Update the locations
    *y = curY;
    *x = curX;
    refresh();
}

int main(int argc, char ** argv) {
    // Init curses
    initscr();
    raw();
    noecho();
    clear();
    keypad(stdscr, TRUE);
    curs_set(0); // Hide the cursor


    // Start here
    printw("Hello World.\nA simple movement tool.\nPress 'q' to quit.\nPress any key to continue...");
    refresh();
    int ch = getch();

    switch(ch) {
        case KEY_F(1):
            attron(A_BOLD);
            addstr("\nYou pressed F1");
            attroff(A_BOLD);
            refresh();
            break;
        case 'q':
            endwin();
            return 0;
    }

    int curY, curX;
    resetCursor(&curY, &curX);

    bool flag = true;
    while (flag) {
        // Check for terminal size change
        if (!isInBounds(curY, curX)) {
            clear();
            resetCursor(&curY, &curX);
            refresh(); 
        }
        
        int input = getch();
        switch (input) {
            case KEY_UP:
                moveCursor(&curY, &curX, -1, 0);
                break;
            case KEY_DOWN:
                moveCursor(&curY, &curX, 1, 0);
                break;
            case KEY_LEFT:
                moveCursor(&curY, &curX, 0, -1);
                break;
            case KEY_RIGHT:
                moveCursor(&curY, &curX, 0, 1);
                break;
            case 'q':
                flag = false;
                break;
        }
        refresh();
    }

    // Close window
    endwin();
    return 0;
}

