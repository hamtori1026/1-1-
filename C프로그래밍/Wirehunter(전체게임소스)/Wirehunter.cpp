// Wirehunter.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//
// WireHunter01.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.

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
	int		nLife;			// ����
	int		nX, nY;			// ��ǥ	
	int		nYMoveDist;		// �̵� �Ÿ� 
	int		nOldFrame;	
	int		nAppearFrame;   // ���� ������	
};

// �Ѿ��� �ӵ��� ���� ������ �Ѵ�.
struct BULLET
{
	int		nLife;
	int		nX, nY;
	int		nOldFrame;
};

PLAY	g_sPlay;

/* 1�ʿ� 30fps�� ������ 1 ���������� 30�ʷ� ������ �ߴٸ� 30 * 30 ���� �� 30�ʵ��ȿ� 900 fps�� �Ͼ��.
�� 900 fps�� �� ĳ���Ͱ� ������ �Ǵ� ��ü �ð��� �ȴ�.*/

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

int g_nGrade = 0;		// ���� 
int g_nGoal = 0; 

struct STAGE_INFO
{
	int nGoal;  // ���������� ��ǥ �� ĳ���ͼ� 
	int nYDist; // ���������� �̵� �ӵ�
};

STAGE_INFO g_StageInfo[6] = { { 10, 10 }, { 15, 10 }, { 15, 8 }, { 18, 8 }, { 15, 6 }, { 17, 6 } };
GAME_STATE g_GameState = INIT;  // 0 �ʱ� ���� 1 �� ���� 2 ���� ���� 
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
	printf( "����������������������������������������������������������������\n");
	printf( "��     ��                                         ��          ��\n");
	printf( "��     ��                                         ��          ��\n");
	printf( "��     ��                                         ��          ��\n");
	printf( "��     ��             W i r e H u n t e r         ��          ��\n");
	printf( "��     ��                                         ��          ��\n");
	printf( "��                                                ��          ��\n");
	printf( "��             �̵�Ű : ��(J), ��(L), �Ѿ�(K)     ��          ��\n");
	printf( "��                                                ��          ��\n");
	printf( "��                                                            ��\n");
	printf( "��  #######                                                   ��\n");
	printf( "�� #       ##                                                 ��\n");
	printf( "��#          ###         ##                                   ��\n");
	printf( "��              ###    ##  ######             ####            ��\n");
	printf( "��                 ####          ####     ####     #          ��\n");
	printf( "��                                   #####          #         ��\n");
	printf( "��                                                    #       ��\n");
	printf( "��                                                      #     ��\n");
	printf( "��                                                        ### ��\n");
	printf( "��                                                           #��\n");
	printf( "��                                                            ��\n");
	printf( "��                                                            ��\n");
	printf( "����������������������������������������������������������������\n");
}

void BackScreen()
{
	// ��� ȭ�� �׸��� 
	gotoxy( 0, 0 );
	printf( "����������������������������������������������������������������\n");
	printf( "��    ��    ��     ��     ��                                  ��\n");
	printf( "��    ��    ��     ��     ��                                  ��\n");
	printf( "��    ��    ��     ��     ��                                  ��\n");
	printf( "��    ��    ��     ��     ��                                  ��\n");
	printf( "��    ��    ��     ��     ��              ���� :              ��\n");
	printf( "��    ��    ��     ��     ��                                  ��\n");
	printf( "��    ��    ��     ��     ��              �� ĳ���ͼ� :       ��\n");
	printf( "��    ��    ��     ��     ��                                  ��\n");
	printf( "��    ��    ��     ��     ��              ����� :            ��\n");
	printf( "��    ��    ��     ��     ��                                  ��\n");
	printf( "��    ��    ��     ��     ��              �������� :          ��\n");
	printf( "��    ��    ��     ��     ��                                  ��\n");
	printf( "��    ��    ��     ��     ��                                  ��\n");
	printf( "��    ��    ��     ��     ��                                  ��\n");
	printf( "��    ��    ��     ��     ��                                  ��\n");
	printf( "��    ��    ��     ��     ��                                  ��\n");
	printf( "��    ��    ��     ��     ��                                  ��\n");
	printf( "��    ��    ��     ��     ��                                  ��\n");
	printf( "��    ��    ��     ��     ��                                  ��\n");
	printf( "��    ��    ��     ��     ��                                  ��\n");
	printf( "��    ��    ��     ��     ��                                  ��\n");
	printf( "����������������������������������������������������������������\n");
}

