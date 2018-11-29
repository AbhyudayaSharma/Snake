/*
 * Copyright (c) Abhyudaya Sharma, 2018
 */

#include <stdio.h>
#include <curses.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "arraylist.h"

// ASCII Ctrl+C
#define KEY_CTRL_C 0x3

// PDCurses defines bool as unsigned char
// Use common way to invoke strcpy
#ifdef _WIN32
#define true TRUE
#define false FALSE

#define stringCopy(dest, size, source) strcpy_s(dest, size, source)
#else
#include <stdbool.h>

#define stringCopy(dest, size, source) strncpy(dest, source, size)
#endif // _WIN32

// Chars drawn for snake head and snake body points
#define SNAKE_HEAD ACS_DIAMOND 
#define SNAKE_BODY ACS_BLOCK

// Initial snake length
#define SNAKE_LEN 3

// macros for drawing body points and snake head
#define DRAW_POINT(point) do { mvaddch(point.y, point.x, SNAKE_BODY); } while (0)
#define DRAW_HEAD(point) do { mvaddch(point.y, point.x, SNAKE_HEAD); } while (0)

// max length for game over reason
#define MAX_REASON_LEN 100

typedef struct {
    int y;
    int x;
} Point;

/* Checks if the two points are equal */
inline bool arePointsEqual(Point a, Point b) {
    return a.x == b.x && a.y == b.y;
}

/* Initializes ncurses (or PDCurses) */
void initCurses() {
    initscr();
    raw();
    noecho();
    clear();
    start_color();
    keypad(stdscr, TRUE);
    curs_set(0); // Hide the cursor
    init_pair(1, COLOR_RED, COLOR_BLACK);
}

/* Acts as a splash screen */
bool runGame() {
    printw("A Snake Game\nCopyright (c) Abhyudaya Sharma 2018.\nPress 'q' to quit or press any other key to continue...");
    refresh();
    int ch = getch();

    if (ch == 'q' || ch == KEY_CTRL_C) {
        return false;
    } else {
        return true;
    }
}

/* Checks if the Point p is in the bounds of the display */
inline bool isInBounds(Point p) {
    int xMax, yMax;
    getmaxyx(stdscr, yMax, xMax);
    if (p.y >= 0 && p.y < yMax && p.x >= 0 && p.x < xMax) {
        return true;
    } else {
        return false;
    }
}

/* Function to check if the Point p is contained in the ArrayList of Points */
bool isOverlapped(Point p, const ArrayList* points) {
    size_t len = listGetLength(points);
    for (size_t i = 0; i < len; i++) {
        if (arePointsEqual(p, listGet(Point, points, i))) {
            return true;
        }
    }
    return false;
}

/*
 * reason is changed iff the function returns false
 * dy = change in y; dx = change in x
 */
bool moveSnake(ArrayList* points, int dy, int dx, char* reason) {
    Point head = listGet(Point, points, 0); // head before moving
    Point movedHead; // head after moving
    movedHead.x = head.x + dx;
    movedHead.y = head.y + dy;

    // restrict moving off the screen
    if (!isInBounds(movedHead)) {
        stringCopy(reason, MAX_REASON_LEN, "Your snake went out of bounds!");
        return false; // Reaching the bounds means the end of the game.
    }

    // check overlap
    if (isOverlapped(movedHead, points)) {
        stringCopy(reason, MAX_REASON_LEN, "Your snake ate itself!");
        return false;
    }

    clear();

    // add snake's body points
    for (int i = listGetLength(points) - 1; i > 0; i--) {
        Point prevPoint = listGet(Point, points, i - 1);
        listSet(points, i, &prevPoint);
        DRAW_POINT(listGet(Point, points, i));
    }

    listSet(points, 0, &movedHead);
    mvaddch(movedHead.y, movedHead.x, SNAKE_HEAD);
    return true;
}

/*
 * Returns the timeout (ms) based on the difficulty
 * Higher the level, less is the timeout
 * Timeout is the delay between the snake moving automatically.
 * At lower levels (higer timeouts), the snake can be accelerated
 * by keeping the key pressed.
 */
int getTimeout(int level) {
    const int initTimeout = 175;
    const double scale = -0.030;
    return (int) (initTimeout * exp(level * scale));
}

/*
 * Checks if the input is valid or not.
 * If the input is not valid, input is set to ERR.
 * WASD keys are also converted to Arrow keys.
 */
void validateInput(int* input) {
    switch (*input) {
        case 'w':
        case 'W':
            *input = KEY_UP;
            break;
        case 'a':
        case 'A':
            *input = KEY_LEFT;
            break;
        case 's':
        case 'S':
            *input = KEY_DOWN;
            break;
        case 'd':
        case 'D':
            *input = KEY_RIGHT;
            break;
        case KEY_UP:
        case KEY_DOWN:
        case KEY_RIGHT:
        case KEY_LEFT:
        case 'q':
        case KEY_CTRL_C:
            break;
        default:
            *input = ERR;
    }
}

