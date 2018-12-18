/* ////////////////////////////////////////////////////////////

File Name: sample05-key.cpp
Copyright (c) 2017 Soji Yamakawa.  All rights reserved.
http://www.ysflight.com

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS 
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE 
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

//////////////////////////////////////////////////////////// */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#ifdef _WIN32
	#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
	#endif
	#include <windows.h>
#endif

#ifndef __APPLE__
#include <GL/gl.h>
#include <GL/glu.h>
#else
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#endif

#include <fssimplewindow.h>
#include <ysglfontdata.h>


class GameObject
{
public:
	int x,y,vx,vy;
	int alive;

	GameObject();
	void Move(void);
	virtual void Draw(void);
	virtual int HitTest(int tx,int ty);
};

GameObject::GameObject()
{
	x=0;
	y=0;
	vx=0;
	vy=0;
	alive=1;
}

void GameObject::Move(void)
{
	x+=vx;
	y+=vy;
}

void GameObject::Draw(void)
{
}

int GameObject::HitTest(int tx,int ty)
{
	return 0;
}

class Enemy : public GameObject
{
public:
	int wid,hei;
	Enemy();
	void Generate(void);
	virtual void Draw(void);
	virtual int HitTest(int tx,int ty);
};

Enemy::Enemy()
{
	wid=40;
	hei=10;
	alive=0;
}

void Enemy::Generate(void)
{
	int wid,hei;
	FsGetWindowSize(wid,hei);

	alive=1;
	y=rand()%100+hei/10;
	x=rand()%wid;
	vx=4+rand()%10;
	vy=0;
	if(rand()%100<50)
	{
		vx=-vx;
	}
}

void Enemy::Draw(void)
{
	glColor3ub(0,255,255);
	glBegin(GL_QUADS);
	glVertex2i(x-wid,y-hei/2);
	glVertex2i(x+wid,y-hei/2);
	glVertex2i(x+wid,y+hei/2);
	glVertex2i(x-wid,y+hei/2);

	glVertex2i(x-wid*8/10,y-hei);
	glVertex2i(x+wid*8/10,y-hei);
	glVertex2i(x+wid*8/10,y+hei);
	glVertex2i(x-wid*8/10,y+hei);
	glEnd();
}

int Enemy::HitTest(int tx,int ty)
{
	if(x-wid<=tx && tx<=x+wid && y-hei<=ty && ty<=y+hei)
	{
		return 1;
	}
	return 0;
}

class Player : public GameObject
{
public:
	virtual void Draw(void);
	virtual int HitTest(int tx,int ty);
};

void Player::Draw(void)
{
	glColor3ub(0,255,0);
	glBegin(GL_QUADS);
	glVertex2i(x-20,y);
	glVertex2i(x+20,y);
	glVertex2i(x+20,y-10);
	glVertex2i(x-20,y-10);

	glVertex2i(x-10,y);
	glVertex2i(x+10,y);
	glVertex2i(x+10,y-20);
	glVertex2i(x-10,y-20);
	glEnd();
}

int Player::HitTest(int tx,int ty)
{
	if(x-20<=tx && tx<=x+20 && y-20<=ty && ty<=y)
	{
		return 1;
	}
	return 0;
}

class Missile : public GameObject
{
public:
	int alive;
	int shotByPlayer;
	virtual void Draw(void);
	virtual int HitTest(int tx,int ty);
};

void Missile::Draw(void)
{
	if(shotByPlayer!=0)
	{
		glColor3ub(255,255,0);
	}
	else
	{
		glColor3ub(255,0,0);
	}
	glBegin(GL_LINES);
	glVertex2i(x,y);
	glVertex2i(x,y+10);
	glEnd();
}

int Missile::HitTest(int tx,int ty)
{
	return 0;
}

void GenerateEnemy(int nEnemyAry,Enemy enemy[])
{
	int i;
	for(i=0; i<nEnemyAry; i++)
	{
		if(enemy[i].alive==0)
		{
			enemy[i].Generate();
			return;
		}
	}
}

