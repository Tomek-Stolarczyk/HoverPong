#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "ui.h"

// Declare global variables
int p1_h;
int p2_h;
short p1_c;
short p2_c;
int game_mode;
int game_paused = 0;
int game_menu;
int game_audio;
int game_speed = 1;

int ball_size;
int ball_x;
int ball_y;
int ball_radius = 4;
int prev_x_ball;
int prev_y_ball;

int prev_p1_h;
int prev_p1_x;
int prev_p2_h;
int prev_p2_x;

int p1_score = 0;
int p2_score = 0;

// Potential code for double buffer
//void init_vga_mem() {
//	volatile short *vga_mem = malloc(16*76800);
//}
//void draw_swap() {
//	memccpy(0x08000000, vga_mem, (16*76800));
//}

/*  getSpeedHex
 *  Get value of game speed for 7seg
 */
long getSpeedHex(game_speed){
	switch(game_speed){
		case 1:
			return 2 + 2*2;
		case 2:
			return 1 + 2 + 2*2*2*2*2*2 + 2*2*2*2 + 2*2*2;
		case 3:
			return 1 + 2 + 2*2 + 2*2*2 + 2*2*2*2*2*2;
		case 4:
			return 2*2*2*2*2 + 2*2*2*2*2*2 + 2 + 2*2;
		case 5:
			return 0xFA;
		default:
		return 0;
	}
}

/*  console_out
 *  Print characters to JTAG UART (Nios II terminal)
 */
void console_out(unsigned char* txt) {
	unsigned char *pOutput;
	pOutput = txt;
	while(*pOutput) //strings in C are zero terminated
	{
		 //if room in output buffer
		 if((*JTAG_UART_CONTROL)&0xffff0000  )
		 {
			//then write the next character
			*JTAG_UART_DATA = (*pOutput++);
		 }
	 }
}
/*  write_pixel
 *  Write a pixel with colour to the VGA
 *  [x coordinate, y coordinate, colour]
 */
void write_pixel(int x, int y, short c) {
  volatile short *vga_addr=(volatile short*)(0x08000000 + (y<<10) + (x<<1));
  *vga_addr=c;
}

// Vars for palette printing
int px = 0;
int py = 0;

/*  palette
 *  Print a colours like a swatch. Used for selecting colours/design elements
 *  [row, colour]
 */
void palette(int r, short c) {
  int x, y;
	if(px == 320) {
		px = 0;
	}
	py = r*60;
  for (x = px; x < (x+8); x++) {
    for (y = py; y < (y+60); y++) {
	  write_pixel(x,y,c);
		}
  }
	px += 8;
	py = r*60;
}

/*  draw_menu
 *  Draw menu interface
 */
void draw_menu() {
	palette(0, ALICEBLUE);
	palette(1, ANTIQUEWHITE);
	palette(2, AQUA);
	palette(3, AQUAMARINE);
}

/*  fill_screen
 *  Fill screen with colour
 *  [colour]
 */
void fill_screen(short c) {
  int x, y;
  for (x = 0; x < 320; x++) {
    for (y = 0; y < 240; y++) {
	  write_pixel(x,y,c);
	}
  }
}

/*  subset
 *  Fill subset of screen with colour
 *  [width, colour]
 */
void subset(int w, short c) {
  int x, y;
  for (x = w; x < (320-w); x++) {
    for (y = w; y < (240-w); y++) {
	  write_pixel(x,y,c);
	}
  }
}

/*  border
 *  Draw border of specific colour
 *  [x offset, y offset, width, colour]
 */
void border(int xo, int yo, int w, short c) {
  int x, y;
  for (x = xo; x < (320-xo); x++) {
	  for (y = yo; y < (240-yo); y++) {
		  if (((x >= xo) && (x <= (xo+w-1))) || ((x <= (320-xo)) && (x >= (320-xo-w)))) {
			write_pixel(x,y,c);
		  }
		  if (((y >= yo) && (y <= (yo+w-1))) || ((y >= (240-yo-w)) && (y <= (240-yo)))) {
			write_pixel(x,y,c);
		  }
	  }
  }
}

/*  meta_bar
 *  Draw meta bar of specific colour
 *  [colour]
 */
void meta_bar(short c) {
  int x, y;
  for (y = 4; y < 20; y++) {
    for (x = 32; x < 288; x++) {
      write_pixel(x,y,c);
    }
  }
  for (y = 220; y < 236; y++) {
    for (x = 32; x < 288; x++) {
      write_pixel(x,y,c);
    }
  }
}

