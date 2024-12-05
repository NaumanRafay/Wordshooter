//============================================================================
// Name        : cookie-crush.cpp
// Author      : Sibt ul Hussain
// Version     :
// Copyright   : (c) Reserved
// Description : Basic 2D game of Cookie  Crush...
//============================================================================
#ifndef WORD_SHOOTER_CPP
#define WORD_SHOOTER_CPP
#include <GL/gl.h>
#include <GL/glut.h>
#include <iostream>
#include<string>
#include<cmath>
#include<fstream>
#include "util.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
using namespace std;
#define MAX(A,B) ((A) > (B) ? (A):(B)) // defining single line functions....
#define MIN(A,B) ((A) < (B) ? (A):(B))
#define ABS(A) ((A) < (0) ? -(A):(A))
#define FPS 10

string * dictionary;
int dictionarysize = 369646;
#define KEY_ESC 27 // 
//#define KEY_ESC 32



// 20,30,30
const int bradius = 30; // ball radius in pixels...

int width = 930, height = 660;
int byoffset = bradius;

int nxcells = (width - bradius) / (2 * bradius);
int nycells = (height - byoffset /*- bradius*/) / (2 * bradius);
int nfrows = 2; // initially number of full rows //
float score = 0;
int **board; // 2D-arrays for holding the data...
int bwidth = 130;
int bheight = 10;
int bsx, bsy;
const int nalphabets = 26;
enum alphabets {
	AL_A, AL_B, AL_C, AL_D, AL_E, AL_F, AL_G, AL_H, AL_I, AL_J, AL_K, AL_L, AL_M, AL_N, AL_O, AL_P, AL_Q, AL_R, AL_S, AL_T, AL_U, AL_W, AL_X, AL_y, AL_Z
};
GLuint texture[nalphabets];
GLuint tid[nalphabets];
string tnames[] = { "a.bmp", "b.bmp", "c.bmp", "d.bmp", "e.bmp", "f.bmp", "g.bmp", "h.bmp", "i.bmp", "j.bmp",
"k.bmp", "l.bmp", "m.bmp", "n.bmp", "o.bmp", "p.bmp", "q.bmp", "r.bmp", "s.bmp", "t.bmp", "u.bmp", "v.bmp", "w.bmp",
"x.bmp", "y.bmp", "z.bmp" };
GLuint mtid[nalphabets];
int awidth = 60, aheight = 60; // 60x60 pixels bubbles...

int startdisplay=0;
int timee=1500;
int displaytime=150;
float dx=0;
float dy=0;
int alphabetGrid[5][15];
int ball=0;
float speedx=0;  
float speedy=0;
int locx=width/2;  
int locy=0;          
bool ballLaunched=false;  
bool gameover=false; 
int globally=0;
int arr[26]={19, 0, 18, 25, 10, 5, 15, 1, 4, 17, 12, 13, 2, 14, 3, 6, 7, 8, 9, 20, 11, 16, 21, 22, 23, 24};
void initializeaudio(){
    if (SDL_Init(SDL_INIT_AUDIO)<0){
        cerr<<"Failed to initialize SDL audio:"<<SDL_GetError()<<endl;
        exit(-1);
    }
    if(Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,2048)<0){
        cerr<<"Failed to initialize SDL_mixer "<<Mix_GetError()<<endl;
        exit(-1);
    }
}

void cleanupaudio(){
    Mix_CloseAudio();
    SDL_Quit();
}

bool wordisindictionary(const string dict[],int size,const string& word){
    for(int i = 0; i < size; ++i){
        if(dict[i] == word){
            return true; 
        }
    }
    return false; 
}

