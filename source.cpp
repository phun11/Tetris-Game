#include <iostream>
#include <conio.h>
#include <windows.h>
using namespace std;
#define H 20
#define W 15
char board[H][W] = {};
char blocks[7][4][4] = {
    // I
    {{' ','I',' ',' '},
     {' ','I',' ',' '},
     {' ','I',' ',' '},
     {' ','I',' ',' '}},

     // O
    {{' ',' ',' ',' '},
     {' ','O','O',' '},
     {' ','O','O',' '},
     {' ',' ',' ',' '}},

     // T
    {{' ','T',' ',' '},
     {'T','T','T',' '},
     {' ',' ',' ',' '},
     {' ',' ',' ',' '}},

       // S
    {{' ','S','S',' '},
     {'S','S',' ',' '},
     {' ',' ',' ',' '},
     {' ',' ',' ',' '}},

        // Z
    {{'Z','Z',' ',' '},
     {' ','Z','Z',' '},
     {' ',' ',' ',' '},
     {' ',' ',' ',' '}},

         // J
    {{'J',' ',' ',' '},
     {'J','J','J',' '},
     {' ',' ',' ',' '},
     {' ',' ',' ',' '}},

          // L
    {{' ',' ','L',' '},
     {'L','L','L',' '},
     {' ',' ',' ',' '},
     {' ',' ',' ',' '}}
};


int x = 6, y = 1, b = 1;
int speed = 200;   // tốc độ rơi mặc định

void gotoxy(int x, int y) {
    COORD c = { x, y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}
void boardDelBlock() {
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (blocks[b][i][j] != ' ' && y + j < H)
                board[y + i][x + j] = ' ';
}
void block2Board() {
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (blocks[b][i][j] != ' ')
                board[y + i][x + j] = blocks[b][i][j];
}
void initBoard() {
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {

            // Góc
            if (i == 0 && j == 0) board[i][j] = '+';
            else if (i == 0 && j == W - 1) board[i][j] = '+';
            else if (i == H - 1 && j == 0) board[i][j] = '+';
            else if (i == H - 1 && j == W - 1) board[i][j] = '+';

            // Viền trên/dưới
            else if (i == 0 || i == H - 1) board[i][j] = '-';

            // Viền trái/phải
            else if (j == 0 || j == W - 1) board[i][j] = '|';

            // Bên trong
            else board[i][j] = ' ';
        }
    }
}
void draw() {
    gotoxy(0, 0);
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {

            char c = board[i][j];

            // Nếu là ký tự block (I,O,T,S,Z,J,L) → in ra █
            if (c == 'I' || c == 'O' || c == 'T' || c == 'S' ||
                c == 'Z' || c == 'J' || c == 'L') {
                cout << char(219);
            }
            else {
                cout << c;
            }

        }
        cout << endl;
    }
}
bool canMove(int dx, int dy) {
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (blocks[b][i][j] != ' ') {
                int tx = x + j + dx;
                int ty = y + i + dy;
                if (tx < 1 || tx >= W - 1 || ty >= H - 1) return false;
                if (board[ty][tx] != ' ') return false;
            }
    return true;
}
void removeLine() {
    int j;
    for (int i = H - 2; i > 0; i--) {
        for (j = 0; j < W - 1; j++)
            if (board[i][j] == ' ') break;
        if (j == W - 1) {
            for (int ii = i; ii > 1; ii--)
                for (int j = 0; j < W - 1; j++) board[ii][j] = board[ii - 1][j];
            i++;
            draw();
            Sleep(200);
            if (speed > 50) speed -= 10;   // tăng tốc sau khi clear line
        }
    }
}
void rotateBlock() {
    char temp[4][4];

    // copy
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            temp[i][j] = blocks[b][i][j];

    // xoay 90 độ
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            blocks[b][j][3 - i] = temp[i][j];

    // kiểm tra va chạm
    if (!canMove(0, 0)) {
        // nếu xoay bị kẹt → xoay lại (undo)
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                blocks[b][i][j] = temp[i][j];
    }
}
int main()
{

    system("chcp 437 > nul");// thêm đúng codepage vào


    system("cls");

    srand(time(0));
    b = rand() % 7;
    system("cls");
    initBoard();
    while (1) {
        boardDelBlock();
        if (_kbhit()) {
            char c = _getch();
            if (c == 'w') rotateBlock(); //điều khiển xoay
            if (c == 'a' && canMove(-1, 0)) x--;
            if (c == 'd' && canMove(1, 0)) x++;
            if (c == 's' && canMove(0, 1))  y++;
            if (c == 'q') break;
        }
        if (canMove(0, 1)) y++;
        else {
            block2Board();
            removeLine();
            x = 6; y = 1; b = rand() % 7;
        }
        block2Board();
        draw();
        Sleep(speed); // thay biến speed kiểm soát tốc độ vào
    }
    return 0;
}