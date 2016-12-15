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
#include "interpreter_if.h"

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

	menu_ptr->maxItemsPerPage = MAX_ROW - 1;
//	menu_ptr->maxItemsPerPage = MAX_ROW;
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
	uint8_t row = 0;

	ut_lcd_clear();
	/* Title handling */
	if(menu_ptr->boShowTitle)
	{
		/* Clear previous data */
	//	ut_lcd_clear();
		ut_lcd_clear_str();
		/* Copy data */
	//	ut_lcd_drawString(row++, 0, menu_ptr->title, true);

		ut_lcd_drawStr(row++, 0, menu_ptr->title, BACKGROUND_FRAMED,ITEM_NO_MARKED,u8g_font_helvB08);

	}

	/* Items */
	uint8_t menuItem = menu_ptr->offset;
	for(; (menuItem < menu_ptr->numItems) && (row < (menu_ptr->maxItemsPerPage+1)); row++, menuItem++)
	{
	//	ut_lcd_drawString(row, 0, menu_ptr->items[menuItem].text, (menuItem == menu_ptr->selectedItem));
		ut_lcd_drawStr(row, 0, menu_ptr->items[menuItem].text, (menuItem == menu_ptr->selectedItem),(menu_ptr->itemMarked == row),u8g_font_6x10);
	}

	/* Put on screen */
	//ut_lcd_output();
	ut_lcd_output_str(menu_ptr->currentState);
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
	while(xQueueReceive( qKeyboard, &keyEntry, timeout ) && menu_ptr->selectedItem < menu_ptr->numItems)
	{
		/* Check which key */
		switch (keyEntry)
		{
		case KEY_DOWN:
			iif_func_down();
			ut_menu_go_down(menu_ptr);
			break;

		case KEY_UP:
			iif_func_up();
			ut_menu_go_up(menu_ptr);
			break;

		case KEY_RIGHT:
			iif_func_right();
			break;

		case KEY_LEFT:
			iif_func_left();
			break;

		case KEY_Z_UP:
			iif_func_zup();
			break;

		case KEY_Z_DOWN:
			iif_func_zdown();
			break;

		case KEY_ENTER:
			/* Callback function, if any */
			iif_func_enter();
			if(menu_ptr->items[menu_ptr->selectedItem].callback_func)
			{
				menu_ptr->items[menu_ptr->selectedItem].callback_func(menu_ptr->selectedItem);
			}
			return menu_ptr->selectedItem;

		case KEY_ESC:
			iif_func_esc();
			return -1;

		case KEY_RELEASED:
			iif_func_released();
			break;

		default:
			break;
		}

		/* Show again */
		ut_menu_show(menu_ptr);
	}

	/* Timeout */
	return -2;
}
