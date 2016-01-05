#define Timer 0xFF202000
#define TimerStatus ((volatile short*) (Timer))
#define TimerControl ((volatile short*) (Timer+4))
#define TimerTimeoutL ((volatile short*) (Timer+8))
#define TimerTimeoutH ((volatile short*) (Timer+12))
#define TimerSnapshotL ((volatile short*) (Timer+16))
#define TimerSnapshotH ((volatile short*) (Timer+20))

#define RED_LEDS 0xFF200000
#define GPIO 0xFF200060
#define LEGO GPIO
#define DirectionRegister 0x07F557FF
#define LEGODirectionRegister ((volatile short*) (LEGO+4))
#define LEGOInterruptsRegister ((volatile short*) (LEGO+8))
#define LEGOEdgeCaptureRegister ((volatile short*) (LEGO+12))

#define Controller1Up Sensor1
#define Controller1Down Sensor2

#define Controller2Up Sensor3
#define Controller2Down Sensor4

#define ControllerMaxHeight 26
#define ControllerMinHeight 150

#define gameWidth 300
#define gameHeight 188

// some exception code taken from example code provided by Altera development IDE
#include "nios_ctrl_reg_macros.h"
#include "ui.h"

volatile int using_controllers;

extern int p1_h;
extern int p2_h;

extern int ball_x;
extern int ball_y;

extern int p1_score;
extern int p2_score;

extern volatile int ball_radius;

extern int game_paused;
extern volatile int game_speed;

volatile int prev_controller1Height;
volatile int prev_controller2Height;

/*
 * Game area is 300 x 188
 */

volatile int ballPos[2];
volatile int ballVel[2];
volatile int ballRadius = 1;

void the_reset (void) __attribute__ ((section (".reset")));

/************************************************************************************
 * Reset code. By giving the code a section attribute with the name ".reset" we     *
 * allow the linker program to locate this code at the proper reset vector address. *
 * This code just calls the main program.                                           *
 ***********************************************************************************/
void the_reset (void)
{
asm ("	.set	noat");					// Magic, for the C compiler
asm ("	.set 	nobreak");				// Magic, for the C compiler
asm ("	movia	r2, main");				// Call the C language main program
asm ("	jmp		r2"); 
}

void the_exception (void) __attribute__ ((section (".exceptions")));

/*******************************************************************************
 * Exceptions code. By giving the code a section attribute with the name       *
 * ".exceptions" we allow the linker program to locate this code at the proper *
 * exceptions vector address.                                                  *
 * This code calls the interrupt handler and later returns from the exception. *
 ******************************************************************************/
