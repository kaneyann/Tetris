//==========================================================
// テトリス(コンソール)
//==========================================================

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <time.h>

//--------------------------------------------------------------------
//  マクロ定義
//--------------------------------------------------------------------

//--- テトリスのサイズ
#define HEIGHT 21    // 縦
#define WIDTH  12    // 横

//--- ブロックに関するマクロ
#define BLOCK_SIZE 4    // ブロックのサイズ
#define X_INIT 4        // ブロックの初期座標(x)
#define Y_INIT 0        // ブロックの初期座標(y)
#define VARIETY 7       // ブロックの種類

/* 各キーの文字コード */
//--- 矢印キー(1バイト目)
#define ALLOW_KEY_FIRST 224    // 各矢印キーの1バイト目の文字コードは224で共通
//--- 矢印キー(2バイト目)
#define LEFT_KEY  75    // 左矢印キーの文字コード
#define RIGHT_KEY 77    // 右矢印キーの文字コード
#define DOWN_KEY  80    // 下矢印キーの文字コード
//--- スペースキー
#define SPACE_KEY 32    // スペースキーの文字コード

//--------------------------------------------------------------------
//  大域定義
//--------------------------------------------------------------------

enum { EMPTY = 0, BLOCK = 1, WALL = 9 };    // 各ブロック

//--------------------------------------------------------------------
//  大域宣言
//--------------------------------------------------------------------

//--- フィールドとステージ
int field[HEIGHT][WIDTH];    // フィールド
int stage[HEIGHT][WIDTH];    // ステージ

//--- ブロックの宣言
/* 例(1種類目)
    □■□□                           □□□□
    □■□□  右に90度回転させると→   ■■■■
    □■□□                           □□□□
    □■□□                           □□□□
*/
int blockList[VARIETY][BLOCK_SIZE][BLOCK_SIZE] = {
    //--- 縦棒
    {{ EMPTY, BLOCK, EMPTY, EMPTY },
     { EMPTY, BLOCK, EMPTY, EMPTY },
     { EMPTY, BLOCK, EMPTY, EMPTY },
     { EMPTY, BLOCK, EMPTY, EMPTY }},
    //--- 正方形
    {{ EMPTY, EMPTY, EMPTY, EMPTY },
     { EMPTY, BLOCK, BLOCK, EMPTY },
     { EMPTY, BLOCK, BLOCK, EMPTY },
     { EMPTY, EMPTY, EMPTY, EMPTY }},
    //--- 稲妻型
    {{ EMPTY, BLOCK, EMPTY, EMPTY },
     { EMPTY, BLOCK, BLOCK, EMPTY },
     { EMPTY, EMPTY, BLOCK, EMPTY },
     { EMPTY, EMPTY, EMPTY, EMPTY }},
    //--- 逆稲妻型
    {{ EMPTY, EMPTY, BLOCK, EMPTY },
     { EMPTY, BLOCK, BLOCK, EMPTY },
     { EMPTY, BLOCK, EMPTY, EMPTY },
     { EMPTY, EMPTY, EMPTY, EMPTY }},
    //--- L字型
    {{ EMPTY, BLOCK, EMPTY, EMPTY },
     { EMPTY, BLOCK, EMPTY, EMPTY },
     { EMPTY, BLOCK, BLOCK, EMPTY },
     { EMPTY, EMPTY, EMPTY, EMPTY }},
    //--- 逆L字型
    {{ EMPTY, EMPTY, BLOCK, EMPTY },
     { EMPTY, EMPTY, BLOCK, EMPTY },
     { EMPTY, BLOCK, BLOCK, EMPTY },
     { EMPTY, EMPTY, EMPTY, EMPTY }},
    //--- 逆T字型
    {{ EMPTY, BLOCK, EMPTY, EMPTY },
     { BLOCK, BLOCK, BLOCK, EMPTY },
     { EMPTY, EMPTY, EMPTY, EMPTY },
     { EMPTY, EMPTY, EMPTY, EMPTY }},
};
int block[BLOCK_SIZE][BLOCK_SIZE];

//--- 落下ブロックの座標
int x = X_INIT;
int y = Y_INIT;

//--- ゲームオーバーフラグ
int gameOver = false;

//--- 消去したライン数の合計
int totalLines = 0;

//--------------------------------------------------------------------
//  関数原型宣言
//--------------------------------------------------------------------

void screenInit(void);                        // 画面の初期化
void createBlock(void);                       // 落下用ブロックの生成
void showGameField(void);                     // フィールドの描画
void moveBlock(int xNext, int yNext);         // ブロックの移動
bool checkCollision(int xNext, int yNext);    // ブロックの衝突判定
void lockBlock(void);                         // ブロックの固定
void controlBlock(void);                      // ブロックのコントロール
void turnBlock(void);                         // ブロックの回転
void checkLines(void);                        // ラインの消去判定
void deleteLine(int line);                    // 横1列の消去 ＆ stage全体を1段下にずらす
void processBlock(void);                      // ブロックに関する処理

