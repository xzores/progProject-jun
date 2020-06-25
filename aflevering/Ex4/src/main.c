/*
**
**                           Main.c
**
**
**********************************************************************/
/*
   Last committed:     $Revision: 00 $
   Last changed by:    $Author: $
   Last changed date:  $Date:  $
   ID:                 $Id:  $

**********************************************************************/
#include "stm32f30x_conf.h"
#include "vector.h"
#include "ansi.h"
#include "30010_io.h"

struct worldInfo{
    uint16_t xn, yn, xp, yp, hits;
};

void clearTile(uint16_t x, uint16_t y){

    gotoxy(y,x);
    printf(" ");
}

void drawBall(uint16_t x, uint16_t y){

    gotoxy(y,x);
    printf("o");
}

void updateBall(struct worldInfo* w, struct vector_t* pos, struct vector_t* vel){

    pos->x += vel->x;
    pos->y += vel->y;

    if(pos->x <= w->xn + 2){
        w->hits += 1;
        vel->x *= -1;
    }

    if(pos->x >= w->xp - 1){
        w->hits += 1;
        vel->x *= -1;
    }

    if(pos->y <= w->yn + 2){
        w->hits += 1;
        vel->y *= -1;
    }

    if(pos->y >= w->yp - 1){
        w->hits += 1;
        vel->y *= -1;
    }
}

int main(void)
{
    uart_init( 9600 );
    clearTermninal();
    inverse(0);

    window(0,0,30,60,'B',"");


    struct vector_t ballPos = {10, 10};
    struct vector_t ballVel = {1, 1};

    struct worldInfo world = {0,0,30,60,0};

    while(1)
    {
        showCursor(0);
        window(13,25,2,10,'A',"");
        gotoxy(25+2,13+2);
        printf("hits:%d", world.hits);


        clearTile(ballPos.x, ballPos.y);
        updateBall(&world, &ballPos, &ballVel);
        drawBall(ballPos.x, ballPos.y);
    }
}
