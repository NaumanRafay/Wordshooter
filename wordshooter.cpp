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
int loc1y=18;
int loc1x=465;
int loc2y=18;
int loc2x=465;
int loc3y=18;
int loc3x=465;
int loc4y=18;
int loc4x=465;
int loc5y=18;
int loc5x=465;
int loc6y=18;
int loc6x=465;
int loc7y=18;
int loc7x=465;
int loc8y=18;
int loc8x=465;
int loc9y=18;
int loc9x=465;
int loc10y=18;
int loc10x=465;
int loc11y=18;
int loc11x=465;
int loc12y=18;
int loc12x=465;
int loc13y=18;
int loc13x=465;
int loc14y=18;
int loc14x=465;
int loc15y=18;
int loc15x=465;
int loc16y=18;
int loc16x=465;
int loc17y=18;
int loc17x=465;
int loc18y=18;
int loc18x=465;
int loc19y=18;
int loc19x=465;
int loc20y=18;
int loc20x=465;
int loc21y=18;
int loc21x=465;
int loc22y=18;
int loc22x=465;
int loc23y=18;
int loc23x=465;
int loc24y=18;
int loc24x=465;
int loc25y=18;
int loc25x=465;
int loc26y=18;
int loc26x=465;
int loc27y=18;
int loc27x=465;
float locx=18;
float locy=465;
int displaytime=150;
const int NUM_COUNT = 50;
int num[NUM_COUNT]; 

float shotx=-1;
float shoty=-1;
bool bubblemoving=false;

bool boundarycheck;
int clickcount=0;
float dx=0;
float dy=0;
const int NUM_ROWS=2; 
const int NUM_COLS=15; 
int turn=0;
string newforward;
string backward;
bool isBubbleBurst[10][15];
int bubbles[10][15];
char topalphabets[NUM_ROWS][NUM_COLS];

void InitializeAudio(){
    if (SDL_Init(SDL_INIT_AUDIO)<0){
        cerr<<"Failed to initialize SDL audio:"<<SDL_GetError()<<endl;
        exit(-1);
    }
    if(Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,2048)<0){
        cerr<<"Failed to initialize SDL_mixer "<<Mix_GetError()<<endl;
        exit(-1);
    }
}

void CleanupAudio() {
    Mix_CloseAudio();
    SDL_Quit();
}




void loadDictionary(std::string* dictionary) {
    ifstream file("dictionary.txt");
    if (!file.is_open()) {
        cerr << "Error: Could not open dictionary file!" << std::endl;
        return;
    }

    string word;
    int index = 0;
    while (file >> word && index < dictionarysize) {
        dictionary[index++] = word; 
    }
    file.close();
}
bool isWordInDictionary(const std::string& word, std::string* dictionary, int dictionarysize) {
    for (int i = 0; i < dictionarysize; ++i) {
        if (dictionary[i] == word) {
            return true; 
        }
    }
    return false; 
}

alphabets charToAlphabet(char c){
    if(c>='A'&&c<='Z'){
      return static_cast<alphabets>(c - 'A'); 
    }
    return static_cast<alphabets>(-1); 
}


