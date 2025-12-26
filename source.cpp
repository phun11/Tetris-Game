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

class Piece {
protected:
    char shape[4][4];

public:
    Piece(char src[4][4]) {
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                shape[i][j] = src[i][j];
    }

    virtual void rotate() {
        char temp[4][4] = { ' ' };   // RẤT QUAN TRỌNG

        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                temp[j][3 - i] = shape[i][j];

        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                shape[i][j] = temp[i][j];
    }

    char getCell(int i, int j) const {
        return shape[i][j];
    }

    virtual ~Piece() {}
};

class OPiece : public Piece {
public:
    OPiece(char src[4][4]) : Piece(src) {}
    void rotate() override {}   // O không xoay
};

class IPiece : public Piece { public: IPiece(char s[4][4]) : Piece(s) {} };
class TPiece : public Piece { public: TPiece(char s[4][4]) : Piece(s) {} };
class SPiece : public Piece { public: SPiece(char s[4][4]) : Piece(s) {} };
class ZPiece : public Piece { public: ZPiece(char s[4][4]) : Piece(s) {} };
class JPiece : public Piece { public: JPiece(char s[4][4]) : Piece(s) {} };
class LPiece : public Piece { public: LPiece(char s[4][4]) : Piece(s) {} };

Piece* createPiece(int type) {
    switch (type) {
    case 0: return new IPiece(blocks[0]);
    case 1: return new OPiece(blocks[1]);
    case 2: return new TPiece(blocks[2]);
    case 3: return new SPiece(blocks[3]);
    case 4: return new ZPiece(blocks[4]);
    case 5: return new JPiece(blocks[5]);
    case 6: return new LPiece(blocks[6]);
    }
    return nullptr;
}

Piece* curPiece = nullptr;
Piece* nextPiece = nullptr;

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

void setBlockColor(char c) {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

    switch (c) {
    case 'I': SetConsoleTextAttribute(h, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY); break; // Cyan
    case 'O': SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY); break; // Yellow
    case 'T': SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY); break; // Magenta
    case 'S': SetConsoleTextAttribute(h, FOREGROUND_GREEN | FOREGROUND_INTENSITY); break;
    case 'Z': SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_INTENSITY); break;
    case 'J': SetConsoleTextAttribute(h, FOREGROUND_BLUE | FOREGROUND_INTENSITY); break;
    case 'L': SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_GREEN); break; // Orange-ish
    default:
        SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }
}

void boardDelBlock() {
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (curPiece->getCell(i, j) != ' ' && y + j < H)
                board[y + i][x + j] = ' ';
}

void block2Board() {
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (curPiece->getCell(i, j) != ' ')
                board[y + i][x + j] = curPiece->getCell(i, j);
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

void drawBorderOnly() {
    gotoxy(0, 0);
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            if (i == 0 || i == H - 1 || j == 0 || j == W - 1)
                cout << board[i][j];
            else
                cout << ' ';
        }
        cout << endl;
    }
}

void draw() {
    for (int i = 1; i < H - 1; i++) {
        gotoxy(1, i);
        for (int j = 1; j < W - 1; j++) {

            char c = board[i][j];
            if (c != ' ') {
                setBlockColor(c);
                cout << char(219);
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                    FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            }
            else {
                cout << ' ';
            }
        }
    }
}


int getGhostY() {
    int ghostY = y;

    while (true) {
        ghostY++;

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (curPiece->getCell(i, j) != ' ') {
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
            if (curPiece->getCell(i, j) != ' ') {
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

bool isGameOver() {
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (curPiece->getCell(i, j) != ' ') {
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
    int boxX = W + 15;
    int boxY = 12;

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
        gotoxy(boxX + 1, boxY + 2 + i);
        for (int j = 0; j < 4; j++) {
            char c = nextPiece->getCell(i, j);
            if (c != ' ') {
                setBlockColor(c);
                cout << char(219);
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                    FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            }
            else cout << ' ';
        }
    }
}

void drawHUD() {
    int xHUD = W + 3;

    gotoxy(xHUD, 2);
    cout << "Score : " << score << "    ";

    gotoxy(xHUD, 4);
    cout << "Lines : " << linesCleared << "    ";

    gotoxy(xHUD, 6);
    cout << "Level : " << level << "    ";

    gotoxy(xHUD, 8);
    cout << "Time  : ";

    if (isUltraMode)
        cout << timeLeft << "s    ";
    else
        cout << elapsedTime << "s    ";
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

    // ===== HIDE CONSOLE CURSOR =====
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.dwSize = 1;
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);

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
    curPiece = createPiece(rand() % 7);
    nextPiece = createPiece(rand() % 7);
    system("cls");
    initBoard();
    drawBorderOnly();
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
            if (c == 'w') {
                boardDelBlock();

                Piece backup = *curPiece;   // sao lưu
                curPiece->rotate();

                if (!canMove(0, 0)) {
                    *curPiece = backup;    // rollback
                }
            }
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

            x = 6; y = 1;
            delete curPiece;
            curPiece = nextPiece;
            nextPiece = createPiece(rand() % 7);

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
                if (curPiece->getCell(i, j) != ' ') {
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
