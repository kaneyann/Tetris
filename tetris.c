//==========================================================
// �e�g���X(�R���\�[��)
//==========================================================

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <time.h>

//--------------------------------------------------------------------
//  �}�N����`
//--------------------------------------------------------------------

//--- �e�g���X�̃T�C�Y
#define HEIGHT 21    // �c
#define WIDTH  12    // ��

//--- �u���b�N�Ɋւ���}�N��
#define BLOCK_SIZE 4    // �u���b�N�̃T�C�Y
#define X_INIT 4        // �u���b�N�̏������W(x)
#define Y_INIT 0        // �u���b�N�̏������W(y)
#define VARIETY 7       // �u���b�N�̎��

/* �e�L�[�̕����R�[�h */
//--- ���L�[(1�o�C�g��)
#define ALLOW_KEY_FIRST 224    // �e���L�[��1�o�C�g�ڂ̕����R�[�h��224�ŋ���
//--- ���L�[(2�o�C�g��)
#define LEFT_KEY  75    // �����L�[�̕����R�[�h
#define RIGHT_KEY 77    // �E���L�[�̕����R�[�h
#define DOWN_KEY  80    // �����L�[�̕����R�[�h
//--- �X�y�[�X�L�[
#define SPACE_KEY 32    // �X�y�[�X�L�[�̕����R�[�h

//--------------------------------------------------------------------
//  ����`
//--------------------------------------------------------------------

enum { EMPTY = 0, BLOCK = 1, WALL = 9 };    // �e�u���b�N

//--------------------------------------------------------------------
//  ���錾
//--------------------------------------------------------------------

//--- �t�B�[���h�ƃX�e�[�W
int field[HEIGHT][WIDTH];    // �t�B�[���h
int stage[HEIGHT][WIDTH];    // �X�e�[�W

//--- �u���b�N�̐錾
/* ��(1��ޖ�)
    ��������                           ��������
    ��������  �E��90�x��]������Ɓ�   ��������
    ��������                           ��������
    ��������                           ��������
*/
int blockList[VARIETY][BLOCK_SIZE][BLOCK_SIZE] = {
    //--- �c�_
    {{ EMPTY, BLOCK, EMPTY, EMPTY },
     { EMPTY, BLOCK, EMPTY, EMPTY },
     { EMPTY, BLOCK, EMPTY, EMPTY },
     { EMPTY, BLOCK, EMPTY, EMPTY }},
    //--- �����`
    {{ EMPTY, EMPTY, EMPTY, EMPTY },
     { EMPTY, BLOCK, BLOCK, EMPTY },
     { EMPTY, BLOCK, BLOCK, EMPTY },
     { EMPTY, EMPTY, EMPTY, EMPTY }},
    //--- ��Ȍ^
    {{ EMPTY, BLOCK, EMPTY, EMPTY },
     { EMPTY, BLOCK, BLOCK, EMPTY },
     { EMPTY, EMPTY, BLOCK, EMPTY },
     { EMPTY, EMPTY, EMPTY, EMPTY }},
    //--- �t��Ȍ^
    {{ EMPTY, EMPTY, BLOCK, EMPTY },
     { EMPTY, BLOCK, BLOCK, EMPTY },
     { EMPTY, BLOCK, EMPTY, EMPTY },
     { EMPTY, EMPTY, EMPTY, EMPTY }},
    //--- L���^
    {{ EMPTY, BLOCK, EMPTY, EMPTY },
     { EMPTY, BLOCK, EMPTY, EMPTY },
     { EMPTY, BLOCK, BLOCK, EMPTY },
     { EMPTY, EMPTY, EMPTY, EMPTY }},
    //--- �tL���^
    {{ EMPTY, EMPTY, BLOCK, EMPTY },
     { EMPTY, EMPTY, BLOCK, EMPTY },
     { EMPTY, BLOCK, BLOCK, EMPTY },
     { EMPTY, EMPTY, EMPTY, EMPTY }},
    //--- �tT���^
    {{ EMPTY, BLOCK, EMPTY, EMPTY },
     { BLOCK, BLOCK, BLOCK, EMPTY },
     { EMPTY, EMPTY, EMPTY, EMPTY },
     { EMPTY, EMPTY, EMPTY, EMPTY }},
};
int block[BLOCK_SIZE][BLOCK_SIZE];

