#include <stdio.h>
#include <stdlib.h>
#include "gba.h"
#include "game.h"
#include "mainGame.h"
#include "images/title.h"
#include "images/subtitle1.h"
#include "images/subtitle2.h"
#include "images/badend1.h"
#include "images/badend2.h"
#include "images/win.h"
#include "images/pShip.h"
#include "images/eShip.h"
#include "images/laser.h"

#define ESPACING 60;
#define LASWIDTH 5;
#define LASHEIGHT 5;
#define LASDAMAGE 5;

PShip playerShip = {.health=5, .x=100, .y=110, .dx=3, .dy=3, .width=20, .height=40};
EShip eShipList[4];
int eLaserCount = 30;
Laser eLaserList[30];
int eCurrentLaser = 0;
char buffer[128];
int winFrame = 0;

void initializeGame(void) {
    winFrame = 0;
    playerShip.health = 5;
    playerShip.x = 100;
    playerShip.y = 110;
    playerShip.dx = 3;
    playerShip.dy = 3;
    playerShip.width = 20;
    playerShip.height = 40;
    fillScreenDMA(BLACK);
    eCurrentLaser = 0;
    volatile int startx = 20;
    for(int i = 0; i < 4; i++) {
        eShipList[i].health=1;
        eShipList[i].x=0;
        eShipList[i].y=startx;
        eShipList[i].width=15;
        eShipList[i].height=20;
        startx += ESPACING;
        drawImageDMA(eShipList[i].x, eShipList[i].y, eShipList[i].width, eShipList[i].height, eShip);
    }
    drawImageDMA(playerShip.x, playerShip.y, playerShip.width, playerShip.height, pShip);
    for(int i = 0; i < 240; i++) {
        setPixel(140, i, BLUE);
    }

    for(int i = 0; i < 10; i++){
        eLaserList[i].x = -1;
        eLaserList[i].y = -1;
        eLaserList[i].dx = 1;
        eLaserList[i].dy = 1;
        eLaserList[i].width = LASWIDTH;
        eLaserList[i].height = LASHEIGHT;
        eLaserList[i].damage = LASDAMAGE;
    }
    snprintf(buffer,128,"Health: %d", playerShip.health);
    drawString(0, 145, buffer, GREEN);
}

int updateGame(u32 currentButtons, u32 previousButtons) {
    int randChance = randint(0, 20+eCurrentLaser);
    if(randChance == 0) {
        shootELaser();
    }
    int loseCon = updateELaser();
    if(loseCon == 1) {
        return 1;
    }

    if(eCurrentLaser == eLaserCount && winFrame == 0) {
        winFrame = vBlankCounter;
    }

    if(winFrame > 0 && vBlankCounter-winFrame >= 40) {
        return 2;
    }

    if(KEY_JUST_PRESSED(BUTTON_LEFT, currentButtons, previousButtons)) {
        if(playerShip.y - playerShip.dy >= 0) {
            drawRectDMA(playerShip.x, playerShip.y, playerShip.width, playerShip.height, BLACK);
            playerShip.y -= playerShip.dy;
            drawImageDMA(playerShip.x, playerShip.y, playerShip.width, playerShip.height, pShip);
        }

    }
    if(KEY_JUST_PRESSED(BUTTON_RIGHT, currentButtons, previousButtons)) {
        if(playerShip.y + playerShip.width + playerShip.dy <= WIDTH) {
            drawRectDMA(playerShip.x, playerShip.y, playerShip.width, playerShip.height, BLACK);
            playerShip.y += playerShip.dy;
            drawImageDMA(playerShip.x, playerShip.y, playerShip.width, playerShip.height, pShip);
        }
    }
    if(KEY_JUST_PRESSED(BUTTON_UP, currentButtons, previousButtons)) {
        if(playerShip.x + playerShip.dx >= 30) {
            drawRectDMA(playerShip.x, playerShip.y, playerShip.width, playerShip.height, BLACK);
            playerShip.x -= playerShip.dx;
            drawImageDMA(playerShip.x, playerShip.y, playerShip.width, playerShip.height, pShip);
        }
    }
    if(KEY_JUST_PRESSED(BUTTON_DOWN, currentButtons, previousButtons)) {
        if(playerShip.x + playerShip.height + playerShip.dx <= 140) {
            drawRectDMA(playerShip.x, playerShip.y, playerShip.width, playerShip.height, BLACK);
            playerShip.x += playerShip.dx;
            drawImageDMA(playerShip.x, playerShip.y, playerShip.width, playerShip.height, pShip);
        }
    }
    return 0;
}

