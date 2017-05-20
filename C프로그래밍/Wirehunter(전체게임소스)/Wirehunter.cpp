// Wirehunter.cpp : ÄÜ¼Ö ÀÀ¿ë ÇÁ·Î±×·¥¿¡ ´ëÇÑ ÁøÀÔÁ¡À» Á¤ÀÇÇÕ´Ï´Ù.
//
// WireHunter01.cpp : ÄÜ¼Ö ÀÀ¿ë ÇÁ·Î±×·¥¿¡ ´ëÇÑ ÁøÀÔÁ¡À» Á¤ÀÇÇÕ´Ï´Ù.

#include "stdafx.h"
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <fmod.h>

#define ENEMY_COUNT 30
#define END_LINE	21
#define BULLET_COUNT 4

typedef enum _GAME_STATE { INIT, RUNNING, SUCCESS, FAILED, RESULT, END } GAME_STATE;

struct PLAY
{
	int		nIndex;
	int		nX, nY;
	int		nLifePower; 
};

struct ENEMY
{
	int		nLife;			// »ý¸í
	int		nX, nY;			// ÁÂÇ¥	
	int		nYMoveDist;		// ÀÌµ¿ °Å¸® 
	int		nOldFrame;	
	int		nAppearFrame;   // ÃâÇö ÇÁ·¹ÀÓ	
};

// ÃÑ¾ËÀÇ ¼Óµµ´Â ÀüºÎ °°µµ·Ï ÇÑ´Ù.
struct BULLET
{
	int		nLife;
	int		nX, nY;
	int		nOldFrame;
};

PLAY	g_sPlay;

/* 1ÃÊ¿¡ 30fps°¡ ³ª¿À¸ç 1 ½ºÅ×ÀÌÁö¸¦ 30ÃÊ·Î Á¦¾ÈÀ» Çß´Ù¸é 30 * 30 À¸·Î ÃÑ 30ÃÊµ¿¾È¿¡ 900 fps°¡ ÀÏ¾î³­´Ù.
ÀÌ 900 fps´Â Àû Ä³¸¯ÅÍ°¡ ³ª¿À°Ô µÇ´Â ÀüÃ¼ ½Ã°£ÀÌ µÈ´Ù.*/

ENEMY	g_sEnemy[ENEMY_COUNT] = { { 0, 1, 0, 0, 0, 5 }, { 0, 1, 0, 0, 0, 5 }, { 0, 1, 0, 0, 0, 5 }, { 0, 1, 0, 0, 0, 5 }, 
{ 0, 1, 0, 0, 0, 30 }, { 0, 1, 0, 0, 0, 35 }, { 0, 1, 0, 0, 0, 50 }, { 0, 1, 0, 0, 0, 100 }, { 0, 1, 0, 0, 0, 150 },
{ 0, 1, 0, 0, 0, 150 }, { 0, 1, 0, 0, 0, 200 }, { 0, 1, 0, 0, 0, 300 }, { 0, 1, 0, 0, 0, 320 }, { 0, 1, 0, 0, 0, 350 }, 
{ 0, 1, 0, 0, 0, 400 }, { 0, 1, 0, 0, 0, 420 }, { 0, 1, 0, 0, 0, 470 }, { 0, 1, 0, 0, 0, 500 }, { 0, 1, 0, 0, 0, 530 },
{ 0, 1, 0, 0, 0, 560 }, { 0, 1, 0, 0, 0, 600 }, { 0, 1, 0, 0, 0, 620 }, { 0, 1, 0, 0, 0, 650 }, { 0, 1, 0, 0, 0, 680 }, 
{ 0, 1, 0, 0, 0, 700 }, { 0, 1, 0, 0, 0, 720 }, { 0, 1, 0, 0, 0, 750 }, { 0, 1, 0, 0, 0, 780 }, { 0, 1, 0, 0, 0, 800 },
{ 0, 1, 0, 0, 0, 850 }
};