//--- �����u���b�N�̍��W
int x = X_INIT;
int y = Y_INIT;

//--- �Q�[���I�[�o�[�t���O
int gameOver = false;

//--- �����������C�����̍��v
int totalLines = 0;

//--------------------------------------------------------------------
//  �֐����^�錾
//--------------------------------------------------------------------

void screenInit(void);                        // ��ʂ̏�����
void createBlock(void);                       // �����p�u���b�N�̐���
void showGameField(void);                     // �t�B�[���h�̕`��
void moveBlock(int xNext, int yNext);         // �u���b�N�̈ړ�
bool checkCollision(int xNext, int yNext);    // �u���b�N�̏Փ˔���
void lockBlock(void);                         // �u���b�N�̌Œ�
void controlBlock(void);                      // �u���b�N�̃R���g���[��
void turnBlock(void);                         // �u���b�N�̉�]
void checkLines(void);                        // ���C���̏�������
void deleteLine(int line);                    // ��1��̏��� �� stage�S�̂�1�i���ɂ��炷
void processBlock(void);                      // �u���b�N�Ɋւ��鏈��

//====================================================================
//  �{�̏���
//====================================================================

//--------------------------------------------------------------------
//  main�֐�
//--------------------------------------------------------------------

int main(void)
{
    int sleepTime = 0;
    
    srand((unsigned)time(NULL));    // �u���b�N�����p�̗����̃V�[�h��ݒ�
    screenInit();       // ��ʂ̏�����
    createBlock();      // �u���b�N�̐���
    showGameField();    // �t�B�[���h�̕`��
    Sleep(500);         // �u���b�N�𐶐�������A��x�����ŉ�ʂ��~�߂čl���鎞�Ԃ�����

    //--- �Q�[���I�[�o�[�܂ŏ������p��
    while ( 1 ) {
        if ( gameOver ) {
            system("cls");
            printf("\n\nGAME OVER\n\n");
            break;
        }
        //--- ���̓`�F�b�N
        if ( _kbhit() ) {    // ���͂�����Ȃ��
            controlBlock();
        }
        //--- time��6�疜�ɂȂ�܂ňȍ~�̏������s��Ȃ�(�����̋x�~�ɑ���)
        while ( sleepTime < 60000000 ) {
            sleepTime++;
            continue;
        }
        processBlock();      // �u���b�N�Ɋւ��鏈��
        showGameField();     // �t�B�[���h�̕`��
        sleepTime = 0;
    }
    
    return 0;
}

//--------------------------------------------------------------------
//  ��ʂ̏�����
//--------------------------------------------------------------------