void shootELaser(void) {
    if(eCurrentLaser < eLaserCount) {
        int shipNum = randint(0,4);
        eLaserList[eCurrentLaser].x = eShipList[shipNum].x + eShipList[shipNum].width + 5;
        eLaserList[eCurrentLaser].y = eShipList[shipNum].y + eShipList[shipNum].height/3 - 1;

        int xDist = playerShip.x + playerShip.height/2 - eLaserList[eCurrentLaser].x;
        int yDist = playerShip.y + playerShip.width/2 - eLaserList[eCurrentLaser].y;

        int mDist = xDist;
        if(yDist > 0) {
            mDist += yDist;
        }
        else{
            mDist -= yDist;
        }
        // int xAbs = xDist;
        // int yAbs = yDist;
        // if(xAbs < 0) {
        //     xAbs = -1*xAbs;
        // }
        // if(yAbs < 0) {
        //     yAbs = -1*yAbs;
        // }
        // if(xAbs>yAbs) {
        //     mDist = mDist/(1+.4*yAbs/xAbs);
        // }
        // else {
        //     mDist = mDist/(1+.4*xAbs/yAbs);
        // }

        // eLaserList[eCurrentLaser].dx = xDist*2.2/mDist;
        // eLaserList[eCurrentLaser].dy = yDist*2.2/mDist;
        eLaserList[eCurrentLaser].dx = 3*xDist/mDist;
        eLaserList[eCurrentLaser].dy = 3*yDist/mDist;
        eLaserList[eCurrentLaser].width = LASWIDTH;
        eLaserList[eCurrentLaser].height = LASHEIGHT;
        eLaserList[eCurrentLaser].damage = LASDAMAGE;
        eCurrentLaser++;
    }
}

int updateELaser(void) {
    for(int i = 0; i < eLaserCount; i++) {

        if(playerShip.health <= 0) {
            return 1;
        }

        if(eLaserList[i].x > 0) {
            if(eLaserList[i].x + eLaserList[i].height + eLaserList[i].dx >= 140 || eLaserList[i].y + eLaserList[i].width + eLaserList[i].dy >= 240 || eLaserList[i].y + eLaserList[i].width + eLaserList[i].dy <= 0) {
                drawRectDMA(eLaserList[i].x, eLaserList[i].y, eLaserList[i].width, eLaserList[i].height, BLACK);
                eLaserList[i].x = -10;
                eLaserList[i].y = -10;
                eLaserList[i].dx = 0;
                eLaserList[i].dy = 0;
            }
            if(eLaserList[i].x + eLaserList[i].height + eLaserList[i].dx <= 140 || eLaserList[i].y + eLaserList[i].width + eLaserList[i].dy <= 240) {
                drawRectDMA(eLaserList[i].x, eLaserList[i].y, eLaserList[i].width, eLaserList[i].height, BLACK);
                eLaserList[i].x += eLaserList[i].dx;
                eLaserList[i].y += eLaserList[i].dy;
                drawImageDMA(eLaserList[i].x, eLaserList[i].y, eLaserList[i].width, eLaserList[i].height, laser);
            }
            if((eLaserList[i].x + 2 <= playerShip.x+playerShip.height && eLaserList[i].x + 2 >= playerShip.x)
                && (eLaserList[i].y + 2 <= playerShip.y+playerShip.width && eLaserList[i].y + 2 >= playerShip.y)) {
                eLaserList[i].dx = 0;

                drawRectDMA(eLaserList[i].x, eLaserList[i].y, eLaserList[i].width, eLaserList[i].height, BLACK);
                eLaserList[i].x = -10;
                eLaserList[i].y = -10;
                eLaserList[i].dx = 0;
                eLaserList[i].dy = 0;
                playerShip.health--;
                updateHealth();
            }
        }
    }
    return 0;
}

void updateHealth(void) {
    drawRectDMA(145, 0, 60, 20, BLACK);
    snprintf(buffer,128,"Health: %d", playerShip.health);
    drawString(0, 145, buffer, GREEN);
}