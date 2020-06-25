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

//clears the terminal
void clearTermninal(){

    printf("%c[2J", ESC);
}

void clreol(){

    printf("%c[K", ESC);
}

//sets the cursor to the x,y position
void gotoxy(uint8_t x_1, uint8_t y_1) {
    int x = y_1;
    int y = x_1;

    printf("%c[%d;%dH", ESC, x, y);
}

void homeCurser(){
    printf("%c[H", ESC);
}

//turns on/off underline
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

//turn on/off inverse colors
void inverse(uint8_t on) {
     if (on == 0) {
        printf("%c[%dm", ESC, 27);
    } else {
        printf("%c[%dm", ESC, 7);
    }
}

//move the cursor i chars forward
void moveForward(uint8_t i) {
    printf("%c[1%dm", ESC, i, 'C');
}

//show/hide cursor
void showCursor(uint8_t show) {

  if (show) {
    printf("%c[?25h", ESC);
  }
  else {
    printf("%c[?25l", ESC);
  }

}

//sets the bg color
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

//this struct is used to save what chars should be placed where.
struct borderStyle{
    uint16_t leftTopCorner, leftWall, rightWall,
    rightTopcorner, vertical, leftBottomCorner,
    horizontal, rigtBottomCorner;
} default_borderStyle = {218,180,195,191,179,192,196,217};

//creates a window in putty
void window(uint8_t x_1, uint8_t y_1,uint8_t x_2, uint8_t y_2, char style, char title[]) {
    int x1 = y_1 + 1;
    int y1 = x_1 + 1;
    int x2 = y_2;
    int y2 = x_2;

    struct borderStyle myStyle;

    int i = 0;

    uint8_t ypos = y1;
    uint8_t xpos = x1;
    gotoxy(xpos,ypos);

    //char a = 'A';
    if (style == 'A') {
        myStyle = default_borderStyle;
    }
    else if (style == 'B') { //we we want to change to be, these are the new values for borderStyle
        myStyle.leftTopCorner = 201;
        myStyle.leftWall = 185;
        myStyle.rightWall = 204;
        myStyle.rightTopcorner = 187;
        myStyle.vertical = 186;
        myStyle.leftBottomCorner = 200;
        myStyle.horizontal = 205;
        myStyle.rigtBottomCorner = 188;

    }


    int charCount = 0;
    while (title[charCount] != '\0') {
        charCount++;
    }

    //if the title is an empty string then we simply make a box.
    if(charCount != 0)
    {
        //with title

        //prints the top
        gotoxy(xpos,ypos);
        printf("%c%c", myStyle.leftTopCorner, myStyle.leftWall);
        inverse(1);
        for (i = 0; i < (((x2)-2) - charCount)/2; i++) {
            printf(" ");
        }
        printf(title);
        for (i = 0; i < (((x2)-2) - charCount)/2 - 1; i++) {
            printf(" ");
        }

        inverse(0);
        printf("%c%c", myStyle.rightWall, myStyle.rightTopcorner);
    }
    else
    {
        //simply a box

        //prints the top
        gotoxy(xpos,ypos);
        printf("%c", myStyle.leftTopCorner);
        for (i = 0; i < (x2)-2; i++) {
            printf("%c", myStyle.horizontal);
        }
        printf("%c", myStyle.rightTopcorner);
    }

    ypos = y1 + 1;
    xpos = x1;

    //prints the Lodrette linjer venstre
    for(i = 0; i < (y2-1); i++){
        gotoxy(xpos,ypos);
        printf("%c", myStyle.vertical);
        ypos++;
    }

    ypos = y1 + 1;
    xpos = x1 + x2 - 1;

    //prints the Lodrette linjer right
    for(i = 0; i < (y2-1); i++){
        gotoxy(xpos,ypos);
        printf("%c", myStyle.vertical);
        ypos++;
    }

    ypos = y1 + y2;
    xpos = x1;

    //prints the bottom
    gotoxy(xpos,ypos);
    printf("%c", myStyle.leftBottomCorner);
    for (i = 0; i < x2 - 2; i++) {
        printf("%c", myStyle.horizontal);
    }
    printf("%c", myStyle.rigtBottomCorner);
}