void screenInit(void)
{
    int i, j;
    
    //--- stage��field�Ɋe�u���b�N�̐��l����
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
//  �����p�u���b�N�̐���
//--------------------------------------------------------------------

void createBlock(void)
{
    int blockType;
    int i, j;
    
    blockType = rand() % VARIETY;
    //--- ��������u���b�N���u���b�N���X�g����ǂݍ���
    for ( i = 0; i < BLOCK_SIZE; i++ ) {
        for ( j = 0; j < BLOCK_SIZE; j++ ) {
            block[i][j] = blockList[blockType][i][j];
        }
    }
    
    //--- ���������u���b�N���t�B�[���h�ɓǂݍ���
    for ( i = 0; i < BLOCK_SIZE; i++ ) {
        for ( j = 0; j < BLOCK_SIZE; j++ ) {
            field[i+Y_INIT][j+X_INIT] = stage[i+Y_INIT][j+X_INIT] + block[i][j];
            printf("%d\n", stage[i+Y_INIT][j+X_INIT]);
            //--- �����ʒu�ɐ������ꂽ�u���b�N���Œ�u���b�N�ɏd�Ȃ��Ă���΃Q�[���I�[�o�[
            if ( field[i+Y_INIT][j+X_INIT] > BLOCK ) {
                gameOver = true;
                return;
            }
        }
    }
}

//--------------------------------------------------------------------
//  �t�B�[���h�̕`��
//--------------------------------------------------------------------

void showGameField(void)
{
    int i, j;
    
    //--- ��ʂ̃N���A
    system("cls");
    
    //--- �t�B�[���h�̕`��
    for ( i = 0; i < HEIGHT; i++ ) {
        for ( j = 0; j < WIDTH; j++ ) {
            switch ( field[i][j] ) {
                case EMPTY :
                    printf("  ");
                    break;
                case WALL :
                    printf("��");
                    break;
                case BLOCK :
                    printf("��");
            }
        }
        printf("\n");
    }
    printf("Total Lines : %d\n", totalLines);
}

//--------------------------------------------------------------------
//  �u���b�N�̈ړ�
//--------------------------------------------------------------------

void moveBlock(int xNext, int yNext)
{
    int i, j;
    
    //--- �������O�̃u���b�N������
    for ( i = 0; i < BLOCK_SIZE; i++ ) {
        for ( j = 0; j < BLOCK_SIZE; j++ ) {
            field[i+y][j+x] -= block[i][j];
        }
    }
    
    //--- �u���b�N�̍��W���X�V
    x = xNext;
    y = yNext;
    
    // �V�K���W���t�B�[���h�ɐݒ�
    for ( i = 0; i < BLOCK_SIZE; i++ ) {
        for ( j = 0; j < BLOCK_SIZE; j++ ) {
            field[i+y][j+x] += block[i][j];
        }
    }
}

//--------------------------------------------------------------------
//  �u���b�N�̏Փ˔���
//--------------------------------------------------------------------

bool checkCollision(int xNext, int yNext)
{
    int i, j;
    
    //--- ��or�Œ�u���b�N�̏Փ˔���
    for ( i = 0; i < BLOCK_SIZE; i++ ) {
        for ( j = 0; j < BLOCK_SIZE; j++ ) {
            //--- �u���b�N�Ȃ��
            if ( block[i][j] == BLOCK ) {
                //--- �ړ�����W����łȂ��Ȃ��(��or�Œ�u���b�N�Ȃ��)�ړ��ł��Ȃ�
                if ( stage[i+yNext][j+xNext] != EMPTY ) {
                    return false;
                }
            }
        }
    }
    return true;    // �ړ��ł���
}

//--------------------------------------------------------------------
//  �u���b�N�̌Œ�
//--------------------------------------------------------------------

void lockBlock(void)
{
    int i, j;
    
    //--- �u���b�N�̌Œ�
    for ( i = 0; i < HEIGHT; i++ ) {
        for ( j = 0; j < WIDTH; j++ ) {
            stage[i][j] = field[i][j];
        }
    }
}

//--------------------------------------------------------------------
//  �u���b�N�̃R���g���[��
//--------------------------------------------------------------------

void controlBlock(void)
{
    int key;
    
    //--- ���L�[�̕����R�[�h��2�o�C�g(�ŏ��Ɂu224�v�A���̎��Ɋe�L�[��\�����l������)�Ȃ̂ŁA2�i�K�ɕ����Ď擾
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
//  �u���b�N�̉�]
//--------------------------------------------------------------------

void turnBlock(void)
{
    int tmp[BLOCK_SIZE][BLOCK_SIZE];
    int i, j;
    
    //--- ��]�O�̃u���b�N���ꎞ�I�ɕۑ�
    for ( i = 0; i < BLOCK_SIZE; i++ ) {
        for ( j = 0; j < BLOCK_SIZE; j++ ) {
            tmp[i][j] = block[i][j];
        }
    }
    
    //--- �u���b�N����]
    for ( i = 0; i < BLOCK_SIZE; i++ ) {
        for ( j = 0; j < BLOCK_SIZE; j++ ) {
            // x���W��y���W�����ւ��āAy���W�̒l����납��擾
            block[i][j] = tmp[(BLOCK_SIZE-1)-j][i];
        }
    }
    
    //--- ��]��̃u���b�N����or�Œ�u���b�N�ɏՓ˂���Ȃ�΁A��]�O�ɖ߂��ď����𒆎~
    if ( checkCollision(x, y) == false ) {
        for ( i = 0; i < BLOCK_SIZE; i++ ) {
            for ( j = 0; j < BLOCK_SIZE; j++ ) {
                block[i][j] = tmp[i][j];
            }
        }
        return;
    }
    
    //--- ��]�O�̃u���b�N��field��������āA��]��̃u���b�N��field�ɐݒ�
    for ( i = 0; i < BLOCK_SIZE; i++ ) {
        for ( j = 0; j < BLOCK_SIZE; j++ ) {
            field[i+y][j+x] = block[i][j];
        }
    }
}

//--------------------------------------------------------------------
//  ���C���̏�������
//--------------------------------------------------------------------

void checkLines(void)
{
    int lines = 0;    // �����ɏ��������C����
    bool complete;
    int i, j;
    
    for ( i = 0; i < HEIGHT-1; i++ ) {       // ���[�̍s�͑������Ȃ�(�ǂ�����)
        complete = true;
        for ( j = 1; j < WIDTH-1; j++ ) {    // ���[�̗�͑������Ȃ�(�ǂ�����)
            if ( stage[i][j] == EMPTY ) {
                complete = false;
                break;                       // ���̍s��
            }
        }
        if ( complete == true ) {    // ��1�񂪃��C���ɂȂ��Ă���Ȃ��
            //--- ��1��̏��� �� stage�S�̂�1�i���ɂ��炷
            deleteLine(i);
            lines++;
        }
    }
    
    //--- ���������C���������Z���Ă���
    totalLines += lines;
}

//--------------------------------------------------------------------
// ��1��̏��� �� stage�S�̂�1�i���ɂ��炷
//--------------------------------------------------------------------

void deleteLine(int line)
{
    int i, j;
    
    //--- ��1�������
    for ( j = 1; j < WIDTH-1; j++ ) {
        stage[line][j] = EMPTY;
    }

    //--- stage�S�̂�1�i���ɂ��炷
    for ( i = line; i > 0; i-- ) {
        for ( j = 1; j < WIDTH-1; j++ ) {
            stage[i][j] = stage[i-1][j];
        }
    }
    
    //--- stage�̏���field�ɔ��f
    for ( i = 0; i < HEIGHT; i++ ) {
        for ( j = 0; j < WIDTH; j++ ) {
            field[i][j] = stage[i][j];
        }
    }
}

//--------------------------------------------------------------------
// �u���b�N�Ɋւ��鏈��
//--------------------------------------------------------------------

void processBlock(void)
{
    if ( checkCollision(x, y+1) ) {    // ���̍��W�Ɉړ������Ƃ��ɏՓ˂��Ȃ��Ȃ��
        moveBlock(x, y+1);    // �u���b�N��1���W���Ɉړ�
    } else {                  // �Փ˂���Ȃ��
        lockBlock();          // �u���b�N�̌Œ�
        checkLines();         // ���C���̏�������
        x = X_INIT;           // x���������W�ɖ߂�
        y = Y_INIT;           // y���������W�ɖ߂�
        createBlock();        // �����p�u���b�N�̐���
        showGameField();      // �t�B�[���h�̕`��
        Sleep(500);           // �u���b�N�𐶐�������A��x�����ŉ�ʂ��~�߂čl���鎞�Ԃ�����
    }
}