void checkgrid(const string dict[], int dictionarysize, int alphabetGrid[][2]) {
    const int minWordLength=2;
    const int maxWordLength=15;

    for(int row=0;row<2;row++){
      for(int col=0;col<15;col++){
            if (alphabetGrid[col][row]==-1)
                continue;
            if (col+minWordLength<=15){
                string wordLR = "";
                for(int k=col;k<15&&(k-col)<maxWordLength;k++){
                  if(alphabetGrid[k][row]==-1)
                        break;
                    wordLR+=('a'+alphabetGrid[k][row]);
                    int wordLen=0;
                    while (wordLR[wordLen]!='\0') {
                        wordLen++;
                    }
                    if (wordLen>=minWordLength) {
                        if (wordisindictionary(dict,dictionarysize,wordLR)){
                            for(int m=col;m<col+wordLen&&m<15;m++){
                              alphabetGrid[m][row]=-1;
                            }
                            cout<<"Word found and removed:"<<wordLR<<endl;
                        }
                    }
                }
            }
            if (row+minWordLength<=2){
                string wordTB="";
                for(int k=row;k<2&&(k-row)<maxWordLength;k++){
                  if(alphabetGrid[col][k]==-1)
                      break;
                    wordTB+=('a'+alphabetGrid[col][k]);
                    int wordLen=0;
                    while(wordTB[wordLen]!='\0') {
                        wordLen++;
                    }
                    if(wordLen>=minWordLength){
                        if(wordisindictionary(dict,dictionarysize, wordTB)) {
                            for(int m=row;m<row+wordLen&&m<2;m++) {
                                alphabetGrid[col][m]=-1;
                            }
                            cout << "Word found and removed: " << wordTB << endl;
                        }
                    }
                }
            }

            if(row-minWordLength+1>=0){
                string wordBT="";
                for (int k=row;k>=0&&(row-k)<maxWordLength;k--){
                    if(alphabetGrid[col][k]==-1)
                      break;
                    wordBT+=('a'+alphabetGrid[col][k]);
                    int wordLen=0;
                    while(wordBT[wordLen]!='\0'){
                        wordLen++;
                    }
                    if(wordLen>=minWordLength){
                        if(wordisindictionary(dict,dictionarysize,wordBT)){
                            for(int m=row;m>=row-wordLen+1&&m>=0;m--){
                              alphabetGrid[col][m]=-1;
                            }
                            cout<<"Word found and removed: "<<wordBT<<endl;
                        }
                    }
                }
            }
        }
    }
}
bool analyzewordsingrid(int grid[2][15], const string dictionary[], int dictionarySize) {
    string foundword[100];
    int wordindex=0;
    int wordsfound=0;
    for (int w=0;w<dictionarySize;w++){
        string word=dictionary[w];
        int wordLen=0;
        while(word[wordLen]!='\0'){
            wordLen++;
        }
        if(wordLen<3||wordLen>15){
            continue;
        }
        for(int col=0;col<5;col++){
          for(int row=0;row<15;row++){
              for (int dcol = -1; dcol <= 1; dcol++) {
                  for (int drow = -1; drow <= 1; drow++) {
                      if (dcol == 0 && drow == 0) {
                            continue;
                        }             
                        int end_col=col+(wordLen-1)*dcol;
                        int end_row=row+(wordLen-1)*drow;
                        if(end_col<0||end_col>=5||end_row<0||end_row>=15){
                          continue; 
                        }
                        bool match=true; 
                        for (int k=0;k<wordLen;k++) {
                            int current_col =col+k*dcol;
                            int current_row =row+k*drow;

                            int grid_val = grid[current_col][current_row];
                            char grid_char ='a'+grid_val; 

                            if (grid_char != word[k]) {
                                match = false;
                                break; 
                            }
                        }

                        if (match){
                            if (wordindex < 100) {
                                foundword[wordindex++] = word;
                            }
                            cout<<word<<endl;
                            for (int m = 0; m < wordLen; ++m) {
                                int current_col=col+m*dcol;
                                int current_row=row+m*drow;
                                grid[current_col][current_row]=rand()%26; 
                            }
                            wordsfound++;
                            if(wordsfound==4){
                            return true;
                           }
                        } 
                    }
                }
            }
        }
    }
    return false;
}