/* Returns the location of the added food */
Point addFood(bool eaten, const ArrayList* points) {
    static Point food;

    int xMax;
    int yMax;

    getmaxyx(stdscr, yMax, xMax);

    if (eaten) {
        // Check if the food is not part of the snake
        // Generally O(1)
        do {
            // Food should be inside bounds
            food.y = rand() % yMax;
            food.x = rand() % xMax;
        } while (isOverlapped(food, points));
    }

    // Food is coloured and bold
    attron(COLOR_PAIR(1)); 
    attron(A_BOLD);
    mvaddch(food.y, food.x, ACS_BLOCK);
    attroff(COLOR_PAIR(1));
    attroff(A_BOLD);

    refresh();
    return food;
}

/* expects a blank ArrayList */
void resetSnake(ArrayList* points) {
    clear();
    int xMax, yMax;
    getmaxyx(stdscr, yMax, xMax);

    //Add and draw snake head
    Point head;
    head.x = xMax / 2;
    head.y = yMax / 2;
    listAddLast(points, &head);
    mvaddch(head.y, head.x, SNAKE_HEAD);

    // Snake body
    for (int i = 1; i < SNAKE_LEN; i++) {
        Point point = listGet(Point, points, i - 1);
        point.x -= 1;
        listAddLast(points, &point);
        DRAW_POINT(point);
    }
}

/* Contains the loop which is constantly run when the game is running */
void loop() {
    // Snake head is stored at index 0
    ArrayList points;
    initArrayList(&points, SNAKE_LEN * 2, sizeof(Point));
    resetSnake(&points);

    // add food
    Point foodLocation = addFood(true, &points);
    refresh();

    bool flag = true;
    int direction = KEY_RIGHT; // Snake is initially aligned along the +x direction
    bool moved = false;
    bool getInput = true;
    unsigned int score = 0;
    char gameOverReason[MAX_REASON_LEN];

    while (flag) {
        // Check for terminal size change
        Point head = listGet(Point, &points, 0);
        if (!isInBounds(head)) {
            // resetSnake requires an empty list
            freeArrayList(&points);
            initArrayList(&points, SNAKE_LEN * 2, sizeof(Point));
            resetSnake(&points);
            foodLocation = addFood(true, &points);
            refresh();
        }
        
        foodLocation = addFood(false, &points);

        int input = ERR;
        if (getInput) {
            input = getch();
        }

        validateInput(&input);
        input = (input == ERR) ? direction : input;

        switch (input) {
            case KEY_UP:
                if (direction != KEY_DOWN) {
                    moved = moveSnake(&points, -1, 0, gameOverReason);
                    direction = input;
                    getInput = true;
                } else {
                    getInput = false;
                    continue;
                }
                break;
            case KEY_DOWN:
                if (direction != KEY_UP) {
                    moved = moveSnake(&points, 1, 0, gameOverReason);
                    direction = input;
                    getInput = true;
                } else {
                    getInput = false;
                    continue;
                }
                break;
            case KEY_LEFT:
                if (direction != KEY_RIGHT) {
                    moved = moveSnake(&points, 0, -1, gameOverReason);
                    direction = input;
                    getInput = true;
                } else {
                    getInput = false;
                    continue;
                }
                break;
            case KEY_RIGHT:
                if (direction != KEY_LEFT) {
                    moved = moveSnake(&points, 0, 1, gameOverReason);
                    direction = input;
                    getInput = true;
                } else {
                    getInput = false;
                    continue;
                }
                break;
            case 'q':
            case KEY_CTRL_C:
                stringCopy(gameOverReason, MAX_REASON_LEN, "Why did you quit? You were doing quite well!");
                flag = false;
                break;
        }
        
        // Terminate execution if snake was unable to move
        if (!moved) {
            flag = false;
        }

        // Check for eating food
        if (arePointsEqual(listGet(Point, &points, 0), foodLocation)) {
            foodLocation = addFood(true, &points);
            listAddLast(&points, &listGet(Point, &points, listGetLength(&points) - 1));
            score++;
        }

        refresh();
        timeout(getTimeout(score)); // Start automatic movement after first keystroke
    }

    // End the game
    freeArrayList(&points);
    endwin(); // cleanup curses

    // Print the details of this session
    printf("Game Over!\n%s\nYour score: %u\n", gameOverReason, score);
#ifdef _WIN32
	// avoid unexpected termination when running the .exe file outside
	// a terminal window 
	system("pause");
#endif // _WIN32
}

/* The main method */
int main(int argc, char ** argv) {
    initCurses(); // Init curses
    srand((unsigned int) time(NULL)); // Initialize rand with seed as current time

    if (runGame()) {
        loop(); /* Main logic of the program. Cleans up curses when game gets over. */
    }

    return 0;
}

