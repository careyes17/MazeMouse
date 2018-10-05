/******************************************************************************
* File: maze.cpp
* Author: R.Carlos
* Class: COP2001; 201805; 50135
* Purpose Random maze generator using a recursive algorithm.
******************************************************************************/
#include <iostream>
#include <string>
#include <cstdlib>                // random gen
#include <time.h>                // time for seed

using namespace std;

#define BYTE unsigned char

// menu
enum ORIENTATION {
    UP, DOWN, LEFT, RIGHT
};

enum MENU {
    EXIT, GENERATE, RUN
};

ORIENTATION MOUSE_ORIENTATION;

struct FACING {
    ORIENTATION Foward;
    ORIENTATION Back;
    ORIENTATION Left;
    ORIENTATION Right;
};
FACING MOUSE_FACING;

// wall & visited flag values
const int CELL_TOP = 1;
const int CELL_BOTTOM = 2;
const int CELL_LEFT = 4;
const int CELL_RIGHT = 8;
const int CELL_VISITED = 16;
const int CELL_MOUSE = 32;

// maze dimensions
const int MAZE_ROWS = 3;
const int MAZE_COLS = 3;

// starting cell
const int START_CELL_ROW = 0;
const int START_CELL_COL = 0;
const int START_WALL = CELL_LEFT;

// ending cell
const int END_CELL_ROW = 2;
const int END_CELL_COL = 2;
const int END_WALL = CELL_RIGHT;

// location indexs
const int CELL_ROW_INDEX = 0;
const int CELL_COL_INDEX = 1;


// maze print characters
const char OUT_TOP_LEFT = '-';
const char OUT_TOP_MID = '-';
const char OUT_TOP_RIGHT = '-';
const char OUT_SIDE_LEFT = '|';
const char OUT_SIDE_RIGHT = '|';
const char OUT_BOT_LEFT = '-';
const char OUT_BOT_MID = '-';
const char OUT_BOT_RIGHT = '-';
const char INSIDE_MIDDLE = '+';
const char CELL_TOP_BOT = '-';
const char CELL_LEFT_RIGHT = '|';
const char CELL_OPEN_HORIZONTAL = ' ';
const char CELL_OPEN_VERTICAL = ' ';
const char CELL_VISITED_ON = '.';
const char CELL_VISITED_OFF = ' ';
const char CELL_MOUSE_ON = '*';

// function declarations
bool hasUnvisitedCells(BYTE cells[][MAZE_COLS]);

bool hasAvailableNeighbors(BYTE cells[][MAZE_COLS], int location[]);

void chooseRandomNeighbor(BYTE cells[][MAZE_COLS], int current[], int neighbor[]);

void removeWalls(BYTE cells[][MAZE_COLS], int current[], int neighbor[]);

void initMaze(BYTE cells[][MAZE_COLS]);

int pushStack(int stack[][2], int locations[], int stackPoint);

void popStack(int stack[][2], int locations[], int &stackPoint);

void copyOneLocTwo(int locOne[], int locTwo[]);

void printMaze(BYTE cells[][MAZE_COLS]);

void printMazeDebug(BYTE cells[][MAZE_COLS]);

void generateMaze(BYTE maze[][MAZE_COLS], int startCell[2], int endCell[2]);

FACING fitOrientation();

void mouseMotion(BYTE maze[][MAZE_COLS], int mouseCell[2]);

int main() {
    // init random generator
    srand(time(NULL));            // cstdlib; time.h

    // storage for the maze cells
    BYTE maze[MAZE_ROWS][MAZE_COLS] = {0};

    //set starting cell
    int startCell[2];
    startCell[CELL_ROW_INDEX] = START_CELL_ROW;
    startCell[CELL_COL_INDEX] = START_CELL_COL;

    //set ending cell
    int endCell[2];
    endCell[CELL_ROW_INDEX] = END_CELL_ROW;
    endCell[CELL_COL_INDEX] = END_CELL_COL;

    MENU choice;

    do {
        cout << "Select from the following menu:" << endl
             << "\t1 - Generate maze" << endl
             << "\t2 - Run Mouse" << endl
             << "\t0 - Exit" << endl << "> ";

        int ask;
        cin >> ask;

        choice = MENU(ask);

        switch (choice) {

            case GENERATE:
                generateMaze(maze, startCell, endCell);
                break;

            case RUN:
                mouseMotion(maze, startCell);
                break;

            default:
                if (choice != 0) {
                    cout << "Invalid choice!";
                }
        }

        cout << endl << endl;


    } while (choice != EXIT);

    return 0;
} // end main

