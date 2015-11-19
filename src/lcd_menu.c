// ***********************************************************************
// LCD menu
// ***********************************************************************



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//************************************************************************
// Projekt includes						// Include your own functions here
//************************************************************************

#include "iodefine.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "lcd_menu.h"
#include "keyboard.h"
#include "r_usb_hmsc.h"
#include "u8g.h"

u8g_t u8g;

static void u8g_prepare(void) {
  u8g_SetFont(&u8g, u8g_font_6x10);
  u8g_SetFontRefHeightExtendedText(&u8g);
  u8g_SetDefaultForegroundColor(&u8g);
  u8g_SetFontPosTop(&u8g);
}

void liga_led(void)
{
	/* All outputs LED in PORTE are on */
		PORTE.PODR.BYTE = 0x00;
}
void desliga_led(void)
{
	/* All outputs LED in PORTE are off */
		PORTE.PODR.BYTE = 0xFF;
}


bool btnDown = false;
bool btnUp = false;
bool btnEnter = false;


// ***********************************************************************
// Macros
// ***********************************************************************

// Delay macro
#define DELAY_MS(a)				DelayMs(a)

// LCD macros
#define LCDWriteChar(a)  		LCDWriteData(a)
#define LCDWriteFromROM(a,b,c) 	LCDWriteStringROM(a,b,c,1)

// ***********************************************************************
// Variable definitions
// ***********************************************************************
// General
static unsigned char selected = 1;			// Start with first entry (apart from header)

// Menu strings
const char menu_000[] = "Principal";  	// 0
const char menu_001[] = "  Liga LED";  		// 1
const char menu_002[] = "  Desl. LED";  		// 2
const char menu_003[] = "  Options3";  		// 3
const char menu_004[] = "  Options4";  		// 4
const char menu_005[] = "  Options5";  		// 5
const char menu_006[] = "  Options6";  		// 6
const char menu_007[] = "  Options7";  		// 7
const char menu_008[] = "  Options8";  		// 8
const char menu_009[] = "  Options9";  		// 9
const char menu_010[] = "  return";  		// 10

const char menu_100[] = " [Header1]";  		// 11
const char menu_101[] = "  Option101";  	// 12
const char menu_102[] = "  Option102";  	// 13
const char menu_103[] = "  Option103";  	// 14
const char menu_104[] = "  Option104";  	// 15
const char menu_105[] = "  Option105";  	// 16
const char menu_106[] = "  Option106";  	// 17
const char menu_107[] = "  Option107";  	// 18
const char menu_108[] = "  return";  		// 19

const char menu_200[] = " [Header2]";  		// 20
const char menu_201[] = "  Option201";  	// 21
const char menu_202[] = "  Option202";  	// 22	
const char menu_203[] = "  Option203";  	// 23
const char menu_204[] = "  Option204";  	// 24
const char menu_205[] = "  Option205";  	// 25
const char menu_206[] = "  Option206";  	// 26
const char menu_207[] = "  Option207";  	// 27
const char menu_208[] = "  return";  		// 28

const char menu_300[] = " [g files]";  		// 29
char menu_301[] = "";  	// 30
char menu_302[] = "";  	// 31
char menu_303[] = "";  	// 32
char menu_304[] = "";  	// 33
const char menu_305[] = "  return";  		// 34


