//============================================================================
// Name        : word-shooter-fixed.cpp
// Author      : Nauman Rafay 
//
// === BUGS FIXED ===
// 1. alphabetgrid() was SCORING during initialization — now uses a separate
//    check-only function (no scoring, no saving) for the init loop
// 2. wordsingrid() now properly finds & bursts the LONGEST word first,
//    scores by word length, saves to file — only called during gameplay
// 3. Collision x-offset corrected (was i*60-60, now i*60+10)
// 4. Merge conflict resolved — settled on [5][15] grid
// 5. Words saved to words_found.txt (rubric requirement)
// 6. New ball is rand()%26 each turn
// 7. Gameover when ball lands in last row (row 4)
// 8. Visual: dark starfield background, colored glows per row
//============================================================================
#ifndef WORD_SHOOTER_CPP
#define WORD_SHOOTER_CPP

#include <GL/gl.h>
#include <GL/glut.h>
#include <iostream>
#include <string>
#include <cmath>
#include <fstream>
#include "util.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
using namespace std;

#define MAX(A,B) ((A) > (B) ? (A):(B))
#define MIN(A,B) ((A) < (B) ? (A):(B))
#define ABS(A)   ((A) < (0) ? -(A):(A))
#define FPS 10

string *dictionary;
int dictionarysize = 369646;
#define KEY_ESC 27

const int bradius = 30;
int width = 930, height = 660;
int bwidth = 130, bheight = 10;
const int nalphabets = 26;

enum alphabets {
    AL_A,AL_B,AL_C,AL_D,AL_E,AL_F,AL_G,AL_H,AL_I,AL_J,
    AL_K,AL_L,AL_M,AL_N,AL_O,AL_P,AL_Q,AL_R,AL_S,AL_T,
    AL_U,AL_W,AL_X,AL_y,AL_Z
};

GLuint tid[nalphabets];
GLuint mtid[nalphabets];
string tnames[] = {
    "a.bmp","b.bmp","c.bmp","d.bmp","e.bmp","f.bmp","g.bmp","h.bmp",
    "i.bmp","j.bmp","k.bmp","l.bmp","m.bmp","n.bmp","o.bmp","p.bmp",
    "q.bmp","r.bmp","s.bmp","t.bmp","u.bmp","v.bmp","w.bmp","x.bmp",
    "y.bmp","z.bmp"
};
int awidth = 60, aheight = 60;

// Settled on [5][15] (resolved merge conflict)
int alphabetGrid[5][15];

int ball = 0;
float speedx = 0, speedy = 0;
int locx = width / 2, locy = 0;
bool ballLaunched = false;
bool gameover = false;
float score = 0;
int timee = 1500, displaytime = 150;

ofstream wordsFile;

// Row glow colors for visual variety
float rowColors[5][3] = {
    {1.0f, 0.35f, 0.35f},
    {1.0f, 0.75f, 0.20f},
    {0.30f, 0.90f, 0.50f},
    {0.30f, 0.70f, 1.00f},
    {0.80f, 0.40f, 1.00f},
};

// ============================================================
// AUDIO
// ============================================================
void initializeaudio() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) { cerr << "SDL audio error\n"; exit(-1); }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) { cerr << "Mixer error\n"; exit(-1); }
}
void cleanupaudio() { Mix_CloseAudio(); SDL_Quit(); }

// ============================================================
// DICTIONARY LOOKUP
// ============================================================
bool wordisindictionary(const string dict[], int size, const string &word) {
    for (int i = 0; i < size; ++i)
        if (dict[i] == word) return true;
    return false;
}