void the_exception (void)
{
	/* This needs to be written in assembly because you don't have access to registers
	 * from a C program. Any work with control registers also requires use of assembly
	 */
asm ("	.set		noat"		);		// Magic, for the C compiler
asm ("	.set		nobreak"	);		// Magic, for the C compiler
asm (	"subi	sp,  sp, 128"	);
asm (	"stw	et,  96(sp)"	);
asm (	"rdctl	et,  ctl4"		);
asm (	"beq	et,  r0, SKIP_EA_DEC");	// Interrupt is not external         
asm (	"subi	ea,  ea, 4"		);		/* Must decrement ea by one instruction  
										 * for external interupts, so that the 
										 * interrupted instruction will be run */

asm ("SKIP_EA_DEC:"				);
asm (	"stw	r1,  4(sp)"		);				// Save all registers 
asm (	"stw	r2,  8(sp)"		);
asm (	"stw	r3,  12(sp)"	);
asm (	"stw	r4,  16(sp)"	);
asm (	"stw	r5,  20(sp)"	);
asm (	"stw	r6,  24(sp)"	);
asm (	"stw	r7,  28(sp)"	);
asm (	"stw	r8,  32(sp)"	);
asm (	"stw	r9,  36(sp)"	);
asm (	"stw	r10, 40(sp)"	);
asm (	"stw	r11, 44(sp)"	);
asm (	"stw	r12, 48(sp)"	);
asm (	"stw	r13, 52(sp)"	);
asm (	"stw	r14, 56(sp)"	);
asm (	"stw	r15, 60(sp)"	);
asm (	"stw	r16, 64(sp)"	);
asm (	"stw	r17, 68(sp)"	);
asm (	"stw	r18, 72(sp)"	);
asm (	"stw	r19, 76(sp)"	);
asm (	"stw	r20, 80(sp)"	);
asm (	"stw	r21, 84(sp)"	);
asm (	"stw	r22, 88(sp)"	);
asm (	"stw	r23, 92(sp)"	);
asm (	"stw	r25, 100(sp)"	);			// r25 = bt (skip r24 = et, because it is saved above)
asm (	"stw	r26, 104(sp)"	);			// r26 = gp
asm (	"nop				"	);			// skip r27 because it is sp, and there is no point in saving this
asm (	"stw	r28, 112(sp)"	);			// r28 = fp
asm (	"stw	r29, 116(sp)"	);			// r29 = ea
asm (	"stw	r30, 120(sp)"	);			// r30 = ba
asm (	"stw	r31, 124(sp)"	);			// r31 = ra
asm (	"addi	fp,  sp, 128"	);

asm (	"call	interrupt_handler");		// Call the C language interrupt handler

asm (	"ldw	r1,  4(sp)"		);				// Restore all registers
asm (	"ldw	r2,  8(sp)"		);
asm (	"ldw	r3,  12(sp)"	);
asm (	"ldw	r4,  16(sp)"	);
asm (	"ldw	r5,  20(sp)"	);
asm (	"ldw	r6,  24(sp)"	);
asm (	"ldw	r7,  28(sp)"	);
asm (	"ldw	r8,  32(sp)"	);
asm (	"ldw	r9,  36(sp)"	);
asm (	"ldw	r10, 40(sp)"	);
asm (	"ldw	r11, 44(sp)"	);
asm (	"ldw	r12, 48(sp)"	);
asm (	"ldw	r13, 52(sp)"	);
asm (	"ldw	r14, 56(sp)"	);
asm (	"ldw	r15, 60(sp)"	);
asm (	"ldw	r16, 64(sp)"	);
asm (	"ldw	r17, 68(sp)"	);
asm (	"ldw	r18, 72(sp)"	);
asm (	"ldw	r19, 76(sp)"	);
asm (	"ldw	r20, 80(sp)"	);
asm (	"ldw	r21, 84(sp)"	);
asm (	"ldw	r22, 88(sp)"	);
asm (	"ldw	r23, 92(sp)"	);
asm (	"ldw	r24, 96(sp)"	);
asm (	"ldw	r25, 100(sp)"	);			// r25 = bt
asm (	"ldw	r26, 104(sp)"	);			// r26 = gp
asm (	"nop				"	);			// skip r27 because it is sp, and we did not save this on the stack
asm (	"ldw	r28, 112(sp)"	);			// r28 = fp
asm (	"ldw	r29, 116(sp)"	);			// r29 = ea
asm (	"ldw	r30, 120(sp)"	);			// r30 = ba
asm (	"ldw	r31, 124(sp)"	);			// r31 = ra

asm (	"addi	sp,  sp, 128"	);

asm (	"eret");
}

void update_ball_only_bounce();
void update_ball_with_paddle();
void timer_setup();