void VictroyScreen()
{
	gotoxy( 0, 0 );
	printf( "����������������������������������������������������������������\n");
	printf( "�� �ƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢ� ��\n");
	printf( "�� �ƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢ� ��\n");
	printf( "�� �ƢƢƢƢƢƢƢƢƢƾƱ��� �̰�� !!! �ƢƢƢƢƢƢƢƢƢ� ��\n");
	printf( "�� �ƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢ� ��\n");
	printf( "�� �ƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢ� ��\n");
	printf( "�� �ƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢ� ��\n");
	printf( "��                                                            ��\n");
	printf( "��      Score :                                               ��\n");
	printf( "��                                                            ��\n");
	printf( "��                                 ��                         ��\n");
	printf( "��                                ����                        ��\n");
	printf( "��                               ������                       ��\n");
	printf( "��                             ����������                     ��\n");
	printf( "��                            ������������                    ��\n");
	printf( "��                         ������������������                 ��\n");
	printf( "�� �ƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢ� ��\n");
	printf( "�� �ƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢ� ��\n");
	printf( "�� �ƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢ� ��\n");
	printf( "�� �ƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢ� ��\n");
	printf( "�� �ƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢ� ��\n");
	printf( "�� �ƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢ� ��\n");
	printf( "����������������������������������������������������������������\n");
}

void FailScreen()
{
	gotoxy( 0, 0 );
	printf( "����������������������������������������������������������������\n");
	printf( "�� �ƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢ� ��\n");
	printf( "�� �ƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢ� ��\n");
	printf( "�� �ƢƢƢƢƢƢƢƢƢ������� �̰�� !!! �ƢƢƢƢƢƢƢƢƢ� ��\n");
	printf( "�� �ƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢ� ��\n");
	printf( "�� �ƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢ� ��\n");
	printf( "�� �ƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢ� ��\n");
	printf( "��    ��    ��     ��     ��                                  ��\n");
	printf( "��    ��    ��     ��     ��                                  ��\n");
	printf( "��    ��    ��     ��     ��                                  ��\n");
	printf( "��    ��    ��     ��     ��                                  ��\n");
	printf( "��    ��    ��     ��     ��                                  ��\n");
	printf( "��    ��    ��     ��     ��                                  ��\n");
	printf( "��    ��    ��     ��     ��                                  ��\n");
	printf( "��    ��    ��     ��     ��                                  ��\n");
	printf( "��    ��    ��     ��     ��     ��    ��    ��   ��   ��     ��\n");
	printf( "�� �ƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢ� ��\n");
	printf( "�� �ƢƢƢƢơ�ƢƢƢƢƢƢơ�ƢƢƢƢƢƢƢƢƢƢƢƢƢƢ� ��\n");
	printf( "�� �ƢƢƢƢƢƢƢƢơ�ƢƢƢƢƢƢơ�ƢƢƢƢƢƢƢƢƢƢ� ��\n");
	printf( "�� �ƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢ� ��\n");
	printf( "�� �ƢƢƢƢƢƢơ�ƢƢƢƢƢƢơ�ƢƢƢƢơ�ƢƢƢƢƢƢ� ��\n");
	printf( "�� �ƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢ� ��\n");
	printf( "����������������������������������������������������������������\n");	
}

void GameoverScreen()
{
	gotoxy( 0, 0 );
	printf( "����������������������������������������������������������������\n");
	printf( "��     ��                                         ��          ��\n");
	printf( "��     ��                                         ��          ��\n");
	printf( "��     ��   �ƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢ�    ��          ��\n");
	printf( "��     ��   �ƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢ�    ��          ��\n");
	printf( "��     ��   �ƢƢƢƢ�   Game Over  �ƢƢƢƢ�    ��          ��\n");
	printf( "��          �ƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢ�    ��          ��\n");
	printf( "��          �ƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢ�    ��          ��\n");
	printf( "��          �ƢƢƢƢƢ� Score      �ƢƢƢƢ�    ��          ��\n");
	printf( "��          �ƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢ�                ��\n");
	printf( "��  #######                                                   ��\n");
	printf( "�� #       ##                                                 ��\n");
	printf( "��#          ###         ##                                   ��\n");
	printf( "��              ###    ##  ######             ####            ��\n");
	printf( "��                 ####          ####     ####     #          ��\n");
	printf( "��                                   #####          #         ��\n");
	printf( "��                                                    #       ��\n");
	printf( "��                                                      #     ��\n");
	printf( "��                                                        ### ��\n");
	printf( "��                                                           #��\n");
	printf( "��                                                            ��\n");
	printf( "��                                                            ��\n");
	printf( "����������������������������������������������������������������\n");
}