// Array of entries
MenuEntry menu[] =
{
	{menu_000, 11,  0,  0,  0,  0}, 		// 0
	{menu_001, 11,  1,  2, 12,  liga_led},
	{menu_002, 11,  1,  3, 21,  desliga_led},
	{menu_003, 11,  2,  4, 30,  0},
	{menu_004, 11,  3,  5,  4,  0},
	{menu_005, 11,  4,  6,  5,  0},
	{menu_006, 11,  5,  7,  6,  0},
	{menu_007, 11,  6,  8,  7,  0},
	{menu_008, 11,  7,  9,  8,  0},
	{menu_009, 11,  8, 10,  9,  0},
	{menu_010, 11,  9, 10, 10,  0}, 		// 10

	{menu_100,  9,  0,  0,  0,  0},			// 11
	{menu_101,  9, 12, 13, 12,  0},
	{menu_102,  9, 12, 14, 13,  0},
	{menu_103,  9, 13, 15, 14,  0},
	{menu_104,  9, 14, 16, 15,  0},
	{menu_105,  9, 15, 17, 16,  0},
	{menu_106,  9, 16, 18, 17,  0},
	{menu_107,  9, 17, 19, 18,  0},
	{menu_108,  9, 18, 19,  1,  0},			// 19

	{menu_200,  9,  0,  0,  0,  0},			// 20
	{menu_201,  9, 21, 22, 21,  0},
	{menu_202,  9, 21, 23, 22,  0},
	{menu_203,  9, 22, 24, 23,  0},
	{menu_204,  9, 23, 25, 24,  0},
	{menu_205,  9, 24, 26, 25,  0},
	{menu_206,  9, 25, 27, 26,  0},
	{menu_207,  9, 26, 28, 27,  0},
	{menu_208,  9, 27, 28,  2,  0},			// 28

	{menu_300,  6,  0,  0,  0,  0},			// 29
	{menu_301,  6, 30, 31, 30,  0},
	{menu_302,  6, 30, 32, 31,  0},
	{menu_303,  6, 31, 33, 32,  0},
	{menu_304,  6, 32, 34, 33,  0},
	{menu_305,  6, 33, 34,  3,  0}			// 34
};



// ***********************************************************************
// Show menu function
// ***********************************************************************
void show_menu(void)
{	unsigned char line_cnt;					// Count up lines on LCD
 	unsigned char from;
 	unsigned char till = 0;

	unsigned char temp;						// Save "from" temporarily for always visible header and cursor position
 
	 // Get beginning and end of current (sub)menu
 	while (till <= selected)
 	{	till += menu[till].num_menupoints;
 	}
	from = till - menu[selected].num_menupoints;
 	till--;

	temp = from;							// Save from for later use


	//--------------------------------------------------------------------
	// Always visible Header
	//--------------------------------------------------------------------
#if         defined USE_ALWAYS_VISIBLE_HEADER

	line_cnt = 1;							// Set line counter to one since first line is reserved for header

	// Write header
	LCDWriteFromROM(0,0,menu[temp].text);
	

	// Output for two row display becomes fairly easy
	#if defined USE_TWO_ROW_DISPLAY

		LCDWriteFromROM(0,UPPER_SPACE,menu[selected].text);
		gotoxy(0,UPPER_SPACE);
		LCDWriteChar(SELECTION_CHAR);

	#endif	// USE_TWO_ROW_DISPLAY


	// Output for four row display
	#if defined USE_FOUR_ROW_DISPLAY	||	defined		USE_THREE_ROW_DISPLAY

		// Output formatting for selection somewhere in between (sub)menu top and bottom
		if ( (selected >= (from + UPPER_SPACE)) && (selected <= (till - LOWER_SPACE)) )
    	{	from = selected - (UPPER_SPACE - 1);
			till = from + (DISPLAY_ROWS - 2);

	 		for (from; from<=till; from++)
	 		{	LCDWriteFromROM(0,line_cnt,menu[from].text);
	 	 	 	line_cnt++;
			}
	
			gotoxy(0, UPPER_SPACE);
			LCDWriteChar(SELECTION_CHAR);
		}

		// Output formatting for selection close to (sub)menu top and bottom 
		// (distance between selection and top/bottom defined as UPPER- and LOWER_SPACE)
		else
		{	// Top of (sub)menu
			if (selected < (from + UPPER_SPACE))
			{	from = selected;
				till = from + (DISPLAY_ROWS - 2);
	 			
				for (from; from<=till; from++)
	 			{	LCDWriteFromROM(0,line_cnt,menu[from].text);
	 	 	 		line_cnt++;
				}
	
				gotoxy(0, (UPPER_SPACE-1));
				LCDWriteChar(SELECTION_CHAR);
			}
		
			// Bottom of (sub)menu
			if (selected == till)
			{	from = till - (DISPLAY_ROWS - 2);
	
				for (from; from<=till; from++)
	 			{	LCDWriteFromROM(0,line_cnt,menu[from].text);
	 	 	 		line_cnt++;
				}
	
				gotoxy(0, (DISPLAY_ROWS - 1));
				LCDWriteChar(SELECTION_CHAR);
			}
		}

	#endif	// USE_FOUR_ROW_DISPLAY


	//--------------------------------------------------------------------
	// Header not always visible
	//--------------------------------------------------------------------
#else	// !defined USE_ALWAYS_VISIBLE_HEADER

	line_cnt = 0;							// Set line counter to zero since all rows will be written

	// Output formatting for selection somewhere in between (sub)menu top and bottom
	if ( (selected >= (from + UPPER_SPACE)) && (selected <= (till - LOWER_SPACE)) )
    {	from = selected - UPPER_SPACE;
		till = from + (DISPLAY_ROWS - 1);

 		for (from; from<=till; from++)
 		{
 			u8g_DrawStr(&u8g, 7*line_cnt, 10, menu[from].text);
 			u8g_NextPage(&u8g);
 		//	BSP_GraphLCD_ClrLine(line_cnt);
 		//	BSP_GraphLCD_StringPos(line_cnt,0,menu[from].text);
 	 	 	line_cnt++;
		}
 		//BSP_GraphLCD_CharPixel(UPPER_SPACE,0,SELECTION_CHAR);
	}

	// Output formatting for selection close to (sub)menu top and bottom 
	// (distance between selection and top/bottom defined as UPPER- and LOWER_SPACE)
	else
	{	// Top of (sub)menu
		if (selected < (from + UPPER_SPACE))
		{ 	uint8_t dif = till;
			till = from + (DISPLAY_ROWS - 1);
			for (from; from<=till; from++)
 			{	//BSP_GraphLCD_ClrLine(line_cnt);
 				if (from <= dif)
 				{
 		 			u8g_DrawStr(&u8g, 7*line_cnt, 10, menu[from].text);
 		 			u8g_NextPage(&u8g);
 				}
 	 	 		line_cnt++;
			}

			//BSP_GraphLCD_CharPixel((selected-(till-UPPER_SPACE)),0,SELECTION_CHAR);
		}
	
		// Bottom of (sub)menu
		if (selected == till)
		{	from = till - (DISPLAY_ROWS - 1);

			for (from; from<=till; from++)
 			{//	BSP_GraphLCD_ClrLine(line_cnt);
			//	BSP_GraphLCD_StringPos(line_cnt,0,menu[from].text);
 	 	 		line_cnt++;
			}

			//BSP_GraphLCD_CharPixel((DISPLAY_ROWS - 1),0,SELECTION_CHAR);
		}
	}

#endif	// !defined USE_ALWAYS_VISIBLE_HEADER

}