void RegisterTextures_Write()
{
	glGenTextures(nalphabets, tid);
	vector<unsigned char> data;
	ofstream ofile("image-data.bin", ios::binary | ios::out);

	for (int i = 0; i < nalphabets; ++i) {

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
void generateRandomAlphabets(char topalphabets[NUM_ROWS][NUM_COLS]) {
    for(int i=0;i<NUM_ROWS;i++){ 
        for(int j=0;j<NUM_COLS;j++){
            int randomIndex=rand()%26;
            topalphabets[i][j]='A'+randomIndex;
        }
    }
}
void initializeBubbles(){
    for(int i=0;i<10;i++){
      for(int j=0;j<15;j++){
         isBubbleBurst[i][j]=false; 
        }
    }
}
void maxlengthword_h(){
    for(int i=0;i<10;i++){  
        for(int j=0;j<15;j++) {
          for(int k=14;k>=j+2;k--){
                string newforward="";
                string backward="";
                for(int l=j;l<=k;l++){
                    if(isBubbleBurst[i][l]){
                        continue;  
                     }
                    newforward+=char(bubbles[i][l]+97);  
                }
                for(int l=k;l>=j;l--){
                    if(isBubbleBurst[i][l]){
                        continue;  
                    }
                    backward+=char(bubbles[i][l]+97);  
                }
      if(isWordInDictionary(newforward,dictionary,dictionarysize)||isWordInDictionary(backward,dictionary,dictionarysize)){
        for (int l = j; l <= k; l++) {
            isBubbleBurst[i][l] = true;
                score++;
              }
            }
          }
      }
    }
}
void maxlengthword_v() {
    for(int i=0;i<15;i++){  
        for(int j=0;j<10;j++){
            for(int k=9;k>=j+2;k--){
                std::string newforward = "";
                std::string backward = "";
                for (int l = j; l <= k; l++) {
                    if (isBubbleBurst[l][i]) {
                        continue;  
                    }
                    newforward+=char(bubbles[l][i]+97); 
                }
                for(int l=k;l>=j;l--){
                    if(isBubbleBurst[l][i]){
                      continue; 
                    }
                    backward+=char(bubbles[l][i]+97); 
                }
      if(isWordInDictionary(newforward,dictionary,dictionarysize)||isWordInDictionary(backward,dictionary,dictionarysize)){
          for (int l=j;l<=k;l++){
              isBubbleBurst[l][i]=true;
                score++;
              }
            }
          }
        }
    }
}
bool reachedtarget(){
  return locy<=430; 
}         
void DisplayFunction() {
	// set the background color using function glClearColor.
	// to change the background play with the red, green and blue values below.
	// Note that r, g and b values must be in the range [0,1] where 0 means dim red and 1 means pure red and so on.
	//#if 0
	glClearColor(1/*Red Component*/, 1.0/*Green Component*/,
		1.0/*Blue Component*/, 0 /*Alpha component*/); // Red==Green==Blue==1 --> White Colour
	glClear(GL_COLOR_BUFFER_BIT); //Update the colors
	turn++;
    if(turn==1){
    generateRandomAlphabets(topalphabets);
    }
    for (int col=0;col<NUM_COLS;col++){
    DrawAlphabet(charToAlphabet(topalphabets[0][col]),10+col*60,height-100, awidth, aheight);
}
    for(int col=0;col<NUM_COLS;col++){
    DrawAlphabet(charToAlphabet(topalphabets[1][col]),10+col*60,height-160,awidth,aheight); 
}    
     if(startdisplay>0 && timee!=0){
	//shooting alphabet
	if(loc1y<435&&clickcount==1){
	if (shotx!=-1&&shoty!=-1&&reachedtarget){
        //float dx=shotx-loc1x;
        //float dy=shoty-loc1y;
        float distance=sqrt(dx*dx+dy*dy);
        if(distance>1.0f){
        float dirx=dx/distance;
        float diry=dy/distance;
        loc1x+=dirx*30;
        loc1y+=diry*30;
        if(loc1x>870){
        loc1x=870;
        dx=-dx;
        }
        if(loc1x<0){
        loc1x=0;
        dx=-dx;
        }
        if(loc1y>660){
        loc1y=660;
        dy=-dy; 
        }
        }
	}
	/*if (loc1y<=height-100&&loc1y<height-160){
            int targetRow=(loc1y>height-130)?0:1; 
            int targetCol=(loc1x-10)/60; 
            if(targetCol>=0&&targetCol<NUM_COLS&&!isBubbleBurst[targetRow][targetCol]){
              topalphabets[targetRow][targetCol]; 
              maxlengthword_h(); 
              maxlengthword_v(); 
            }
        }*/
    }
	//maxlengthword_h();  
       // maxlengthword_v();
	DrawAlphabet((alphabets)num[31], loc1x, loc1y, awidth, aheight);
	
	if(loc2y<435 && loc1y>415&&clickcount==2){
	if (shotx!=-1&&shoty!=-1&&bubblemoving) {
        //float dx=shotx-loc2x;
        //float dy=shoty-loc2y;
        float distance=sqrt(dx*dx+dy*dy);
        if(distance>1.0f){
        float dirx=dx/distance;
        float diry=dy/distance;
        
        loc2x+=dirx*30;
        loc2y+=diry*30;
        if(loc2x>870){
        loc2x=870;
        dx=-dx;
        }
        if(loc2x<0){
        loc2x=0;
        dx=-dx;
        }
        if(loc2y>660){
        loc2y=660;
        dy=-dy;
        }
        }
	}
	if(loc2y==435){
	bubblemoving=false;
	}
	/*if(loc2y<=height-100&&loc2y<height-160){
          int targetRow=(loc2y>height-130)?0:1; 
          int targetCol=(loc2x-10)/60; 
            if(targetCol>=0&&targetCol<NUM_COLS&&!isBubbleBurst[targetRow][targetCol]){
              topalphabets[targetRow][targetCol];
              maxlengthword_h(); 
              maxlengthword_v(); 
            }
        }*/
    }
	if(loc1y>415){
	DrawAlphabet((alphabets)num[32], loc2x, loc2y, awidth, aheight);
	}
	
	
	if(loc3y<435 && loc2y>415&&clickcount==3){
	if (shotx!=-1&&shoty!=-1&&bubblemoving) {
        //float dx=shotx-loc3x;
        //float dy=shoty-loc3y;
        float distance=sqrt(dx*dx+dy*dy);
        if(distance>1.0f){
        float dirx=dx/distance;
        float diry=dy/distance;
        
        loc3x+=dirx*30;
        loc3y+=diry*30;
        if(loc3x>870){
        loc3x=870;
        dx=-dx;
        }
        if(loc3x<0){
        loc3x=0;
        dx=-dx;
        }
        if(loc3y>660){
        loc3y=660;
        dy=-dy;
        }
        }
	}
	if(loc3y==435){
	bubblemoving=false;
	}
	/*if(loc3y<=height-100&&loc3y<height-160){
            int targetRow=(loc3y>height-130)?0:1; 
            int targetCol=(loc3x-10)/60; 
            if(targetCol>=0&&targetCol<NUM_COLS&&!isBubbleBurst[targetRow][targetCol]) {
                topalphabets[targetRow][targetCol]; 
                maxlengthword_h(); 
                maxlengthword_v(); 
            }
        }*/
    }
	
	//loc3y=loc3y+8;
	
	if(loc2y>415){
	DrawAlphabet((alphabets)num[33], loc3x, loc3y, awidth, aheight);
	}
	
	//forth shoot
	if(loc4y<435 && loc3y>415&&clickcount==4){
	if (shotx!=-1&&shoty!=-1&&bubblemoving) {
        //float dx=shotx-loc4x;
        //float dy=shoty-loc4y;
        float distance=sqrt(dx*dx+dy*dy);
        if(distance>1.0f){
        float dirx=dx/distance;
        float diry=dy/distance;
        
        loc4x+=dirx*30;
        loc4y+=diry*30;
        if(loc4x>870){
        loc4x=870;
        dx=-dx;
        }
        if(loc4x<0){
        loc4x=0;
        dx=-dx;
        }
        if(loc4y>660){
        loc4y=660;
        }
        }
	}
	if(loc4y==435)
	bubblemoving=false;
	}
	if(loc3y>415){
	DrawAlphabet((alphabets)num[34],loc4x,loc4y,awidth,aheight);
	}
	
	//fifth shoot
	if(loc5y<435 && loc4y>415&&clickcount==5){
	if(shotx!=-1&&shoty!=-1&&bubblemoving){
    //    float dx=shotx-loc5x;
    //    float dy=shoty-loc5y;
        float distance=sqrt(dx*dx+dy*dy);
        if(distance>1.0f){
        float dirx=dx/distance;
        float diry=dy/distance;
        
        loc5x+=dirx*30;
        loc5y+=diry*30;
        if(loc5x>870){
        loc5x=870;
        dx=-dx;
        }
        if(loc5x<0){
        loc5x=0;
        dx=-dx;
        }
        if(loc5y>660){
        loc5y=660;
        }
        }
	}
	if(loc5y==435)
	bubblemoving=false;
	}
	if(loc4y>415){
	DrawAlphabet((alphabets)num[35], loc5x, loc5y, awidth, aheight);
	}
	
	//sixth shoot
	if(loc6y<435 && loc5y>415&&clickcount==6){
	if (shotx!=-1&&shoty!=-1&&bubblemoving) {
       // float dx=shotx-loc6x;
       // float dy=shoty-loc6y;
        float distance=sqrt(dx*dx+dy*dy);
        if(distance>1.0f){
        float dirx=dx/distance;
        float diry=dy/distance;
        
        loc6x+=dirx*30;
        loc6y+=diry*30;
        if(loc6x>870){
        loc6x=870;
        dx=-dx;
        }
        if(loc6x<0){
        loc6x=0;
        dx=-dx;
        }
        if(loc6y>660){
        loc6y=660;
        }
        }
	}
	if(loc6y==435)
	bubblemoving=false;
	}
	if(loc5y>415){
	DrawAlphabet((alphabets)num[36], loc6x, loc6y, awidth, aheight);
	}
	
	//seventh shoot
	if(loc7y<435 && loc6y>415&&clickcount==7){
	if (shotx!=-1&&shoty!=-1&&bubblemoving) {
        //float dx=shotx-loc7x;
        //float dy=shoty-loc7y;
        float distance=sqrt(dx*dx+dy*dy);
        if(distance>1.0f){
        float dirx=dx/distance;
        float diry=dy/distance;
        
        loc7x+=dirx*30;
        loc7y+=diry*30;
        if(loc7x>870){
        loc7x=870;
        dx=-dx;
        }
        if(loc7x<0){
        loc7x=0;        
        dx=-dx;
        }
        if(loc7y>660){
        loc7y=660;
        }
        }
	}
	if(loc7y==435)
	bubblemoving=false;
	}
	if(loc6y>415){
	DrawAlphabet((alphabets)num[37], loc7x, loc7y, awidth, aheight);
	}
	
	//eighth shoot
	if(loc8y<435 && loc7y>415&&clickcount==8){
	if (shotx!=-1&&shoty!=-1&&bubblemoving) {
       // float dx=shotx-loc8x;
       // float dy=shoty-loc8y;
        float distance=sqrt(dx*dx+dy*dy);
        if(distance>1.0f){
        float dirx=dx/distance;
        float diry=dy/distance;
        
        loc8x+=dirx*30;
        loc8y+=diry*30;
        if(loc4x>870){
        loc8x=870;
        dx=-dx;
        }
        if(loc8x<0){
        loc8x=0;
        dx=-dx;
        }
        if(loc8y>660){
        loc8y=660;
        }
        }
	}
	if(loc8y==435)
	bubblemoving=false;
	}
	if(loc7y>415){
	DrawAlphabet((alphabets)num[38], loc8x, loc8y, awidth, aheight);
	}
	
	//ninth shoot
	if(loc9y<435 && loc8y>415&&clickcount==9){
	if (shotx!=-1&&shoty!=-1&&bubblemoving) {
      //  float dx=shotx-loc9x;
        //float dy=shoty-loc9y;
        float distance=sqrt(dx*dx+dy*dy);
        if(distance>1.0f){
        float dirx=dx/distance;
        float diry=dy/distance;
        
        loc9x+=dirx*30;
        loc9y+=diry*30;
        if(loc9x>870){
        loc4x=870;
        dx=-dx;
        }
        if(loc9x<0){
        loc9x=0;
        dx=-dx;
        }
        if(loc9y>660){
        loc9y=660;
        }
        }
	}
	if(loc9y==435)
	bubblemoving=false;
	}
	if(loc8y>415){
	DrawAlphabet((alphabets)num[39], loc9x, loc9y, awidth, aheight);
	}
	
	//tenth shoot
	if(loc10y<435 && loc9y>415&&clickcount==10){
	if (shotx!=-1&&shoty!=-1&&bubblemoving) {
      //  float dx=shotx-loc10x;
      //  float dy=shoty-loc10y;
        float distance=sqrt(dx*dx+dy*dy);
        if(distance>1.0f){
        float dirx=dx/distance;
        float diry=dy/distance;
        
        loc10x+=dirx*30;
        loc10y+=diry*30;
        if(loc10x>870){
        loc10x=870;
        dx=-dx;
        }
        if(loc10x<0){
        loc10x=0;
        dx=-dx;
        }
        if(loc10y>660){
        loc10y=660;
        }
        }
	}
	if(loc10y==435)
	bubblemoving=false;
	}
	if(loc9y>415){
	DrawAlphabet((alphabets)num[40], loc10x, loc10y, awidth, aheight);
	}
	//eleventh shoot
	if(loc11y<435 && loc10y>415&&clickcount==11){
	if (shotx!=-1&&shoty!=-1&&bubblemoving) {
        //float dx=shotx-loc11x;
       // float dy=shoty-loc11y;
        float distance=sqrt(dx*dx+dy*dy);
        if(distance>1.0f){
        float dirx=dx/distance;
        float diry=dy/distance;
        
        loc11x+=dirx*30;
        loc11y+=diry*30;
        if(loc11x>870){
        loc11x=870;
        dx=-dx;
        }
        if(loc11x<0){
        loc11x=0;
        dx=-dx;
        }
        if(loc11y>660){
        loc11y=660;
        }
        }
	}
	if(loc11y==435)
	bubblemoving=false;
	}
	if(loc10y>415){
	DrawAlphabet((alphabets)num[41], loc11x, loc11y, awidth, aheight);
	}
	//twelveth shoot
	if(loc12y<435 && loc11y>415&&clickcount==12){
	if (shotx!=-1&&shoty!=-1&&bubblemoving) {
     //   float dx=shotx-loc12x;
       // float dy=shoty-loc12y;
        float distance=sqrt(dx*dx+dy*dy);
        if(distance>1.0f){
        float dirx=dx/distance;
        float diry=dy/distance;
        
        loc12x+=dirx*30;
        loc12y+=diry*30;
        if(loc12x>870){
        loc12x=870;
        dx=-dx;
        }
        if(loc12x<0){
        loc12x=0;
        dx=-dx;
        }
        if(loc12y>660){
        loc12y=660;
        }
        }
	}
	if(loc12y==435)
	bubblemoving=false;
	}
	if(loc11y>415){
	DrawAlphabet((alphabets)num[42], loc12x, loc12y, awidth, aheight);
	}
	//thirteenth shoot
	if(loc13y<435 && loc12y>415&&clickcount==13){
	if (shotx!=-1&&shoty!=-1&&bubblemoving) {
  //      float dx=shotx-loc13x;
    //    float dy=shoty-loc13y;
        float distance=sqrt(dx*dx+dy*dy);
        if(distance>1.0f){
        float dirx=dx/distance;
        float diry=dy/distance;
        
        loc13x+=dirx*30;
        loc13y+=diry*30;
        if(loc13x>870){
        loc13x=870;
        dx=-dx;
        }
        if(loc13x<0){
        loc13x=0;
        dx=-dx;
        }
        if(loc13y>660){
        loc13y=660;
        }
        }
	}
	if(loc13y==435)
	bubblemoving=false;
	}
	if(loc12y>415){
	DrawAlphabet((alphabets)num[43], loc13x, loc13y, awidth, aheight);
	}
	if(loc14y<435 && loc12y>415&&clickcount==14){
	if (shotx!=-1&&shoty!=-1&&bubblemoving) {
   //     float dx=shotx-loc14x;
     //   float dy=shoty-loc14y;
        float distance=sqrt(dx*dx+dy*dy);
        if(distance>1.0f){
        float dirx=dx/distance;
        float diry=dy/distance;
        
        loc14x+=dirx*30;
        loc14y+=diry*30;
        if(loc14x>870){
        loc14x=870;
        dx=-dx;
        }
        if(loc14x<0){
        loc14x=0;
        dx=-dx;
        }
        if(loc14y>660){
        loc14y=660;
        }
        }
	}
	if(loc14y==435)
	bubblemoving=false;
	}
	if(loc13y>415){
	DrawAlphabet((alphabets)num[44], loc14x, loc14y, awidth, aheight);
	}
	if(loc15y<435 && loc14y>415&&clickcount==15){
	if (shotx!=-1&&shoty!=-1&&bubblemoving) {
      //  float dx=shotx-loc15x;
      //  float dy=shoty-loc15y;
        float distance=sqrt(dx*dx+dy*dy);
        if(distance>1.0f){
        float dirx=dx/distance;
        float diry=dy/distance;
        
        loc15x+=dirx*30;
        loc15y+=diry*30;
        if(loc15x>870){
        loc15x=870;
        dx=-dx;
        }
        if(loc15x<0){
        loc15x=0;
        dx=-dx;
        }
        if(loc15y>660){
        loc15y=660;
        }
        }
	}
	if(loc15y==435)
	bubblemoving=false;
	}
	if(loc14y>415){
	DrawAlphabet((alphabets)num[45], loc15x, loc15y, awidth, aheight);
	}

        if(loc16y<390 && loc15y>380&&clickcount==16){
	if (shotx!=-1&&shoty!=-1&&bubblemoving) {
       // float dx=shotx-loc16x;
       // float dy=shoty-loc16y;
        float distance=sqrt(dx*dx+dy*dy);
        if(distance>1.0f){
        float dirx=dx/distance;
        float diry=dy/distance;
        
        loc16x+=dirx*30;
        loc16y+=diry*30;
        if(loc16x>870){
        loc16x=870;
        dx=-dx;
        }
        if(loc16x<0){
        loc16x=0;
        dx=-dx;
        }
        if(loc16y>660){
        loc16y=660;
        }
        }
	}
	if(loc16y==435)
	bubblemoving=false;
	}
	if(loc15y>380){
	DrawAlphabet((alphabets)num[46], loc16x, loc16y, awidth, aheight);
	}
	if(loc17y<390 && loc16y>380&&clickcount==17){
	if (shotx!=-1&&shoty!=-1&&bubblemoving) {
       // float dx=shotx-loc17x;
       // float dy=shoty-loc17y;
        float distance=sqrt(dx*dx+dy*dy);
        if(distance>1.0f){
        float dirx=dx/distance;
        float diry=dy/distance;
        
        loc17x+=dirx*30;
        loc17y+=diry*30;
        if(loc17x>870){
        loc17x=870;
        dx=-dx;
        }
        if(loc17x<0){
        loc17x=0;
        dx=-dx;
        }
        if(loc17y>660){
        loc17y=660;
        }
        }
	}
	if(loc17y==435)
	bubblemoving=false;
	}
	if(loc16y>380){
	DrawAlphabet((alphabets)num[47], loc17x, loc17y, awidth, aheight);
	}
	if(loc18y<390 && loc17y>380&&clickcount==18){
	if (shotx!=-1&&shoty!=-1&&bubblemoving) {
      //  float dx=shotx-loc18x;
        //float dy=shoty-loc18y;
        float distance=sqrt(dx*dx+dy*dy);
        if(distance>1.0f){
        float dirx=dx/distance;
        float diry=dy/distance;
        
        loc18x+=dirx*30;
        loc18y+=diry*30;
        if(loc18x>870){
        loc18x=870;
        dx=-dx;
        }
        if(loc18x<0){
        loc18x=0;
        dx=-dx;
        }
        if(loc18y>660){
        loc18y=660;
        }
        }
	}
	if(loc18y==435)
	bubblemoving=false;
	}
	if(loc17y>380){
	DrawAlphabet((alphabets)num[48], loc18x, loc18y, awidth, aheight);
	}
	if(loc19y<390 && loc18y>380&&clickcount==19){
	if (shotx!=-1&&shoty!=-1&&bubblemoving) {
    //    float dx=shotx-loc19x;
      //  float dy=shoty-loc19y;
        float distance=sqrt(dx*dx+dy*dy);
        if(distance>1.0f){
        float dirx=dx/distance;
        float diry=dy/distance;
        
        loc19x+=dirx*30;
        loc19y+=diry*30;
        if(loc19x>870){
        loc19x=870;
        dx=-dx;
        }
        if(loc19x<0){
        loc19x=0;
        dx=-dx;
        }
        if(loc19y>660){
        loc19y=660;
        }
        }
	}
	if(loc19y==435)
	bubblemoving=false;
	}
	if(loc18y>380){
	DrawAlphabet((alphabets)num[49], loc19x, loc19y, awidth, aheight);
	}
	if(loc20y<390 && loc19y>380&&clickcount==20){
	if (shotx!=-1&&shoty!=-1&&bubblemoving) {
   //     float dx=shotx-loc20x;
    //    float dy=shoty-loc20y;
        float distance=sqrt(dx*dx+dy*dy);
        if(distance>1.0f){
        float dirx=dx/distance;
        float diry=dy/distance;
        
        loc20y+=dirx*30;
        loc20y+=diry*30;
        if(loc20x>870){
        loc20x=870;
        dx=-dx;
        }
        if(loc20x<0){
        loc20x=0;
        dx=-dx;
        }
        if(loc20y>660){
        loc20y=660;
        }
        }
	}
	if(loc20y==435)
	bubblemoving=false;
	}
	if(loc19y>380){
	DrawAlphabet((alphabets)num[50], loc20x, loc20y, awidth, aheight);
	}
	if(loc21y<390 && loc20y>380&&clickcount==21){
	if (shotx!=-1&&shoty!=-1&&bubblemoving) {
    //    float dx=shotx-loc21x;
    //    float dy=shoty-loc21y;
        float distance=sqrt(dx*dx+dy*dy);
        if(distance>1.0f){
        float dirx=dx/distance;
        float diry=dy/distance;
        
        loc21x+=dirx*30;
        loc21y+=diry*30;
        if(loc21x>870){
        loc21x=870;
        dx=-dx;
        }
        if(loc21x<0){
        loc21x=0;
        dx=-dx;
        }
        if(loc21y>660){
        loc21y=660;
        }
        }
	}
	if(loc21y==435)
	bubblemoving=false;
	}
	if(loc20y>380){
	DrawAlphabet((alphabets)num[28], loc21x, loc21y, awidth, aheight);
	}
	if(loc22y<390 && loc21y>380&&clickcount==22){
	if (shotx!=-1&&shoty!=-1&&bubblemoving) {
   //     float dx=shotx-loc22x;
     //   float dy=shoty-loc22y;
        float distance=sqrt(dx*dx+dy*dy);
        if(distance>1.0f){
        float dirx=dx/distance;
        float diry=dy/distance;
        
        loc22x+=dirx*30;
        loc22y+=diry*30;
        if(loc22x>870){
        loc22x=870;
        dx=-dx;
        }
        if(loc22x<0){
        loc22x=0;
        dx=-dx;
        }
        if(loc22y>660){
        loc22y=660;
        }
        }
	}
	if(loc22y==435)
	bubblemoving=false;
	}
	
	if(loc21y>380){
	DrawAlphabet((alphabets)num[30], loc22x, loc22y, awidth, aheight);
	}
        if(loc23y<390 && loc22y>380&&clickcount==23){
	if (shotx!=-1&&shoty!=-1&&bubblemoving) {
     //   float dx=shotx-loc23x;
       // float dy=shoty-loc23y;
        float distance=sqrt(dx*dx+dy*dy);
        if(distance>1.0f){
        float dirx=dx/distance;
        float diry=dy/distance;
        
        loc23x+=dirx*30;
        loc23y+=diry*30;
        if(loc23x>870){
        loc23x=870;
        dx=-dx;
        }
        if(loc23x<0){
        loc23x=0;
        dx=-dx;
        }
        if(loc23y>660){
        loc23y=660;
        }
        }
	}
	if(loc23y==435)
	bubblemoving=false;
	}	
	if(loc22y>380){
	DrawAlphabet((alphabets)num[26], loc23x, loc23y, awidth, aheight);
	}
	if(loc24y<390 && loc23y>380&&clickcount==24){
	if (shotx!=-1&&shoty!=-1&&bubblemoving) {
//        float dx=shotx-loc24x;
  //      float dy=shoty-loc24y;
        float distance=sqrt(dx*dx+dy*dy);
        if(distance>1.0f){
        float dirx=dx/distance;
        float diry=dy/distance;
        
        loc24x+=dirx*30;
        loc24y+=diry*30;
        if(loc24x>870){
        loc24x=870;
        dx=-dx;
        }
        if(loc24x<0){
        loc24x=0;
        dx=-dx;
        }
        if(loc24y>660){
        loc24y=660;
        }
        }
	}
	if(loc24y==435)
	bubblemoving=false;
	}
	if(loc23y>380){
	DrawAlphabet((alphabets)num[25], loc24x, loc24y, awidth, aheight);
	}
	if(loc25y<390 && loc24y>380&&clickcount==25){
	if (shotx!=-1&&shoty!=-1&&bubblemoving) {
//        float dx=shotx-loc24x;
  //      float dy=shoty-loc24y;
        float distance=sqrt(dx*dx+dy*dy);
        if(distance>1.0f){
        float dirx=dx/distance;
        float diry=dy/distance;
        
        loc25x+=dirx*30;
        loc25y+=diry*30;
        if(loc25x>870){
        loc25x=870;
        dx=-dx;
        }
        if(loc25x<0){
        loc25x=0;
        dx=-dx;
        }
        if(loc25y>660){
        loc25y=660;
        }
        }
	}
	if(loc25y==435)
	bubblemoving=false;
	}
	if(loc24y>380){
	DrawAlphabet((alphabets)num[27], loc25x, loc25y, awidth, aheight);
	}
	if(loc26y<390 && loc25y>380&&clickcount==26){
	if (shotx!=-1&&shoty!=-1&&bubblemoving) {
//        float dx=shotx-loc24x;
  //      float dy=shoty-loc24y;
        float distance=sqrt(dx*dx+dy*dy);
        if(distance>1.0f){
        float dirx=dx/distance;
        float diry=dy/distance;
        
        loc26x+=dirx*30;
        loc26y+=diry*30;
        if(loc26x>870){
        loc26x=870;
        dx=-dx;
        }
        if(loc26x<0){
        loc26x=0;
        dx=-dx;
        }
        if(loc26y>660){
        loc26y=660;
        }
        }
	}
	if(loc26y==435)
	bubblemoving=false;
	}
	if(loc25y>380){
	DrawAlphabet((alphabets)num[29], loc26x, loc26y, awidth, aheight);
	}
	if(loc27y<390 && loc26y>380&&clickcount==27){
	if (shotx!=-1&&shoty!=-1&&bubblemoving) {
//        float dx=shotx-loc24x;
  //      float dy=shoty-loc24y;
        float distance=sqrt(dx*dx+dy*dy);
        if(distance>1.0f){
        float dirx=dx/distance;
        float diry=dy/distance;
        
        loc27x+=dirx*30;
        loc27y+=diry*30;
        if(loc27x>870){
        loc27x=870;
        dx=-dx;
        }
        if(loc27x<0){
        loc27x=0;
        dx=-dx;
        }
        if(loc27y>660){
        loc27y=660;
        }
        }
	}
	if(loc27y==435)
	bubblemoving=false;
	}
	if(loc26y>380){
	DrawAlphabet((alphabets)num[30], loc27x, loc27y, awidth, aheight);
	}
    
	DrawString(40, height - 20, width, height + 5, "Score " + Num2Str(score), colors[BLUE_VIOLET]);
	DrawString(width / 2 - 30, height - 25, width, height,
		"Time Left:" + Num2Str(displaytime) + " secs", colors[RED]);
	DrawString(80, height - 20, 500, height + 5, "Nauman Rafay\t24i-2558 ", colors[BLUE_VIOLET]);
		
	//end of game
	}
	if(timee==0 && startdisplay>=1){
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
	}
//	if(timee>0 && startdisplay>=1){
//	timee--;
//	}

	// #----------------- Write your code till here ----------------------------#
	//DO NOT MODIFY THESE LINES
	DrawShooter((width / 2) - 35, 0, bwidth, bheight);
	glutSwapBuffers();
	//DO NOT MODIFY THESE LINES..
}	
	

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
          if(loc1y<435&&loc1x>0){
		loc1x=loc1x-15;
		}
		if(loc1y>400 &&(loc2y<435&&loc2x>0)){
		loc2x=loc2x-15;
		}
		//third shoot
		if(loc2y>400 &&(loc3y<435&&loc3x>0)){
		loc3x=loc3x-15;
		}
		//forth shoot
		if(loc3y>400 &&(loc4y<435&&loc4x>0)){
		loc4x=loc4x-15;
		}
		//fifth shoot
		if(loc4y>400 &&(loc5y<435&&loc5x>0)){
		loc5x=loc5x-15;
		}
		//sixth shoot
		if(loc5y>400&&(loc6y<435&&loc6x>0)){
		loc6x=loc6x-15;
		}
		//seventh shoot
		if(loc6y>400&&(loc7y<435&&loc7x>0)){
		loc7x=loc7x-15;
		}
		//eighth shoot
		if(loc7y>400&&(loc8y<435&&loc8x>0)){
		loc8x=loc8x-15;
		}
		//ninth shoot
		if(loc8y>400&&(loc9y<435&&loc9x>0)){
		loc9x=loc9x-15;
		}
		//tenth shoot
		if(loc9y>400&&(loc10y<435&&loc10x>0)){
		loc10x=loc10x-15;
		}
		//eleventh shoot
		if(loc10y>400&&(loc11y<435&&loc11x>0)){
		loc11x=loc11x-15;
		}
		//twelveth shoot
		if(loc11y>400&&(loc12y<435&&loc12x>0)){
		loc12x=loc12x-15;
		}
		//thirteenth shoot
		if(loc12y>400&&(loc13y<435&&loc13x>0)){
		loc13x=loc13x-15;
		}
		if(loc13y>400&&(loc14y<435&&loc14x>0)){
		loc14x=loc14x-15;
		}
		if(loc14y>400&&(loc15y<435&&loc15x>0)){
		loc15x=loc15x-15;
		}
		if(loc15y>380&&(loc16y<380&&loc16x>0)){
		loc16x=loc16x-15;
		}
		if(loc16y>380&&(loc17y<380&&loc17x>0)){
		loc17x=loc17x-15;
		}
		if(loc17y>380&&(loc18y<380&&loc18x>0)){
		loc18x=loc18x-15;
		}
		if(loc18y>380&&(loc19y<380&&loc19x>0)){
		loc19x=loc19x-15;
		}
		if(loc19y>380&&(loc20y<380&&loc20x>0)){
		loc20x=loc20x-15;
		}
		if(loc20y>380&&(loc21y<380&&loc21x>0)){
		loc21x=loc21x-15;
		}
		if(loc21y>380&&(loc22y<380&&loc22x>0)){
		loc22x=loc22x-15;
		}
		if(loc22y>380&&(loc23y<380&&loc23x>0)){
		loc23x=loc23x-15;
		}
		if(loc23y>380&&(loc24y<380&&loc24x>0)){
		loc24x=loc24x-15;
		}
		if(loc24y>380&&(loc25y<380&&loc25x>0)){
		loc25x=loc25x-15;
		}
		if(loc25y>380&&(loc26y<380&&loc26x>0)){
		loc26x=loc26x-15;
		}
		if(loc26y>380&&(loc27y<380&&loc27x>0)){
		loc27x=loc27x-15;
		}
	}
	else if (key == GLUT_KEY_RIGHT /*GLUT_KEY_RIGHT is constant and contains ASCII for right arrow key*/) {
        
               if(loc1y<435&&loc1x<870){
		loc1x=loc1x+15;
		}
		//second shoot
		if(loc1y>400&&(loc2y<435&&loc2x<870)){
		loc2x=loc2x+15;
		}
		//third shoot
		if(loc2y>400&&(loc3y<435&&loc3x<870)){
		loc3x=loc3x+15;
		}
		//forth shoot
		if(loc3y>400&&(loc4y<435&&loc4x<870)){
		loc4x=loc4x+15;
		}
		//fifth shoot
		if(loc4y>400&&(loc5y<435&&loc5x<870)){
		loc5x=loc5x+15;
		}
		//sixth shoot
		if(loc5y>400&&(loc6y<435&&loc6x<870)){
		loc6x=loc6x+15;
		}
		//seventh shoot
		if(loc6y>400&&(loc7y<435&&loc7x<870)){
		loc7x=loc7x+15;
		}
		//eighth shoot
		if(loc7y>400&&(loc8y<435&&loc8x<870)){
		loc8x=loc8x+15;
		}
		//ninth shoot
		if(loc8y>400&&(loc9y<435&&loc9x<870)){
		loc9x=loc9x+15;
		}
		//tenth shoot
		if(loc9y>400&&(loc10y<435&&loc10x<870)){
		loc10x=loc10x+15;
		}
		//eleventh shoot
		if(loc10y>400&&(loc11y<435&&loc11x<870)){
		loc11x=loc11x+15;
		}
		//twelveth shoot
		if(loc11y>400&&(loc12y<435&&loc12x<870)){
		loc12x=loc12x+15;
		}
		if(loc12y>400&&(loc13y<435&&loc13x<870)){
		loc13x=loc13x+15;
		}
		if(loc13y>400&&(loc14y<435&&loc14x<870)){
		loc14x=loc14x+15;
		}
		if(loc14y>400&&(loc15y<435&&loc15x<870)){
		loc15x=loc15x+15;
		}
		if(loc15y>380&&(loc16y<380&&loc16x<870)){
		loc16x=loc16x+15;
		}
		if(loc16y>380&&(loc17y<380&&loc17x<870)){
		loc17x=loc17x+15;
		}
		if(loc17y>380&&(loc18y<380&&loc18x<870)){
		loc18x=loc18x+15;
		}
		if(loc18y>380&&(loc19y<380&&loc19x<870)){
		loc19x=loc19x+15;
		}
		if(loc19y>380&&(loc20y<380&&loc20x<870)){
		loc20x=loc20x+15;
		}
		if(loc20y>380&&(loc21y<380&&loc21x<870)){
		loc21x=loc21x+15;
		}
		if(loc21y>380&&(loc22y<380&&loc22x<870)){
		loc22x=loc22x+15;
		}
		if(loc22y>380&&(loc23y<380&&loc23x<870)){
		loc23x=loc23x+15;
		}
		if(loc23y>380&&(loc24y<380&&loc24x<870)){
		loc24x=loc24x+15;
		}
		if(loc24y>380&&(loc25y<380&&loc25x<870)){
		loc25x=loc25x+15;
		}
		if(loc25y>380&&(loc26y<380&&loc26x<870)){
		loc26x=loc26x+15;
		}
		if(loc26y>380&&(loc27y<380&&loc27x<870)){
		loc27x=loc27x+15;
		}
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
		
		  clickcount++;
                  startdisplay++;
                  shotx=x-435; 
                  shoty=400+y;
                  bubblemoving=true;
                  dx=shotx-locx;
                  dy=shoty-locy;
              //    Distance=sqrt(dx*dx+dy*dy);
		
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
	glutPostRedisplay();
	glutTimerFunc(1000.0/FPS, Timer, 0);
 }

/*
* our gateway main function
* */
int main(int argc, char*argv[]) {
	InitRandomizer(); 
        InitializeAudio();
        
        Mix_Music* bgMusic=Mix_LoadMUS("background.mp3");
        if(!bgMusic){
        cerr<<"Failed to load background music "<<Mix_GetError()<<endl;
        CleanupAudio();
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
        for(int i=0;i<NUM_COUNT;i++){
        num[i]=rand()%26; 
        }

        
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
        CleanupAudio();

  
	return 1;
}
#endif /* */
