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

//tui mang hết biến toàn cục lên đây khai báo nha ae

//biến dùng chung
int x = 6, y = 1, b = 1;
int speed = 200;   // tốc độ rơi mặc định
int level = 1;        // level hiện tại (bắt đầu từ 1)
int linesCleared = 0; // tổng số hàng đã xóa
int score = 0;
int nextBlock;   // khối tiếp theo
DWORD gameStartTime;     // thời điểm bắt đầu game
int elapsedTime = 0;     // thời gian đã trôi (giây) dùng cho timer

//dùng cho ultra mode
int timeLeft = 0;  // thời gian còn lại (Ultra)
bool isUltraMode = false;
const int ULTRA_TIME = 120; // 2 phút

//dùng cho sprint mode
bool isSprintMode = false;
const int SPRINT_TARGET = 40;


int showMainMenu() {
    system("cls");

    cout << "\n\n";
    cout << "=============================================\n";
    cout << "                 \033[1;36mT E T R I S\033[0m\n";
    cout << "=============================================\n";
    cout << "\n";
    cout << "              \033[1mCHON CHE DO CHOI\033[0m\n";
    cout << "\n";

    cout << "   [1] Marathon           - Choi den khi thua\n";
    cout << "   [2] Sprint (40 Lines)  - Hoan thanh 40 hang nhanh nhat\n";
    cout << "   [3] Ultra (2 Minutes)  - Ghi diem trong 2 phut\n";

    cout << "\n";
    cout << "   Chon 1 che do (1-3): ";

    int mode;
    cin >> mode;

    return mode;
}

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

int getGhostY() {
    int ghostY = y;

    while (true) {
        ghostY++;

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (blocks[b][i][j] != ' ') {
                    int ty = ghostY + i;
                    int tx = x + j;

                    if (ty >= H - 1 || board[ty][tx] != ' ') {
                        return ghostY - 1;
                    }
                }
            }
        }
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