/**
 * checks to see if there are more unvisited cells in maze
 * @param cells - maze 2D array
 * @return bool denoting if the maze has unvisited cells
 */
bool hasUnvisitedCells(BYTE cells[][MAZE_COLS]) {

    bool isVisited = true;

    int row = 0;
    while (isVisited && row < MAZE_ROWS) {

        int col = 0;

        while (isVisited & col < MAZE_COLS) {

            isVisited = cells[row][col] & CELL_VISITED;

            col++;
        }

        row++;
    }

    return !isVisited;
}

/**
 * checks around chosen cell to find possible available neighbors
 * @param cells - maze 2D array
 * @param location - the location to check around
 * @return boolean value denoting if there are available neighboring cells
 */
bool hasAvailableNeighbors(BYTE cells[][MAZE_COLS], int location[]) {
    // check if has neighbor above
    if (location[CELL_ROW_INDEX] > 0) {
        // check if not visited
        if (!(cells[location[CELL_ROW_INDEX] - 1][location[CELL_COL_INDEX]] & CELL_VISITED)) {
            return true;
        }

    }

    // check if has neighbor below
    if (location[CELL_ROW_INDEX] < MAZE_ROWS - 1) {
        // check if not visited
        if (!(cells[location[CELL_ROW_INDEX] + 1][location[CELL_COL_INDEX]] & CELL_VISITED)) {
            return true;
        }

    }

    // check if has left neighbor
    if (location[CELL_ROW_INDEX] > 0) {
        // check if not visited
        if (!(cells[location[CELL_ROW_INDEX]][location[CELL_COL_INDEX] - 1] & CELL_VISITED)) {
            return true;
        }

    }

    // check if has right neighbor
    if (location[CELL_COL_INDEX] < MAZE_COLS - 1) {
        // check if not visited
        if (!(cells[location[CELL_ROW_INDEX]][location[CELL_COL_INDEX] + 1] & CELL_VISITED)) {
            return true;
        }

    }

    return false;
} // end hasAvailableNeighbors

/**
 * Chooses an existing/available neighboring cell
 * @param cells - maze 2D array
 * @param current - the current cell being manipulated
 * @param neighbor - neighboring cell (adjacent N S W E)
 */
void chooseRandomNeighbor(BYTE cells[][MAZE_COLS], int current[], int neighbor[]) {

    bool done = false;

    while (!done) {

        int randNeighbor = rand() % 4;    // random 0...3

        switch (randNeighbor) {
            case 0:                    // TOP
                if (current[CELL_ROW_INDEX] > 0) {
                    if (!(cells[current[CELL_ROW_INDEX] - 1][current[CELL_COL_INDEX]] & CELL_VISITED)) {
                        neighbor[CELL_ROW_INDEX] = current[CELL_ROW_INDEX] - 1;
                        neighbor[CELL_COL_INDEX] = current[CELL_COL_INDEX];
                        done = true;
                    }
                }
                break;

            case 1:                    // BOTTOM
                if (current[CELL_ROW_INDEX] < MAZE_ROWS - 1) {
                    if (!(cells[current[CELL_ROW_INDEX] + 1][current[CELL_COL_INDEX]] & CELL_VISITED)) {
                        neighbor[CELL_ROW_INDEX] = current[CELL_ROW_INDEX] + 1;
                        neighbor[CELL_COL_INDEX] = current[CELL_COL_INDEX];
                        done = true;
                    }
                }
                break;

            case 2:                    // LEFT
                if (current[CELL_COL_INDEX] > 0) {
                    if (!(cells[current[CELL_ROW_INDEX]][current[CELL_COL_INDEX] - 1] & CELL_VISITED)) {
                        neighbor[CELL_ROW_INDEX] = current[CELL_ROW_INDEX];
                        neighbor[CELL_COL_INDEX] = current[CELL_COL_INDEX] - 1;
                        done = true;
                    }
                }
                break;

            case 3:                    // RIGHT
                if (current[CELL_COL_INDEX] < MAZE_COLS - 1) {
                    if (!(cells[current[CELL_ROW_INDEX]][current[CELL_COL_INDEX] + 1] & CELL_VISITED)) {
                        neighbor[CELL_ROW_INDEX] = current[CELL_ROW_INDEX];
                        neighbor[CELL_COL_INDEX] = current[CELL_COL_INDEX] + 1;
                        done = true;
                    }
                }
                break;
        } // end switch

    } // end loop

}    // end chooseRandomNeighbor