void interrupt_handler(){
	int ipending;
	NIOS_READ_IPENDING(ipending); // Get the Ipending interupt identifier to tell us which IRQ triggered interrupt
	volatile int* leds = (int *)RED_LEDS;
	
	if ( ipending & 0x1 )				// Timer 0 has IRQ 0
	{
		/* Game Timer went off.
		 *
		 * flip back buffer and front buffer
		 * check if any user won
		 * update ball positions to new positions
		 * 
		 */
		 ball_x = ballPos[0]; // update global ball pos for drawing frame
		 ball_y = ballPos[1];
		 update_frame(); // Update only the graphic elements we need
		 
		 *(TimerStatus) = 0; // clear interrupt
		 update_ball_with_paddle(); // update ball position

		 
	}
	else if ( ipending & 0x800 )		// Lego controller has IRQ 11
	{
		volatile long* clearingEdge = (long *)LEGOEdgeCaptureRegister;
		(*clearingEdge) = 0xFFFFFFFF; // Ack the lego interrupt
		volatile int * GPIO_ptr = (int *) GPIO;
		long whichSensor = *(GPIO_ptr);
		whichSensor >>= 27;
		whichSensor &= 0x01F; // which sensor triggered interrupt can be read from the state bits in LEGO base
		
		if(whichSensor == 0x01F) // All sensors reporting not triggered. False alarm, or we didn't react fast enough.
			return;
		
		if(whichSensor == 0x01E) // Sensor 1 Triggered
		{
			if(!(p1_h < ControllerMaxHeight))
				p1_h -= 10; // Move paddle up 10 pixels
		}
		else if(whichSensor == 0x01D) // Sensor 2 Triggered
		{
			if(!(p1_h > ControllerMinHeight))
				p1_h += 10; // Move paddle down 10 pixels
		}
		
		if(whichSensor == 0x01B) // Sensor 3 Triggered
		{
			if(!(p2_h < ControllerMaxHeight))
				p2_h -= 10; // move paddle up 10 pixels
		}
		else if(whichSensor == 0x017) // Sensor 4 Triggered
		{
			if(!(p2_h > ControllerMinHeight))
				p2_h += 10; // move paddle down 10 pixel
		}
		
		if(whichSensor == 0x00F) // Sensor 5. Not used right now.
		{
			(*leds) = 0x80;
		}
		
	}
}

/** update the ball position ignoring the position of paddles.
 *  Used as a stepping stone for collisions
 */
void update_ball_only_bounce()
{

	int newBallx = ballPos[0] + ballVel[0];
	if(newBallx > gameWidth+8 || newBallx < 10) // hit the sides of the game
	{
		// bounce back for now
		ballVel[0] = -ballVel[0];
	}
	else
	{
		ballPos[0] = newBallx;
	}
	
	int newBally = ballPos[1] + ballVel[1];
	if(newBally > gameHeight+24 || newBally < 26) // hit the top/bottom of the game
	{
		ballVel[1] = -ballVel[1];
		// bounce back for now
	}
	else
	{
		ballPos[1] = newBally;
	}

	return;
}

/** now take into account the position of the paddles.
 * Include with the collision detection notion of a ball radius
 */
void update_ball_with_paddle()
{
	/*
	 * paddle1 X is at 18 + 8
	 * paddle1 Y is at controller1Height + 64
	 *
	 * paddle2 X is at 294 + 8
	 * paddle2 Y is at controller2Height + 64
	 */

	int newBallX = ballPos[0] + ballVel[0];
	int newBallY = ballPos[1] + ballVel[1];


	int BallHitPaddle1 = (newBallX - ball_radius < PADDLE_1_XOFF+8) && // Front of paddle here is 18 + 8
						  (newBallY - ball_radius < p1_h + 64) && // below the upper bound of the paddle
						  (newBallY + ball_radius > p1_h); // above the lower bound of the paddle
	
	int BallHitPaddle2 = (newBallX + ball_radius > PADDLE_2_XOFF) &&  // Don't need the + 8 here because we want to be in front of the paddle
						  (newBallY - ball_radius < p2_h + 64) && // below the upper bound of the paddle
						  (newBallY + ball_radius > p2_h); // above the lower bound of the paddle

	if(BallHitPaddle1 || BallHitPaddle2 || newBallX + ball_radius > gameWidth+10 || newBallX - ball_radius < 10) // hit the paddles
	{
		if(newBallX + ball_radius > gameWidth+10)
			p1_score++; // if we're hitting the walls, increase the scores as well
		else if(newBallX - ball_radius < 10)
			p2_score++; 
		
		// bounce back when we hit paddles
		sound();
		ballVel[0] = -ballVel[0];
	}
	else
	{
		// could be in a losing situation now
		ballPos[0] = newBallX;
	}
	
	if(newBallY + ball_radius > gameHeight+24 || newBallY - ball_radius < 26) // hit the top/bottom of the game
	{
		// bounce back for now
		sound();
		ballVel[1] = -ballVel[1];
	}
	else
	{
		ballPos[1] = newBallY;
	}

	return;
}