BULLET g_sBullet[BULLET_COUNT] = { 0, };

unsigned int g_nFrameCount = 0;
int g_nXPos[4] = { 6, 12, 19, 26 };
unsigned int	g_nBulletVeloc = 4;

int g_nGrade = 0;		// Á¡¼ö 
int g_nGoal = 0; 

struct STAGE_INFO
{
	int nGoal;  // ½ºÅ×ÀÌÁöº° ¸ñÇ¥ Àû Ä³¸¯ÅÍ¼ö 
	int nYDist; // ½ºÅ×ÀÌÁöº° ÀÌµ¿ ¼Óµµ
};

STAGE_INFO g_StageInfo[6] = { { 10, 10 }, { 15, 10 }, { 15, 8 }, { 18, 8 }, { 15, 6 }, { 17, 6 } };
GAME_STATE g_GameState = INIT;  // 0 ÃÊ±â »óÅÂ 1 º» °ÔÀÓ 2 °ÔÀÓ Á¾·á 
int g_nEnemyIndex = 0;
int g_nDeadEnemy = 0;

FMOD_SYSTEM     *g_System;
FMOD_SOUND		*g_BGSound, *g_Sound, *g_FailSound, *g_IntroSound, *g_EffectSound[4];
FMOD_CHANNEL	*g_BGChannel, *g_IntroChannel, *g_FailChannel, *g_pTempChannel;

int g_nStage = 1;

void gotoxy( int x, int y )
{
	COORD CursorPosition = { x, y };
	SetConsoleCursorPosition( GetStdHandle( STD_OUTPUT_HANDLE ), CursorPosition );
}

void IntroScreen()
{
	gotoxy( 0, 0);
	printf( "¦®¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¯\n");
	printf( "¦­     ¦­                                         ¦­          ¦­\n");
	printf( "¦­     ¡á                                         ¦­          ¦­\n");
	printf( "¦­     ¦­                                         ¦­          ¦­\n");
	printf( "¦­     ¦­             W i r e H u n t e r         ¦­          ¦­\n");
	printf( "¦­     ¦­                                         ¦­          ¦­\n");
	printf( "¦­                                                ¡á          ¦­\n");
	printf( "¦­             ÀÌµ¿Å° : ÁÂ(J), ¿ì(L), ÃÑ¾Ë(K)     ¦­          ¦­\n");
	printf( "¦­                                                ¦­          ¦­\n");
	printf( "¦­                                                            ¦­\n");
	printf( "¦­  #######                                                   ¦­\n");
	printf( "¦­ #       ##                                                 ¦­\n");
	printf( "¦­#          ###         ##                                   ¦­\n");
	printf( "¦­              ###    ##  ######             ####            ¦­\n");
	printf( "¦­                 ####          ####     ####     #          ¦­\n");
	printf( "¦­                                   #####          #         ¦­\n");
	printf( "¦­                                                    #       ¦­\n");
	printf( "¦­                                                      #     ¦­\n");
	printf( "¦­                                                        ### ¦­\n");
	printf( "¦­                                                           #¦­\n");
	printf( "¦­                                                            ¦­\n");
	printf( "¦­                                                            ¦­\n");
	printf( "¦±¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦°\n");
}