int Game(void)
{
	int i,j,score,wid,hei,gameover;

	const int maxNumEnemy=5;
	Enemy enemy[maxNumEnemy];

	const int maxNumPlayerMissile=5;
	Missile playerMissile[maxNumPlayerMissile];

	const int maxNumEnemyMissile=10;
	Missile enemyMissile[maxNumEnemyMissile];


	FsPassedTime();

	for(i=0; i<maxNumEnemy; i++)
	{
		enemy[i].alive=0;
	}
	for(i=0; i<maxNumPlayerMissile; i++)
	{
		playerMissile[i].alive=0;
	}
	for(i=0; i<maxNumEnemyMissile; i++)
	{
		enemyMissile[i].alive=0;
	}

	GenerateEnemy(maxNumEnemy,enemy);


	FsGetWindowSize(wid,hei);

	Player player;
	player.x=wid/2;
	player.y=hei*9/10;

	score=0;
	gameover=0;
	while(gameover==0)
	{
		int lb,mb,rb,mx,my,key;
		int passedTime;

		FsPollDevice();
		FsGetMouseState(lb,mb,rb,mx,my);
		FsGetWindowSize(wid,hei);

		player.y=hei*9/10;

		while(key=FsInkey())
		{
			switch(key)
			{
			case FSKEY_SPACE:
				for(i=0; i<maxNumPlayerMissile; i++)
				{
					if(playerMissile[i].alive==0)
					{
						playerMissile[i].alive=1;
						playerMissile[i].shotByPlayer=1;
						playerMissile[i].x=player.x;
						playerMissile[i].y=player.y;
						playerMissile[i].vx=0;
						playerMissile[i].vy=-10;
						break;
					}
				}
				break;
			case FSKEY_ESC:
				return score;
			}
		}

		if(FsGetKeyState(FSKEY_LEFT)!=0)
		{
			player.vx=-4;
		}
		else if(FsGetKeyState(FSKEY_RIGHT)!=0)
		{
			player.vx=4;
		}
		else
		{
			player.vx=0;
		}
		player.Move();

		for(i=0; i<maxNumPlayerMissile; i++)
		{
			if(playerMissile[i].alive!=0)
			{
				playerMissile[i].Move();
				for(j=0; j<maxNumEnemy; j++)
				{
					if(enemy[j].alive!=0 && enemy[j].HitTest(playerMissile[i].x,playerMissile[i].y)!=0)
					{
						score++;
						enemy[j].alive=0;
						GenerateEnemy(maxNumEnemy,enemy);
						GenerateEnemy(maxNumEnemy,enemy);
						playerMissile[i].alive=0;
					}
				}

				if(playerMissile[i].x<0 || wid<playerMissile[i].x ||
				   playerMissile[i].y<0 || hei<playerMissile[i].y)
				{
					playerMissile[i].alive=0;
				}
			}
		}
		for(i=0; i<maxNumEnemy; i++)
		{
			if(enemy[i].alive!=0)
			{
				enemy[i].Move();
				if((enemy[i].x<0 && enemy[i].vx<0) || (wid<enemy[i].x && 0<enemy[i].vx))
				{
					enemy[i].vx*=-1;
				}
				if(rand()%20==0)
				{
					for(j=0; j<maxNumEnemyMissile; j++)
					{
						if(enemyMissile[j].alive==0)
						{
							enemyMissile[j].alive=1;
							enemyMissile[j].shotByPlayer=0;
							enemyMissile[j].x=enemy[i].x;
							enemyMissile[j].y=enemy[i].y;
							enemyMissile[j].vx=0;
							enemyMissile[j].vy=8;
							break;
						}
					}
				}
			}
		}
		for(i=0; i<maxNumEnemyMissile; i++)
		{
			if(enemyMissile[i].alive!=0)
			{
				enemyMissile[i].Move();
				if(player.HitTest(enemyMissile[i].x,enemyMissile[i].y)!=0)
				{
					gameover=1;
				}
				if(enemyMissile[i].x<0 || wid<enemyMissile[i].x ||
				   enemyMissile[i].y<0 || hei<enemyMissile[i].y)
				{
					enemyMissile[i].alive=0;
				}
			}
		}


		glViewport(0,0,wid,hei);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0,(float)wid-1,(float)hei-1,0,-1,1);

		glClearColor(0.0,0.0,0.0,0.0);
		glClear(GL_COLOR_BUFFER_BIT);

		player.Draw();

		for(i=0; i<maxNumPlayerMissile; i++)
		{
			if(playerMissile[i].alive!=0)
			{
				playerMissile[i].Draw();
			}
		}
		for(i=0; i<maxNumEnemyMissile; i++)
		{
			if(enemyMissile[i].alive!=0)
			{
				enemyMissile[i].Draw();
			}
		}
		for(i=0; i<maxNumEnemy; i++)
		{
			if(enemy[i].alive!=0)
			{
				enemy[i].Draw();
			}
		}
		char str[256];
		sprintf(str,"SCORE:%d",score);
		glRasterPos2i(32,32);
		glCallLists(strlen(str),GL_UNSIGNED_BYTE,str);

		FsSwapBuffers();

		passedTime=FsPassedTime(); // Making it up to 50fps
		FsSleep(20-passedTime);
	}
	return score;
}