void RegisterTextures_Write()
{
	glGenTextures(nalphabets, tid);
	vector<unsigned char> data;
	ofstream ofile("image-data.bin", ios::binary | ios::out);

	for (int i = 0; i < nalphabets; i++) {

		ReadImage(tnames[i], data);
		if (i == 0) {
			int length = data.size();
			ofile.write((char*)&length, sizeof(int));
		}
		ofile.write((char*)&data[0], sizeof(char) * data.size());

		mtid[i] = tid[i];
		// select our current texture
		glBindTexture(GL_TEXTURE_2D, tid[i]);

		// select modulate to mix texture with color for shading
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		// when texture area is small, bilinear filter the closest MIP map
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_NEAREST);
		// when texture area is large, bilinear filter the first MIP map
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// if wrap is true, the texture wraps over at the edges (repeat)
		//       ... false, the texture ends at the edges (clamp)
		bool wrap = true;
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
			wrap ? GL_REPEAT : GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
			wrap ? GL_REPEAT : GL_CLAMP);

		// build our texture MIP maps
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, awidth, aheight, GL_RGB,
			GL_UNSIGNED_BYTE, &data[0]);
	}
	ofile.close();

}
void RegisterTextures()
/*Function is used to load the textures from the
* files and display*/
{
	// allocate a texture name
	glGenTextures(nalphabets, tid);

	vector<unsigned char> data;
	ifstream ifile("image-data.bin", ios::binary | ios::in);

	if (!ifile) {
		cout << " Couldn't Read the Image Data file ";
		//exit(-1);
	}
	// now load each bubbles data...
	int length;
	ifile.read((char*)&length, sizeof(int));
	data.resize(length, 0);
	for (int i = 0; i < nalphabets; ++i) {
		// Read current cookie
		//ReadImage(tnames[i], data);
		/*if (i == 0) {
		int length = data.size();
		ofile.write((char*) &length, sizeof(int));
		}*/
		ifile.read((char*)&data[0], sizeof(char)* length);

		mtid[i] = tid[i];
		// select our current texture
		glBindTexture(GL_TEXTURE_2D, tid[i]);

		// select modulate to mix texture with color for shading
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		// when texture area is small, bilinear filter the closest MIP map
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_NEAREST);
		// when texture area is large, bilinear filter the first MIP map
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// if wrap is true, the texture wraps over at the edges (repeat)
		//       ... false, the texture ends at the edges (clamp)
		bool wrap = true;
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
			wrap ? GL_REPEAT : GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
			wrap ? GL_REPEAT : GL_CLAMP);

		// build our texture MIP maps
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, awidth, aheight, GL_RGB,
			GL_UNSIGNED_BYTE, &data[0]);
	}
	ifile.close();
}
void wordsingrid(int grid[2][15], const string dictionary[], int dictionarySize) {
    string largestWord = ""; 
    int start_col = -1, start_row = -1; 
    int final_dcol = 0, final_drow = 0; 
    int wordsfound=0;

    for (int w = 0; w < dictionarySize; w++) {
        string word = dictionary[w];
        int wordLen = 0;
        while (word[wordLen] != '\0') {
            ++wordLen;
        }
        if (wordLen < 3 || wordLen > 15) {
            continue;
        }
        for (int col = 0; col < 5; col++) {
            for (int row = 0; row < 15; row++) {

                for (int d = 0; d < 8; d++) {
                    int dcol = 0, drow = 0;

                    if (d==0){
                    dcol=1;
                    drow=0; 
                    }   
                    else if(d==1){
                    dcol=-1;
                    drow=0; 
                    } 
                    else if(d==2){
                    dcol=0;
                    drow=1;
                    }   
                    else if(d==3){
                    dcol=0;
                    drow=-1;
                    }  
                    else if(d==4){
                    dcol=1;
                    drow=1;
                    }   
                    else if(d==5){
                    dcol=-1; 
                    drow=1;
                    }  
                    else if(d==6){
                    dcol=1;
                    drow=-1;
                    }  
                    else if(d==7){
                    dcol=-1;
                    drow=-1;
                    } 
                    int end_col=col+(wordLen-1)*dcol;
                    int end_row=row+(wordLen-1)*drow;
                    if (end_col<0||end_col>=5||end_row<0||end_row>=15){
                        continue;
                    }
                    bool match=true; 
                    for (int k=0;k<wordLen;k++){
                        int current_col=col+k*dcol;
                        int current_row=row+k*drow;
                        int grid_val=grid[current_col][current_row];
                        char grid_char='a'+grid_val; 
                        if (grid_char!=word[k]){
                            match=false;
                            break;
                        }
                    }
                    if (match){
                        int largestWordLen=0;
                        while (largestWord[largestWordLen]!='\0') {
                            largestWordLen++;
                        }

                        if (wordLen>largestWordLen) {
                            largestWord=word;
                            start_col=col;
                            start_row=row;
                            final_dcol=dcol;
                            final_drow=drow;
                        }
                        wordsfound++;
                        if(wordsfound==4){
                        break;
                        }
                    }
                }
            }
        }
    }
    if (largestWord[0]!='\0'){ 
        int wordLen=0;
        while(largestWord[wordLen]!='\0'){
            wordLen++;
        }

        for (int m=0;m<wordLen;m++){
            int current_col=start_col+m*final_dcol;
            int current_row=start_row+m*final_drow;
            grid[current_col][current_row]=-1; 
            score++;
        }
  cout<<largestWord<<endl; 
    }
}