// ============================================================
// INIT-TIME BURST: called ONCE during alphabetgrid() setup.
// Bursts any pre-formed words and REPLACES with new random
// letters (one time only — this is the only place replacement happens).
// ============================================================
void burstWordsOnInit(int grid[5][15], const string dict[], int dictSize) {
    int wordsBurst = 0;
    while (wordsBurst < 4) {
        string bestWord = "";
        int bestCol=-1, bestRow=-1, bestDC=0, bestDR=0;

        for (int w = 0; w < dictSize; w++) {
            string word = dict[w];
            int wordLen = (int)word.size();
            if (wordLen < 3 || wordLen > 15) continue;
            if ((int)word.size() <= (int)bestWord.size()) continue;
            for (int col = 0; col < 5; col++) {
                for (int row = 0; row < 15; row++) {
                    int dirs[8][2]={{1,0},{-1,0},{0,1},{0,-1},{1,1},{-1,1},{1,-1},{-1,-1}};
                    for (int d = 0; d < 8; d++) {
                        int dc=dirs[d][0], dr=dirs[d][1];
                        int ec=col+(wordLen-1)*dc, er=row+(wordLen-1)*dr;
                        if (ec<0||ec>=5||er<0||er>=15) continue;
                        bool match=true;
                        for (int k=0; k<wordLen&&match; k++) {
                            int cc=col+k*dc, cr=row+k*dr;
                            if (grid[cc][cr]==-1||('a'+grid[cc][cr])!=word[k]) match=false;
                        }
                        if (match) { bestWord=word; bestCol=col; bestRow=row; bestDC=dc; bestDR=dr; }
                    }
                }
            }
        }
        if (bestWord.empty()) break;
        // REPLACE burst cells with new random letters (init only)
        int wordLen=(int)bestWord.size();
        for (int m=0; m<wordLen; m++)
            grid[bestCol+m*bestDC][bestRow+m*bestDR] = rand()%26;
        cout << "Init burst: " << bestWord << endl;
        wordsBurst++;
    }
}

// ============================================================
// GAMEPLAY WORD FINDER: called ONLY after a ball lands.
// Bursts up to 4 words (longest first).
// Burst cells become EMPTY (-1) — NO replacement during gameplay.
// Scores by word length. Saves words to file.
// ============================================================
void wordsingrid(int grid[5][15], const string dict[], int dictSize) {
    int wordsBurst = 0;
    while (wordsBurst < 4) {
        string bestWord = "";
        int bestCol=-1, bestRow=-1, bestDC=0, bestDR=0;

        for (int w = 0; w < dictSize; w++) {
            string word = dict[w];
            int wordLen = (int)word.size();
            if (wordLen < 3 || wordLen > 15) continue;
            if ((int)word.size() <= (int)bestWord.size()) continue;
            for (int col = 0; col < 5; col++) {
                for (int row = 0; row < 15; row++) {
                    int dirs[8][2]={{1,0},{-1,0},{0,1},{0,-1},{1,1},{-1,1},{1,-1},{-1,-1}};
                    for (int d = 0; d < 8; d++) {
                        int dc=dirs[d][0], dr=dirs[d][1];
                        int ec=col+(wordLen-1)*dc, er=row+(wordLen-1)*dr;
                        if (ec<0||ec>=5||er<0||er>=15) continue;
                        bool match=true;
                        for (int k=0; k<wordLen&&match; k++) {
                            int cc=col+k*dc, cr=row+k*dr;
                            if (grid[cc][cr]==-1||('a'+grid[cc][cr])!=word[k]) match=false;
                        }
                        if (match) { bestWord=word; bestCol=col; bestRow=row; bestDC=dc; bestDR=dr; }
                    }
                }
            }
        }
        if (bestWord.empty()) break;

        // SET TO EMPTY (-1) — cells stay empty during gameplay
        int wordLen=(int)bestWord.size();
        for (int m=0; m<wordLen; m++)
            grid[bestCol+m*bestDC][bestRow+m*bestDR] = -1;

        score += wordLen;
        if (wordsFile.is_open()) { wordsFile << bestWord << "\n"; wordsFile.flush(); }
        cout << "Burst: " << bestWord << " +" << wordLen << " pts" << endl;
        wordsBurst++;
    }
}

