/*
 * Copyright (c) Abhyudaya Sharma, 2018
 */
#include <stdio.h>
#include <stdbool.h>
#include <curses.h>
#include <string.h>

#define SNAKE_HEAD ACS_DIAMOND
#define SNAKE_LEN 10
#define ADD_POINT(point) do { mvaddch(point.y, point.x, ACS_BLOCK); } while (0)

struct Point {
    int y;
    int x;
};

inline bool isInBounds(int y, int x) {
    int maxX, maxY;
    getmaxyx(stdscr, maxY, maxX);
    if (y >= 0 && y < maxY && x >= 0 && x < maxX) {
        return true;
    } else {
        return false;
    }
}

void moveSnake(struct Point* points, int snakeLen, int dy, int dx) {
    clear();
    for (int i = snakeLen- 1; i > 0; i--) {
        points[i].y = points[i - 1].y;
        points[i].x = points[i - 1].x;

        ADD_POINT(points[i]);
    }
    points[0].y += dy;
    points[0].x += dx;
    mvaddch(points[0].y, points[0].x, SNAKE_HEAD); 
}

void resetCursor(int* y, int* x, struct Point* points, int snakeLen) {
    clear();
    int maxX, maxY;
    getmaxyx(stdscr, maxY, maxX);

    int curX = maxX / 2;
    int curY = maxY / 2;

    mvaddch(curY, curX, SNAKE_HEAD); 
    points[0].y = curY;
    points[0].x = curX;
    for (int i = 1; i < snakeLen; i++) {
        points[i].y = points[i - 1].y;
        points[i].x = points[i - 1].x - 1;
        ADD_POINT(points[i]);
    }

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

    // head is points[0]
    struct Point points[SNAKE_LEN];
    resetCursor(&curY, &curX, points, SNAKE_LEN);

    bool flag = true;
    while (flag) {
        // Check for terminal size change
        /*
        if (!isInBounds(curY, curX)) {
            clear();
            resetCursor(&curY, &curX, points, SNAKE_LEN);
            refresh(); 
        }
        */
        
        int input = getch();
        switch (input) {
            case KEY_UP:
                moveSnake(points, SNAKE_LEN, -1, 0);
                break;
            case KEY_DOWN:
                moveSnake(points, SNAKE_LEN, 1, 0);
                break;
            case KEY_LEFT:
                moveSnake(points, SNAKE_LEN, 0, -1);
                break;
            case KEY_RIGHT:
                moveSnake(points, SNAKE_LEN, 0, 1);
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