void DrawAlphabet(const alphabets &cname, int sx, int sy, int cwidth = 60,
	int cheight = 60)
	/*Draws a specfic cookie at given position coordinate
	* sx = position of x-axis from left-bottom
	* sy = position of y-axis from left-bottom
	* cwidth= width of displayed cookie in pixels
	* cheight= height of displayed cookiei pixels.
	* */
{
	float fwidth = (float)cwidth / width * 2, fheight = (float)cheight
		/ height * 2;
	float fx = (float)sx / width * 2 - 1, fy = (float)sy / height * 2 - 1;
        
      
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, mtid[cname]);
	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0);
	glVertex2d(fx, fy);
	glTexCoord2d(1.0, 0.0);
	glVertex2d(fx + fwidth, fy);
	glTexCoord2d(1.0, 1.0);
	glVertex2d(fx + fwidth, fy + fheight);
	glTexCoord2d(0.0, 1.0);
	glVertex2d(fx, fy + fheight);
	glEnd();

	glColor4f(1, 1, 1, 1);

	//	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//glutSwapBuffers();
}
int GetAlphabet() {
	return GetRandInRange(1, 26);
}

void Pixels2Cell(int px, int py, int & cx, int &cy) {
}
void Cell2Pixels(int cx, int cy, int & px, int &py)
// converts the cell coordinates to pixel coordinates...
{
}
void DrawShooter(int sx, int sy, int cwidth = 60, int cheight = 60)

{
	float fwidth = (float)cwidth / width * 2, fheight = (float)cheight
		/ height * 2;
	float fx = (float)sx / width * 2 - 1, fy = (float)sy / height * 2 - 1;

	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, -1);
	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0);
	glVertex2d(fx, fy);
	glTexCoord2d(1.0, 0.0);
	glVertex2d(fx + fwidth, fy);
	glTexCoord2d(1.0, 1.0);
	glVertex2d(fx + fwidth, fy + fheight);
	glTexCoord2d(0.0, 1.0);
	glVertex2d(fx, fy + fheight);
	glEnd();

	glColor4f(1, 1, 1, 1);

	//	glBindTexture(GL_TEXTURE_2D, 0);
       // DrawAlphabet(AL_A,430, 0+ cheight, 55, 55);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//glutSwapBuffers();
}
/*
* Main Canvas drawing function.
* */
    
