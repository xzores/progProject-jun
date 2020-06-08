#define ESC 0x1B
#include "ansi.h"

void fgcolor(uint8_t foreground) {
/*  Value      foreground     Value     foreground
    ------------------------------------------------
      0        Black            8       Dark Gray
      1        Red              9       Light Red
      2        Green           10       Light Green
      3        Brown           11       Yellow
      4        Blue            12       Light Blue
      5        Purple          13       Light Purple
      6        Cyan            14       Light Cyan
      7        Light Gray      15       White
*/
  uint8_t type = 22;             // normal text
	if (foreground > 7) {
	  type = 1;                // bold text
		foreground -= 8;
	}
  printf("%c[%d;%dm", ESC, type, foreground+30);
}

void clearTermninal(){

    printf("%c[2J", ESC);
}

void clreol(){

    printf("%c[K", ESC);
}

void gotoxy(uint8_t x_1, uint8_t y_1) {
    int x = y_1;
    int y = x_1;

    printf("%c[%d;%dH", ESC, x, y);
}

void homeCurser(){
    printf("%c[H", ESC);
}

void underline (uint8_t on) {
    if (on == 0) {
        printf("%c[%dm", ESC, 24);
    } else {
        printf("%c[%dm", ESC, 4);
    }
}

void blink(uint8_t on) {
    if (on == 0) {
        printf("%c[%dm", ESC, 25);
    } else {
        printf("%c[%dm", ESC, 5);
    }
}

void inverse(uint8_t on) {
     if (on == 0) {
        printf("%c[%dm", ESC, 27);
    } else {
        printf("%c[%dm", ESC, 7);
    }
}


void bgcolor(uint8_t background) {
/* IMPORTANT:   When you first use this function you cannot get back to true white background in HyperTerminal.
   Why is that? Because ANSI does not support true white background (ANSI white is gray to most human eyes).
                The designers of HyperTerminal, however, preferred black text on white background, which is why
                the colors are initially like that, but when the background color is first changed there is no
 	              way comming back.
   Hint:        Use resetbgcolor(); clrscr(); to force HyperTerminal into gray text on black background.

    Value      Color
    ------------------
      0        Black
      1        Red
      2        Green
      3        Brown
      4        Blue
      5        Purple
      6        Cyan
      7        Gray
*/
  printf("%c[%dm", ESC, background+40);
}

void color(uint8_t foreground, uint8_t background) {
// combination of fgcolor() and bgcolor() - uses less bandwidth
  uint8_t type = 22;             // normal text
	if (foreground > 7) {
	  type = 1;                // bold text
		foreground -= 8;
	}
  printf("%c[%d;%d;%dm", ESC, type, foreground+30, background+40);
}

void resetbgcolor() {
// gray on black text, no underline, no blink, no reverse
  printf("%c[m", ESC);
}

struct borderStyle{
    uint16_t leftTopCorner, leftWall, rightWall,
    rightTopcorner, vertical, leftBottomCorner,
    horizontal, rigtBottomCorner;
} default_borderStyle = {218,180,195,191,179,192,196,217};

void window(uint8_t x_1, uint8_t y_1,uint8_t x_2, uint8_t y_2, char style, char title[]) {
    int x1 = y_1;
    int y1 = x_1;
    int x2 = y_2;
    int y2 = x_2;

    struct borderStyle myStyle;


    gotoxy(0,y1);
    //char a = 'A';
    if (style == 'A') {
        myStyle = default_borderStyle;
    }
    else if (style == 'B') {
        myStyle.leftTopCorner = 201;
        myStyle.leftWall = 185;
        myStyle.rightWall = 204;
        myStyle.rightTopcorner = 187;
        myStyle.vertical = 186;
        myStyle.leftBottomCorner = 200;
        myStyle.horizontal = 205;
        myStyle.rigtBottomCorner = 188;

    }

        //charcount
        int charCount = 0;
        while (title[charCount] != '\0') {
            charCount++;
        }

        int i;
        int k;

        if(charCount != 0)
        {
            inverse(1);
            printf("%c%c", myStyle.leftTopCorner, myStyle.leftWall);
            for (i = 0; i < x1; i++) {
                printf(" ");
            }
            printf("%s", title);
            for (i = 0; i < ((x2-x1)-charCount)/2-2; i++) {
                printf(" ");
            }

            inverse(0);
            printf("%c%c\n", myStyle.rightWall,myStyle.rightTopcorner);
        }
        else{
            for (i = 0; i < x1; i++) {
                printf(" ");
            }

            printf("%c", myStyle.leftTopCorner);
            for (i = 0; i < (x2-x1)-2; i++) {
                printf("%c", myStyle.horizontal);
            }
            printf("%c\n", myStyle.rightTopcorner);
        }

        //Lodrette linjer
        for(i = 0; i < (y2-y1-2); i++){
            for (k = 0; k < x1; k++) {
                printf(" ");
            }
            printf("%c", myStyle.vertical);
            int j;
            for (j = 0; j < (x2-x1)-2; j++) {
                printf(" ");
            }
            printf("%c\n", myStyle.vertical);
        }


        //Bundlinje
        for (k = 0; k < x1; k++) {
                printf(" ");
            }
        printf("%c",myStyle.leftBottomCorner);
        for (i = 0; i < (x2-x1)-2; i++) {
          printf("%c", myStyle.horizontal);
        }
        printf("%c\n",myStyle.rigtBottomCorner);
}