void BackScreen()
{
	// ¹è°æ È­¸é ±×¸®±â 
	gotoxy( 0, 0 );
	printf( "¦®¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¯\n");
	printf( "¦­    ¦­    ¦­     ¦­     ¦­                                  ¦­\n");
	printf( "¦­    ¦­    ¦­     ¦­     ¦­                                  ¦­\n");
	printf( "¦­    ¦­    ¦­     ¦­     ¦­                                  ¦­\n");
	printf( "¦­    ¦­    ¦­     ¦­     ¦­                                  ¦­\n");
	printf( "¦­    ¦­    ¦­     ¦­     ¦­              Á¡¼ö :              ¦­\n");
	printf( "¦­    ¦­    ¦­     ¦­     ¦­                                  ¦­\n");
	printf( "¦­    ¦­    ¦­     ¦­     ¦­              Àû Ä³¸¯ÅÍ¼ö :       ¦­\n");
	printf( "¦­    ¦­    ¦­     ¦­     ¦­                                  ¦­\n");
	printf( "¦­    ¦­    ¦­     ¦­     ¦­              »ý¸í·Â :            ¦­\n");
	printf( "¦­    ¦­    ¦­     ¦­     ¦­                                  ¦­\n");
	printf( "¦­    ¦­    ¦­     ¦­     ¦­              ½ºÅ×ÀÌÁö :          ¦­\n");
	printf( "¦­    ¦­    ¦­     ¦­     ¦­                                  ¦­\n");
	printf( "¦­    ¦­    ¦­     ¦­     ¦­                                  ¦­\n");
	printf( "¦­    ¦­    ¦­     ¦­     ¦­                                  ¦­\n");
	printf( "¦­    ¦­    ¦­     ¦­     ¦­                                  ¦­\n");
	printf( "¦­    ¦­    ¦­     ¦­     ¦­                                  ¦­\n");
	printf( "¦­    ¦­    ¦­     ¦­     ¦­                                  ¦­\n");
	printf( "¦­    ¦­    ¦­     ¦­     ¦­                                  ¦­\n");
	printf( "¦­    ¦­    ¦­     ¦­     ¦­                                  ¦­\n");
	printf( "¦­    ¦­    ¦­     ¦­     ¦­                                  ¦­\n");
	printf( "¦­    ¦­    ¦­     ¦­     ¦­                                  ¦­\n");
	printf( "¦±¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦°\n");
}

void VictroyScreen()
{
	gotoxy( 0, 0 );
	printf( "¦®¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¯\n");
	printf( "¦­ ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ ¦­\n");
	printf( "¦­ ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ ¦­\n");
	printf( "¦­ ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¾Æ±ºÀÌ ÀÌ°å´Ù !!! ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ ¦­\n");
	printf( "¦­ ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ ¦­\n");
	printf( "¦­ ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ ¦­\n");
	printf( "¦­ ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ ¦­\n");
	printf( "¦­                                                            ¦­\n");
	printf( "¦­      Score :                                               ¦­\n");
	printf( "¦­                                                            ¦­\n");
	printf( "¦­                                 ¢¼                         ¦­\n");
	printf( "¦­                                ¢¼¢¼                        ¦­\n");
	printf( "¦­                               ¢¼¢¼¢¼                       ¦­\n");
	printf( "¦­                             ¢¼¢¼¢¼¢¼¢¼                     ¦­\n");
	printf( "¦­                            ¢¼¢¼¢¼¢¼¢¼¢¼                    ¦­\n");
	printf( "¦­                         ¢¼¢¼¢¼¢¼¢¼¢¼¢¼¢¼¢¼                 ¦­\n");
	printf( "¦­ ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ ¦­\n");
	printf( "¦­ ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ ¦­\n");
	printf( "¦­ ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ ¦­\n");
	printf( "¦­ ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ ¦­\n");
	printf( "¦­ ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ ¦­\n");
	printf( "¦­ ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ ¦­\n");
	printf( "¦±¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦°\n");
}