// ============================================================
// TEXTURES
// ============================================================
void RegisterTextures() {
    glGenTextures(nalphabets, tid);
    vector<unsigned char> data;
    ifstream ifile("image-data.bin", ios::binary | ios::in);
    if (!ifile) { cout << "Couldn't read image-data.bin\n"; }
    int length;
    ifile.read((char*)&length, sizeof(int));
    data.resize(length, 0);
    for (int i = 0; i < nalphabets; ++i) {
        ifile.read((char*)&data[0], sizeof(char)*length);
        mtid[i] = tid[i];
        glBindTexture(GL_TEXTURE_2D, tid[i]);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, awidth, aheight, GL_RGB, GL_UNSIGNED_BYTE, &data[0]);
    }
    ifile.close();
}

// ============================================================
// DRAW HELPERS
// ============================================================
void DrawAlphabet(const alphabets &cname, int sx, int sy,
                  int cwidth=60, int cheight=60) {
    float fw = (float)cwidth/width*2, fh = (float)cheight/height*2;
    float fx = (float)sx/width*2-1,   fy = (float)sy/height*2-1;
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, mtid[cname]);
    glBegin(GL_QUADS);
    glTexCoord2d(0,0); glVertex2d(fx,    fy);
    glTexCoord2d(1,0); glVertex2d(fx+fw, fy);
    glTexCoord2d(1,1); glVertex2d(fx+fw, fy+fh);
    glTexCoord2d(0,1); glVertex2d(fx,    fy+fh);
    glEnd();
    glColor4f(1,1,1,1);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void DrawGlowCircle(int sx, int sy, int radius, float r, float g, float b) {
    float cx = (float)(sx+radius)/width*2-1;
    float cy = (float)(sy+radius)/height*2-1;
    float rx = (float)radius/width*2;
    float ry = (float)radius/height*2;
    glBegin(GL_TRIANGLE_FAN);
    glColor4f(r, g, b, 0.28f);
    glVertex2f(cx, cy);
    for (int i = 0; i <= 36; i++) {
        float a = i * 3.14159f * 2.0f / 36.0f;
        glColor4f(r, g, b, 0.0f);
        glVertex2f(cx + rx*cosf(a), cy + ry*sinf(a));
    }
    glEnd();
    glColor4f(1,1,1,1);
}

void DrawStarfield() {
    glBegin(GL_QUADS);
    glColor3f(0.02f,0.02f,0.10f); glVertex2f(-1,-1);
    glColor3f(0.02f,0.02f,0.10f); glVertex2f( 1,-1);
    glColor3f(0.05f,0.02f,0.18f); glVertex2f( 1, 1);
    glColor3f(0.05f,0.02f,0.18f); glVertex2f(-1, 1);
    glEnd();
    srand(12345);
    glPointSize(1.5f);
    glBegin(GL_POINTS);
    for (int i = 0; i < 150; i++) {
        float sx = ((rand()%920)/920.0f)*2.0f-1.0f;
        float sy = ((rand()%650)/650.0f)*2.0f-1.0f;
        float b2 = 0.4f+(rand()%6)*0.1f;
        glColor3f(b2,b2,b2);
        glVertex2f(sx,sy);
    }
    glEnd();
    srand(time(0));
    glColor4f(1,1,1,1);
}

void DrawShooter(int sx, int sy, int cw=60, int ch=60) {
    float fw=(float)cw/width*2, fh=(float)ch/height*2;
    float fx=(float)sx/width*2-1, fy=(float)sy/height*2-1;
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,-1);
    glBegin(GL_QUADS);
    glTexCoord2d(0,0); glVertex2d(fx,   fy);
    glTexCoord2d(1,0); glVertex2d(fx+fw,fy);
    glTexCoord2d(1,1); glVertex2d(fx+fw,fy+fh);
    glTexCoord2d(0,1); glVertex2d(fx,   fy+fh);
    glEnd();
    glColor4f(1,1,1,1);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