/* Setup light sensors to start in state mode and disable all lego interrupts
 *
 */
void light_setup()
{
   // Need to disable any buttons setup so we can choose in game which controllers we want
   asm (	"rdctl   r4, ctl3"	);
   asm (	"movia   r5, 0xF7FF"); // inverse IRQ line for GPIO 11
   asm (	"and     r4, r4, r5");
   asm (	"wrctl   ctl3, r4"	); // disable our line for interrupts

    volatile int *Lego_interrupts = (int *) LEGOInterruptsRegister;
   *(Lego_interrupts) = 0x00000000; // disable interrupts on all sensors
   return;
}

/* Setup the buttons to trigger on interrupts when they're pressed
 *
 */
void buttons_setup()
{
	volatile int* GPIO_ptr = (int*) GPIO;
   *(GPIO_ptr) = 0xffbffbff; // load F for sensor 1
   *(GPIO_ptr) = 0xffbfefff; // load F for sensor 2
   *(GPIO_ptr) = 0xffbfbfff; // load F for sensor 3
   *(GPIO_ptr) = 0xffbeffff; // load F for sensor 4

   /* We use F because the lego push buttons have a value of F when they're
    * not pressed. If we use F, the interrupt will trigger when the value becomes
    * lower than F. ie, it's been pressed.
    */
   
   *(GPIO_ptr) = 0xffdfffff; // Turn on State mode

   volatile int *Lego_interrupts = (int *) LEGOInterruptsRegister;
   *(Lego_interrupts) = 0xf8000000; // Enable interrupts on all sensors

   asm (	"movi    r4, 0x1"	);
   asm (	"wrctl   ctl0, r4"	); // Enable global interrupts
   asm (	""					);
   asm (	"rdctl   r4, ctl3"	);
   asm (	"movia   r5, 0x0800"); // IRQ line for GPIO 11
   asm (	"or      r4, r4, r5");
   asm (	"wrctl   ctl3, r4"	); // Enable our line for interrupts
}

/* Setup the direction register and either light sensors or push buttons
 *
 */
void lego_setup()
{
	volatile int* Lego_dir = (int *) LEGODirectionRegister;
	*(Lego_dir) = DirectionRegister; // Setup LEGO controller
	
   if(using_controllers) // Using prebuilt joysticks
      buttons_setup();
   else
      light_setup();
}

/* Setup the game timer to interrupt every time it ticks
 *
 */
void timer_setup()
{
   // Configure the timeout period to 1/60 seconds for a 60fps framerate
   *(TimerStatus) = 0x0;
   
   int game_rate = 50000000/60; // timer ticks at 50000000 per second. Divide by 60 to get 60fps
   
   game_rate *= 2; // Slow down the game a bit so game speed isn't ridiculous
   
   game_rate /= game_speed; // take the game speed global variable into account
   
   *(TimerTimeoutL)= game_rate & 0xFFFF; // Lo bits
   *(TimerTimeoutH)=(game_rate & 0xFFFF0000) >> 16 ; // Hi bits
   
   // Configure timer to start counting with interrupts enabled
   asm (	"movi    r4, 0x1"	);
   asm (	"wrctl   ctl0, r4"	); // Enable global interrupts
   asm (	""					);
   asm (	"rdctl   r4, ctl3"	);
   asm (	"movia   r5, 0x1"	); // IRQ line for Timer
   asm (	"or      r4, r4, r5");
   asm (	"wrctl   ctl3, r4"	); // Enable our line for interrupts
   *(TimerControl)=7; // Interrupt + continue + start
}

