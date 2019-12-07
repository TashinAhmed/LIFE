#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <windows.h>
#include <glut.h>

#define rep_de(i,j,k)  for(int i=j;i<=k;i++)
#define rep_dn(i,j,k)  for(int i=j;i<k;i++)
#define rep_ue(i,j,k)  for(i=j;i<=k;i++)
#define rep_un(i,j,k)  for(i=j;i<k;i++)

/*************************************************************************/
/**NOT DEPLYOED YET :: Option: Represents an option such as "gravity on" */
/*************************************************************************/
//typedef struct{
//    char* label;
//    int value;
//} Option;

/*
Enum in C++ is a data type that contains fixed set of constants.
It can be used for days of the week (SUNDAY, MONDAY, TUESDAY, WEDNESDAY, THURSDAY, FRIDAY and SATURDAY) ,
directions (NORTH, SOUTH, EAST and WEST) etc. The C++ enum constants are static and final implicitly.
*/

//enum option{
//    OPT_GRAV,
//    OPT_INTER_OBJ_GRAV,
//    OPT_INTER_OBJ_BOUNCE,
//    OPT_MOUSE_ATTR,
//    OPT_AIR_RESIST,
//    OPT_WALL_BOUNCE
//};

/**NOT DEPLOYED YET*/
//const Option options[] ={
////    {"[g] gravity", false},
////    {"[i] inter-obj gravity", false},
////    {"[b] inter-obj bounce", false},
////    {"[m] mouse attraction", true},
////    {"[d] air resistance", false},
////    {"[w] wall bounce", false}
//};
//const char* otherCommands[] ={
////    "[r] add object",
////    "[r] remove object",
////    "[c] clear objects"
//};

//const char* HELP_STR = "ARGUMENTS\n\n";

/* ------------------------------------------------------------------- */
/************************************************************************/



/**NON-LOCAL DECLARATIONS*/
int width=1000,height=600,numCols,numRows,pixelsPerSquareSide=10,xMousePos=-1,yMousePos=-1,lastUpdate=0;
char rotateMode='x',running=0,drawNeighborCounts=0;
char** grid;
char** gridBack;
/*************************/

/**FUNCTIONS DECLARATIONS*/
void init();
void display();
void reshape(int w, int h);
void renderScene();
void mouse(int x, int y);
bool processArgs(int argc, char* argv[]);
void drawString(char* str, int x, int y);
void keyPressed(unsigned char key, int x, int y);
void specialKeyPressed(int key, int x, int y);
float maxm(float a, float b){return a>b?a:b;}
float minm(float a, float b){return a<b?a:b;}
void runGameOfLife();
char** initMatrix(int n, int m);
void zeroMatrix(int n, int m, char** a);
void copyMatrix(int n, int m, char** a, char** b);
/**DECLARATIONS ENDS*/