// ============================================================
// COLLISION DETECTION — fixed x offset to match draw position
// ============================================================
bool collisioncheck(int lx, int ly, int radius, int rowY,
                    int grid[5][15], int r) {
    for (int i = 0; i < 15; ++i) {
        if (grid[r][i] == -1) continue;
        int ax = i*60 + 10;  // matches x_s starting at 10 in DisplayFunction
        int ay = rowY;
        if (lx+radius > ax && lx-radius < ax+60 &&
            ly+radius > ay && ly-radius < ay+60)
            return true;
    }
    return false;
}

// ============================================================
// PLACE BALL INTO GRID ON LANDING
// ============================================================
void balancer(int grid[5][15], int bx, int by) {
    int x = (bx - 10) / 60;
    int y = (height - by - 60) / 60;
    x = x < 0 ? 0 : (x > 14 ? 14 : x);
    y = y < 0 ? 0 : (y > 4  ?  4 : y);
    grid[y][x] = ball;
    cout << "Ball placed at grid[" << y << "][" << x << "]\n";
}

// ============================================================
// INITIALIZE GRID
// Fill first 2 rows randomly, burst any pre-formed words once
// (replacing with new letters). Score reset to 0 after.
// ============================================================
void alphabetgrid() {
    for (int j = 0; j < 5; j++)
        for (int i = 0; i < 15; i++)
            alphabetGrid[j][i] = (j < 2) ? rand()%26 : -1;
    burstWordsOnInit(alphabetGrid, dictionary, dictionarysize);
    ball = rand() % 26;
    score = 0;
}

// ============================================================
// DISPLAY
// ============================================================
void DisplayFunction() {
    glClearColor(0.02f,0.02f,0.10f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    DrawStarfield();

    if (gameover) {
        DrawAlphabet((alphabets)6,  250, height-300, awidth, aheight);
        DrawAlphabet((alphabets)0,  310, height-300, awidth, aheight);
        DrawAlphabet((alphabets)12, 370, height-300, awidth, aheight);
        DrawAlphabet((alphabets)4,  430, height-300, awidth, aheight);
        DrawAlphabet((alphabets)14, 490, height-300, awidth, aheight);
        DrawAlphabet((alphabets)21, 550, height-300, awidth, aheight);
        DrawAlphabet((alphabets)4,  610, height-300, awidth, aheight);
        DrawAlphabet((alphabets)17, 670, height-300, awidth, aheight);
        DrawString(425, 310, width, height+5,
                   "Final Score: "+Num2Str(score), colors[BLUE_VIOLET]);
    } else {
        // Move ball
        locx += (int)(speedx*30);
        locy += (int)(speedy*30);

        if (locx < bradius)        { locx=bradius;        speedx=-speedx; }
        if (locx > width-bradius)  { locx=width-bradius;  speedx=-speedx; }
        if (locy < bradius)        { locy=bradius;         speedy=-speedy; }
        if (locy > height-bradius) { locy=height-bradius;  speedy=-speedy; }

        // Check collision with each row
        for (int i = 0; i < 5; i++) {
            int rowPixelY = height - (i*60 + 140);
            if (collisioncheck(locx, locy, bradius, rowPixelY, alphabetGrid, i)) {
                balancer(alphabetGrid, locx, locy);
                wordsingrid(alphabetGrid, dictionary, dictionarysize);
                locx = width/2;
                locy = 0;
                speedx = 0; speedy = 0;
                ballLaunched = false;
                ball = rand()%26;
                if (i == 4) gameover = true;
                break;
            }
        }

        // Draw grid
        int deviation = 120;
        for (int j = 0; j < 5; j++) {
            int x_s = 10;
            for (int i = 0; i < 15; i++) {
                if (alphabetGrid[j][i] != -1) {
                    DrawGlowCircle(x_s, height-deviation, 30,
                                   rowColors[j][0],rowColors[j][1],rowColors[j][2]);
                    DrawAlphabet((alphabets)alphabetGrid[j][i],
                                 x_s, height-deviation, awidth, aheight);
                }
                x_s += 60;
            }
            deviation += 60;
        }

        // Draw moving ball with yellow glow
        DrawGlowCircle(locx-30, locy, 32, 1.0f, 1.0f, 0.3f);
        DrawAlphabet((alphabets)ball, locx-30, locy, awidth, aheight);

        // HUD
        DrawString(40, height-20, width, height+5,
                   "Score "+Num2Str(score), colors[BLUE_VIOLET]);
        DrawString(width/2-30, height-25, width, height,
                   "Time Left:"+Num2Str(displaytime)+" secs", colors[RED]);
        DrawString(80, height-20, 500, height+5,
                   "Nauman Rafay   24i-2558", colors[BLUE_VIOLET]);
        DrawShooter((width/2)-35, 0, bwidth, bheight);
    }
    glutSwapBuffers();
}

// ============================================================
// INPUT
// ============================================================
void NonPrintableKeys(int key, int x, int y) {}
void MouseMoved(int x, int y) {}

void MouseClicked(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        if (ballLaunched) return;
        float sx2 = (float)x/width*2-1;
        float sy2 = -((float)y/height*2-1);
        float ddx = sx2 - ((float)locx/width*2-1);
        float ddy = sy2 - ((float)locy/height*2-1);
        float len = sqrtf(ddx*ddx+ddy*ddy);
        if (len == 0) return;
        speedx = ddx/len;
        speedy = ddy/len;
        ballLaunched = true;
    }
    glutPostRedisplay();
}