/* Poll the light sensors, reading the values and interpolating in
 * order to get a cleaner reading of the controller height
 */
void poll_light_sensors()
{
	volatile int* GPIO_ptr = (int*) GPIO;
	volatile int* leds = (int *)RED_LEDS;
	// controller1Height
	do
	{
		(*GPIO_ptr) = 0xfffffbff; // keep polling light sensor 1

	} while(((*GPIO_ptr) >> 11 & 0x1) == 0); // Only exit when we have a valid state

	int t_value = (((*GPIO_ptr) >> 27) & 0x0f); // Read value
	t_value -= 3; // Normalize to 0
	
	// Use linear interpolation between minheight and maxheight
	p1_h = ControllerMinHeight + (t_value) * ((ControllerMaxHeight - ControllerMinHeight) / 7); 
	
	// controller2Height
	do
	{
		(*GPIO_ptr) = 0xffffefff; // same thing, but now for sensor 2

	} while(((*GPIO_ptr) >> 13 & 0x1) == 0);

	t_value = (((*GPIO_ptr) >> 27) & 0x0f); // Read value
	t_value -= 3; // Normalize to 0
	
	// Use linear interpolation between minheight and maxheight
	p2_h = ControllerMinHeight + (t_value) * ((ControllerMaxHeight - ControllerMinHeight) / 7); 
}

/* intialize variables for game
 *
 */
void game_setup(){

   ballPos[0] = 160; // Initialize the ball to be in the middle of the screen
   ballPos[1] = 120;

   ballVel[0] = 2; // Initialize the speed of the ball to be 2 x + 1 at each tick
   ballVel[1] = 1;

   volatile int* leds = (int *)RED_LEDS;
   (*leds) = 0; // clear LEDs on startup to restart any previous interrupts
   
   // This variable determines if we're using the light sensors or controllers
   using_controllers = 0; 
	
   get_and_set(); // Setup game UI + speeds
   timer_setup(); // Setup game timer
   lego_setup(); // setup lego interrupts for buttons or state mode for sensors
   
   first_draw(); // on first draw, only draw what won't be changing
	
}

/* busy loop for pausing. Need to poll push buttons to see if we're paused
 *
 */
void busyloop(){

	if(*PUSHBUTTONS){ // If we're pressed;
		game_paused = 1; // we're paused
		do{
			asm("nop"); // keep doing nothing until user releases
		} while(*PUSHBUTTONS);
		
		game_paused = 0; // unpause game
		game_setup(); // resetup with new variables
	}
		
}

int main()
{
	game_setup();
   
   if(using_controllers)
   {
      while(1){
		 busyloop();
	     asm (	"nop"); // Loop infinitely so we can do everything in interrupts without our program exiting
   	  }
   }
   else
   {
   	   while(1){
		  busyloop();
          poll_light_sensors(); // Keep polling light sensors. Game will update with interrupts
   	   }
   }
}

/* sound module for making dings for ball
 */
int sound()
{
   volatile int * audio_ptr = (int *) 0xFF203040; // audio port
   long sineWave[] = {-1, 0, 1, 0}; // just a basic description of a wave
   
   int j;
	for(j = 0; j < 5; j++){ // 5 cycles
		int i;
		int factor = 40; // period of 40
		for(i = 0; i < ((sizeof(sineWave)/sizeof(long)) * factor); i++){
			// Write to both channels
			*(audio_ptr + 2) = (1<<25) * sineWave[i/factor];
			*(audio_ptr + 3) = (1<<25) * sineWave[i/factor];
	   }
	}
}