int countNeighbors(int c,int r){
    int count=0,cc,rr,thisC,thisR;
    /**
      X X X
      X O X
      X X X
    */
    rep_ue(cc,-1,1){
        thisC=c+cc;
        if(thisC>=0 && thisC<numCols){
            rep_ue(rr,-1,1){
                if(cc!=0 || rr!=0){
                    thisR=r+rr;
                    if (thisR>=0 && thisR<numRows)
                        count+=grid[thisC][thisR];
                }
            }
        }
    }
    return count;
}
void runGameOfLife(){
    int r,c,neighbors;

//    if(!running)return;
//    int now=glutGet(GLUT_ELAPSED_TIME);
//    int since=now-lastUpdate;
//    if (since<=10)return;
//    lastUpdate=now;

    rep_un(r,0,numRows){
        rep_un(c,0,numCols){
            neighbors=countNeighbors(c,r);
            if (grid[c][r]){                    /** Any alive cell...*/
                if(neighbors<2)
                    gridBack[c][r]=0;           // fewer than two live neighbors dies (underpopulation)
                else if (neighbors<4)           // with three neighbors lives
                    gridBack[c][r]=1;
                else
                    gridBack[c][r]=0;           // with more than three dies (overcrowding)
            }
            else{                               /** Any dead cell...*/
                if(neighbors==3)                // with exactly three neighbors lives (reproduction)
                    gridBack[c][r]=1;
            }
        }
    }
    copyMatrix(numCols, numRows, gridBack, grid);
    glutPostRedisplay();
}
char** initMatrix(int n, int m){                    /** Creates a 2D array of ints and returns a pointer.*/
    char** a=(char**)malloc(sizeof(char*) * n);     // reserve space for n pointers to int arrays
    rep_dn(i,0,n)                                   // make each pointer to a long array point to an array of m elements
        a[i]=(char*)malloc(sizeof(char) * m);
    return a;
}
void zeroMatrix(int n, int m, char** a){            /** Fills matrix a with zeroes */
    rep_dn(i,0,n)
        rep_dn(j,0,m)
            a[i][j]=0;
}
void copyMatrix(int n, int m, char** a, char** b){   /** copies matrix a to matrix b */
    rep_dn(i,0,n)
        rep_dn(j,0,m)
            b[i][j]=a[i][j];
}
void init(){                                /** Set up screen stuff*/
    glEnable(GL_DEPTH_TEST);                // enable z axis
//    glDepthMask(GL_TRUE);
    glShadeModel(GL_SMOOTH);
    glViewport(0, 0, width, height);
    glClearColor(0.0, 0.0, 0.0, 0.0);       // choose background color
    glPointSize(1.0);                       // set point size
    glMatrixMode(GL_PROJECTION);            // load matrix mode
    // load identity matrix
    glLoadIdentity();
    glOrtho(0, width, 0, height, -maxm(width, height), maxm(width, height));
//    glFrustum(0, width, 0, height, -maxm(width,height), maxm(width,height));
    gluLookAt(0, 0, 0, 0, 0, -1, 0, 1, 0);  //eye from top position
}
void display(){         /**GAME STATE*/
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the screen
    //glLoadIdentity();
    //glRotatef(10, 0, 1, 1);
    //glTranslatef(0, 0, -1);
    renderScene();                  //current state of the game
    glutSwapBuffers();              // swap front and back buffers
}
void renderScene(){
    int x1, y1,x2, y2,neighbors;
    double rf,gf,bf,rb,gb,bb;   // foreground r,g,b,background r,g,b
    rep_dn(r,0,numRows){
        rep_dn(c,0,numCols){
            if(grid[c][r])
                rf=gf=bf=1,rb=gb=bb=0;
            else
                rf=gf=bf=0,rb=gb=bb=1;
            glColor3f(rf, gf, bf);
//            glColor3f(1.*r / numRows, 1.*c / numCols, 0);
            x1=c * pixelsPerSquareSide;
            x2=x1 + pixelsPerSquareSide;
            y1=r * pixelsPerSquareSide;
            y2=y1 + pixelsPerSquareSide;
            glBegin(GL_QUADS);
            glVertex2f(x1, y1);
            glVertex2f(x1, y2);
            glVertex2f(x2, y2);
            glVertex2f(x2, y1);
            glEnd();
            if(drawNeighborCounts){
                glColor3f(rb,gb,bb);  // count neighbors
                neighbors=countNeighbors(c,r);
                glRasterPos2f(x1 + 4.5, y1 + 7.5);
                glutBitmapCharacter(GLUT_BITMAP_9_BY_15, neighbors + '0');
            }
        }
    }
    glFlush();
}
void reshape(int w, int h){
    width = w;
    height = h;
    glViewport(0, 0, w, h);
    glutPostRedisplay();
}
void mouse(int x, int y){
    xMousePos = x;
    yMousePos = height - y;
}
void timer(){
    glutPostRedisplay();
}
bool processArgs(int argc, char* argv[]){
    int argNum;
    char* argLabel, * argVal;
    argNum=1;                     // read in all arguments
    while (argNum<argc){
        argLabel=argv[argNum];
        argVal=argv[argNum+1];
        argNum++;
    }
    return true;
}
void drawString(char* str, int x, int y){
    int len;
    glColor3f(0.0, 1.0, 0.0);                               // set color
    glRasterPos2i(x, y);                                    // set position
    len = strlen(str);                                      // draw each character
    rep_dn(i,0,len)
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, str[i]);
}
void specialKeyPressed(int key, int x, int y){
    float rx,ry,rz;
    rx=ry=rz=0;
    switch (rotateMode){
        case 'x':
            rx=1;
            break;
        case 'y':
            ry=1;
            break;
        case 'z':
            rz=1;
            break;
    }
    double angle=5;
    glPopMatrix();
    glTranslatef(width/2,height/2,0);
    switch (key){
        case GLUT_KEY_LEFT:
            glRotatef(-angle,rx,ry,rz);
            break;
        case GLUT_KEY_RIGHT:
            glRotatef(angle,rx,ry,rz);
            break;
        case GLUT_KEY_UP:
            glScalef(1.1,1.1,1.1);
            break;
        case GLUT_KEY_DOWN:
            glScalef(.9,.9,.9);
            break;
    }
    glTranslatef(-width/2,-height/2,0);
    glutPostRedisplay();
    glPushMatrix();

}
void keyPressed(unsigned char key, int x, int y){
    int c, r;
    int turnSquareOn;
    if (key == 'q')     turnSquareOn=1;
    else if (key == 'w')turnSquareOn=0;
    y = height - y;
    switch(key){
        case 'q':
        case 'w':
            c = x / pixelsPerSquareSide;
            r = y / pixelsPerSquareSide;
            grid[c][r] = turnSquareOn;
            glutPostRedisplay();
            break;
        case 's':
            running = !running;
            runGameOfLife();
            glutPostRedisplay();
            break;
        case 'c':
            zeroMatrix(numCols, numRows, grid);
            zeroMatrix(numCols, numRows, gridBack);
            glutPostRedisplay();
            break;
        case 'n':
            drawNeighborCounts = !drawNeighborCounts;
            glutPostRedisplay();
            break;
        case 'r':
            glutPostRedisplay();
            break;
        case 'x':
        case 'y':
        case 'z':
            rotateMode = key;
            break;
    }
}

int main(int argc, char* argv[])
{
    int validArgs=processArgs(argc, argv);
    if (validArgs==false){
        printf("\nQuitting...\n");
        return 0;
    }
    numCols = width / pixelsPerSquareSide;          // initialize both front and back grids
    numRows = height / pixelsPerSquareSide;
    grid = initMatrix(numCols, numRows);
    zeroMatrix(numCols, numRows, grid);
    gridBack = initMatrix(numCols, numRows);
    zeroMatrix(numCols, numRows, gridBack);
    glutInit(&argc, argv);                          /** GLUT STUFF SETUP */
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(width, height);
    glutCreateWindow("Conway's Game of Life");
    glutDisplayFunc(display);                       // register display callback
//    glutMotionFunc(mouse);                        // when mouse moves with button down
    glutPassiveMotionFunc(mouse);                   // when mouse moves without button down
    glutKeyboardFunc(keyPressed);                   // keyboard functions
    glutSpecialFunc(specialKeyPressed);
    //glutIdleFunc(runGameOfLife);                  // redisplay
    glutReshapeFunc(reshape);                       // reshape
    init();
    glutMainLoop();
}