/**
 * 3. Remove the wall between the current and the chosen cell
 */

/**
 * Removes walls in the maze
 * @param cells - maze 2D array
 * @param current - the current cell being manipulated
 * @param neighbor - neighboring cell (adjacent N S W E)
 */
void removeWalls(BYTE cells[][MAZE_COLS], int current[], int neighbor[]) {

    // test for location of curent and neighbor
    // test for neighbor above
    if (neighbor[CELL_ROW_INDEX] < current[CELL_ROW_INDEX]) {
        cells[neighbor[CELL_ROW_INDEX]][neighbor[CELL_COL_INDEX]] ^= CELL_BOTTOM;    // toggle wall off
        cells[current[CELL_ROW_INDEX]][current[CELL_COL_INDEX]] ^= CELL_TOP;        // toggle wall off
    }

        // test for neighbor below
    else if (neighbor[CELL_ROW_INDEX] > current[CELL_ROW_INDEX]) {
        cells[neighbor[CELL_ROW_INDEX]][neighbor[CELL_COL_INDEX]] ^= CELL_TOP;        // toggle wall off
        cells[current[CELL_ROW_INDEX]][current[CELL_COL_INDEX]] ^= CELL_BOTTOM;        // toggle wall off
    }

        // test for neighbor left
    else if (neighbor[CELL_COL_INDEX] < current[CELL_COL_INDEX]) {
        cells[neighbor[CELL_ROW_INDEX]][neighbor[CELL_COL_INDEX]] ^= CELL_RIGHT;    // toggle wall off
        cells[current[CELL_ROW_INDEX]][current[CELL_COL_INDEX]] ^= CELL_LEFT;        // toggle wall off
    }

        // neighbor must be right
    else {
        cells[neighbor[CELL_ROW_INDEX]][neighbor[CELL_COL_INDEX]] ^= CELL_LEFT;        // toggle wall off
        cells[current[CELL_ROW_INDEX]][current[CELL_COL_INDEX]] ^= CELL_RIGHT;        // toggle wall off
    }

} // end removeWalls

/**
 * Initialize maze array elets to all
 * walls on and visited off
 * @param cells - grid of cells in the maze
 * @return void
 */
void initMaze(BYTE cells[][MAZE_COLS]) {

    for (int row = 0; row < MAZE_ROWS; row++) {

        for (int col = 0; col < MAZE_COLS; col++) {
            cells[row][col] = CELL_TOP | CELL_BOTTOM | CELL_LEFT | CELL_RIGHT;
        }

    }

}

/**
* Adds a cell location onto the stack
* @param stack - the location stack
* @param location - row  col of a maze cell
* @param stackPoint - last location added to stack
* @return int - new stack pointer
*/
int pushStack(int stack[][2], int locations[], int stackPoint) {

    int spNew = stackPoint;

    spNew++;                                // move sp up the stack
    copyOneLocTwo(locations, stack[spNew]);

    return spNew;
}

/**
* Pull a cell location onto the stack
* @param stack - the location stack
* @param location - row  col of a maze cell
* @param stackPoint IN/OUT - last location added to stack and return new
* @return int - new stack pointer
*/
void popStack(int stack[][2], int locations[], int &stackPoint) {

    copyOneLocTwo(stack[stackPoint], locations);
    stackPoint--;
}

/**
 * Copies the location of one cell to desired target
 * @param locOne - Cell to copy
 * @param locTwo  - Cell to be copied to
 */
void copyOneLocTwo(int locOne[], int locTwo[]) {
    locTwo[CELL_ROW_INDEX] = locOne[CELL_ROW_INDEX];
    locTwo[CELL_COL_INDEX] = locOne[CELL_COL_INDEX];
}