int removeLine() {
    int count = 0;
    int j;

    for (int i = H - 2; i > 0; i--) {
        for (j = 1; j < W - 1; j++)
            if (board[i][j] == ' ') break;

        if (j == W - 1) {
            count++;

            for (int ii = i; ii > 1; ii--)
                for (int jj = 1; jj < W - 1; jj++)
                    board[ii][jj] = board[ii - 1][jj];

            i++;
        }
    }
    return count;
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

bool isGameOver() {
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (blocks[b][i][j] != ' ') {
                int ty = y + i;
                int tx = x + j;
                if (board[ty][tx] != ' ')
                    return true;
            }
    return false;
}

void showGameOver() {
    system("cls");
    cout << "\n\n";
    cout << "====================================\n";
    cout << "           G A M E  O V E R\n";
    cout << "====================================\n\n";

    cout << "Score : " << score << endl;
    cout << "Lines : " << linesCleared << endl;
    cout << "Level : " << level << endl;

    cout << "\nNhan phim bat ky de thoat...";
    _getch();
}

void hardDrop() {
    boardDelBlock();
    while (canMove(0, 1)) {
        y++;
    }
}

void updateScore(int lines) {
    switch (lines) {
    case 1: score += 40; break;
    case 2: score += 100; break;
    case 3: score += 300; break;
    case 4: score += 1200; break;
    }
    linesCleared += lines;
}

void updateLevel() {
    int newLevel = linesCleared / 10 + 1;

    if (newLevel > level) {
        level = newLevel;

        // tăng tốc độ rơi lên 10%
        speed = (int)(speed * 0.9);

        // giới hạn tốc độ tối đa
        if (speed < 50)
            speed = 50;
    }
}

void drawNextBlock() {
    int boxX = W + 3;
    int boxY = 11;

    // tiêu đề
    gotoxy(boxX, boxY);
    cout << "Next";

    // khung 4x4
    for (int i = 0; i <= 5; i++) {
        gotoxy(boxX - 1, boxY + 1 + i); cout << "|";
        gotoxy(boxX + 6, boxY + 1 + i); cout << "|";
    }

    for (int j = 0; j <= 5; j++) {
        gotoxy(boxX - 1 + j, boxY + 1); cout << "-";
        gotoxy(boxX - 1 + j, boxY + 6); cout << "-";
    }

    gotoxy(boxX - 1, boxY + 1); cout << "+";
    gotoxy(boxX + 6, boxY + 1); cout << "+";
    gotoxy(boxX - 1, boxY + 6); cout << "+";
    gotoxy(boxX + 6, boxY + 6); cout << "+";

    // vẽ block
    for (int i = 0; i < 4; i++) {
        gotoxy(boxX, boxY + 2 + i);
        for (int j = 0; j < 4; j++) {
            if (blocks[nextBlock][i][j] != ' ')
                cout << char(219);
            else
                cout << ' ';
        }
    }
}

void drawHUD() {
    int xHUD = W + 3;

    gotoxy(xHUD, 2);
    cout << "Score : " << score << "   ";

    gotoxy(xHUD, 4);
    cout << "Lines : " << linesCleared << "   ";

    gotoxy(xHUD, 6);
    cout << "Level : " << level << "   ";

    gotoxy(xHUD, 8);
    cout << "Time  : ";

    if (isUltraMode)
        cout << timeLeft << "s   ";
    else
        cout << elapsedTime << "s   ";
}

void showUltraResult() {
    system("cls");
    cout << "\n\n";
    cout << "========== ULTRA MODE ==========\n\n";
    cout << "TIME'S UP!\n\n";

    cout << "Final Score : " << score << endl;
    cout << "Lines Cleared : " << linesCleared << endl;
    cout << "Level Reached : " << level << endl;

    cout << "\nNhan phim bat ky de thoat...";
    _getch();
}

void showSprintResult() {
    system("cls");
    cout << "\n\n";
    cout << "========== SPRINT MODE ==========\n\n";
    cout << "40 LINES CLEARED!\n\n";

    cout << "Time Taken : " << elapsedTime << "s\n";
    cout << "Final Score : " << score << endl;
    cout << "Level Reached : " << level << endl;

    cout << "\nNhan phim bat ky de thoat...";
    _getch();
}

int main()
{
    system("chcp 437 > nul");// thêm đúng codepage vào


    system("cls");

    int mode = showMainMenu();

    // đổi biến chế độ chơi
    if (mode == 2) { // Sprint 40 Lines
        isSprintMode = true;
    }
    if (mode == 3) { // Ultra
        isUltraMode = true;
    }

    // Sau đó mới chạy game chính
    system("pause");

    srand(time(0));
    b = rand() % 7;
    nextBlock = rand() % 7;
    system("cls");
    initBoard();
    gameStartTime = GetTickCount();

    while (1) {
        boardDelBlock();

        // ===== UPDATE TIMER =====
        elapsedTime = (GetTickCount() - gameStartTime) / 1000;

        if (isUltraMode) {
            timeLeft = ULTRA_TIME - elapsedTime;
        }

        // ===== ULTRA END CONDITION =====
        if (isUltraMode && timeLeft <= 0) {
            showUltraResult();
            break;
        }

        // điều chỉnh tốc độ
        Sleep(speed); // thay biến speed kiểm soát tốc độ vào

        // nhận input điều khiển từ bàn phím
        if (_kbhit()) {
            char c = _getch();
            if (c == 'w') rotateBlock(); //điều khiển xoay
            if (c == 'a' && canMove(-1, 0)) x--;
            if (c == 'd' && canMove(1, 0)) x++;
            if (c == 's' && canMove(0, 1))  y++; //soft drop
            if (c == 'q') break;
            if (c == 'x') {
                hardDrop();
            }
        }

        // gameplay, xoá line,...
        if (canMove(0, 1)) y++;
        else {
            lock_block:
            block2Board();
            int cleared= removeLine();
            if(cleared>0) {
                updateScore(cleared);
                updateLevel();
            }

            // ===== SPRINT END CONDITION =====
            if (isSprintMode && linesCleared >= SPRINT_TARGET) {
                showSprintResult();
                break;
            }

            x = 6; y = 1; b = nextBlock; nextBlock = rand() % 7;

            //check game over sau khi sinh block mới
            if (isGameOver()) {
                showGameOver();
                break;
            }
        }

        // render
        block2Board();
        draw();

        // ===== DRAW GHOST PIECE =====
        int ghostY = getGhostY();

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (blocks[b][i][j] != ' ') {
                    int gx = x + j;
                    int gy = ghostY + i;

                    // không vẽ đè block thật
                    if (gy != y + i) {
                        gotoxy(gx, gy);
                        cout << '.';
                    }
                }
            }
        }

        //render
        drawHUD();
        drawNextBlock();

    }
    return 0;
}