void FailScreen()
{
	gotoxy( 0, 0 );
	printf( "¦®¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¯\n");
	printf( "¦­ ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ ¦­\n");
	printf( "¦­ ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ ¦­\n");
	printf( "¦­ ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢ÆÀû±ºÀÌ ÀÌ°å´Ù !!! ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ ¦­\n");
	printf( "¦­ ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ ¦­\n");
	printf( "¦­ ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ ¦­\n");
	printf( "¦­ ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ ¦­\n");
	printf( "¦­    ¦­    ¦­     ¦­     ¦­                                  ¦­\n");
	printf( "¦­    ¡á    ¦­     ¡á     ¡á                                  ¦­\n");
	printf( "¦­    ¦­    ¦­     ¦­     ¦­                                  ¦­\n");
	printf( "¦­    ¦­    ¡á     ¦­     ¦­                                  ¦­\n");
	printf( "¦­    ¦­    ¦­     ¦­     ¡á                                  ¦­\n");
	printf( "¦­    ¡á    ¦­     ¡á     ¦­                                  ¦­\n");
	printf( "¦­    ¦­    ¦­     ¦­     ¦­                                  ¦­\n");
	printf( "¦­    ¦­    ¡á     ¦­     ¦­                                  ¦­\n");
	printf( "¦­    ¦­    ¦­     ¦­     ¦­     ¡á    ¡á    ¡á   ¡á   ¡á     ¦­\n");
	printf( "¦­ ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ ¦­\n");
	printf( "¦­ ¢Æ¢Æ¢Æ¢Æ¢Æ¡á¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¡á¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ ¦­\n");
	printf( "¦­ ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¡á¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¡á¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ ¦­\n");
	printf( "¦­ ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ ¦­\n");
	printf( "¦­ ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¡á¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¡á¢Æ¢Æ¢Æ¢Æ¢Æ¡á¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ ¦­\n");
	printf( "¦­ ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ ¦­\n");
	printf( "¦±¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦°\n");	
}

void GameoverScreen()
{
	gotoxy( 0, 0 );
	printf( "¦®¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¯\n");
	printf( "¦­     ¦­                                         ¦­          ¦­\n");
	printf( "¦­     ¡á                                         ¦­          ¦­\n");
	printf( "¦­     ¦­   ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ    ¦­          ¦­\n");
	printf( "¦­     ¦­   ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ    ¦­          ¦­\n");
	printf( "¦­     ¦­   ¢Æ¢Æ¢Æ¢Æ¢Æ   Game Over  ¢Æ¢Æ¢Æ¢Æ¢Æ    ¦­          ¦­\n");
	printf( "¦­          ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ    ¡á          ¦­\n");
	printf( "¦­          ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ    ¦­          ¦­\n");
	printf( "¦­          ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ Score      ¢Æ¢Æ¢Æ¢Æ¢Æ    ¦­          ¦­\n");
	printf( "¦­          ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ¢Æ                ¦­\n");
	printf( "¦­  #######                                                   ¦­\n");
	printf( "¦­ #       ##                                                 ¦­\n");
	printf( "¦­#          ###         ##                                   ¦­\n");
	printf( "¦­              ###    ##  ######             ####            ¦­\n");
	printf( "¦­                 ####          ####     ####     #          ¦­\n");
	printf( "¦­                                   #####          #         ¦­\n");
	printf( "¦­                                                    #       ¦­\n");
	printf( "¦­                                                      #     ¦­\n");
	printf( "¦­                                                        ### ¦­\n");
	printf( "¦­                                                           #¦­\n");
	printf( "¦­                                                            ¦­\n");
	printf( "¦­                                                            ¦­\n");
	printf( "¦±¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦°\n");
}

void SoundInit()
{
	// ½Ã½ºÅÛ ¿ÀºêÁ§Æ® »ý¼º°ú ÃÊ±âÈ­
	FMOD_System_Create(&g_System);
	FMOD_System_Init( g_System, 32, FMOD_INIT_NORMAL, NULL);

	// ¹Ýº¹ »ç¿îµå
	FMOD_System_CreateSound( g_System, "start.wav", FMOD_DEFAULT | FMOD_LOOP_NORMAL , 0, &g_IntroSound);
	FMOD_System_CreateSound( g_System, "bgsound.wav", FMOD_DEFAULT | FMOD_LOOP_NORMAL, 0, &g_BGSound);
	FMOD_System_CreateSound( g_System, "fail.wav", FMOD_DEFAULT | FMOD_LOOP_NORMAL, 0, &g_FailSound); // Àû±ºÀÌ ÀÌ°åÀ» ¶§	

	// È¿°ú »ç¿îµå
	FMOD_System_CreateSound( g_System, "ak.wav", FMOD_DEFAULT, 0, &g_EffectSound[0]); // Àû±ºÀÌ Á×´Â ¼Ò¸®
	FMOD_System_CreateSound( g_System, "ubs.wav", FMOD_DEFAULT, 0, &g_EffectSound[1]); // ¾Æ±ºÀÌ Á×´Â ¼Ò¸®
	FMOD_System_CreateSound( g_System, "gun1.wav", FMOD_DEFAULT, 0, &g_EffectSound[2]); // ÁÖÀÎ°ø ÃÑ¼Ò¸®	
	FMOD_System_CreateSound( g_System, "success.wav", FMOD_DEFAULT , 0, &g_EffectSound[3]); // ÁÖÀÎ°øÀÌ ÀÌ°åÀ» ¶§	
}


