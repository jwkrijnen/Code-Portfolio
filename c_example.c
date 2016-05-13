/** @file   psr.c
    @author J. Krijnen and M. Choi
    @date   12 October 2014
    @brief  Paper Scissors Rock Game Functions
*/
#include "system.h"
#include "tinygl.h"
#include "psr.h"
#include "navswitch.h"
#include "pacer.h"
#include "ir_uart.h"

static char character = 'X';
static char opchar = 'X';
static char character_temp = '\0';
static char* currentchar;

static uint8_t wins = 0;
static uint16_t timer = 0;
static int8_t symbolnum = 0;

static bool charselected = 0;
static bool wininc = 0;

/** Displays a message when the program is first started with start 
 *  instructions and author names. Starts the game countdown when either
 * 	the NavSwitch button is pressed (and sends 'x' to other player to 
 * 	signal ready) or recieves a 'x' from other player. */
bool psr_idle(void)
{
	pacer_wait();
	tinygl_update ();
	navswitch_update();
	
	/*On NavSwitch push, begin game countdown, and send start signal 'x'
	to opponent.*/
	if (navswitch_push_event_p (NAVSWITCH_PUSH))
	{
		tinygl_text_mode_set (TINYGL_TEXT_MODE_STEP);
		ir_uart_putc('x');

		return 1;
	}
	

	/*Begin game countdown on recieving start signal 'x'.*/
	else if (ir_uart_read_ready_p())
	{
		character_temp = 0;
		character_temp = ir_uart_getc ();
		if(character_temp == 'x')
		{
			tinygl_text_mode_set (TINYGL_TEXT_MODE_STEP);
			character_temp = 0;
			return 1;
		} 
	}
	return 0;
}

/** Allows the player to scroll through the symbols for paper, scissors 
 * 	and rock. When NavSwitch is pushed, symbol is selected and sent to 
 *  other player and then waits to recieve symbol from other player. */
bool psr_choose (void) {
	
	charselected = 0;
	character = 'X';
	opchar = 'X';
	
	/*Defines the 3 symbols for paper, scissors and rock.*/
	char symbols[3];
	symbols[0] = '#'; //Paper
	symbols[1] = '>'; //Scissors
	symbols[2] = '.'; //Rock
	
	/*Loops through while the player has not chosen a symbol or has not
	recieved a symbol for the opponent.*/
	while (character == 'X' ||  opchar =='X') 
	{
		pacer_wait();
		tinygl_update ();
		navswitch_update ();
		
		currentchar = &symbols[symbolnum];
		tinygl_text(currentchar);
		
		/*If push north, scroll up through symbols[].*/
		if (charselected == 0 && navswitch_push_event_p (NAVSWITCH_NORTH))
		{
			if (symbolnum == 2)
			{
				symbolnum = -1;
			}
			symbolnum++;
		}
		/*If push south, scroll down through symbols[].*/
		if (charselected == 0 && navswitch_push_event_p (NAVSWITCH_SOUTH))
		{
			if (symbolnum == 0)
			{
				symbolnum = 3;
			}
			symbolnum--;
		}
		/*On NavSwitch push, lock in symbol selection and send to
		 * opponent player.*/
		if (navswitch_push_event_p (NAVSWITCH_PUSH))
		{	
			character = *currentchar;
			charselected = 1;
			ir_uart_putc(character);
		}
		/*Recieve symbol from opponent, checking to see if symbol is 
		 * valid.*/
		if (ir_uart_read_ready_p())
		{
			character_temp = 0;
			character_temp = ir_uart_getc ();
			if(character_temp == '.' || character_temp == '>' || character_temp == '#')
			{
				opchar = character_temp;
			} 
		}
	}
	return 1;
}

/** Determines whether the player wins or loses based on the symbol that
 *  the player and the opponent selected. Displays whether the player 
 *  won 'W', lost 'L', or drew 'D'. */
bool psr_calc_winner(void)
{
	pacer_wait();
	tinygl_update ();
	navswitch_update();
	
	/*Determines outcome based on player symbol and opponent symbol and 
	 * designates correct win/loss/draw symbol to screen.*/
	 
	if (opchar == character)
	{
		tinygl_text ("D");
	}
	
	else if (wininc == 0 && character == '.' && opchar == '>')
	{
		wininc = 1;
		wins++;
		tinygl_text ("W");
	}
	else if (character == '.' && opchar == '#')
	{
		tinygl_text ("L");
	}
	else if (character == '>' && opchar == '.')
	{
		tinygl_text ("L");
	}
	else if (wininc == 0 &&  character == '>' && opchar == '#')
	{
		wininc = 1;
		wins++;
		tinygl_text ("W");
	}
	else if (character == '#' && opchar == '>')
	{
		tinygl_text ("L");
	}
	else if (wininc == 0 && character == '#' && opchar == '.')
	{
		wininc = 1;
		wins++;
		tinygl_text ("W");
	}	
	/* On NavSwitch push break out of function.*/ 
	if (navswitch_push_event_p (NAVSWITCH_PUSH))
	{	
		wininc = 0;
		tinygl_text_mode_set (TINYGL_TEXT_MODE_SCROLL);
		timer = 0;
		char winstext[10] = " Wins = X";
		winstext[8] = '0' + wins;
		tinygl_text (winstext);
		return 1;
	}
	return 0;
}


/** Displays a message showing the number of games that the player has 
 *  won. */
bool psr_show_score(void)
{
	pacer_wait();
	tinygl_update ();
	navswitch_update();

	/* Updates screen with player score*/
	while (timer < 250)
	{ 
		tinygl_update ();
		pacer_wait();
		timer++;
	}
	
	/* On NavSwitch push, begin game countdown. Send start signal 'x' 
	 * to opponent.*/
	if (navswitch_push_event_p (NAVSWITCH_PUSH))
	{
		
		tinygl_text_mode_set (TINYGL_TEXT_MODE_STEP);
		ir_uart_putc('x');
		return 1;
	}
	
	
	/*Begin game countdown on recieving start signal 'x'.*/
	if (ir_uart_read_ready_p())
	{
		character_temp = 0;
		character_temp = ir_uart_getc ();
		if(character_temp == 'x')
		{
			tinygl_text_mode_set (TINYGL_TEXT_MODE_STEP);
			character_temp =0;
			return 1;
		} 
	}
	
	return 0;
}

	