//====================================================================
//  本体処理
//====================================================================

//--------------------------------------------------------------------
//  main関数
//--------------------------------------------------------------------

int main(void)
{
    int sleepTime = 0;
    
    srand((unsigned)time(NULL));    // ブロック生成用の乱数のシードを設定
    screenInit();       // 画面の初期化
    createBlock();      // ブロックの生成
    showGameField();    // フィールドの描画
    Sleep(500);         // ブロックを生成したら、一度そこで画面を止めて考える時間をつくる

    //--- ゲームオーバーまで処理を継続
    while ( 1 ) {
        if ( gameOver ) {
            system("cls");
            printf("\n\nGAME OVER\n\n");
            break;
        }
        //--- 入力チェック
        if ( _kbhit() ) {    // 入力があるならば
            controlBlock();
        }
        //--- timeが6千万になるまで以降の処理を行わない(処理の休止に相当)
        while ( sleepTime < 60000000 ) {
            sleepTime++;
            continue;
        }
        processBlock();      // ブロックに関する処理
        showGameField();     // フィールドの描画
        sleepTime = 0;
    }
    
    return 0;
}

//--------------------------------------------------------------------
//  画面の初期化
//--------------------------------------------------------------------

void screenInit(void)
{
    int i, j;
    
    //--- stageとfieldに各ブロックの数値を代入
    for ( i = 0; i < HEIGHT; i++ ) {
        for ( j = 0; j < WIDTH; j++ ) {
            if ( i == HEIGHT-1 || j == 0 || j == WIDTH-1 ) {
                stage[i][j] = field[i][j] = WALL;
            } else {
                stage[i][j] = field[i][j] = EMPTY;
            }
        }
    }
}

//--------------------------------------------------------------------
//  落下用ブロックの生成
//--------------------------------------------------------------------

void createBlock(void)
{
    int blockType;
    int i, j;
    
    blockType = rand() % VARIETY;
    //--- 生成するブロックをブロックリストから読み込む
    for ( i = 0; i < BLOCK_SIZE; i++ ) {
        for ( j = 0; j < BLOCK_SIZE; j++ ) {
            block[i][j] = blockList[blockType][i][j];
        }
    }
    
    //--- 生成したブロックをフィールドに読み込む
    for ( i = 0; i < BLOCK_SIZE; i++ ) {
        for ( j = 0; j < BLOCK_SIZE; j++ ) {
            field[i+Y_INIT][j+X_INIT] = stage[i+Y_INIT][j+X_INIT] + block[i][j];
            printf("%d\n", stage[i+Y_INIT][j+X_INIT]);
            //--- 初期位置に生成されたブロックが固定ブロックに重なっていればゲームオーバー
            if ( field[i+Y_INIT][j+X_INIT] > BLOCK ) {
                gameOver = true;
                return;
            }
        }
    }
}

//--------------------------------------------------------------------
//  フィールドの描画
//--------------------------------------------------------------------

void showGameField(void)
{
    int i, j;
    
    //--- 画面のクリア
    system("cls");
    
    //--- フィールドの描画
    for ( i = 0; i < HEIGHT; i++ ) {
        for ( j = 0; j < WIDTH; j++ ) {
            switch ( field[i][j] ) {
                case EMPTY :
                    printf("  ");
                    break;
                case WALL :
                    printf("■");
                    break;
                case BLOCK :
                    printf("□");
            }
        }
        printf("\n");
    }
    printf("Total Lines : %d\n", totalLines);
}

//--------------------------------------------------------------------
//  ブロックの移動
//--------------------------------------------------------------------

void moveBlock(int xNext, int yNext)
{
    int i, j;
    
    //--- 落下直前のブロックを消去
    for ( i = 0; i < BLOCK_SIZE; i++ ) {
        for ( j = 0; j < BLOCK_SIZE; j++ ) {
            field[i+y][j+x] -= block[i][j];
        }
    }
    
    //--- ブロックの座標を更新
    x = xNext;
    y = yNext;
    
    // 新規座標をフィールドに設定
    for ( i = 0; i < BLOCK_SIZE; i++ ) {
        for ( j = 0; j < BLOCK_SIZE; j++ ) {
            field[i+y][j+x] += block[i][j];
        }
    }
}

//--------------------------------------------------------------------
//  ブロックの衝突判定
//--------------------------------------------------------------------

bool checkCollision(int xNext, int yNext)
{
    int i, j;
    
    //--- 壁or固定ブロックの衝突判定
    for ( i = 0; i < BLOCK_SIZE; i++ ) {
        for ( j = 0; j < BLOCK_SIZE; j++ ) {
            //--- ブロックならば
            if ( block[i][j] == BLOCK ) {
                //--- 移動先座標が空でないならば(壁or固定ブロックならば)移動できない
                if ( stage[i+yNext][j+xNext] != EMPTY ) {
                    return false;
                }
            }
        }
    }
    return true;    // 移動できる
}

//--------------------------------------------------------------------
//  ブロックの固定
//--------------------------------------------------------------------

