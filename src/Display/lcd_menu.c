// ***********************************************************************
// LCD menu
// ***********************************************************************

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//************************************************************************
// Projekt includes						// Include your own functions here
//************************************************************************

#include "FreeRTOS.h"
#include "task.h"
#include "keyboard.h"

#include "lcd_menu.h"
#include "lcd.h"

// ***********************************************************************
// Variable definitions
// ***********************************************************************

// ***********************************************************************
// Init menu function
// ***********************************************************************
void ut_menu_init(ut_menu* menu_ptr)
{
	memset((void *)menu_ptr, 0, sizeof(ut_menu));

	menu_ptr->maxItemsPerPage = MAX_ROW - 2;
	menu_ptr->boShowTitle = true;
}

/**
 *
 */
void ut_menu_go_up(ut_menu* menu_ptr)
{
	if(menu_ptr->selectedItem > 0)
	{
		/* If selection is the first item in screen */
		if(menu_ptr->selectedItem == menu_ptr->offset && menu_ptr->offset > 0)
		{
			menu_ptr->offset--;
		}
		menu_ptr->selectedItem--;
	}
}

/**
 *
 */
void ut_menu_go_down(ut_menu* menu_ptr)
{
	/* Check if it is last position */
	if(menu_ptr->selectedItem < (menu_ptr->numItems - 1))
	{
		/* Last screen position and selected item */
		if((menu_ptr->selectedItem == (menu_ptr->offset + menu_ptr->maxItemsPerPage - 1)) &&
				((menu_ptr->offset + menu_ptr->maxItemsPerPage) < menu_ptr->numItems))
		{
			menu_ptr->offset++;
		}

		menu_ptr->selectedItem++;
	}

}

// ***********************************************************************
// Show menu function
// ***********************************************************************
void ut_menu_show(ut_menu* menu_ptr)
{
	uint8_t index = 0;

	/* Title handling */
	if(menu_ptr->boShowTitle)
	{
		ut_lcd_enableHeader(true);
		/* Clear previous data */
		ut_lcd_clear(SCREEN_HEADER_ID);
		/* Copy data */
		ut_lcd_drawString(SCREEN_HEADER_ID, 0, 0, menu_ptr->title, true);
	}
	else
	{
		ut_lcd_enableHeader(false);
	}

	/* Clear previous data */
	ut_lcd_clear(SCREEN_MAIN_ID);
	/* Items */
	uint8_t menuItem = menu_ptr->offset;
	for(index = 0; (menuItem < menu_ptr->numItems) && (index < menu_ptr->maxItemsPerPage); index++, menuItem++)
	{
		ut_lcd_drawString(SCREEN_MAIN_ID, index, 0, menu_ptr->items[menuItem].text, (menuItem == menu_ptr->selectedItem));
	}

	/* Put on screen */
	ut_lcd_output();
}

/**
 *
 * @param menu_ptr
 * @param timeout
 * @return
 */
int8_t ut_menu_browse(ut_menu* menu_ptr, uint32_t timeout)
{
	uint32_t keyEntry;

	/* show menu */
	ut_menu_show(menu_ptr);
	/* Wait for keyboard */
	while(xQueueReceive( qKeyboard, &keyEntry, timeout ))
	{
		/* Check which key */
		switch (keyEntry)
		{

		case KEY_DOWN:
			ut_menu_go_down(menu_ptr);
			break;

		case KEY_UP:
			ut_menu_go_up(menu_ptr);
			break;

		case KEY_ENTER:
			/* Callback function, if any */
			if(menu_ptr->items[menu_ptr->selectedItem].callback_func)
			{
				menu_ptr->items[menu_ptr->selectedItem].callback_func(menu_ptr->selectedItem);
			}
			return menu_ptr->selectedItem;

		case KEY_ESC:
			return -1;

		default:
			break;
		}

		/* Show again */
		ut_menu_show(menu_ptr);
	}

	/* Timeout */
	return -1;
}