/*  paddle
 *  Draw paddle at certain coordinate of specific colour
 *  [x origin, y origin, colour]
 */
void paddle(int xc, int yc, short c) {
  int old_value = prev_p1_h;
	int x, y;

	if(xc == PADDLE_2_XOFF) { old_value = prev_p2_h; }
  if (yc < 26) { yc = 26; }
  if (yc > 150) { yc = 150; }

  for (y = old_value; y < (old_value+64); y++) {
    for (x = xc; x < (xc+8); x++) {
      write_pixel(x,y,C_GREEN);
    }
  }

  for (y = yc; y < (yc+64); y++) {
    for (x = xc; x < (xc+8); x++) {
      write_pixel(x,y,c);
    }
  }

  if(xc == PADDLE_2_XOFF) { prev_p2_h = yc; }
  else if (xc == PADDLE_1_XOFF) { prev_p1_h = yc; }
}

/*  ball
 *  Draw ball at certain coordinate of specific colour
 *  [x origin, y origin, colour]
 */
void ball(int xc, int yc, short c) {
  int radius = ball_radius;
  int x, y;

  for (x = prev_x_ball - radius; x < (prev_x_ball+radius); x++) {
		for (y = prev_y_ball - radius; y < (prev_y_ball+radius); y++) {
       write_pixel(x, y, C_GREEN);
    }
  }

  for (x = xc - radius; x < xc+radius; x++) {
    for (y = yc - radius; y < yc+radius; y++) {
       write_pixel(x,y,c);
    }
  }

  prev_x_ball = xc;
  prev_y_ball = yc;
}

/*  colour_select
 *  Return colour value depending on switch value
 *  [switch value]
 */
short colour_select(int i) {
  switch(i) {
    case 0:
      return C_WHITE;
    case 1:
      return C_PINK;
    case 2:
      return C_RED;
    case 3:
      return C_ORANGE;
    case 4:
      return C_YELLOW;
    case 5:
      return C_AQUA;
    case 6:
      return C_DBLUE;
    case 7:
      return C_PURPLE;
    default:
      return C_BLACK;
  }
}

/*  write_char
 *  Write a character to the character buffer
 *  [x coordinate, y coordinate, character]
 */
void write_char(int x, int y, char c) {
  volatile char * character_buffer = (char *) (0x09000000 + (y<<7) + x);
  *character_buffer = c;
}

/*  clear_text
 *  Clear the character buffer
 */
void clear_text() {
	int x, y;
	for (x = 0; x < 80; x++) {
		for (y = 0; y < 60; y++) {
			write_char(x, y, '\0');
			//volatile char * character_buffer = (char *) (0x09000000 + (y<<7) + x);
			//*character_buffer = ' ';
		}
	}
}

/*  update_frame
 *  Draw interface for a given frame
 */
void update_frame() {
  if (game_menu == 1) { draw_menu(); }
  else {
    // Regular game
    if ((game_mode == 0) && (game_paused == 0)) {
      paddle(PADDLE_1_XOFF,p1_h,p1_c);
      paddle(PADDLE_2_XOFF,p2_h,p2_c);
	  	ball(ball_x, ball_y, C_WHITE);

		  char* hw = "Score: ";
		  int x = 10;
		  while (*hw) {
				write_char(x, 2, *hw);
				x++;
				hw++;
		  }
	  	write_char(x, 2, '0' + p1_score);
		  hw = "Score: ";
		  x = 60;
		  while (*hw) {
				write_char(x, 2, *hw);
				x++;
				hw++;
		  }
	  	write_char(x, 2, '0' + p2_score);
    }
    // Paused game
    if (game_paused == 1) {
			unsigned char txt3[] = {'P','A','U','S','E','D','\n','\0'};
			console_out(txt3);
      fill_screen(C_RED);
		  char* hw = "GAME PAUSED";
	  	int x = 10;
		  while (*hw) {
				write_char(x, 2, *hw);
				x++;
				hw++;
		   }
    }
    // Different mode, not implemented
    if (game_mode != 0) {
			unsigned char txt4[] = {'U','N','K','N','O','W','N','\n','\0'};
			console_out(txt4);
      fill_screen(C_DBLUE);
    }
  }
}

/*  first_draw
 *  Draw interface for the first frame
 */