void SoundRelease()
{
	FMOD_Sound_Release(g_IntroSound);
	FMOD_Sound_Release(g_BGSound);
	FMOD_Sound_Release(g_FailSound);

	for( int i = 0 ; i < 4 ; i++ )
		FMOD_Sound_Release(g_EffectSound[i]);	

	FMOD_System_Close( g_System );
	FMOD_System_Release( g_System );
}


void Init()
{	
	int i;

	// Àü¿ª º¯¼ö ÃÊ±âÈ­
	g_nFrameCount = 0;		
	g_nEnemyIndex = 0;  // Àû Ä³¸¯ÅÍÀÇ ÃâÇö ½ÃÀÛ ÀÎµ¦½º 
	g_nDeadEnemy = 0;
	g_nGoal = g_StageInfo[g_nStage - 1].nGoal;

	// Àû Ä³¸¯ÅÍ ÃÊ±âÈ­ 
	for( i = 0 ; i < ENEMY_COUNT ; i++ )
	{	
		g_sEnemy[i].nLife = 0;
		g_sEnemy[i].nY = 1;
		g_sEnemy[i].nX = g_nXPos[ rand() % 4 ]; // 4°³ÀÇ ÁÙ 	
		g_sEnemy[i].nYMoveDist = rand() % g_StageInfo[g_nStage-1].nYDist + 1;// ¶³¾îÁö´Â ¼Óµµ¸¦ Á¶ÀýÇÏ±â À§ÇÑ °ª, 1Àº ¼Óµµ°¡ 0ÀÌ µÇ¸é ¾ÊµÇ±â¿¡ 	 			
	}

	// ÁÖÀÎ°ø ÃÊ±âÈ­ 
	g_sPlay.nIndex = 2;
	g_sPlay.nLifePower = 3;
	g_sPlay.nX = g_nXPos[ g_sPlay.nIndex ]; // À§Ä¡¸¦ ÀÚÀ¯·ÎÀÌ º¯°æÀ» ÇÒ ¼ö ÀÖ´Â ±¸Á¶
	g_sPlay.nY = END_LINE;	
	g_GameState = RUNNING; // °ÔÀÓ ÁøÇà »óÅÂ·Î ÀüÈ¯

	// ÃÑ¾Ë ÃÊ±âÈ­
	for( i = 0 ; i < BULLET_COUNT ; i++ )
	{
		g_sBullet[i].nLife = 0;
	}	
}

