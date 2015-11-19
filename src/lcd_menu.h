// ***********************************************************************
// LCD menu - HEADER
// ***********************************************************************



#ifndef __LCD_MENU
#define __LCD_MENU
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


// ***********************************************************************
// Macros
// ***********************************************************************
// Define display type (outcomment other two)
#define USE_FOUR_ROW_DISPLAY
//#define USE_THREE_ROW_DISPLAY
//#define USE_TWO_ROW_DISPLAY

// Want an always visible floaty header?
//#define USE_ALWAYS_VISIBLE_HEADER

// Define char which gets shown as selection
#define SELECTION_CHAR	'>'

// Don't touch these
#if         defined USE_FOUR_ROW_DISPLAY

	#define DISPLAY_ROWS	8
	#define UPPER_SPACE 	7
	#define LOWER_SPACE 	1

#endif

#if			defined USE_THREE_ROW_DISPLAY
	
	#define DISPLAY_ROWS	3
	#define UPPER_SPACE 	1
	#define LOWER_SPACE 	1

#endif

#if			defined USE_TWO_ROW_DISPLAY
	
	#define DISPLAY_ROWS	2
	#define UPPER_SPACE 	1
	#define LOWER_SPACE 	0

#endif

// ***********************************************************************
// Typedefs
// ***********************************************************************
// Define single menu entry
typedef const struct MenuStructure
{	const char *text;
	unsigned char num_menupoints;
	unsigned char up;
	unsigned char down;
	unsigned char enter;
  	void ( *fp )( void );
	// unsigned char value;
}MenuEntry;

// ***********************************************************************
// Variable declarations
// ***********************************************************************
extern const MenuEntry menu[];


// ***********************************************************************
// Prototypes
// ***********************************************************************
extern void show_menu(void);
extern void browse_menu(void);

// ***********************************************************************
// Add User prototypes & variables
// ***********************************************************************



#endif // __LCD_MENU
