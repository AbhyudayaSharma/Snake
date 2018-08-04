/*
 * Copyright (c) Abhyudaya Sharma, 2018
 */

#include <stdio.h>
#include <stdbool.h>
#include <curses.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define SNAKE_HEAD ACS_DIAMOND 
#define SNAKE_LEN 20
#define ADD_POINT(point) do { mvaddch(point.y, point.x, ACS_BLOCK); } while (0)

struct Point {
    int y;
    int x;
};

inline bool arePointsEqual(struct Point x, struct Point y) {
    return x.x == y.x && x.y == y.y;
}

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

// Acts as a splash screen
bool runGame() {
    printw("Hello World.\nA simple movement tool.\nPress 'q' to quit or press any other key to continue...");
    refresh();
    int ch = getch();

    if (ch == 'q') {
        return false;
    } else {
        return true;
    }
}

inline bool isInBounds(int y, int x) {
    int maxX, maxY;
    getmaxyx(stdscr, maxY, maxX);
    if (y >= 0 && y < maxY && x >= 0 && x < maxX) {
        return true;
    } else {
        return false;
    }
}

bool isOverlapped(struct Point p, const struct Point* points, int snakeLen) {
    for (int i = 0; i < snakeLen; i++) {
        if (arePointsEqual(p, points[i])) {
            return true;
        }
    }

    return false;
}

bool moveSnake(struct Point* points, int snakeLen, int dy, int dx) {
    // restrict moving off the screen
    if (!isInBounds(points[0].y + dy, points[0].x + dx)) {
        return false; // Reaching the bounds means the end of the game.
    }

    // check overlap
    if (isOverlapped((struct Point) { points[0].y + dy, points[0].x + dx }, points, snakeLen)) {
        return false;
    }

    clear();

    // add snake's body points
    for (int i = snakeLen- 1; i > 0; i--) {
        points[i].y = points[i - 1].y;
        points[i].x = points[i - 1].x;

        ADD_POINT(points[i]);
    }

    points[0].y += dy;
    points[0].x += dx;
    mvaddch(points[0].y, points[0].x, SNAKE_HEAD);
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
    // TODO Create a difficulty based timeout
    return 200;
}

void validateInput(int* input) {
    switch (*input) {
        case KEY_UP:
        case KEY_DOWN:
        case KEY_RIGHT:
        case KEY_LEFT:
        case 'q':
            break;
        default:
            *input = ERR;
    }
}

/* Returns the location of the added food */
struct Point addFood(bool eaten, const struct Point* points, int snakeLen) {
    static int foodX;
    static int foodY;

    int maxX;
    int maxY;

    getmaxyx(stdscr, maxY, maxX);

    if (eaten) {
        // Check if the food is not part of the snake
        do {
            foodY = rand() % maxY;
            foodX = rand() % maxX;
        } while (isOverlapped((struct Point) {foodY, foodX}, points, snakeLen));
    }

    // Food is coloured
    attron(COLOR_PAIR(1)); 
    mvaddch(foodY, foodX, ACS_BLOCK);
    attroff(COLOR_PAIR(1));

    refresh();
    return (struct Point) { foodY, foodX };
}

/* Returns the location of the added food */
struct Point resetSnake(struct Point* points, int snakeLen) {
    clear();
    int maxX, maxY;
    getmaxyx(stdscr, maxY, maxX);

    int curX = maxX / 2;
    int curY = maxY / 2;

    // Snake head
    mvaddch(curY, curX, SNAKE_HEAD);
    points[0].y = curY;
    points[0].x = curX;

    // Snake body
    for (int i = 1; i < snakeLen; i++) {
        points[i].y = points[i - 1].y;
        points[i].x = points[i - 1].x - 1;
        ADD_POINT(points[i]);
    }

    refresh();

    return addFood(true, points, snakeLen); 
}

void loop() {
    // head is points[0]
    struct Point points[SNAKE_LEN];
    struct Point foodLocation = resetSnake(points, SNAKE_LEN);

    bool flag = true;
    int direction = KEY_RIGHT; // Snake is initially aligned along the +x direction
    bool overlap = false;
    bool getInput = true;
    unsigned int score = 0;

    while (flag) {
        // Check for terminal size change
        if (!isInBounds(points[0].y, points[0].x)) {
            resetSnake(points, SNAKE_LEN);
        }
        
        addFood(false, points, SNAKE_LEN);

        int input = ERR;
        if (getInput) {
            input = getch();
        }

        validateInput(&input);
        input = (input == ERR) ? direction : input;

        switch (input) {
            case KEY_UP:
                if (direction != KEY_DOWN) {
                    overlap = !moveSnake(points, SNAKE_LEN, -1, 0);
                    direction = input;
                    getInput = true;
                } else {
                    getInput = false;
                    continue;
                }
                break;
            case KEY_DOWN:
                if (direction != KEY_UP) {
                    overlap = !moveSnake(points, SNAKE_LEN, 1, 0);
                    direction = input;
                    getInput = true;
                } else {
                    getInput = false;
                    continue;
                }
                break;
            case KEY_LEFT:
                if (direction != KEY_RIGHT) {
                    overlap = !moveSnake(points, SNAKE_LEN, 0, -1);
                    direction = input;
                    getInput = true;
                } else {
                    getInput = false;
                    continue;
                }
                break;
            case KEY_RIGHT:
                if (direction != KEY_LEFT) {
                    overlap = !moveSnake(points, SNAKE_LEN, 0, 1);
                    direction = input;
                    getInput = true;
                } else {
                    getInput = false;
                    continue;
                }
                break;
            case 'q':
                flag = false;
                break;
        }

        if (overlap) {
            flag = false;
        }

        // Check for eating food
        if (arePointsEqual(points[0], foodLocation)) {
            foodLocation = addFood(true, points, SNAKE_LEN);
            score++;
        }

        refresh();
        timeout(getTimeout(0)); // Start automatic movement after first keystroke
    }

    if (overlap) {
        clear();
        timeout(-1); // reset timeout
        addstr("Game Over!\nYour snake ate itself!\nPress any key to exit...");
        refresh();
        getch();
    }
}

int main(int argc, char ** argv) {
    // Init curses
    initCurses();
    srand(time(NULL)); // Initialize rand with seed as current time

    if (runGame()) {
        /* Main logic of the program */
        loop();
    }

    endwin(); // cleanup curses
    return 0;
}

