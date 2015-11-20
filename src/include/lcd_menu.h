// ***********************************************************************
// LCD menu - HEADER
// ***********************************************************************

#ifndef __LCD_MENU
#define __LCD_MENU

#include "FreeRTOS.h"
#include "task.h"

// ***********************************************************************
// Macros
// ***********************************************************************
#define MENU_MAX_ITEMS		6
#define MENU_MAX_ITEM_STR	32

// ***********************************************************************
// Typedefs
// ***********************************************************************

typedef void (*ut_item_func_ptr)(uint8_t);

/**
 *
 */
typedef struct
{
	const char* text;
	ut_item_func_ptr callback_func;
} ut_menu_item;

/**
 *
 */
typedef struct
{
	ut_menu_item items[MENU_MAX_ITEMS];
	const char* title;
	uint8_t selectedItem;
	uint8_t numItems;
	uint8_t offset;
	uint8_t maxItemsPerPage;
	uint8_t boShowTitle;
} ut_menu;

// ***********************************************************************
// Variable declarations
// ***********************************************************************


// ***********************************************************************
// Prototypes
// ***********************************************************************

extern void ut_menu_go_up(ut_menu* menu_ptr);
extern void ut_menu_go_down(ut_menu* menu_ptr);
extern void ut_menu_init(ut_menu* menu_ptr);
extern void ut_menu_show(ut_menu* menu_ptr);
extern int8_t ut_menu_browse(ut_menu* menu_ptr, uint32_t timeout);

// ***********************************************************************
// Add User prototypes & variables
// ***********************************************************************

#endif // __LCD_MENU
