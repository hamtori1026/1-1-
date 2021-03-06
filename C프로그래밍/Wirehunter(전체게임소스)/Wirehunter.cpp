// Wirehunter.cpp : 夔樂 擬辨 Щ煎斜極縑 渠и 霞殮薄擊 薑曖м棲棻.
//
// WireHunter01.cpp : 夔樂 擬辨 Щ煎斜極縑 渠и 霞殮薄擊 薑曖м棲棻.

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
	int		nLife;			// 儅貲
	int		nX, nY;			// 謝ル	
	int		nYMoveDist;		// 檜翕 剪葬 
	int		nOldFrame;	
	int		nAppearFrame;   // 轎⑷ Щ溯歜	
};

// 識憲曖 樓紫朝 瞪睡 偽紫煙 и棻.
struct BULLET
{
	int		nLife;
	int		nX, nY;
	int		nOldFrame;
};

PLAY	g_sPlay;

/* 1蟾縑 30fps陛 釭螃貊 1 蝶纔檜雖蒂 30蟾煎 薯寰擊 ц棻賊 30 * 30 戲煎 識 30蟾翕寰縑 900 fps陛 橾橫陪棻.
檜 900 fps朝 瞳 議葛攪陛 釭螃啪 腎朝 瞪羹 衛除檜 脹棻.*/

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

int g_nGrade = 0;		// 薄熱 
int g_nGoal = 0; 

struct STAGE_INFO
{
	int nGoal;  // 蝶纔檜雖滌 跡ル 瞳 議葛攪熱 
	int nYDist; // 蝶纔檜雖滌 檜翕 樓紫
};

STAGE_INFO g_StageInfo[6] = { { 10, 10 }, { 15, 10 }, { 15, 8 }, { 18, 8 }, { 15, 6 }, { 17, 6 } };
GAME_STATE g_GameState = INIT;  // 0 蟾晦 鼻鷓 1 獄 啪歜 2 啪歜 謙猿 
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
	printf( "旨收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收旬\n");
	printf( "早     早                                         早          早\n");
	printf( "早     ﹥                                         早          早\n");
	printf( "早     早                                         早          早\n");
	printf( "早     早             W i r e H u n t e r         早          早\n");
	printf( "早     早                                         早          早\n");
	printf( "早                                                ﹥          早\n");
	printf( "早             檜翕酈 : 謝(J), 辦(L), 識憲(K)     早          早\n");
	printf( "早                                                早          早\n");
	printf( "早                                                            早\n");
	printf( "早  #######                                                   早\n");
	printf( "早 #       ##                                                 早\n");
	printf( "早#          ###         ##                                   早\n");
	printf( "早              ###    ##  ######             ####            早\n");
	printf( "早                 ####          ####     ####     #          早\n");
	printf( "早                                   #####          #         早\n");
	printf( "早                                                    #       早\n");
	printf( "早                                                      #     早\n");
	printf( "早                                                        ### 早\n");
	printf( "早                                                           #早\n");
	printf( "早                                                            早\n");
	printf( "早                                                            早\n");
	printf( "曲收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收旭\n");
}

void BackScreen()
{
	// 寡唳 �飛� 斜葬晦 
	gotoxy( 0, 0 );
	printf( "旨收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收旬\n");
	printf( "早    早    早     早     早                                  早\n");
	printf( "早    早    早     早     早                                  早\n");
	printf( "早    早    早     早     早                                  早\n");
	printf( "早    早    早     早     早                                  早\n");
	printf( "早    早    早     早     早              薄熱 :              早\n");
	printf( "早    早    早     早     早                                  早\n");
	printf( "早    早    早     早     早              瞳 議葛攪熱 :       早\n");
	printf( "早    早    早     早     早                                  早\n");
	printf( "早    早    早     早     早              儅貲溘 :            早\n");
	printf( "早    早    早     早     早                                  早\n");
	printf( "早    早    早     早     早              蝶纔檜雖 :          早\n");
	printf( "早    早    早     早     早                                  早\n");
	printf( "早    早    早     早     早                                  早\n");
	printf( "早    早    早     早     早                                  早\n");
	printf( "早    早    早     早     早                                  早\n");
	printf( "早    早    早     早     早                                  早\n");
	printf( "早    早    早     早     早                                  早\n");
	printf( "早    早    早     早     早                                  早\n");
	printf( "早    早    早     早     早                                  早\n");
	printf( "早    早    早     早     早                                  早\n");
	printf( "早    早    早     早     早                                  早\n");
	printf( "曲收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收旭\n");
}