void lockBlock(void)
{
    int i, j;
    
    //--- ブロックの固定
    for ( i = 0; i < HEIGHT; i++ ) {
        for ( j = 0; j < WIDTH; j++ ) {
            stage[i][j] = field[i][j];
        }
    }
}

//--------------------------------------------------------------------
//  ブロックのコントロール
//--------------------------------------------------------------------

void controlBlock(void)
{
    int key;
    
    //--- 矢印キーの文字コードは2バイト(最初に「224」、その次に各キーを表す数値がある)なので、2段階に分けて取得
    key = _getch();
    if ( key == ALLOW_KEY_FIRST ) { key = _getch(); }
    
    switch ( key ) {
    case LEFT_KEY :
        if ( checkCollision(x-1, y) ) {
            moveBlock(x-1, y);
        }
        break;
    case RIGHT_KEY :
        if ( checkCollision(x+1, y) ) {
            moveBlock(x+1, y);
        }
        break;        
    case DOWN_KEY :
        if ( checkCollision(x, y+1) ) {
            moveBlock(x, y+1);
        }
        break;
    case SPACE_KEY :
        turnBlock();
        break;
    }
}

//--------------------------------------------------------------------
//  ブロックの回転
//--------------------------------------------------------------------

void turnBlock(void)
{
    int tmp[BLOCK_SIZE][BLOCK_SIZE];
    int i, j;
    
    //--- 回転前のブロックを一時的に保存
    for ( i = 0; i < BLOCK_SIZE; i++ ) {
        for ( j = 0; j < BLOCK_SIZE; j++ ) {
            tmp[i][j] = block[i][j];
        }
    }
    
    //--- ブロックを回転
    for ( i = 0; i < BLOCK_SIZE; i++ ) {
        for ( j = 0; j < BLOCK_SIZE; j++ ) {
            // x座標とy座標を入れ替えて、y座標の値を後ろから取得
            block[i][j] = tmp[(BLOCK_SIZE-1)-j][i];
        }
    }
    
    //--- 回転後のブロックが壁or固定ブロックに衝突するならば、回転前に戻して処理を中止
    if ( checkCollision(x, y) == false ) {
        for ( i = 0; i < BLOCK_SIZE; i++ ) {
            for ( j = 0; j < BLOCK_SIZE; j++ ) {
                block[i][j] = tmp[i][j];
            }
        }
        return;
    }
    
    //--- 回転前のブロックをfieldから消して、回転後のブロックをfieldに設定
    for ( i = 0; i < BLOCK_SIZE; i++ ) {
        for ( j = 0; j < BLOCK_SIZE; j++ ) {
            field[i+y][j+x] = block[i][j];
        }
    }
}

//--------------------------------------------------------------------
//  ラインの消去判定
//--------------------------------------------------------------------

void checkLines(void)
{
    int lines = 0;    // 同時に消したライン数
    bool complete;
    int i, j;
    
    for ( i = 0; i < HEIGHT-1; i++ ) {       // 下端の行は走査しない(壁だから)
        complete = true;
        for ( j = 1; j < WIDTH-1; j++ ) {    // 両端の列は走査しない(壁だから)
            if ( stage[i][j] == EMPTY ) {
                complete = false;
                break;                       // 次の行へ
            }
        }
        if ( complete == true ) {    // 横1列がラインになっているならば
            //--- 横1列の消去 ＆ stage全体を1段下にずらす
            deleteLine(i);
            lines++;
        }
    }
    
    //--- 消したライン数を加算していく
    totalLines += lines;
}

//--------------------------------------------------------------------
// 横1列の消去 ＆ stage全体を1段下にずらす
//--------------------------------------------------------------------

void deleteLine(int line)
{
    int i, j;
    
    //--- 横1列を消去
    for ( j = 1; j < WIDTH-1; j++ ) {
        stage[line][j] = EMPTY;
    }

    //--- stage全体を1段下にずらす
    for ( i = line; i > 0; i-- ) {
        for ( j = 1; j < WIDTH-1; j++ ) {
            stage[i][j] = stage[i-1][j];
        }
    }
    
    //--- stageの情報をfieldに反映
    for ( i = 0; i < HEIGHT; i++ ) {
        for ( j = 0; j < WIDTH; j++ ) {
            field[i][j] = stage[i][j];
        }
    }
}

//--------------------------------------------------------------------
// ブロックに関する処理
//--------------------------------------------------------------------

void processBlock(void)
{
    if ( checkCollision(x, y+1) ) {    // 次の座標に移動したときに衝突しないならば
        moveBlock(x, y+1);    // ブロックを1座標下に移動
    } else {                  // 衝突するならば
        lockBlock();          // ブロックの固定
        checkLines();         // ラインの消去判定
        x = X_INIT;           // xを初期座標に戻す
        y = Y_INIT;           // yを初期座標に戻す
        createBlock();        // 落下用ブロックの生成
        showGameField();      // フィールドの描画
        Sleep(500);           // ブロックを生成したら、一度そこで画面を止めて考える時間をつくる
    }
}