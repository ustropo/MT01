// ***********************************************************************
// LCD menu - HEADER
// ***********************************************************************

#ifndef __LCD_MENU
#define __LCD_MENU

#include "FreeRTOS.h"
#include "task.h"
#include "ut_state.h"

// ***********************************************************************
// Macros
// ***********************************************************************
#define MENU_MAX_ITEMS		20
#define MENU_MAX_ITEM_STR	32
#define ITEM_NO_MARKED  	0
#define ITEM_MARKED  		1

// ***********************************************************************
// Typedefs
// ***********************************************************************

/**
 * Type of menu item
 */
typedef enum
{
	ITEM_TYPE_BUTTON = 0,//!< MENU_TYPE_BUTTON
	ITEM_TYPE_BOOL,      //!< MENU_TYPE_BOOL
	ITEM_TYPE_INT        //!< MENU_TYPE_INT
} ut_menu_item_type;

typedef void (*ut_item_func_ptr)(uint8_t);

/**
 * Menu item struct
 */
typedef struct ut_menu_item_tag
{
	ut_menu_item_type st_type;
	uint8_t interval;
	uint8_t min;
	uint8_t max;
	uint32_t value;
	const char* text;
	ut_item_func_ptr callback_func;
} ut_menu_item;

/**
 *
 */
typedef struct ut_menu_tag
{
	ut_menu_item items[MENU_MAX_ITEMS];
	const char* title;
	uint16_t selectedItem;
	uint16_t numItems;
	uint16_t offset;
	uint8_t maxItemsPerPage;
	uint8_t boShowTitle;
	uint8_t itemMarked;
	ut_state currentState;
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

#endif // __LCD_MENU