void Update()
{	
	int i, j, nCount = 0;		

	if( g_GameState == RUNNING )
	{			
		//Note: ÁÖÀÎ°øÀº »ì¾Æ ÀÖ°í  Àû Ä³¸¯ÅÍ°¡ ¸ðµÎ Á×¾î ¸ñÇ¥¸¦ ÀÌ·ê¼ö ¾ø´Â °æ¿ì
		if( g_sPlay.nLifePower > 0 && g_nDeadEnemy == ENEMY_COUNT )
		{
			if( g_nGoal > 0 )
			{
				g_GameState = FAILED;
				return ;
			}else{
				g_GameState = SUCCESS;
				return ;
			}
		}		

		// Àû Ä³¸¯ÅÍ ÃâÇöÀ» °áÁ¤ÇÏ´Â ºÎºÐ nEnemyIndexÀÌÇÑÀÇ ÀÎµ¦½º´Â ´õ ÀÌ»ó »ì¸®Áö ¾Ê´Â´Ù.
		for( i = g_nEnemyIndex; i < ENEMY_COUNT ; i++ )
		{
			if( g_sEnemy[i].nLife == 0 )
			{
				if( g_sEnemy[i].nAppearFrame == g_nFrameCount )
				{
					g_sEnemy[i].nLife = 1; // Ãâ¹ß 
					g_nEnemyIndex++;
				}else{											
					break;
				}
			}	
		}

		// Àû Ä³¸¯ÅÍ ÀÌµ¿  
		for( i = 0 ; i < g_nEnemyIndex ; i++ )
		{
			if( g_sEnemy[i].nLife == 1 )
			{
				
				if( ( g_nFrameCount - g_sEnemy[i].nOldFrame ) > g_sEnemy[i].nYMoveDist )
				{
					g_sEnemy[i].nY++;	// ¾Æ·¡·Î ³»·Á°¡´Â °ÍÀÌ±â¿¡ 1¾¿ Áõ°¡ ÇÑ´Ù.
					g_sEnemy[i].nOldFrame = g_nFrameCount;		
				}
			}			
		}

		// ÃÑ¾Ë ÀÌµ¿
		for( i = 0 ; i < BULLET_COUNT ; i++ )
		{
			if( g_sBullet[i].nLife == 1 ) 
			{				
				if( ( g_nFrameCount - g_sBullet[i].nOldFrame ) > g_nBulletVeloc )
				{
					g_sBullet[i].nY--;
					g_sBullet[i].nOldFrame = g_nFrameCount;

					if( g_sBullet[i].nY == 0 )
						g_sBullet[i].nLife = 0;
				}			
			}
		}	

/*-------------------------------------- Ãæµ¹ Ã¼Å© ºÎºÐ -------------------------------------------------------*/

		// Note: Àû Ä³¸¯ÅÍ¿Í ÃÑ¾ËÀÇ Ãæµ¹ Ã¼Å© 
		for( i = 0 ; i < g_nEnemyIndex ; i++ )
		{
			if( g_sEnemy[i].nLife == 1 )
			{
				for( j = 0 ; j < BULLET_COUNT ; j++ )
				{
					if( g_sBullet[j].nLife == 1 )
					{
						if( (g_sBullet[j].nX == g_sEnemy[i].nX) && g_sEnemy[i].nY >= g_sBullet[j].nY )
						{							
							// Ãæµ¹ »ç¿îµå
							FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_EffectSound[0], 0, &g_pTempChannel);
							g_sBullet[j].nLife = 0;
							g_sEnemy[i].nLife = 0;
							g_nDeadEnemy++;       // Note: Àû Ä³¸¯ÅÍ Á×Àº °³¼ö
							g_nGoal--;            // Note: ¸ñÇ¥ °³¼ö
							g_nGrade += 10;		  // Note: µæÁ¡
							
							if( g_sPlay.nLifePower > 0 && g_nGoal == 0 )
							{
								g_GameState = SUCCESS;
								return ;
							}	

							break;
						}	
					}
				}
			}
		}

		// ÁÖÀÎ°ø°ú Àû°úÀÇ Ãæµ¹ Ã¼Å© 
		for( i = 0 ; i < g_nEnemyIndex ; i++ )
		{	
			if( g_sEnemy[i].nLife == 1 )
			{
				// ÁÂÇ¥°¡ °°À¸¸é  
				if( g_sPlay.nX == g_sEnemy[i].nX && g_sPlay.nY == g_sEnemy[i].nY )
				{						
					g_sEnemy[i].nLife = 0;
					g_sPlay.nLifePower--;
					g_nGoal--;		
					g_nDeadEnemy++;
					FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_EffectSound[0], 0, &g_pTempChannel);
					FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_EffectSound[1], 0, &g_pTempChannel);					

					if( g_sPlay.nLifePower == 0 && g_nGoal > 0 )
					{
						g_GameState = FAILED;
						return ;
					}
					
					if( g_sPlay.nLifePower > 0 && g_nGoal == 0 )
					{
						g_GameState = SUCCESS;
						return ;
					}	

					break; 
				}
			}	
		}  

		// ÃÑ¾ËÀÌ Dead Line°ú Ãæµ¹ÇÏ´Â °æ¿ì¸¦ Ã¼Å© ÇÑ´Ù. ÃÑ¾ËÀÇ Dead LineÀº 0 ÀÌ´Ù. 
		for( i = 0 ; i < BULLET_COUNT ; i++ )
		{
			if( g_sBullet[i].nLife == 1 && g_sBullet[i].nY == 0 ) 
			{
				g_sBullet[i].nLife = 0;
			}
		}	

		// ÀûÄ³¸¯ÅÍ°¡ Dead Line¿¡ Ãæµ¹ÇÏ´Â °æ¿ì, Dead LineÀº END_LINE 21ÀÌ´Ù
		for( i = 0 ; i < g_nEnemyIndex ; i++ )
		{
			if( g_sEnemy[i].nLife == 1 && g_sEnemy[i].nY == END_LINE )
			{
				g_sEnemy[i].nLife = 0;
				g_sPlay.nLifePower--;
				g_nDeadEnemy++;
				FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_EffectSound[1], 0, &g_pTempChannel);
				
				if( g_sPlay.nLifePower == 0 && g_nGoal > 0 )
				{
					g_GameState = FAILED;
					return ;
				}
					
				if( g_sPlay.nLifePower > 0 && g_nGoal == 0 )
				{
					g_GameState = SUCCESS;
					return ;
				}	
			}
		}
	}
}