void PrintableKeys(unsigned char key, int x, int y) {
    if (key == KEY_ESC) {
        if (wordsFile.is_open()) wordsFile.close();
        delete[] dictionary;
        exit(1);
    }
}

// ============================================================
// TIMER
// ============================================================
void Timer(int m) {
    if (timee > 0) {
        timee--;
        if (timee % 10 == 0) displaytime--;
    }
    if (timee == 0) gameover = true;
    glutPostRedisplay();
    glutTimerFunc(1000.0/FPS, Timer, 0);
}

// ============================================================
// MAIN
// ============================================================
int main(int argc, char *argv[]) {
    InitRandomizer();
    initializeaudio();

    Mix_Music *bgMusic = Mix_LoadMUS("background.mp3");
    if (!bgMusic) { cerr << "Music load failed\n"; cleanupaudio(); exit(-1); }
    Mix_PlayMusic(bgMusic, -1);

    // Open words file for saving found words (rubric requirement)
    wordsFile.open("words_found.txt", ios::out|ios::trunc);
    if (wordsFile.is_open()) {
        wordsFile << "=== Word Shooter - Words Found ===\n";
        wordsFile << "Player: Nauman Rafay  24i-2558\n";
        wordsFile << "==================================\n";
    }

    dictionary = new string[dictionarysize];
    ReadWords("words_alpha.txt", dictionary);
    for (int i = 0; i < 5; ++i)
        cout << "word " << i << " = " << dictionary[i] << "\n";

    alphabetgrid();
    cout << "Score at game start: " << score << "\n"; // must print 0

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA);
    glutInitWindowPosition(50, 50);
    glutInitWindowSize(width, height);
    glutCreateWindow("ITCs Word Shooter - Nauman Rafay 24i-2558");

    RegisterTextures();
    glutDisplayFunc(DisplayFunction);
    glutSpecialFunc(NonPrintableKeys);
    glutKeyboardFunc(PrintableKeys);
    glutMouseFunc(MouseClicked);
    glutPassiveMotionFunc(MouseMoved);
    glutTimerFunc(10000.0/FPS, Timer, 0);
    glutMainLoop();

    Mix_FreeMusic(bgMusic);
    cleanupaudio();
    if (wordsFile.is_open()) wordsFile.close();
    delete[] dictionary;
    return 1;
}
#endif