/**
* Printing the maze to the console
* @param cells - the grid of cells in the maze
*/
void printMaze(BYTE cells[][MAZE_COLS]) {
    cin.ignore();
    /**
    * Printing the maze to the console
    */
    for (int row = 0; row < MAZE_ROWS; row++) {

        // print the top row of cells
        for (int col = 0; col < MAZE_COLS; col++) {

            /*** print left spacer ***/

            // are we on the top row
            if (row == 0) {

                //are we on the left wall
                if (col == 0) {

                    cout << OUT_TOP_LEFT;

                } else {
                    cout << OUT_TOP_MID;
                }

            } else { // not on top row

                // are we on the left wall
                if (col == 0) {

                    cout << OUT_SIDE_LEFT;

                } else {

                    cout << INSIDE_MIDDLE;

                }

            } //print top left spacer

            /*** print cell top ***/

            if (cells[row][col] & CELL_TOP) {

                cout << CELL_TOP_BOT;

            } else {
                cout << CELL_OPEN_HORIZONTAL;
            }

            // print last right spacer
            if (col == MAZE_COLS - 1) {

                // top row
                if (row == 0) {
                    cout << OUT_TOP_RIGHT;

                } else { // not top row
                    cout << OUT_SIDE_RIGHT;
                }

            }

        } // print top row

        // move down o start printing side walls
        cout << endl;

        // print side walls of the cells
        for (int col = 0; col < MAZE_COLS; col++) {

            // prints cell left side wall
            if (cells[row][col] & CELL_LEFT) {

                cout << CELL_LEFT_RIGHT;

            } else {

                cout << CELL_OPEN_VERTICAL;

            }

            if (cells[row][col] & CELL_MOUSE) {
                cout << CELL_MOUSE_ON;
            } else if (cells[row][col] & CELL_MOUSE) {

                cout << CELL_MOUSE_ON;
            }
                // prints cell visited
            else if (cells[row][col] & CELL_VISITED) {

                cout << CELL_VISITED_ON;

            } else {

                cout << CELL_VISITED_OFF;

            }

            // print right wall
            if (col == MAZE_COLS - 1) {

                //print cell right side wall
                if (cells[row][col] & CELL_RIGHT) {
                    cout << CELL_LEFT_RIGHT;
                } else {
                    cout << CELL_OPEN_VERTICAL;
                }

            }

        } // print side walls

        // move down to start printing next top walls
        cout << endl;

        // print bottom row
        if (row == MAZE_ROWS - 1) {

            // print the bottom row of cell walls
            for (int col = 0; col < MAZE_COLS; col++) {

                // print spacer
                if (col == 0) {

                    cout << OUT_BOT_LEFT;

                } else {

                    cout << OUT_BOT_MID;

                }

                // print cell bottom wall
                if (cells[row][col] & CELL_BOTTOM) {

                    cout << CELL_TOP_BOT;

                } else {
                    cout << CELL_OPEN_HORIZONTAL;
                }

                // print right corner
                if (col == MAZE_COLS - 1) {

                    cout << OUT_BOT_RIGHT;

                }
            } // bottom walls

            // end the maze
            cout << endl;

        } // bottom row

    } // ends row loop
}

/**
 * Prints the binary component of the maze to console
 * @param cells - the maze 2D array
 */
void printMazeDebug(BYTE cells[][MAZE_COLS]) {

    for (int row = 0; row < MAZE_ROWS; row++) {

        for (int col = 0; col < MAZE_COLS; col++) {

            cout << to_string(cells[row][col]) << " ";
        }

        cout << endl;
    }

}

