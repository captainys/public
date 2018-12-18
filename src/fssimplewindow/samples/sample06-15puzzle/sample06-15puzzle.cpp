/* ////////////////////////////////////////////////////////////

File Name: sample06-15puzzle.cpp
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

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <fssimplewindow.h>
#include <ysglfontdata.h>



class Piece
{
public:
	int number;
	void Draw(int x,int y,int w,int h) const ;
};

void Piece::Draw(int x,int y,int w,int h) const
{
	char str[16];
	sprintf(str,"%2d",number);

	glColor3ub(100,100,100);
	glBegin(GL_QUADS);
	glVertex2i(x  ,y);
	glVertex2i(x+w,y);
	glVertex2i(x+w,y+h);
	glVertex2i(x  ,y+h);
	glEnd();

	glColor3ub(0,0,0);
	glBegin(GL_LINE_LOOP);
	glVertex2i(x  ,y);
	glVertex2i(x+w,y);
	glVertex2i(x+w,y+h);
	glVertex2i(x  ,y+h);
	glEnd();

	glRasterPos2d(x+w/2-16,y+h/2+12);

	YsGlDrawFontBitmap16x24(str);
}




class Puzzle
{
public:
	const int nx,ny;
	Piece *piece;

	Puzzle();
	~Puzzle();

	void Initialize(void);
	void Swap(int i,int j);
	void Shuffle(void);
	void Draw(void) const;
	void Move(int dx,int dy);
	bool Completed(void) const;
};

Puzzle::Puzzle() : nx(4),ny(4)
{
	piece=new Piece [nx*ny];
	Initialize();
}

Puzzle::~Puzzle()
{
	delete [] piece;
}

void Puzzle::Initialize(void)
{
	for(int i=0; i<nx*ny; ++i)
	{
		piece[i].number=i;
	}
}

void Puzzle::Draw(void) const
{
	for(int x=0; x<nx; ++x)
	{
		for(int y=0; y<ny; ++y)
		{
			const int i=y*nx+x;
			if(piece[i].number!=nx*ny-1)
			{
				piece[i].Draw(x*100,y*100,100,100);
			}
		}
	}
}

void Puzzle::Swap(int i,int j)
{
	Piece a=piece[i];
	piece[i]=piece[j];
	piece[j]=a;
}

void Puzzle::Shuffle(void)
{
	for(int i=0; i<nx*ny; ++i)
	{
		const int r=rand()%(nx*ny);
		Swap(i,r);
	}
}

void Puzzle::Move(int dx,int dy)
{
	int x0=-1,y0=-1;
	for(int i=0; i<nx*ny; ++i)
	{
		if(piece[i].number==nx*ny-1)
		{
			x0=i%nx;
			y0=i/nx;
		}
	}

	if(0<=x0 && x0<nx && 0<=y0 && y0<ny)
	{
		int x1=x0+dx;
		int y1=y0+dy;

		if(0<=x1 && x1<nx && 0<=y1 && y1<ny)
		{
			Swap(y0*nx+x0,y1*nx+x1);
		}
	}
}

bool Puzzle::Completed(void) const
{
	for(int i=0; i<nx*ny-3; ++i)
	{
		if(piece[i].number!=i)
		{
			return false;
		}
	}
	return true;
}



int main(void)
{
	FsOpenWindow(16,16,800,600,1);

	Puzzle puzzle;
	puzzle.Shuffle();

	srand((int)time(nullptr));

	int state=0;  // 0: Playing  1: Won!
	time_t endTimer=0;

	for(;;)
	{
		FsPollDevice();
		auto key=FsInkey();
		
		if(FSKEY_ESC==key)
		{
			break;
		}

		switch(key)
		{
		case FSKEY_UP:
			puzzle.Move(0,-1);
			break;
		case FSKEY_DOWN:
			puzzle.Move(0,1);
			break;
		case FSKEY_LEFT:
			puzzle.Move(-1,0);
			break;
		case FSKEY_RIGHT:
			puzzle.Move(1,0);
			break;
		}

		if(0==state && true==puzzle.Completed())
		{
			state=1;
			endTimer=time(nullptr)+3;
		}
		if(1==state && endTimer<=time(nullptr))
		{
			break;
		}

		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		puzzle.Draw();

		if(1==state)
		{
			glColor3ub(255,0,0);
			glRasterPos2d(100,100);
			YsGlDrawFontBitmap16x24("You Win!");
		}

		FsSwapBuffers();
		FsSleep(25);
	}

	return 0;
}
