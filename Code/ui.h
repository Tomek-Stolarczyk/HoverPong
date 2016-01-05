#ifndef __INTERFACE_H__
#define __INTERFACE_H__

#define SWITCHES ((volatile long *) 0xFF200040)
#define PUSHBUTTIONS ((volatile long *) 0xFF20005C)
#define RLEDS ((volatile long *) 0xFF00000)

#define C_WHITE   (0xFFFF)
#define C_GREY    (0xCE79)
#define C_BLACK   (0x0000)
#define C_PINK    (0xE14)
#define C_RED     (0xF800)
#define C_ORANGE  (0xF80)
#define C_YELLOW  (0xFFDA)
#define C_GREEN   (0x2C4A)
#define C_AQUA    (0x0ED)
#define C_LBLUE   (0x841F)
#define C_DBLUE   (0x18E)
#define C_PURPLE  (0x77F)

#define PADDLE_1_XOFF   18
#define PADDLE_2_XOFF   294
#define PADDLE_HEIGHT   64
#define PADDLE_START    88
#define GAME_SPEED_MAX  3


#endif /* INTERFACE_H */