void first_draw() {
  if (game_menu == 1) { draw_menu(); }
  else {
    // Regular game
    if ((game_mode == 0) && (game_paused == 0)) {
	  	unsigned char txt2[] = {'D','r','a','w',' ','g','a','m','e','\n','\0'};
	  	console_out(txt2);
      fill_screen(C_BLACK);
	  	clear_text();
      subset(4, C_GREEN);
      meta_bar(C_BLACK);
      border(8,24,2,C_WHITE);
      paddle(PADDLE_1_XOFF,p1_h,p1_c);
      paddle(PADDLE_2_XOFF,p2_h,p2_c);
    }
    // Paused game
    if (game_paused == 1) {
			unsigned char txt3[] = {'P','A','U','S','E','D','\n','\0'};
			console_out(txt3);
      fill_screen(C_RED);
		  char* hw = "GAME PAUSED";
			int x = 10;
			while (*hw) {
				write_char(x, 2, *hw);
				x++;
				hw++;
			}
    }
    // Different mode, not implemented
    if (game_mode != 0) {
			unsigned char txt4[] = {'U','N','K','N','O','W','N','\n','\0'};
			console_out(txt4);
      fill_screen(C_DBLUE);
    }
  }
}

/*  get_and_set
 *  Retrive values from DE1 and update global vars
 */
void get_and_set() {
	unsigned char txt1[] = {'G','e','t',' ','a','n','d',' ','S','e','t','\n','\0'};
	console_out(txt1);
  // Get switch values
  long switchVal = *SWITCHES;
  unsigned int p1cVal = (switchVal & 0x7);           // .......xxx
  unsigned int p2cVal = (switchVal & 0x38) >> 3;     // ....xxx...
  unsigned int speedVal = (switchVal & 0xc0) >> 6;    // ..xx......
  unsigned int audioVal = (switchVal & 0x100) >> 8;  // .x........
  unsigned int menuVal = (switchVal & 0x200) >> 9;   // x.........
  // Diagnostics
  char pc1Valtxt[32];
  sprintf(pc1Valtxt, "Paddle 1 Colour: %d\n", p1cVal);
  console_out(pc1Valtxt);
  char pc2Valtxt[32];
  sprintf(pc2Valtxt, "Paddle 2 Colour: %d\n", p2cVal);
  console_out(pc2Valtxt);
  char speedValtxt[32];
  sprintf(speedValtxt, "Game speed: %d\n", speedVal+1);
  console_out(speedValtxt);
  char audioValtxt[32];
  sprintf(audioValtxt, "Audio: %d\n", audioVal);
  console_out(audioValtxt);
  char menuValtxt[32];
  sprintf(menuValtxt, "Menu/Reset: %d\n", menuVal);
  console_out(menuValtxt);
  // Set paddle colours
  p1_c = colour_select(p1cVal);
  p2_c = colour_select(p2cVal);
  char p1ct[32];
  sprintf(p1ct, "Paddle 1 Colour Val: %hu\n", p1_c);
  console_out(p1ct);
  char p2ct[32];
  sprintf(p2ct, "Paddle 2 Colour Val: %hu\n", p2_c);
  console_out(p2ct);

  // Set game mode (0 to 4)
  game_speed = speedVal + 1;

  // Set audio value (1 == on, 0 == off)
  game_audio = audioVal;

  // Set menu/reset value (1 == on, 0 == off)
  game_menu = menuVal;

  *(GAME_SPEED_HEX) = getSpeedHex(game_speed);

  /*
  // Get push button values
  long buttonVal = *PUSHBUTTONS;
  unsigned int b1Val = (buttonVal & 0x1);    // ..x
  unsigned int b2Val = (buttonVal & 0x2);    // .x.
  unsigned int b3Val = (buttonVal & 0x4);    // x..

  // Increase game speed
  if (game_speed < GAME_SPEED_MAX) {
    if ((b1Val == 1) && (b2Val == 0)) {
      game_speed++;
    }
  }
  // Decrease game speed
  if (game_speed > 0) {
    if ((b1Val == 0) && (b2Val == 1)) {
      game_speed--;
    }
  }
  // Set game to paused
  if (game_speed == 0) {
    game_paused = 1;
  }
  */

	// Defaults
  p1_h = 88;
  p2_h = 88;

  ball_x = 160;
  ball_y = 120;

  prev_p1_h = p1_h;
  prev_p2_h = p2_h;

  prev_x_ball = ball_x;
  prev_y_ball = ball_y;
}

/* -- Used for static frame testing, refer to clockPulse.c --
// Run every frame (1/60 seconds)
int main (void) {

	unsigned char hwld[] = {'H','e','l','l','o',' ','W','o','r','l','d','\n','\0'};
	console_out(hwld);

  //init_vga_mem();

  p1_h = PADDLE_START;
  p2_h = PADDLE_START;

  get_and_set();
  //clear_text();
  draw();

   return 0;
}
*/