void SoundInit()
{
	// �ý��� ������Ʈ ������ �ʱ�ȭ
	FMOD_System_Create(&g_System);
	FMOD_System_Init( g_System, 32, FMOD_INIT_NORMAL, NULL);

	// �ݺ� ����
	FMOD_System_CreateSound( g_System, "start.wav", FMOD_DEFAULT | FMOD_LOOP_NORMAL , 0, &g_IntroSound);
	FMOD_System_CreateSound( g_System, "bgsound.wav", FMOD_DEFAULT | FMOD_LOOP_NORMAL, 0, &g_BGSound);
	FMOD_System_CreateSound( g_System, "fail.wav", FMOD_DEFAULT | FMOD_LOOP_NORMAL, 0, &g_FailSound); // ������ �̰��� ��	

	// ȿ�� ����
	FMOD_System_CreateSound( g_System, "ak.wav", FMOD_DEFAULT, 0, &g_EffectSound[0]); // ������ �״� �Ҹ�
	FMOD_System_CreateSound( g_System, "ubs.wav", FMOD_DEFAULT, 0, &g_EffectSound[1]); // �Ʊ��� �״� �Ҹ�
	FMOD_System_CreateSound( g_System, "gun1.wav", FMOD_DEFAULT, 0, &g_EffectSound[2]); // ���ΰ� �ѼҸ�	
	FMOD_System_CreateSound( g_System, "success.wav", FMOD_DEFAULT , 0, &g_EffectSound[3]); // ���ΰ��� �̰��� ��	
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

	// ���� ���� �ʱ�ȭ
	g_nFrameCount = 0;		
	g_nEnemyIndex = 0;  // �� ĳ������ ���� ���� �ε��� 
	g_nDeadEnemy = 0;
	g_nGoal = g_StageInfo[g_nStage - 1].nGoal;

	// �� ĳ���� �ʱ�ȭ 
	for( i = 0 ; i < ENEMY_COUNT ; i++ )
	{	
		g_sEnemy[i].nLife = 0;
		g_sEnemy[i].nY = 1;
		g_sEnemy[i].nX = g_nXPos[ rand() % 4 ]; // 4���� �� 	
		g_sEnemy[i].nYMoveDist = rand() % g_StageInfo[g_nStage-1].nYDist + 1;// �������� �ӵ��� �����ϱ� ���� ��, 1�� �ӵ��� 0�� �Ǹ� �ʵǱ⿡ 	 			
	}

	// ���ΰ� �ʱ�ȭ 
	g_sPlay.nIndex = 2;
	g_sPlay.nLifePower = 3;
	g_sPlay.nX = g_nXPos[ g_sPlay.nIndex ]; // ��ġ�� �������� ������ �� �� �ִ� ����
	g_sPlay.nY = END_LINE;	
	g_GameState = RUNNING; // ���� ���� ���·� ��ȯ

	// �Ѿ� �ʱ�ȭ
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
		//Note: ���ΰ��� ��� �ְ�  �� ĳ���Ͱ� ��� �׾� ��ǥ�� �̷�� ���� ���
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

		// �� ĳ���� ������ �����ϴ� �κ� nEnemyIndex������ �ε����� �� �̻� �츮�� �ʴ´�.
		for( i = g_nEnemyIndex; i < ENEMY_COUNT ; i++ )
		{
			if( g_sEnemy[i].nLife == 0 )
			{
				if( g_sEnemy[i].nAppearFrame == g_nFrameCount )
				{
					g_sEnemy[i].nLife = 1; // ��� 
					g_nEnemyIndex++;
				}else{											
					break;
				}
			}	
		}

		// �� ĳ���� �̵�  
		for( i = 0 ; i < g_nEnemyIndex ; i++ )
		{
			if( g_sEnemy[i].nLife == 1 )
			{
				
				if( ( g_nFrameCount - g_sEnemy[i].nOldFrame ) > g_sEnemy[i].nYMoveDist )
				{
					g_sEnemy[i].nY++;	// �Ʒ��� �������� ���̱⿡ 1�� ���� �Ѵ�.
					g_sEnemy[i].nOldFrame = g_nFrameCount;		
				}
			}			
		}

		// �Ѿ� �̵�
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

/*-------------------------------------- �浹 üũ �κ� -------------------------------------------------------*/

		// Note: �� ĳ���Ϳ� �Ѿ��� �浹 üũ 
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
							// �浹 ����
							FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_EffectSound[0], 0, &g_pTempChannel);
							g_sBullet[j].nLife = 0;
							g_sEnemy[i].nLife = 0;
							g_nDeadEnemy++;       // Note: �� ĳ���� ���� ����
							g_nGoal--;            // Note: ��ǥ ����
							g_nGrade += 10;		  // Note: ����
							
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

		// ���ΰ��� ������ �浹 üũ 
		for( i = 0 ; i < g_nEnemyIndex ; i++ )
		{	
			if( g_sEnemy[i].nLife == 1 )
			{
				// ��ǥ�� ������  
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

		// �Ѿ��� Dead Line�� �浹�ϴ� ��츦 üũ �Ѵ�. �Ѿ��� Dead Line�� 0 �̴�. 
		for( i = 0 ; i < BULLET_COUNT ; i++ )
		{
			if( g_sBullet[i].nLife == 1 && g_sBullet[i].nY == 0 ) 
			{
				g_sBullet[i].nLife = 0;
			}
		}	

		// ��ĳ���Ͱ� Dead Line�� �浹�ϴ� ���, Dead Line�� END_LINE 21�̴�
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
		// ��� ȭ�� �׸��� 
		BackScreen();
		// �� ĳ���� �׸��� 
		for( i = 0 ; i < ENEMY_COUNT ; i++ )
		{
			if( g_sEnemy[i].nLife )
			{
				gotoxy( g_sEnemy[i].nX, g_sEnemy[i].nY );
				printf( "��" );
			}						
		}

		// �Ѿ� �׸��� 
		for( i = 0 ; i < BULLET_COUNT ; i++ )
		{
			if( g_sBullet[i].nLife )
			{
				gotoxy( g_sBullet[i].nX, g_sBullet[i].nY );
				printf( "��" );
			}
		}

		// ���ΰ� �׸��� 
		gotoxy( g_sPlay.nX, g_sPlay.nY );
		printf("��" );

		// ������ �� ĳ���ͼ�, ����� ��� 
		gotoxy( 50, 5 );
		printf( "%d", g_nGrade );
		gotoxy( 56, 7 );
		printf( "%d", g_nGoal );
		gotoxy( 54, 11 );
		printf( "%d", g_nStage );

		gotoxy( 51, 9 );
		for( i = 0 ; i < g_sPlay.nLifePower ; i++ )
		{
			printf( "��");
		}
		break;

	case SUCCESS :
		// �Ʊ��� �¸� 
		FMOD_Channel_Stop( g_BGChannel );
		FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_EffectSound[3], 0, &g_pTempChannel);

		VictroyScreen();
		gotoxy( 18, 8 ); 
		printf( "%d", g_nGrade );
		gotoxy( 19, 18 ); 
		fflush( stdin );
		printf( "��� �Ͻðڽ��ϱ�? <y/n>  " );

		while( 1 )
		{
			nYN = _getch(); 
			fflush( stdin );
			if( nYN == 'y' || nYN == 'Y' )
			{
				FMOD_Channel_Stop( g_pTempChannel );
				g_GameState = INIT;
				g_nStage++;				// �Ʊ��� �¸��� ����  ���������� �ø���.
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

	case FAILED : // ������ �¸� 
		FMOD_Channel_Stop( g_BGChannel );
		FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_FailSound, 0, &g_FailChannel);

		FailScreen();		
		gotoxy( 19, 18 ); 
		fflush( stdin );
		printf( "��� �Ͻðڽ��ϱ�? <y/n>  " );

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
		// ���� ����  
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
	// Note: �Ҵ��� �޸𸮰� ������ ���� 	
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