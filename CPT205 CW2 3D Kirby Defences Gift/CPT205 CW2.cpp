//This coursework makes a small game about Kirby. 
//The story of this game can be viewed as a prequel to coursework1, Kirby encountered an accident on the way to the birthday party.
//In this game the user should control Kirby to denfence the box at the center which is the birthday gift for Kirby's friend.
//This game is a 3D game and it provides many operations for the users, which includes controlling Kirby and the camera, and some additional functions.
//Such as exit the game and cheating mode menu.
//When Kirby is out of health value or Waddle(the monster) get to the center, this game will be immovable and focus on the perspective of failure.
#define FREEGLUT_STATIC
#include <GL/freeglut.h>  
#include <math.h>
#include <cmath>
#include <iostream>
#include <random>
#include <chrono>
#include <stdio.h> 
#include <stdlib.h> 
#include "vector" 

using namespace std;

//There are some constants pre defined in the project for future use.
const float angleRadian = 3.1415926 / 180.0f;
const int textureNumber = 16;

//There are some variables for controlling the whole game.
//Some variables control the camera, some variables control Kirby, some variables control Waddles, some variables control the failure animation and the scene.
float cameraDistance = 0.8f;
float cameraHorizontalAngle = 0.0f;
float cameraVerticalAngle = 0.0f;
float lookatPositionX = 0.0f, lookatPositionY = 0.0f, lookatPositionZ = 0.0f;
float cameraPositionX = lookatPositionX + cameraDistance * cos(cameraVerticalAngle) * sin(cameraHorizontalAngle), cameraPositionY = lookatPositionY + cameraDistance * sin(cameraVerticalAngle), cameraPositionZ = lookatPositionZ + cameraDistance * cos(cameraVerticalAngle) * cos(cameraHorizontalAngle);
float kirbyPosition[3] = { 0.0f, 0.0f, 0.0f };
float kirbyAngle = 0.0f;
int kirbyLife = 3;
float kirbyAnimationTimer = 0.0f;
bool kirbyNormal = true;
float starBulletPosition[3] = { 10.0f, -10.0f, 0.0f };
float starBulletAngle = 0.0f;
bool starBulletMoving = false;
bool moving = false;
int waddleNumber = 0;
float waddlePosition[4][3] = {
    {0.0f, -10.0f, 0.0f},
    {0.0f, -10.0f, 0.0f},
    {0.0f, -10.0f, 0.0f},
    {0.0f, -10.0f, 0.0f},
};
float waddleAngle[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
int waddleState[4] = { 0, 0, 0, 0 };
float waddleAnimationTimer[4] = { 0.0f, 0.0f , 0.0f, 0.0f };
float waddleMovingSpeed = 4.0f;
float timer = 0.0f;
bool gaming = true;
float transitionTimer = 0.0f;
float R = 10 - (10 * transitionTimer);
float sceneDisappear = 1000.0f;
float angleMoving = 0.0f;
float angleWave = 0.0f;

//These are used for textures and lighting.
GLint imageWidth[textureNumber];
GLint imageHeight[textureNumber];
GLint pixelLength[textureNumber];
vector<GLubyte*>p;
GLuint texture[textureNumber];
GLfloat light_x = 0.0, light_y = 2.0, light_z = 0.0;

//This function is reading the image for the texture.
void ReadImage(const char path[256], GLint& imagewidth, GLint& imageheight, GLint& pixellength)
{
    GLubyte* pixeldata;
    FILE* pfile;
    fopen_s(&pfile, path, "rb");
    if (pfile == 0) exit(0);
    fseek(pfile, 0x0012, SEEK_SET);
    fread(&imagewidth, sizeof(imagewidth), 1, pfile);
    fread(&imageheight, sizeof(imageheight), 1, pfile);
    pixellength = imagewidth * 3;
    while (pixellength % 4 != 0) pixellength++;
    pixellength *= imageheight;
    pixeldata = (GLubyte*)malloc(pixellength);
    if (pixeldata == 0) exit(0);
    fseek(pfile, 54, SEEK_SET);
    fread(pixeldata, pixellength, 1, pfile);
    p.push_back(pixeldata);
    fclose(pfile);
}

//This function is used for some initializations. Such as enabling depth testing, texture and lighting.
//This function call "ReadImage" for the textures and set the texture objects.
//It also set the light.
void myinit(void) {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_TEXTURE_2D);
    ReadImage("kirbyFace.bmp", imageWidth[0], imageHeight[0], pixelLength[0]);
    ReadImage("kirbyFoot.bmp", imageWidth[1], imageHeight[1], pixelLength[1]);
    ReadImage("kirbyHand.bmp", imageWidth[2], imageHeight[2], pixelLength[2]);
    ReadImage("waddleFace.bmp", imageWidth[3], imageHeight[3], pixelLength[3]);
    ReadImage("waddleFoot.bmp", imageWidth[4], imageHeight[4], pixelLength[4]);
    ReadImage("waddleHand.bmp", imageWidth[5], imageHeight[5], pixelLength[5]);
    ReadImage("star.bmp", imageWidth[6], imageHeight[6], pixelLength[6]);
    ReadImage("water.bmp", imageWidth[7], imageHeight[7], pixelLength[7]);
    ReadImage("leftSky.bmp", imageWidth[8], imageHeight[8], pixelLength[8]);
    ReadImage("frontSky.bmp", imageWidth[9], imageHeight[9], pixelLength[9]);
    ReadImage("rightSky.bmp", imageWidth[10], imageHeight[10], pixelLength[10]);
    ReadImage("backSky.bmp", imageWidth[11], imageHeight[11], pixelLength[11]);
    ReadImage("topSky.bmp", imageWidth[12], imageHeight[12], pixelLength[12]);
    ReadImage("box.bmp", imageWidth[13], imageHeight[13], pixelLength[13]);
    ReadImage("specialBox.bmp", imageWidth[14], imageHeight[14], pixelLength[14]);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(textureNumber, &texture[0]);
    for (int i = 0; i < 15; i++)
    {
        glBindTexture(GL_TEXTURE_2D, texture[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, 3, imageWidth[i], imageHeight[i], 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, p[i]);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    GLfloat mat_shininess[] = { 50.0 };
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    GLfloat light_position[] = { light_x, light_y, light_z, 0.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
}

//This function is used for making a skyBox for this game.
void skyBox()
{
    //Water
    glBindTexture(GL_TEXTURE_2D, texture[7]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-10.0, -0.9, 10.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-10.0, -0.9, -10.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(10.0, -0.9, -10.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(10.0, -0.9, 10.0);
    glEnd();
    //LeftSky
    glBindTexture(GL_TEXTURE_2D, texture[8]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-10.0, -2, 10.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-10.0, 18.0, 10.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(-10.0, 18.0, -10.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(-10.0, -2.0, -10.0);
    glEnd();
    //FrontSky
    glBindTexture(GL_TEXTURE_2D, texture[9]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-10.0, -2.0, -10.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-10.0, 18.0, -10.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(10.0, 18.0, -10.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(10.0, -2.0, -10.0);
    glEnd();
    //RightSky
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(10.0, -2.0, -10.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(10.0, 18.0, -10.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(10.0, 18.0, 10.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(10.0, -2.0, 10.0);
    glEnd();
    //BackSky
    glBindTexture(GL_TEXTURE_2D, texture[11]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(10.0, -2.0, 10.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(10.0, 18.0, 10.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(-10.0, 18.0, 10.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(-10.0, -2.0, 10.0);
    glEnd();
    //TopSky
    glBindTexture(GL_TEXTURE_2D, texture[12]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-10.0, 18.0, -10.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-10.0, 18.0, 10.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(10.0, 18.0, 10.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(10.0, 18.0, -10.0);
    glEnd();
}

//This function is the animation for failure.
//When the game is over this function will be called and shows a star viewport for the failure reason
//It will be created outside of the viewport first and be shown when the game is over.
void scene()
{
    R = 10 - (10 * transitionTimer);
    glPushMatrix();
    glTranslatef(cameraPositionX + 0.0f, cameraPositionY - 0.4f, cameraPositionZ - 0.4f);
    glRotatef(-45.0f, 1.0f, 0.0f, 0.0f);
    for (int i = 0; i < 5; i++)
    {
        glRotatef(72.0, 0.0, 0.0, 1.0);
        glBegin(GL_POLYGON);
        glVertex3f(0, R - sceneDisappear, 0);
        glVertex3f(0, 10 - sceneDisappear, 0);
        glVertex3f(-10, 10 - sceneDisappear, 0);
        glVertex3f(-10, R * cosf(72.0f * angleRadian) - sceneDisappear, 0);
        glVertex3f(-R * cosf(72.0f * angleRadian) / cosf(36.0f * angleRadian) * sinf(36.0f * angleRadian), R * cosf(72.0f * angleRadian) - sceneDisappear, 0);
        glEnd();
    }
    glPopMatrix();
}

//The game will end in two ways: one is that Kirby has no haleth value, and the other is that Waddle has reached the center.
//This function is used for the first way.
//It will change the viewport to Kirby.
void kirbyDieEnd()
{
    cameraPositionX = kirbyPosition[0];
    cameraPositionY = kirbyPosition[1] + 1.0f;
    cameraPositionZ = kirbyPosition[2] + 1.0f;
    lookatPositionX = kirbyPosition[0];
    lookatPositionY = kirbyPosition[1];
    lookatPositionZ = kirbyPosition[2];
}

//This function is used for the second way.
//It will change the viewport to the Waddle winner(similiar to Plants V.S. Zombies).
void waddleWinEnd()
{
    cameraPositionX = 0;
    cameraPositionY = 1.0f;
    cameraPositionZ = 1.0f;
    lookatPositionX = 0.0f;
    lookatPositionY = 0.0f;
    lookatPositionZ = 0.0f;
}

//This function is used for game over.
//It will stop all the moving and focus on the the perspective of failure.
//It will call the corresponding ending method based on the reason for the end of the game.
void gameOver()
{
    gaming = false;
    sceneDisappear = 0.0f;
    waddleNumber += 4;
    for (int i = 0; i < 4; i++)
    {
        waddleState[i] = 3;
    }
    if (kirbyLife <= 0)
    {
        kirbyDieEnd();
    }
    else
    {
        waddleWinEnd();
    }
}

//This function creates a star.
//It will be called for Kirby, Waddle and the star bullet.
//"R" is the radius of the star,"height: is the thickness of the star.
void star(float R, float height)
{
    glBindTexture(GL_TEXTURE_2D, texture[6]);
    float r = 1.8 * R / (sinf(36.0f * angleRadian) * cosf(18.0f * angleRadian) / sinf(18.0f * angleRadian) + cosf(36.0f * angleRadian));
    glPushMatrix();
    for (int i = 0; i < 5; i++)
    {
        glRotatef(72.0, 0.0, 1.0, 0.0);
        glBegin(GL_TRIANGLES);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, height, 0.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, 0.0f, R);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(r * sin(36.0f * angleRadian), 0.0f, r * cos(36.0f * angleRadian));
        glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, height, 0.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, 0.0f, R);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-r * sin(36.0f * angleRadian), 0.0f, r * cos(36.0f * angleRadian));
        glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, -height, 0.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, 0.0f, R);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(r * sin(36.0f * angleRadian), 0.0f, r * cos(36.0f * angleRadian));
        glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, -height, 0.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, 0.0f, R);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-r * sin(36.0f * angleRadian), 0.0f, r * cos(36.0f * angleRadian));
        glEnd();
    }
    glPopMatrix();
}

//This function creates the body of Kirby.
//It will be called for creating Kirby.
void kirbyBody()
{
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    GLUquadric* quadricObj = gluNewQuadric();
    gluQuadricTexture(quadricObj, GL_TRUE);
    gluSphere(quadricObj, 0.16, 360, 360);
    gluDeleteQuadric(quadricObj);
}

//This function creates the foot of Kirby.
//It will be called for creating Kirby.
void kirbyFoot()
{
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    GLUquadric* quadricObj = gluNewQuadric();
    gluQuadricTexture(quadricObj, GL_TRUE);
    glPushMatrix();
    glScalef(0.8f, 0.5f, 1.0f);
    gluSphere(quadricObj, 0.12, 360, 360);
    glPopMatrix();
    gluDeleteQuadric(quadricObj);
}

//This function creates the hand of Kirby.
//It will be called for creating Kirby.
void kirbyHand()
{
    glBindTexture(GL_TEXTURE_2D, texture[2]);
    GLUquadric* quadricObj = gluNewQuadric();
    gluQuadricTexture(quadricObj, GL_TRUE);
    glPushMatrix();
    glScalef(1.0, 0.6f, 0.6f);
    gluSphere(quadricObj, 0.08, 360, 360);
    glPopMatrix();
    gluDeleteQuadric(quadricObj);
}

//This function uses hierarchical modeling to creat Kirby with the previous functions.
void kirby() {
    glPushMatrix();

    glTranslatef(kirbyPosition[0], kirbyPosition[1], kirbyPosition[2]);
    glRotatef(-kirbyAngle, 0.0, 1.0, 0.0);
    glRotatef(angleMoving, 1.0, 0.0, 0.0);

    glPushMatrix();
    glRotatef(-90.0, 1.0, 0.0, 0.0);
    kirbyBody();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.16, 0.0, 0.0);
    kirbyHand();
    glTranslatef(-0.06, -0.16, 0.0);
    kirbyFoot();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.16, 0.0, 0.0);
    kirbyHand();
    glTranslatef(0.06, -0.16, 0.0);
    kirbyFoot();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0, -0.28, -0.06);
    star(0.36f, 0.1f);
    glPopMatrix();

    glPopMatrix();
}

//This function is used for the animation of kirby hurt.
//When Kirby collides with Waddle, Kirby will bleed and be knocked unconscious for a while.
void kirbyHurt()
{
    kirbyAngle += 5.0f;
    kirbyAnimationTimer += 0.2f;
}

//This function will check the state of Kirby.
//If Kirby is hurt, Kirby will be unconscious for a while.
//If Kirby is out of health value, it will call "gameOver" to end the game.
void kirbyStateCheck()
{
    if (kirbyAnimationTimer >= 10.0f)
    {
        kirbyNormal = true;
    }
    if (kirbyLife <= 0)
    {
        gameOver();
    }
}

//This function creates the body of Waddle.
//It will be called for creating Waddle.
void waddleBody()
{
    glBindTexture(GL_TEXTURE_2D, texture[3]);
    GLUquadric* quadricObj = gluNewQuadric();
    gluQuadricTexture(quadricObj, GL_TRUE);
    gluSphere(quadricObj, 0.16, 360, 360);
    gluDeleteQuadric(quadricObj);
}

//This function creates the foot of Waddle.
//It will be called for creating Waddle.
void waddleFoot()
{
    glBindTexture(GL_TEXTURE_2D, texture[4]);
    GLUquadric* quadricObj = gluNewQuadric();
    gluQuadricTexture(quadricObj, GL_TRUE);
    glPushMatrix();
    glScalef(0.8f, 0.5f, 1.0f);
    gluSphere(quadricObj, 0.12, 360, 360);
    glPopMatrix();
    gluDeleteQuadric(quadricObj);
}

//This function creates the hand of Waddle.
//It will be called for creating Waddle.
void waddleHand()
{
    glBindTexture(GL_TEXTURE_2D, texture[5]);
    GLUquadric* quadricObj = gluNewQuadric();
    gluQuadricTexture(quadricObj, GL_TRUE);
    glPushMatrix();
    glScalef(1.0, 0.6f, 0.6f);
    gluSphere(quadricObj, 0.08, 360, 360);
    glPopMatrix();
    gluDeleteQuadric(quadricObj);
}

//This function uses hierarchical modeling to creat Waddle with the previous functions.
//Unlike the previous method for creating Kirby, it rotates for showing Waddle is moving.
//"x","y","z" defines the position of Waddle.
//"angle" keeps Waddle can face to the center.
void waddle(float x, float y, float z, float angle) {
    glPushMatrix();

    glTranslated(x, y, z);
    glRotatef(-angle, 0, 1, 0);

    glRotatef(20.0, 1.0, 0.0, 0.0);

    glPushMatrix();
    glRotatef(-90.0, 1.0, 0.0, 0.0);
    waddleBody();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.16, 0.0, 0.0);
    waddleHand();
    glTranslatef(-0.06, -0.16, 0.0);
    waddleFoot();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.16, 0.0, 0.0);
    waddleHand();
    glTranslatef(0.06, -0.16, 0.0);
    waddleFoot();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0, -0.28, -0.06);
    star(0.36f, 0.1f);
    glPopMatrix();

    glPopMatrix();
}

//All waddles will be created in advance, but unseen because of the viewport.
//This function will find the first waiting Waddle which will be shown in the scene for its mission when waddleCreater is calling this function.
//The "waddleState" records the state of all the Waddles, 0 means it is dead, 1 means it is alive, 2 means it is dying.
//So if a new Waddle shoulbd be created a dead waddle index is needed.
int findAvaliableWaddle()
{
    for (int i = 0; i < 4; i++)
    {
        if (waddleState[i] == 0)
        {
            return i;
        }
    }
    return -1;
}

//This function will find the alive waddle and used for kill it for cheating mode.
//It will be called in the cheating mode menu.
int findLiveWaddle() {
    for (int i = 0; i < 4; i++)
    {
        if (waddleState[i] == 1)
        {
            return i;
        }
    }
    return -1;
}

//This function is used for creating a new Waddle or make a dead Waddle alive.
//It will call "findAvaliableWaddle" for creating an alive Waddle.
//"distance" defines the distance between Waddle and the center.
void waddleCreater(float distance)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(-180, 180);
    int waddleIndex = findAvaliableWaddle();
    if (waddleIndex != -1)
    {
        waddleAngle[waddleIndex] = distr(gen);
        waddlePosition[waddleIndex][1] = 0.0f;
        waddlePosition[waddleIndex][2] = -(distance * cos(-waddleAngle[waddleIndex] * angleRadian));
        waddlePosition[waddleIndex][0] = -(distance * sin(-waddleAngle[waddleIndex] * angleRadian));
        waddleState[waddleIndex] = 1;
    }
}

//This function will make Waddles moving to the center.
void waddleMoving()
{
    for (int i = 0; i < 4; i++)
    {
        if (waddleState[i] == 1)
        {
            waddlePosition[i][2] += 0.005 * waddleMovingSpeed * cos(-waddleAngle[i] * angleRadian);
            waddlePosition[i][0] += 0.005 * waddleMovingSpeed * sin(-waddleAngle[i] * angleRadian);
        }
    }
}

//This function will make Waddles moving faster.
//It will be called for the cheating mode menu.
void waddleMovingSpeedUp()
{
    if (waddleMovingSpeed < 20)
    {
        waddleMovingSpeed += 1;
    }
}

//This function will make Waddles moving slower.
//It will be called for the cheating mode menu.
void waddleMovingSpeedDown()
{
    if (waddleMovingSpeed > 1)
    {
        waddleMovingSpeed -= 1;
    }
}

//This function controls Waddle disappear animation
//It will be called when this Waddle is decided to be delete in "waddleDelete"
//"i" is the index of Waddle should be disappeared.
void waddleDisappear(int i)
{
    waddleAngle[i] += 5.0f;
    waddleAnimationTimer[i] += 0.2f;
    if (waddleAnimationTimer[i] >= 10.0f)
    {
        waddleState[i] = 0;
        waddlePosition[i][0] = 0.0f;
        waddlePosition[i][1] = -10.0f;
        waddlePosition[i][2] = 0.0f;
        waddleAngle[i] = 0.0f;
        waddleNumber -= 1;
    }
}

//This function will decide which waddle will be deleted and the reason for deleting it.
//If it is attacked by the bullet star it will disappear normally.
//If it is collision with Kirby, it will make kirby hurt with losing one health value.
//If it is arriving to the center, it will call "gameOver" to end the game.
void waddleDelete()
{
    for (int i = 0; i < 4; i++)
    {
        float starDistanceSquare = (waddlePosition[i][0] - starBulletPosition[0]) * (waddlePosition[i][0] - starBulletPosition[0]) + (waddlePosition[i][2] - starBulletPosition[2]) * (waddlePosition[i][2] - starBulletPosition[2]);
        float kirbyDistanceSquare = (waddlePosition[i][0] - kirbyPosition[0]) * (waddlePosition[i][0] - kirbyPosition[0]) + (waddlePosition[i][2] - kirbyPosition[2]) * (waddlePosition[i][2] - kirbyPosition[2]);
        float winDistanceSquare = waddlePosition[i][0] * waddlePosition[i][0] + waddlePosition[i][2] * waddlePosition[i][2];
        if (winDistanceSquare <= 0.01 && waddleState[i] == 1)
        {
            gameOver();
        }
        if (starDistanceSquare <= 0.08 && waddleState[i] == 1)
        {
            waddleAnimationTimer[i] = 0.0f;
            waddleState[i] = 2;

        }
        if (kirbyDistanceSquare <= 0.08 && waddleState[i] == 1)
        {
            waddleAnimationTimer[i] = 0.0f;
            waddleState[i] = 2;
            kirbyLife -= 1;
            kirbyAnimationTimer = 0.0f;
            kirbyNormal = false;
        }
        if (waddleState[i] == 2)
        {
            waddleDisappear(i);
        }
    }
}

//This function creates a star bullet.
void starBullet()
{
    glPushMatrix();
    glTranslatef(starBulletPosition[0], starBulletPosition[1], starBulletPosition[2]);
    glRotatef(-starBulletAngle, 0.0, 1.0, 0.0);
    glRotatef(-90.0, 1.0, 0, 0);
    star(0.18f, 0.05f);
    glPopMatrix();
}

//This function will make star bullet start moving from the position of Kirby.
//It will be called with the user press "E" or "Space".
void starAttack()
{
    starBulletMoving = true;
    for (int i = 0; i < 3; i++)
    {
        starBulletPosition[i] = kirbyPosition[i];
    }
    starBulletAngle = kirbyAngle;
}

//This function is used for making the bullet dissapear and avoid the error of distance between the star bullet with Waddle.
void starDisappear()
{
    starBulletMoving = false;
    starBulletPosition[1] = -10.0f;
}

//This function controls the moving of star bullet.
//It will call "starDisappear" for avoiding the bullet out of range.
void starMoving()
{
    if (gaming)
    {
        starBulletPosition[2] += 0.1 * cos(-starBulletAngle * angleRadian);
        starBulletPosition[0] += 0.1 * sin(-starBulletAngle * angleRadian);
        if (std::abs(starBulletPosition[2]) > 8.5f)
        {
            starDisappear();
        }
        if (std::abs(starBulletPosition[0]) > 8.5f)
        {
            starDisappear();
        }
    }
}

//It creats the box for decorating the game scene.
// "x","y","z" defines the postion of the box.
// "size" difines the half length of the box.
//It will be called in "boxScene".
void box(float x, float y, float z, float size)
{
    glBindTexture(GL_TEXTURE_2D, texture[13]);
    glBegin(GL_QUADS);
    //front
    glTexCoord2f(0.0, 0.0); glVertex3f(x - size, y - size, z + size);
    glTexCoord2f(0.0, 1.0); glVertex3f(x - size, y + size, z + size);
    glTexCoord2f(1.0, 1.0); glVertex3f(x + size, y + size, z + size);
    glTexCoord2f(1.0, 0.0); glVertex3f(x + size, y - size, z + size);
    //back
    glTexCoord2f(0.0, 0.0); glVertex3f(x + size, y - size, z - size);
    glTexCoord2f(0.0, 1.0); glVertex3f(x + size, y + size, z - size);
    glTexCoord2f(1.0, 1.0); glVertex3f(x - size, y + size, z - size);
    glTexCoord2f(1.0, 0.0); glVertex3f(x - size, y - size, z - size);
    //left
    glTexCoord2f(0.0, 0.0); glVertex3f(x + size, y - size, z + size);
    glTexCoord2f(0.0, 1.0); glVertex3f(x + size, y + size, z + size);
    glTexCoord2f(1.0, 1.0); glVertex3f(x + size, y + size, z - size);
    glTexCoord2f(1.0, 0.0); glVertex3f(x + size, y - size, z - size);
    //right
    glTexCoord2f(0.0, 0.0); glVertex3f(x - size, y - size, z - size);
    glTexCoord2f(0.0, 1.0); glVertex3f(x - size, y + size, z - size);
    glTexCoord2f(1.0, 1.0); glVertex3f(x - size, y + size, z + size);
    glTexCoord2f(1.0, 0.0); glVertex3f(x - size, y - size, z + size);
    //top
    glTexCoord2f(0.0, 0.0); glVertex3f(x - size, y + size, z + size);
    glTexCoord2f(0.0, 1.0); glVertex3f(x - size, y + size, z - size);
    glTexCoord2f(1.0, 1.0); glVertex3f(x + size, y + size, z - size);
    glTexCoord2f(1.0, 0.0); glVertex3f(x + size, y + size, z + size);
    //bottom
    glTexCoord2f(0.0, 0.0); glVertex3f(x - size, y - size, z - size);
    glTexCoord2f(0.0, 1.0); glVertex3f(x - size, y - size, z + size);
    glTexCoord2f(1.0, 1.0); glVertex3f(x + size, y - size, z + size);
    glTexCoord2f(1.0, 0.0); glVertex3f(x + size, y - size, z - size);
    glEnd();
}

//It creates the defence target, which is the gift for friend's birthday.
//"y" controls its position.
//"size" difines the half length of the box.
void specialBox(float y, float size)
{
    glBindTexture(GL_TEXTURE_2D, texture[14]);
    glBegin(GL_QUADS);
    //front
    glTexCoord2f(0.0, 0.0); glVertex3f(0 - size, y - size, 0 + size);
    glTexCoord2f(0.0, 1.0); glVertex3f(0 - size, y + size, 0 + size);
    glTexCoord2f(1.0, 1.0); glVertex3f(0 + size, y + size, 0 + size);
    glTexCoord2f(1.0, 0.0); glVertex3f(0 + size, y - size, 0 + size);
    //back
    glTexCoord2f(0.0, 0.0); glVertex3f(0 + size, y - size, 0 - size);
    glTexCoord2f(0.0, 1.0); glVertex3f(0 + size, y + size, 0 - size);
    glTexCoord2f(1.0, 1.0); glVertex3f(0 - size, y + size, 0 - size);
    glTexCoord2f(1.0, 0.0); glVertex3f(0 - size, y - size, 0 - size);
    //left
    glTexCoord2f(0.0, 0.0); glVertex3f(0 + size, y - size, 0 + size);
    glTexCoord2f(0.0, 1.0); glVertex3f(0 + size, y + size, 0 + size);
    glTexCoord2f(1.0, 1.0); glVertex3f(0 + size, y + size, 0 - size);
    glTexCoord2f(1.0, 0.0); glVertex3f(0 + size, y - size, 0 - size);
    //right
    glTexCoord2f(0.0, 0.0); glVertex3f(0 - size, y - size, 0 - size);
    glTexCoord2f(0.0, 1.0); glVertex3f(0 - size, y + size, 0 - size);
    glTexCoord2f(1.0, 1.0); glVertex3f(0 - size, y + size, 0 + size);
    glTexCoord2f(1.0, 0.0); glVertex3f(0 - size, y - size, 0 + size);
    //top
    glTexCoord2f(0.0, 0.0); glVertex3f(0 - size, y + size, 0 + size);
    glTexCoord2f(0.0, 1.0); glVertex3f(0 - size, y + size, 0 - size);
    glTexCoord2f(1.0, 1.0); glVertex3f(0 + size, y + size, 0 - size);
    glTexCoord2f(1.0, 0.0); glVertex3f(0 + size, y + size, 0 + size);
    //bottom
    glTexCoord2f(0.0, 0.0); glVertex3f(0 - size, y - size, 0 - size);
    glTexCoord2f(0.0, 1.0); glVertex3f(0 - size, y - size, 0 + size);
    glTexCoord2f(1.0, 1.0); glVertex3f(0 + size, y - size, 0 + size);
    glTexCoord2f(1.0, 0.0); glVertex3f(0 + size, y - size, 0 - size);
    glEnd();
}

//It creates some box with "box" functions to decorate the game scene.
//"y" will combines with the global variable "angleWave" to control the y value for the box to make the box up and down like function of the waves.
void boxScene(float y)
{
    box(3.0, 0.1 * cos(y) - 1.0, 7.0, 0.15);
    box(2.0, 0.2 * cos(y) - 1.0, 3.0, 0.15);
    box(-5.0, 0.2 * cos(y) - 1.0, -6.5, 0.15);
    box(-8.0, 0.1 * cos(y) - 1.0, 1.0, 0.15);
    glRotatef(30.0, 0.0, 1.0, 0.0);
    box(-1.0, 0.1 * cos(y) - 1.0, 7.0, 0.15);
    box(7.0, 0.2 * cos(y) - 1.0, -1.0, 0.15);
    box(4.0, 0.1 * cos(y) - 1.0, 6.0, 0.15);
    box(6.0, 0.2 * cos(y) - 1.0, -3.2, 0.15);
    glRotatef(15.0, 0.0, 1.0, 0.0);
    box(-1.4, 0.15 * cos(y) - 1.0, 2.0, 0.15);
    box(8.8, 0.25 * cos(y) - 1.0, -0.7, 0.15);
    box(0.9, 0.1 * cos(y) - 1.0, -6.0, 0.15);
    box(1.8, 0.2 * cos(y) - 1.0, -3.2, 0.15);
    box(-3.0, 0.15 * cos(y) - 1.0, 5.0, 0.15);
    box(-4.0, 0.25 * cos(y) - 1.0, 3.0, 0.15);
    box(-5.0, 0.1 * cos(y) - 1.0, 4.1, 0.15);
    box(5.5, 0.2 * cos(y) - 1.0, 2.9, 0.15);
    box(-2.7, 0.1 * cos(y) - 1.0, 1.1, 0.15);
    box(-1.8, 0.2 * cos(y) - 1.0, -3.4, 0.15);
    glRotatef(-45.0, 0.0, 1.0, 0.0);
}

//This function will add "times" health value for Kirby.
//It will be called in the cheating mode menu.
void lifeCheat(int times)
{
    for (int i = 0; i < times; i++)
    {
        kirbyLife += 1;
    }
}

//This function will kill one alive Waddle.
//It calls "findLiveWaddle" for finding an alive one.
//It will be called in the cheating mode menu.
void killCheat()
{
    int killIndex = findLiveWaddle();
    if (killIndex != -1)
    {
        waddleAnimationTimer[killIndex] = 0.0f;
        waddleState[killIndex] = 2;
        waddleDisappear(killIndex);
    }
}

//This function will kill all alive Waddle.
//It calls "killCheat" four times to kill all alive Waddle.
//It will be called in the cheating mode menu.
void killAllCheat()
{
    for (int i = 0; i < 4; i++)
    {
        killCheat();
    }
}

//Update some value for the whole game.
//It also constantly checks some values that control the game's operation, in order to control the game to develop in the prescribed logical direction.
void update(int value)
{
    angleWave += 0.01 * angleRadian;
    kirbyStateCheck();
    waddleMoving();
    waddleDelete();
    if (waddleNumber < 5 && timer >= 20.0)
    {
        waddleNumber += 1;
        waddleCreater(8.0f);
        timer = 0;
    }
    timer += 0.2;
    if (kirbyNormal != true)
    {
        kirbyHurt();
    }
    if (starBulletMoving == true)
    {
        starMoving();
    }
    glutPostRedisplay();
    glutTimerFunc(1000 / 60, update, 0);
    if (gaming != true) {
        transitionTimer += 0.01f;
    }
    if (gaming)
    {
        cameraPositionX = lookatPositionX + cameraDistance * cos(cameraVerticalAngle) * sin(cameraHorizontalAngle), cameraPositionY = lookatPositionY + cameraDistance * sin(cameraVerticalAngle), cameraPositionZ = lookatPositionZ + cameraDistance * cos(cameraVerticalAngle) * cos(cameraHorizontalAngle);
        lookatPositionX = kirbyPosition[0];
        lookatPositionY = kirbyPosition[1];
        lookatPositionZ = kirbyPosition[2];
    }
}

//This function defines the content that needs to be drawn for each frame.
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(cameraPositionX, cameraPositionY, cameraPositionZ, lookatPositionX, lookatPositionY, lookatPositionZ, 0.0, 1.0, 0.0);
    kirby();
    skyBox();
    starBullet();
    waddle(waddlePosition[0][0], waddlePosition[0][1], waddlePosition[0][2], waddleAngle[0]);
    waddle(waddlePosition[1][0], waddlePosition[1][1], waddlePosition[1][2], waddleAngle[1]);
    waddle(waddlePosition[2][0], waddlePosition[2][1], waddlePosition[2][2], waddleAngle[2]);
    waddle(waddlePosition[3][0], waddlePosition[3][1], waddlePosition[3][2], waddleAngle[3]);
    scene();
    specialBox(-0.7, 0.15);
    boxScene(angleWave * angleRadian);
    glutSwapBuffers();
}

//This function is used to update the viewport and projection.
void reshape(GLsizei w, GLsizei h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluLookAt(cameraPositionX, cameraPositionY, cameraPositionZ, lookatPositionX, lookatPositionY, lookatPositionZ, 0.0, 1.0, 0.0);
    gluPerspective(90.0, 4.0f / 3.0f, 0.01f, 1000.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}

//Following two functions provides the menu for the user.
//In the menu the user can get the help for how to play the game.
//In the menu the user can cheat with the cheat operations submenu for the cheating modes.
//In the menu the user can quit the game directly.
//This function will react the operation with the corresponding "num" value.
void menu(int num) {
    switch (num) {
    case 1:
        exit(0);
        break;
    case 2:
        killCheat();
        break;
    case 3:
        killAllCheat();
        break;
    case 4:
        if (kirbyLife < 3)
        {
            lifeCheat(3 - kirbyLife);
        }
        break;
    case 5:
        lifeCheat(10);
        break;
    case 6:
        lifeCheat(100);
        break;
    case 7:
        lifeCheat(999);
        break;
    case 8:
        break;
    case 9:
        waddleMovingSpeedUp();
        break;
    case 10:
        waddleMovingSpeedDown();
        break;
    }
}

//This function initials and creates the menus.
void initMenu() {
    int help = glutCreateMenu(menu);
    glutAddMenuEntry("w: Kirby go staight", 8);
    glutAddMenuEntry("s: Kirby go back", 8);
    glutAddMenuEntry("a: Kirby turn left", 8);
    glutAddMenuEntry("d: kirby turn right", 8);
    glutAddMenuEntry("Using direction keys controls the camera", 8);
    glutAddMenuEntry("e / space: Launch stars", 8);
    glutAddMenuEntry("esc: Quit the game", 8);

    int difficultyCheatOptions = glutCreateMenu(menu);
    glutAddMenuEntry("Difficulty up", 9);
    glutAddMenuEntry("Difficulty down", 10);

    int killCheatOptions = glutCreateMenu(menu);
    glutAddMenuEntry("Kill one", 2);
    glutAddMenuEntry("Kill all", 3);

    int lifeCheatOptions = glutCreateMenu(menu);
    glutAddMenuEntry("Life max", 4);
    glutAddMenuEntry("Life +10", 5);
    glutAddMenuEntry("Life +100", 6);
    glutAddMenuEntry("Life +999", 7);

    int cheatOptions = glutCreateMenu(menu);
    glutAddSubMenu("Kill cheat options:", killCheatOptions);
    glutAddSubMenu("Life cheat options:", lifeCheatOptions);
    glutAddSubMenu("Difficulty cheat options:", difficultyCheatOptions);

    int menuID = glutCreateMenu(menu);
    glutAddSubMenu("Operation instructions:", help);
    glutAddSubMenu("Cheat options:", cheatOptions);
    glutAddMenuEntry("Quit the game(Esc)", 1);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

//This function controls the interaction with special keys in the keyboard.
void specialKeys(int key, int x, int y)
{
    //The up arrow key can move the camera upwards.
    if (key == GLUT_KEY_UP && gaming == true && cameraVerticalAngle <= 89.0f * angleRadian)
    {
        cameraVerticalAngle += 1.0f * angleRadian;
        cameraPositionX = cameraDistance * cos(cameraVerticalAngle) * sin(cameraHorizontalAngle), cameraPositionY = cameraDistance * sin(cameraVerticalAngle), cameraPositionZ = cameraDistance * cos(cameraVerticalAngle) * cos(cameraHorizontalAngle);
    }
    //The down arrow key can move the camera downwards
    if (key == GLUT_KEY_DOWN && gaming == true && cameraVerticalAngle >= -20.0f * angleRadian)
    {
        cameraVerticalAngle -= 1.0f * angleRadian;
        cameraPositionX = cameraDistance * cos(cameraVerticalAngle) * sin(cameraHorizontalAngle), cameraPositionY = cameraDistance * sin(cameraVerticalAngle), cameraPositionZ = cameraDistance * cos(cameraVerticalAngle) * cos(cameraHorizontalAngle);
    }
    //The left arrow key can make the camera turn left
    if (key == GLUT_KEY_LEFT && gaming == true)
    {
        cameraHorizontalAngle -= 1.0f * angleRadian;
        cameraPositionX = cameraDistance * cos(cameraVerticalAngle) * sin(cameraHorizontalAngle), cameraPositionY = cameraDistance * sin(cameraVerticalAngle), cameraPositionZ = cameraDistance * cos(cameraVerticalAngle) * cos(cameraHorizontalAngle);
    }
    //The right arrow key can make the camera turn right
    if (key == GLUT_KEY_RIGHT && gaming == true)
    {
        cameraHorizontalAngle += 1.0f * angleRadian;
        cameraPositionX = cameraDistance * cos(cameraVerticalAngle) * sin(cameraHorizontalAngle), cameraPositionY = cameraDistance * sin(cameraVerticalAngle), cameraPositionZ = cameraDistance * cos(cameraVerticalAngle) * cos(cameraHorizontalAngle);
    }
    glutPostRedisplay();
}

//This function controls the interaction with normal keys in the keyboard.
void keyboardInput(unsigned char key, int x, int y)
{
    //Press 'W' or 'S' will make Kirby forward or backward.
    //Kirby will tilt towards the direction of motion to make it more like it is in motion.
    //Press 'A' or 'D' will make Kirby turn left or right.
    //Press 'E' or 'Space' to shoot the star bullet.
    //Press 'Esc' to quit the game.
    angleMoving = 0.0f;
    if (((key == 'w' || key == 'W') && kirbyNormal == true) && gaming == true)
    {
        angleMoving = 20.0f;
        if ((std::abs(kirbyPosition[2] + 0.05 * cos(-kirbyAngle * angleRadian)) < 8.5) && (std::abs(kirbyPosition[0] + 0.05 * cos(-kirbyAngle * angleRadian)) < 8.5))
        {
            kirbyPosition[2] += 0.05 * cos(-kirbyAngle * angleRadian);
            kirbyPosition[0] += 0.05 * sin(-kirbyAngle * angleRadian);
        }
        
    }
    if (((key == 's' || key == 'S') && kirbyNormal == true) && gaming == true)
    {
        angleMoving = -20.0f;
        if ((std::abs(kirbyPosition[2] - 0.05 * cos(-kirbyAngle * angleRadian)) < 8.5) && (std::abs(kirbyPosition[0] - 0.05 * cos(-kirbyAngle * angleRadian)) < 8.5))
        {
            kirbyPosition[2] -= 0.05 * cos(-kirbyAngle * angleRadian);
            kirbyPosition[0] -= 0.05 * sin(-kirbyAngle * angleRadian);
        }
    }
    if (((key == 'a' || key == 'A') && kirbyNormal == true) && gaming == true)
    {
        kirbyAngle -= 1;
    }
    if (((key == 'd' || key == 'D') && kirbyNormal == true) && gaming == true)
    {
        kirbyAngle += 1;
    }
    if ((((key == 'e' || key == 'E' || key == ' ') && starBulletMoving != true) && kirbyNormal == true) && gaming == true)
    {
        starAttack();
    }
    if (key == 27)
    {
        exit(0);
    }
    glutPostRedisplay();

}

//This function controls the interation with the mouse.
void mouse(int button, int state, int x, int y)
{
    //Sliding the roller upwards will enlarge.
    if (button == 3 && gaming == true && cameraDistance > 0.4f)
    {
        cameraDistance -= 0.1f;
        cameraPositionX = cameraDistance * cos(cameraVerticalAngle) * sin(cameraHorizontalAngle), cameraPositionY = cameraDistance * sin(cameraVerticalAngle), cameraPositionZ = cameraDistance * cos(cameraVerticalAngle) * cos(cameraHorizontalAngle);
    }
    //Sliding the roller downwards will shrink.
    else if (button == 4 && gaming == true && cameraDistance < 2.0f)
    {
        cameraDistance += 0.1f;
        cameraPositionX = cameraDistance * cos(cameraVerticalAngle) * sin(cameraHorizontalAngle), cameraPositionY = cameraDistance * sin(cameraVerticalAngle), cameraPositionZ = cameraDistance * cos(cameraVerticalAngle) * cos(cameraHorizontalAngle);
    }
    glutPostRedisplay();
}

//Set the windown size and some callback function
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Kirby Defends Gift");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutSpecialFunc(specialKeys);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboardInput);
    glutTimerFunc(0, update, 0);
    initMenu();
    myinit();

    glutMainLoop();

    return 0;
}