void generateMaze(BYTE maze[][MAZE_COLS], int startCell[2], int endCell[2]) {
    char tmp;                    // pause program

    // storage for our stack of visited cells
    int stack[MAZE_ROWS * MAZE_COLS][2] = {0};

    int stackPointer = -1;                // empty stack value

    initMaze(maze);

    /**
     * Initialize maze array elements to turn all
     * walls on and visited off
     */
    for (int row = 0; row < MAZE_ROWS; row++) {

        for (int col = 0; col < MAZE_COLS; col++) {
            maze[row][col] = CELL_TOP | CELL_BOTTOM | CELL_LEFT | CELL_RIGHT;
        }

    }

    // turn off starting wall
    maze[startCell[CELL_ROW_INDEX]][startCell[CELL_COL_INDEX]] ^= START_WALL;

    //turn off ending wall
    maze[END_CELL_ROW][END_CELL_COL] ^= END_WALL;

    // 1. Make the initial cell the current cell and mark it as visited
    int currCell[2];
    copyOneLocTwo(startCell, currCell);

    // mark visited flag
    maze[currCell[CELL_ROW_INDEX]][currCell[CELL_COL_INDEX]] ^= CELL_VISITED;

    printMaze(maze);

    // pause program
    cin >> tmp;

    // 2. While there are unvisited cells
    while (hasUnvisitedCells(maze)) {

        // i. If the current cell has any neighbors which have not been visited
        if (hasAvailableNeighbors(maze, currCell)) {

            // 1. Choose randomly one of the unvisited neighbors
            int neighborCell[2] = {0};
            chooseRandomNeighbor(maze, currCell, neighborCell);

            // 2. Push the current cell to the  stack
            stackPointer = pushStack(stack, currCell, stackPointer);

            // 3. Remove the wall between the current cell and the chosen cell
            removeWalls(maze, currCell, neighborCell);

            // 4. Make the chosen cell the current cell and mark it as visited
            copyOneLocTwo(neighborCell, currCell);
            // mark visited flag
            maze[currCell[CELL_ROW_INDEX]][currCell[CELL_COL_INDEX]] ^= CELL_VISITED;

            // 5. Print the maze
            printMaze(maze);
            cout << endl;

            // pause program
            cin >> tmp;

        } // end hasAvailableNeighbors

            // ii. Else if the stack is not empty
        else if (stackPointer >= 0) {

            // 1. pop last cell from the stack into current
            popStack(stack, currCell, stackPointer);

        }

    } // end hasUnvisitedCells

}

/**
* Can help orientate a mouse in the starting cell of the maze by
* determining which direction the mouse would have entered the maze
* through an outside wall.
*
* Function return the opposite wall direction to the one it entered
* Example - mouse entering through left wall is facing right of maze
*
* @param  cells - the maze array with flags
* @param  start - location of the starting cell in the maze
* @return BYTE  - the opposite wall of opening or right
*/
BYTE getStartDirection(BYTE cells[][MAZE_COLS], BYTE start[]) {
    // start cell on top row
    if (start[CELL_ROW_INDEX] == 0) {
        // is top wall open
        if (!(cells[start[CELL_ROW_INDEX]][start[CELL_COL_INDEX]] & CELL_TOP)) {
            // mouse is facing down
            return CELL_BOTTOM;
        }
    }
    // start cell on bottom row
    if (start[CELL_ROW_INDEX] == MAZE_ROWS - 1) {
        // is bottom wall open
        if (!(cells[start[CELL_ROW_INDEX]][start[CELL_COL_INDEX]] & CELL_BOTTOM)) {
            // mouse is facing up
            return CELL_TOP;
        }
    }
    // start cell on left wall
    if (start[CELL_COL_INDEX] == 0) {
        // is left wall open
        if (!(cells[start[CELL_ROW_INDEX]][start[CELL_COL_INDEX]] & CELL_LEFT)) {
            // mouse is facing right
            return CELL_RIGHT;
        }
    }
    // start cell on right wall
    if (start[CELL_COL_INDEX] == MAZE_COLS - 1) {
        // is right wall open
        if (!(cells[start[CELL_ROW_INDEX]][start[CELL_COL_INDEX]] & CELL_RIGHT)) {
            // mouse is facing left
            return CELL_LEFT;
        }
    }
    return CELL_RIGHT;       // did not enter through outside, assume right
}

FACING fitOrientation() {

    if (MOUSE_ORIENTATION == 0) {
        MOUSE_FACING.Left = LEFT;
        MOUSE_FACING.Right = RIGHT;
        MOUSE_FACING.Back = DOWN;
        MOUSE_FACING.Foward = UP;
    }

    if (MOUSE_ORIENTATION == 1) {
        MOUSE_FACING.Left = RIGHT;
        MOUSE_FACING.Right = LEFT;
        MOUSE_FACING.Back = UP;
        MOUSE_FACING.Foward = DOWN;
    }

    if (MOUSE_ORIENTATION == 2) {
        MOUSE_FACING.Left = DOWN;
        MOUSE_FACING.Right = UP;
        MOUSE_FACING.Back = RIGHT;
        MOUSE_FACING.Foward = LEFT;
    }

    if (MOUSE_ORIENTATION == 3) {
        MOUSE_FACING.Left = UP;
        MOUSE_FACING.Right = DOWN;
        MOUSE_FACING.Back = LEFT;
        MOUSE_FACING.Foward = RIGHT;
    }
    return MOUSE_FACING;
}