bool collisioncheck(int locx,int locy,int radius,int rowY,int alphabetGrid[5][15],int r) {
	    for (int i = 0; i < 15; ++i) {
		int alphabetXU = i*60-60; 
		int alphabetYU = rowY;  
		 if(alphabetGrid[r][i] == -1)
				continue;  
		if(locx+radius>alphabetXU&&locx-radius<alphabetXU+60&&locy+radius>alphabetYU&&locy-radius<alphabetYU+60){
        		cout<<r<<i<<endl;
	      		    return true;  
		}
	    }
    return false;  
}
void balancer(int alphabetGrid[5][15],int bx,int by){
	int x=(bx)/60;
	int y=(height-by-60)/60;
	alphabetGrid[y][x]=ball;
	cout<<height<<' '<<y<<' '<<x<<' '<<by<<' '<<bx<<endl;
}
void alphabetgrid() {
  do{
    for(int j=0;j<5;j++){
      if(j<2)
	for(int i=0;i<15;i++){
	  alphabetGrid[j][i] = rand() % 26; 
    	  }
       else
        for(int i=0;i<15;i++)
      	  alphabetGrid[j][i]=-1;
	  }
	}while(analyzewordsingrid(alphabetGrid,dictionary,dictionarysize));
    ball=rand()%26;
}
void DisplayFunction() {
	// set the background color using function glClearColor.
	// to change the background play with the red, green and blue values below.
	// Note that r, g and b values must be in the range [0,1] where 0 means dim red and 1 means pure red and so on.
	//#if 0
	glClearColor(1/*Red Component*/, 1.0/*Green Component*/,
		1.0/*Blue Component*/, 0 /*Alpha component*/); // Red==Green==Blue==1 --> White Colour
	glClear(GL_COLOR_BUFFER_BIT); //Update the colors
	if (gameover) {
        DrawAlphabet((alphabets)6, 250, height-300, awidth, aheight);

	DrawAlphabet((alphabets)0, 310, height -300, awidth, aheight);
	DrawAlphabet((alphabets)12, 370, height -300, awidth, aheight);

	DrawAlphabet((alphabets)4, 430, height -300, awidth, aheight);
	DrawAlphabet((alphabets)14, 490, height-300, awidth, aheight);

	DrawAlphabet((alphabets)21, 550, height -300, awidth, aheight);
	DrawAlphabet((alphabets)4, 610, height -300, awidth, aheight);

	DrawAlphabet((alphabets)17, 670, height -300, awidth, aheight);

	DrawString(425, 310, width, height + 5, "Final Score: " + Num2Str(score), colors[BLUE_VIOLET]);
	DrawString(235, 50, width, height + 5, "-" , colors[WHITE]);
    }else{
		
		locx+=speedx*30;
		locy+=speedy*30;
		if(locx<bradius-50){
	      	locx = bradius;  
	      	speedx = -speedx; 
		}
		if(locx>width-bradius){
		locx=width-bradius; 
	        speedx = -speedx;  
		}
		if(locy<bradius){
	        locy=bradius; 
		speedy=-speedy;  
		}
		if(locy>height-bradius){
		  locy=height-bradius;  
	      	  speedy=-speedy;  
		}
		for(int i=0;i<5;i++){
		  int value=i*60+140;
		  if (collisioncheck(locx,locy,bradius,height-value,alphabetGrid,i)) {
	    		balancer(alphabetGrid,locx,locy);
	      		wordsingrid(alphabetGrid,dictionary,dictionarysize);
	      		locx = width / 2;  
	      		locy = 0;
	      		speedx = 0;  
	      		speedy = 0;
	      		ballLaunched = false;  
	      		ball = arr[globally%26];  
	      		globally++;
			}
		}	
		int deviation=120;
		for(int j=0;j<5;j++){
			int x_s = 10;
			for (int i = 0; i < 15; i++) {
			int curr_height=height-deviation;
			DrawAlphabet((alphabets)alphabetGrid[j][i], x_s, height - deviation, awidth, aheight);  
			x_s = x_s + 60;
			}
			deviation+=60;
		}
		
		DrawAlphabet((alphabets)ball, locx, locy, awidth, aheight);			
		DrawString(40, height - 20, width, height + 5, "Score " + Num2Str(score), colors[BLUE_VIOLET]);
	DrawString(width / 2 - 30, height - 25, width, height,"Time Left:" + Num2Str(displaytime) + " secs", colors[RED]);
	DrawString(80, height - 20, 500, height + 5, "Nauman Rafay\t24i-2558 ", colors[BLUE_VIOLET]);
	DrawShooter((width / 2) - 35, 0, bwidth, bheight);
	}
	glutSwapBuffers();
}	
	// #----------------- Write your code till here ----------------------------#
	//DO NOT MODIFY THESE LINES
	//DrawShooter((width / 2) - 35, 0, bwidth, bheight);
//	}
//	glutSwapBuffers();
	//DO NOT MODIFY THESE LINES..
//}	
	

/* Function sets canvas size (drawing area) in pixels...
*  that is what dimensions (x and y) your game will have
*  Note that the bottom-left coordinate has value (0,0) and top-right coordinate has value (width-1,height-1)
* */
void SetCanvasSize(int width, int height) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, 0, height, -1, 1); // set the screen size to given width and height.
}

/*This function is called (automatically) whenever any non-printable key (such as up-arrow, down-arraw)
* is pressed from the keyboard
*
* You will have to add the necessary code here when the arrow keys are pressed or any other key is pressed...
*
* This function has three argument variable key contains the ASCII of the key pressed, while x and y tells the
* program coordinates of mouse pointer when key was pressed.
*
* */

void NonPrintableKeys(int key, int x, int y) {
	if (key==GLUT_KEY_LEFT) {
	}
	else if (key == GLUT_KEY_RIGHT /*GLUT_KEY_RIGHT is constant and contains ASCII for right arrow key*/) {
        
	}
	else if (key == GLUT_KEY_UP/*GLUT_KEY_UP is constant and contains ASCII for up arrow key*/) {
	}
	else if (key == GLUT_KEY_DOWN/*GLUT_KEY_DOWN is constant and contains ASCII for down arrow key*/) {
	}

	/* This function calls the Display function to redo the drawing. Whenever you need to redraw just call
	* this function*/
	/*
	glutPostRedisplay();
	*/
}
/*This function is called (automatically) whenever your mouse moves witin inside the game window
*
* You will have to add the necessary code here for finding the direction of shooting
*
* This function has two arguments: x & y that tells the coordinate of current position of move mouse
*
* */