void Render()
{
	int i, nYN;	

	system("cls");

	switch( g_GameState )
	{
	case INIT :			
		IntroScreen();
		FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_IntroSound, 0, &g_IntroChannel); 
		_getch();		
		FMOD_Channel_Stop( g_IntroChannel );
		FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_BGSound, 0, &g_BGChannel); 
		Init();		
		break;

	case RUNNING :	
		// ¹è°æ È­¸é ±×¸®±â 
		BackScreen();
		// Àû Ä³¸¯ÅÍ ±×¸®±â 
		for( i = 0 ; i < ENEMY_COUNT ; i++ )
		{
			if( g_sEnemy[i].nLife )
			{
				gotoxy( g_sEnemy[i].nX, g_sEnemy[i].nY );
				printf( "¡à" );
			}						
		}

		// ÃÑ¾Ë ±×¸®±â 
		for( i = 0 ; i < BULLET_COUNT ; i++ )
		{
			if( g_sBullet[i].nLife )
			{
				gotoxy( g_sBullet[i].nX, g_sBullet[i].nY );
				printf( "¡ã" );
			}
		}

		// ÁÖÀÎ°ø ±×¸®±â 
		gotoxy( g_sPlay.nX, g_sPlay.nY );
		printf("¢¼" );

		// Á¡¼ö¿Í Àû Ä³¸¯ÅÍ¼ö, »ý¸í·Â Ãâ·Â 
		gotoxy( 50, 5 );
		printf( "%d", g_nGrade );
		gotoxy( 56, 7 );
		printf( "%d", g_nGoal );
		gotoxy( 54, 11 );
		printf( "%d", g_nStage );

		gotoxy( 51, 9 );
		for( i = 0 ; i < g_sPlay.nLifePower ; i++ )
		{
			printf( "¢¾");
		}
		break;

	case SUCCESS :
		// ¾Æ±ºÀÇ ½Â¸® 
		FMOD_Channel_Stop( g_BGChannel );
		FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_EffectSound[3], 0, &g_pTempChannel);

		VictroyScreen();
		gotoxy( 18, 8 ); 
		printf( "%d", g_nGrade );
		gotoxy( 19, 18 ); 
		fflush( stdin );
		printf( "°è¼Ó ÇÏ½Ã°Ú½À´Ï±î? <y/n>  " );

		while( 1 )
		{
			nYN = _getch(); 
			fflush( stdin );
			if( nYN == 'y' || nYN == 'Y' )
			{
				FMOD_Channel_Stop( g_pTempChannel );
				g_GameState = INIT;
				g_nStage++;				// ¾Æ±ºÀÌ ½Â¸®ÇÒ ¶§¸¸  ½ºÅ×ÀÌÁö¸¦ ¿Ã¸°´Ù.
				Init();
				FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_BGSound, 0, &g_BGChannel);
				break;
			}

			if( nYN == 'n' || nYN == 'N')
			{
				g_GameState = RESULT;
				break;
			}
		}						
		break;	

	case FAILED : // Àû±ºÀÇ ½Â¸® 
		FMOD_Channel_Stop( g_BGChannel );
		FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_FailSound, 0, &g_FailChannel);

		FailScreen();		
		gotoxy( 19, 18 ); 
		fflush( stdin );
		printf( "°è¼Ó ÇÏ½Ã°Ú½À´Ï±î? <y/n>  " );

		while( 1 )
		{
			nYN = _getch(); 
			fflush( stdin );
			if( nYN == 'y' || nYN == 'Y' )
			{
				FMOD_Channel_Stop( g_FailChannel );
				g_GameState = INIT;	
				Init();
				FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_BGSound, 0, &g_BGChannel);
				break;
			}

			if( nYN == 'n' || nYN == 'N')
			{
				FMOD_Channel_Stop( g_FailChannel );
				g_GameState = RESULT;
				break;
			}
		}				
		break;

	case RESULT : 
		// °ÔÀÓ Á¾·á  
		GameoverScreen();
		gotoxy( 31, 8 );
		printf( "%d", g_nGrade );
		gotoxy( 0, 23 );		
		fflush( stdin );
		_getch();
		g_GameState = END;		
		break;
	}
}