int Menu(void)
{
	int r,key;
	r=0;
	while(r==0)
	{
		FsPollDevice();
		key=FsInkey();
		switch(key)
		{
		case FSKEY_S:
			r=1;
			break;
		case FSKEY_ESC:
			r=2;
			break;
		}

		int wid,hei;
		FsGetWindowSize(wid,hei);

		glViewport(0,0,wid,hei);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-0.5,(GLdouble)wid-0.5,(GLdouble)hei-0.5,-0.5,-1,1);

		glClearColor(0.0,0.0,0.0,0.0);
		glClear(GL_COLOR_BUFFER_BIT);

		const char *msg1="S.....Start Game";
		const char *msg2="ESC...Exit";
		glColor3ub(255,255,255);
		glRasterPos2i(32,32);
		glCallLists(strlen(msg1),GL_UNSIGNED_BYTE,msg1);

		glRasterPos2i(32,48);
		glCallLists(strlen(msg2),GL_UNSIGNED_BYTE,msg2);

		FsSwapBuffers();
		FsSleep(10);
	}
	return r;
}

void GameOver(int score)
{
	int r;
	r=0;

	FsPollDevice();
	while(FsInkey()!=0)
	{
		FsPollDevice();
	}

	while(FsInkey()==0)
	{
		FsPollDevice();

		int wid,hei;
		FsGetWindowSize(wid,hei);

		glViewport(0,0,wid,hei);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-0.5,(GLdouble)wid-0.5,(GLdouble)hei-0.5,-0.5,-1,1);

		glClearColor(0.0,0.0,0.0,0.0);
		glClear(GL_COLOR_BUFFER_BIT);

		const char *msg1="Game Over";
		char msg2[256];
		glColor3ub(255,255,255);
		glRasterPos2i(32,32);
		glCallLists(strlen(msg1),GL_UNSIGNED_BYTE,msg1);

		sprintf(msg2,"Your score is %d",score);

		glRasterPos2i(32,48);
		glCallLists(strlen(msg2),GL_UNSIGNED_BYTE,msg2);

		FsSwapBuffers();
		FsSleep(10);
	}
}

int main(void)
{
	int menu;
	FsOpenWindow(32,32,800,600,1); // 800x600 pixels, useDoubleBuffer=1

	srand(time(NULL));

	int listBase;
	listBase=glGenLists(256);
	YsGlUseFontBitmap8x12(listBase);
	glListBase(listBase);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDepthFunc(GL_ALWAYS);

	while(1)
	{
		menu=Menu();
		if(menu==1)
		{
			int score;
			score=Game();
			GameOver(score);
		}
		else if(menu==2)
		{
			break;
		}
	}

	return 0;
}