void VictroyScreen()
{
	gotoxy( 0, 0 );
	printf( "旨收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收旬\n");
	printf( "早 〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤 早\n");
	printf( "早 〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤 早\n");
	printf( "早 〤〤〤〤〤〤〤〤〤〤嬴捱檜 檜啣棻 !!! 〤〤〤〤〤〤〤〤〤〤 早\n");
	printf( "早 〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤 早\n");
	printf( "早 〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤 早\n");
	printf( "早 〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤 早\n");
	printf( "早                                                            早\n");
	printf( "早      Score :                                               早\n");
	printf( "早                                                            早\n");
	printf( "早                                 Ⅳ                         早\n");
	printf( "早                                ⅣⅣ                        早\n");
	printf( "早                               ⅣⅣⅣ                       早\n");
	printf( "早                             ⅣⅣⅣⅣⅣ                     早\n");
	printf( "早                            ⅣⅣⅣⅣⅣⅣ                    早\n");
	printf( "早                         ⅣⅣⅣⅣⅣⅣⅣⅣⅣ                 早\n");
	printf( "早 〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤 早\n");
	printf( "早 〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤 早\n");
	printf( "早 〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤 早\n");
	printf( "早 〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤 早\n");
	printf( "早 〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤 早\n");
	printf( "早 〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤 早\n");
	printf( "曲收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收旭\n");
}

void FailScreen()
{
	gotoxy( 0, 0 );
	printf( "旨收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收旬\n");
	printf( "早 〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤 早\n");
	printf( "早 〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤 早\n");
	printf( "早 〤〤〤〤〤〤〤〤〤〤瞳捱檜 檜啣棻 !!! 〤〤〤〤〤〤〤〤〤〤 早\n");
	printf( "早 〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤 早\n");
	printf( "早 〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤 早\n");
	printf( "早 〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤 早\n");
	printf( "早    早    早     早     早                                  早\n");
	printf( "早    ﹥    早     ﹥     ﹥                                  早\n");
	printf( "早    早    早     早     早                                  早\n");
	printf( "早    早    ﹥     早     早                                  早\n");
	printf( "早    早    早     早     ﹥                                  早\n");
	printf( "早    ﹥    早     ﹥     早                                  早\n");
	printf( "早    早    早     早     早                                  早\n");
	printf( "早    早    ﹥     早     早                                  早\n");
	printf( "早    早    早     早     早     ﹥    ﹥    ﹥   ﹥   ﹥     早\n");
	printf( "早 〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤 早\n");
	printf( "早 〤〤〤〤〤﹥〤〤〤〤〤〤〤﹥〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤 早\n");
	printf( "早 〤〤〤〤〤〤〤〤〤﹥〤〤〤〤〤〤〤﹥〤〤〤〤〤〤〤〤〤〤〤 早\n");
	printf( "早 〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤 早\n");
	printf( "早 〤〤〤〤〤〤〤﹥〤〤〤〤〤〤〤﹥〤〤〤〤〤﹥〤〤〤〤〤〤〤 早\n");
	printf( "早 〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤 早\n");
	printf( "曲收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收旭\n");	
}

void GameoverScreen()
{
	gotoxy( 0, 0 );
	printf( "旨收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收旬\n");
	printf( "早     早                                         早          早\n");
	printf( "早     ﹥                                         早          早\n");
	printf( "早     早   〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤    早          早\n");
	printf( "早     早   〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤    早          早\n");
	printf( "早     早   〤〤〤〤〤   Game Over  〤〤〤〤〤    早          早\n");
	printf( "早          〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤    ﹥          早\n");
	printf( "早          〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤    早          早\n");
	printf( "早          〤〤〤〤〤〤 Score      〤〤〤〤〤    早          早\n");
	printf( "早          〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤〤                早\n");
	printf( "早  #######                                                   早\n");
	printf( "早 #       ##                                                 早\n");
	printf( "早#          ###         ##                                   早\n");
	printf( "早              ###    ##  ######             ####            早\n");
	printf( "早                 ####          ####     ####     #          早\n");
	printf( "早                                   #####          #         早\n");
	printf( "早                                                    #       早\n");
	printf( "早                                                      #     早\n");
	printf( "早                                                        ### 早\n");
	printf( "早                                                           #早\n");
	printf( "早                                                            早\n");
	printf( "早                                                            早\n");
	printf( "曲收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收旭\n");
}