void Release()
{
	// Note: ÇÒ´çÇÑ ¸Þ¸ð¸®°¡ ÀÖÀ¸¸é ÇØÁ¦ 	
	SoundRelease();
}

int _tmain(int argc, _TCHAR* argv[])
{
	int nKey, i;	
	clock_t sOldTime, sCurTime;

	sOldTime = clock();
	
	SoundInit();	

	while( 1 )
	{
		if( g_GameState == END )
			break;

		if( _kbhit() )
		{
			nKey = _getch();

			switch( nKey )
			{
			case 'j' :
				g_sPlay.nIndex--;
				if( g_sPlay.nIndex < 0 )
					g_sPlay.nIndex = 0;												

				g_sPlay.nX = g_nXPos[ g_sPlay.nIndex ];
				break;

			case 'k' :
				for( i = 0 ; i < BULLET_COUNT ; i++ )
				{
					if( g_sBullet[i].nLife == 0 )
					{
						g_sBullet[i].nLife = 1;
						g_sBullet[i].nOldFrame = g_nFrameCount;
						g_sBullet[i].nY = 20;
						g_sBullet[i].nX = g_sPlay.nX;
						FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_EffectSound[2], 0, &g_pTempChannel);
						break;
					}
				}
				break;

			case 'l' :	
				g_sPlay.nIndex++;
				if( g_sPlay.nIndex > 3 )			
					g_sPlay.nIndex = 3;							

				g_sPlay.nX = g_nXPos[ g_sPlay.nIndex ];
				break;
			}		
		}

		Update();
		Render();	
		FMOD_System_Update( g_System );

		while( 1 )
		{
			sCurTime = clock();

			if( sCurTime - sOldTime > 33 )
			{
				sOldTime = sCurTime;
				break;
			}
		}

		g_nFrameCount++;	
	}
	
	Release();

	return 0;
}