// ***********************************************************************
// Browse menu function
// ***********************************************************************
void browse_menu(void)
{
	uint32_t keyEntry;
	  u8g_InitComFn(&u8g, &u8g_dev_st7920_128x64_hw_spi, u8g_com_rx_hw_spi_fn);
	    /* flip screen, if required */
	//    u8g_SetRot180(&u8g);
	    /* assign default color value */
	    if ( u8g_GetMode(&u8g) == U8G_MODE_R3G3B2 )
	      u8g_SetColorIndex(&u8g, 255);     /* white */
	    else if ( u8g_GetMode(&u8g) == U8G_MODE_GRAY2BIT )
	      u8g_SetColorIndex(&u8g, 3);         /* max intensity */
	    else if ( u8g_GetMode(&u8g) == U8G_MODE_BW )
	      u8g_SetColorIndex(&u8g, 1);         /* pixel on */

	    u8g_FirstPage(&u8g);
	    u8g_prepare();
	    char s[2] = " ";
	    uint8_t x, y;
	    do{
	    u8g_DrawStr(&u8g, 0, 0, "ASCII page 1");
	    for( y = 0; y < 6; y++ ) {
	      for( x = 0; x < 16; x++ ) {
	    	 s[0] = y*16 + x + 32;
	        u8g_DrawStr(&u8g, x*7, y*10+10, s);
	      }
	    }
	    }while(u8g_NextPage(&u8g));
	do
	{
		//show_menu();
		xQueueReceive( qKeyboard, &keyEntry, portMAX_DELAY );
		switch (keyEntry)
		{
		case KEY_UP:
			selected = menu[selected].up;
			break;
		case KEY_DOWN:
			selected = menu[selected].down;
			break;
		case KEY_ENTER:
			if (menu[selected].fp != 0)
				menu[selected].fp();
			selected = menu[selected].enter;
			break;
		case USB_ENTER:
				selected = 30;
				break;
		default:
		}
	}while(1);
}