void SoundInit()
{
	// 衛蝶蠱 螃粽薛お 儅撩婁 蟾晦��
	FMOD_System_Create(&g_System);
	FMOD_System_Init( g_System, 32, FMOD_INIT_NORMAL, NULL);

	// 奩犒 餌遴萄
	FMOD_System_CreateSound( g_System, "start.wav", FMOD_DEFAULT | FMOD_LOOP_NORMAL , 0, &g_IntroSound);
	FMOD_System_CreateSound( g_System, "bgsound.wav", FMOD_DEFAULT | FMOD_LOOP_NORMAL, 0, &g_BGSound);
	FMOD_System_CreateSound( g_System, "fail.wav", FMOD_DEFAULT | FMOD_LOOP_NORMAL, 0, &g_FailSound); // 瞳捱檜 檜啣擊 陽	

	// �膩� 餌遴萄
	FMOD_System_CreateSound( g_System, "ak.wav", FMOD_DEFAULT, 0, &g_EffectSound[0]); // 瞳捱檜 避朝 模葬
	FMOD_System_CreateSound( g_System, "ubs.wav", FMOD_DEFAULT, 0, &g_EffectSound[1]); // 嬴捱檜 避朝 模葬
	FMOD_System_CreateSound( g_System, "gun1.wav", FMOD_DEFAULT, 0, &g_EffectSound[2]); // 輿檣奢 識模葬	
	FMOD_System_CreateSound( g_System, "success.wav", FMOD_DEFAULT , 0, &g_EffectSound[3]); // 輿檣奢檜 檜啣擊 陽	
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

	// 瞪羲 滲熱 蟾晦��
	g_nFrameCount = 0;		
	g_nEnemyIndex = 0;  // 瞳 議葛攪曖 轎⑷ 衛濛 檣策蝶 
	g_nDeadEnemy = 0;
	g_nGoal = g_StageInfo[g_nStage - 1].nGoal;

	// 瞳 議葛攪 蟾晦�� 
	for( i = 0 ; i < ENEMY_COUNT ; i++ )
	{	
		g_sEnemy[i].nLife = 0;
		g_sEnemy[i].nY = 1;
		g_sEnemy[i].nX = g_nXPos[ rand() % 4 ]; // 4偃曖 還 	
		g_sEnemy[i].nYMoveDist = rand() % g_StageInfo[g_nStage-1].nYDist + 1;// 雲橫雖朝 樓紫蒂 褻瞰ж晦 嬪и 高, 1擎 樓紫陛 0檜 腎賊 彊腎晦縑 	 			
	}

	// 輿檣奢 蟾晦�� 
	g_sPlay.nIndex = 2;
	g_sPlay.nLifePower = 3;
	g_sPlay.nX = g_nXPos[ g_sPlay.nIndex ]; // 嬪纂蒂 濠嶸煎檜 滲唳擊 й 熱 氈朝 掘褻
	g_sPlay.nY = END_LINE;	
	g_GameState = RUNNING; // 啪歜 霞ч 鼻鷓煎 瞪��

	// 識憲 蟾晦��
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
		//Note: 輿檣奢擎 髦嬴 氈堅  瞳 議葛攪陛 賅舒 避橫 跡ル蒂 檜瑙熱 橈朝 唳辦
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

		// 瞳 議葛攪 轎⑷擊 唸薑ж朝 睡碟 nEnemyIndex檜и曖 檣策蝶朝 渦 檜鼻 髦葬雖 彊朝棻.
		for( i = g_nEnemyIndex; i < ENEMY_COUNT ; i++ )
		{
			if( g_sEnemy[i].nLife == 0 )
			{
				if( g_sEnemy[i].nAppearFrame == g_nFrameCount )
				{
					g_sEnemy[i].nLife = 1; // 轎嫦 
					g_nEnemyIndex++;
				}else{											
					break;
				}
			}	
		}

		// 瞳 議葛攪 檜翕  
		for( i = 0 ; i < g_nEnemyIndex ; i++ )
		{
			if( g_sEnemy[i].nLife == 1 )
			{
				
				if( ( g_nFrameCount - g_sEnemy[i].nOldFrame ) > g_sEnemy[i].nYMoveDist )
				{
					g_sEnemy[i].nY++;	// 嬴楚煎 頂溥陛朝 匙檜晦縑 1噶 隸陛 и棻.
					g_sEnemy[i].nOldFrame = g_nFrameCount;		
				}
			}			
		}

		// 識憲 檜翕
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

/*-------------------------------------- 醱給 羹觼 睡碟 -------------------------------------------------------*/

		// Note: 瞳 議葛攪諦 識憲曖 醱給 羹觼 
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
							// 醱給 餌遴萄
							FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_EffectSound[0], 0, &g_pTempChannel);
							g_sBullet[j].nLife = 0;
							g_sEnemy[i].nLife = 0;
							g_nDeadEnemy++;       // Note: 瞳 議葛攪 避擎 偃熱
							g_nGoal--;            // Note: 跡ル 偃熱
							g_nGrade += 10;		  // Note: 菜薄
							
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

		// 輿檣奢婁 瞳婁曖 醱給 羹觼 
		for( i = 0 ; i < g_nEnemyIndex ; i++ )
		{	
			if( g_sEnemy[i].nLife == 1 )
			{
				// 謝ル陛 偽戲賊  
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

		// 識憲檜 Dead Line婁 醱給ж朝 唳辦蒂 羹觼 и棻. 識憲曖 Dead Line擎 0 檜棻. 
		for( i = 0 ; i < BULLET_COUNT ; i++ )
		{
			if( g_sBullet[i].nLife == 1 && g_sBullet[i].nY == 0 ) 
			{
				g_sBullet[i].nLife = 0;
			}
		}	

		// 瞳議葛攪陛 Dead Line縑 醱給ж朝 唳辦, Dead Line擎 END_LINE 21檜棻
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
		// 寡唳 �飛� 斜葬晦 
		BackScreen();
		// 瞳 議葛攪 斜葬晦 
		for( i = 0 ; i < ENEMY_COUNT ; i++ )
		{
			if( g_sEnemy[i].nLife )
			{
				gotoxy( g_sEnemy[i].nX, g_sEnemy[i].nY );
				printf( "﹤" );
			}						
		}

		// 識憲 斜葬晦 
		for( i = 0 ; i < BULLET_COUNT ; i++ )
		{
			if( g_sBullet[i].nLife )
			{
				gotoxy( g_sBullet[i].nX, g_sBullet[i].nY );
				printf( "∼" );
			}
		}

		// 輿檣奢 斜葬晦 
		gotoxy( g_sPlay.nX, g_sPlay.nY );
		printf("Ⅳ" );

		// 薄熱諦 瞳 議葛攪熱, 儅貲溘 轎溘 
		gotoxy( 50, 5 );
		printf( "%d", g_nGrade );
		gotoxy( 56, 7 );
		printf( "%d", g_nGoal );
		gotoxy( 54, 11 );
		printf( "%d", g_nStage );

		gotoxy( 51, 9 );
		for( i = 0 ; i < g_sPlay.nLifePower ; i++ )
		{
			printf( "Ⅵ");
		}
		break;

	case SUCCESS :
		// 嬴捱曖 蝓葬 
		FMOD_Channel_Stop( g_BGChannel );
		FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_EffectSound[3], 0, &g_pTempChannel);

		VictroyScreen();
		gotoxy( 18, 8 ); 
		printf( "%d", g_nGrade );
		gotoxy( 19, 18 ); 
		fflush( stdin );
		printf( "啗樓 ж衛啊蝗棲梱? <y/n>  " );

		while( 1 )
		{
			nYN = _getch(); 
			fflush( stdin );
			if( nYN == 'y' || nYN == 'Y' )
			{
				FMOD_Channel_Stop( g_pTempChannel );
				g_GameState = INIT;
				g_nStage++;				// 嬴捱檜 蝓葬й 陽虜  蝶纔檜雖蒂 螢萼棻.
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

	case FAILED : // 瞳捱曖 蝓葬 
		FMOD_Channel_Stop( g_BGChannel );
		FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_FailSound, 0, &g_FailChannel);

		FailScreen();		
		gotoxy( 19, 18 ); 
		fflush( stdin );
		printf( "啗樓 ж衛啊蝗棲梱? <y/n>  " );

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
		// 啪歜 謙猿  
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
	// Note: й渡и 詭賅葬陛 氈戲賊 п薯 	
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