void MouseMoved(int x, int y) {
	//If mouse pressed then check than swap the balls and if after swaping balls dont brust then reswap the balls

}

/*This function is called (automatically) whenever your mouse button is clicked witin inside the game window
*
* You will have to add the necessary code here for shooting, etc.
*
* This function has four arguments: button (Left, Middle or Right), state (button is pressed or released),
* x & y that tells the coordinate of current position of move mouse
*
* */

void MouseClicked(int button, int state, int x, int y) {

	if (button == GLUT_LEFT_BUTTON) // dealing only with left button
	{     
		if (state == GLUT_UP)
		{
		  if (ballLaunched)
		    return;

        float shotx=(float)x/width*2-1;
        float shoty=-((float)y/height*2-1);
        float dx=shotx-(float)locx/width*2+1;
        float dy=shoty-(float)locy/height*2+1;
        float length=sqrt(dx*dx+dy*dy);
        dx/=length;
        dy/=length;
        speedx=dx;
        speedy=dy;
	ballLaunched=true; 
	}
    }
	
	else if (button == GLUT_RIGHT_BUTTON) // dealing with right button
	{

	}
	glutPostRedisplay();
}

/*This function is called (automatically) whenever any printable key (such as x,b, enter, etc.)
* is pressed from the keyboard
* This function has three argument variable key contains the ASCII of the key pressed, while x and y tells the
* program coordinates of mouse pointer when key was pressed.
* */
void PrintableKeys(unsigned char key, int x, int y) {
	if (key == KEY_ESC/* Escape key ASCII*/) {
		exit(1); // exit the program when escape key is pressed.
	}

}


/*
* This function is called after every 1000.0/FPS milliseconds
* (FPS is defined on in the beginning).
* You can use this function to animate objects and control the
* speed of different moving objects by varying the constant FPS.
*
* */
void Timer(int m) {
  if(timee>0){
  timee--;
  if(timee%10==0){
    displaytime--;
    }
  }
  if(timee==0){
  gameover=true;
  }
	glutPostRedisplay();
	glutTimerFunc(1000.0/FPS, Timer, 0);
 }

/*
* our gateway main function
* */
int main(int argc, char*argv[]) {
	InitRandomizer(); 
        initializeaudio();
        
        Mix_Music* bgMusic=Mix_LoadMUS("background.mp3");
        if(!bgMusic){
        cerr<<"Failed to load background music "<<Mix_GetError()<<endl;
        cleanupaudio();
        exit(-1);
      }

    
        Mix_PlayMusic(bgMusic, -1); 
	//Dictionary for matching the words. It contains the 370099 words.
	dictionary = new string[dictionarysize]; 
	ReadWords("words_alpha.txt", dictionary); // dictionary is an array of strings
	//print first 5 words from the dictionary
	for(int i=0; i < 5; ++i)
		cout<< " word "<< i << " =" << dictionary[i] <<endl;

	//Write your code here for filling the canvas with different Alphabets. You can use the Getalphabet function for getting the random alphabets
        alphabetgrid();

        
	glutInit(&argc, argv); // initialize the graphics library...
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // we will be using color display mode
	glutInitWindowPosition(50, 50); // set the initial position of our window
	glutInitWindowSize(width, height); // set the size of our window
	glutCreateWindow("ITCs Word Shooter"); // set the title of our game window
	//SetCanvasSize(width, height); // set the number of pixels...

	// Register your functions to the library,
	// you are telling the library names of function to call for different tasks.
	RegisterTextures();
	glutDisplayFunc(DisplayFunction); // tell library which function to call for drawing Canvas.
	glutSpecialFunc(NonPrintableKeys); // tell library which function to call for non-printable ASCII characters
	glutKeyboardFunc(PrintableKeys); // tell library which function to call for printable ASCII characters
	glutMouseFunc(MouseClicked);
	glutPassiveMotionFunc(MouseMoved); // Mouse
          
	 //This functiontells the library to call our Timer function after 1000.0/FPS milliseconds...
	glutTimerFunc(10000.0/FPS, Timer, 0);

	// now handle the control to library and it will call our registered functions when
	// it deems necessary...

	glutMainLoop();
        Mix_FreeMusic(bgMusic);
        cleanupaudio();

  
	return 1;
}
#endif /* */