void mouseMotion(BYTE maze[][MAZE_COLS], int mouseCell[2]) {
    char tmp;
    bool endMaze = true;
    int orientation = 0;
    maze[mouseCell[0]][mouseCell[1]] ^= CELL_MOUSE;
    printMaze(maze);
    while (endMaze) {
        int randOrientation = rand() % 3;
        if (randOrientation == 2) {
            orientation = randOrientation;
        }
        if (!(mouseCell[0] == END_CELL_ROW && mouseCell[1] == END_CELL_COL)) {
            if (orientation == 0) {
                if (!(maze[mouseCell[0]][mouseCell[1]] & CELL_TOP)) {
                    maze[mouseCell[0] - 1][mouseCell[1]] ^= CELL_MOUSE;
                    maze[mouseCell[0]][mouseCell[1]] ^= CELL_MOUSE;
                    mouseCell[0] -= 1;
                } else if (!(maze[mouseCell[0]][mouseCell[1]] & CELL_RIGHT)) {
                    maze[mouseCell[0]][mouseCell[1] + 1] ^= CELL_MOUSE;
                    maze[mouseCell[0]][mouseCell[1]] ^= CELL_MOUSE;
                    mouseCell[1] += 1;
                } else {
                    orientation++;
                }
                printMaze(maze);
                cin >> tmp;
            } else if (orientation == 1) {
                if (!(maze[mouseCell[0]][mouseCell[1]] & CELL_RIGHT)) {
                    maze[mouseCell[0]][mouseCell[1] + 1] ^= CELL_MOUSE;
                    maze[mouseCell[0]][mouseCell[1]] ^= CELL_MOUSE;
                    mouseCell[1] += 1;
                } else if (!(maze[mouseCell[0]][mouseCell[1]] & CELL_BOTTOM)) {
                    maze[mouseCell[0] + 1][mouseCell[1]] ^= CELL_MOUSE;
                    maze[mouseCell[0]][mouseCell[1]] ^= CELL_MOUSE;
                    mouseCell[0] += 1;
                } else {
                    orientation++;
                }
                printMaze(maze);
                cin >> tmp;
            } else if (orientation == 2) {
                if (!(maze[mouseCell[0]][mouseCell[1]] & CELL_BOTTOM)) {
                    maze[mouseCell[0] + 1][mouseCell[1]] ^= CELL_MOUSE;
                    maze[mouseCell[0]][mouseCell[1]] ^= CELL_MOUSE;
                    mouseCell[0] += 1;
                } else if (!(maze[mouseCell[0]][mouseCell[1]] & CELL_LEFT)) {
                    maze[mouseCell[0]][mouseCell[1] - 1] ^= CELL_MOUSE;
                    maze[mouseCell[0]][mouseCell[1]] ^= CELL_MOUSE;
                    mouseCell[1] -= 1;
                } else {
                    orientation++;
                }
                printMaze(maze);
                cin >> tmp;
            } else if (orientation == 3) {
                if (!(maze[mouseCell[0]][mouseCell[1]] & CELL_LEFT)) {
                    maze[mouseCell[0]][mouseCell[1] - 1] ^= CELL_MOUSE;
                    maze[mouseCell[0]][mouseCell[1]] ^= CELL_MOUSE;
                    mouseCell[1] -= 1;
                } else if (!(maze[mouseCell[0]][mouseCell[1]] & CELL_TOP)) {
                    maze[mouseCell[0] - 1][mouseCell[1]] ^= CELL_MOUSE;
                    maze[mouseCell[0]][mouseCell[1]] ^= CELL_MOUSE;
                    mouseCell[0] -= 1;
                } else {
                    orientation = 0;
                }
                printMaze(maze);
                cin >> tmp;
            } else if (!(mouseCell[0] == START_CELL_ROW && mouseCell[1] == START_CELL_COL)) {
                orientation = 0;
            }

        } else {
            endMaze = !true;
        }
    }
}