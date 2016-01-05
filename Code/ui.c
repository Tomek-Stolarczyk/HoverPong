#include <stdio.h>
#include <unistd.h>
#include "ui.h"

int p1_h;
int p2_h;
short p1_c;
short p2_c;
int game_mode;
int game_paused;
int game_menu;
int game_audio;
int game_speed;
int ball_size;

int prev_xc;
int prev_yc;

// [x coordinate, y coordinate, colour]
void write_pixel(int x, int y, short c) {
  volatile short *vga_addr=(volatile short*)(0x08000000 + (y<<10) + (x<<1));
  *vga_addr=c;
}

// [colour]
void fill_screen(short c) {
  int x, y;
  for (x = 0; x < 320; x++) {
    for (y = 0; y < 240; y++) {
	  write_pixel(x,y,c);
	}
  }
}

// [width, colour]
void subset(int w, short c) {
  int x, y;
  for (x = w; x < (320-w); x++) {
    for (y = w; y < (240-w); y++) {
	  write_pixel(x,y,c);
	}
  }
}

// [x offset, y offset, width, colour]
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

// [colour]
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

// [x origin, y origin, colour]
void paddle(int xc, int yc, short c) {
  if (yc < 26) {
	  yc = 26;
  }
  if (yc > 150) {
	  yc = 150;
  }
  int x, y;
  for (y = yc; y < (yc+64); y++) {
    for (x = xc; x < (xc+8); x++) {
      write_pixel(x,y,c);
    }
  }
}

// [x origin, y origin, colour]
void ball(int xc, int yc, short c) {
	
	int x, y;
	for (x = prev_xc; x < (prev_xc+2); x++) {
    for (y = prev_yc; y < (prev_yc+2); y++) {
       write_pixel(x,y,C_GREEN);
    }
  }
  int radius = 8;
  for (x = xc; x < (xc+2); x++) {
    for (y = yc; y < (yc+2); y++) {
       write_pixel(x,y,c);
    }
  }
  prev_xc = xc;
  prev_yc = yc;
}

// [switch value]
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

// [x coordinate, y coordinate, character]
void write_char(int x, int y, char c) {
  volatile char * character_buffer = (char *) (0x09000000 + (y<<7) + x);
  *character_buffer = c;
}

void clear_text() {
	int x, y;
	for (x = 0; x < 80; x++) {
		for (y = 0; y < 60; y++) {
			volatile char * character_buffer = (char *) (0x09000000 + (y<<7) + x);
			*character_buffer = ' ';
		}	
	}
}

void draw_menu() {
  fill_screen(C_BLACK);
  subset(4, C_WHITE);
  subset(8, C_BLACK);
  subset(12, C_WHITE);
}

void draw(int p1_h, int p2_h, int ball_x, int ball_y) {
  if (game_menu == 1) {
    draw_menu();
  }
  else {
    // Regular game
    if ((game_mode == 0) && (game_paused == 0)) {
      //if (p1_movement == 1) { p1_h++; }
      //if (p1_movement == 1) { p1_h++; }
      //if (p2_movement == 2) { p2_h--; }
      //if (p2_movement == 2) { p2_h--; }
      fill_screen(C_BLACK);
      subset(4, C_GREEN);
      meta_bar(C_BLACK);
      border(8,24,2,C_WHITE);
      paddle(PADDLE_1_XOFF,p1_h, C_YELLOW);
      //paddle(PADDLE_1_XOFF,p2_h,p2_c);
	  //ball(ball_x, ball_y, C_WHITE);
    }
    // Paused game
    if (game_paused == 1) {
      fill_screen(C_BLACK);
    }
    // Different mode, not implemented
    if (game_mode != 0) {
      fill_screen(C_DBLUE);
    }
  }
}

void get_and_set() {
  // Get switch values
  long switchVal = *SWITCHES;
  unsigned int p1cVal = (switchVal & 0x7);           // .......xxx
  unsigned int p2cVal = (switchVal & 0x38) >> 3;     // ....xxx...
  unsigned int modeVal = (switchVal & 0xc0) >> 6;    // ..xx......
  unsigned int audioVal = (switchVal & 0x100) >> 8;  // .x........
  unsigned int menuVal = (switchVal & 0x200) >> 9;   // x.........

  // Set paddle colours
  p1_c = colour_select(p1cVal);
  p2_c = colour_select(p2cVal);

  // Set game mode (0 to 4)
  game_mode = modeVal;

  // Set audio value (1 == on, 0 == off)
  game_audio = audioVal;

  // Set menu/reset value (1 == on, 0 == off)
  game_menu = menuVal;

  // Get push button values
  // TODO: Make sure push buttons don't get triggered too often
  //       i.e. users presses on button for more than 1 frame length
  long buttonVal = *PUSHBUTTIONS;
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
}

void diagnostics() {
  char* diag1 = "Diagnostics:";
  int x = 31;
  while (*diag1) {
    write_char(x, 10, *diag1);
    x++;
    diag1++;
  }
}

// Run every frame (1/60 seconds)
int main_ui (void) {

  p1_h = PADDLE_START;
  p2_h = PADDLE_START;

  get_and_set();
  clear_text();
  //draw(0,0);
  diagnostics();

   char* hw = "Score: 0";
   int x = 10;
   while (*hw) {
     write_char(x, 2, *hw);
  	 x++;
  	 hw++;
   }

   return 0;
}
