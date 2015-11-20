/*
 * ut_state_choose_file.c
 *
 *  Created on: Nov 2, 2015
 *      Author: Fernando
 */

#include "FreeRTOS.h"
#include "task.h"
#include "r_tfat_lib.h"

#include "ut_context.h"
#include "ut_state.h"

#include "lcd_menu.h"
#include "lcd.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// ***********************************************************************
// Global variables
// ***********************************************************************
char gszCurFile[MAX_FILE_PATH_SIZE];

// ***********************************************************************
// Internal variables
// ***********************************************************************
#define MAX_EXT_AVAIL	3
static const char* gaszFileExtensions[MAX_EXT_AVAIL] =
{
		".g",
		".tap",
		".nc"
};

// ***********************************************************************
// Global types
// ***********************************************************************

typedef enum ut_fs_navigate_tag
{
	NAVIGATE_CANCELLED = -1,
	NAVIGATE_CONTINUE,
	NAVIGATE_END
} ut_fs_navigate;

// ***********************************************************************
// Global functions
// ***********************************************************************

static char * ut_strrstr(char *string, char *find)
{
  char *cp;
  uint32_t len = strlen(find);
  for (cp = string + len; cp >= string; cp--)
  {
    if (strncmp(cp, find, len) == 0)
        return cp;
  }
  return NULL;
}

/**
 * Checks if a string has a given suffix
 * @param szSuffix
 * @param szWord
 * @return true if szWord ends with szSuffix
 */
static bool endsWith(const char* szWord, const char* szSuffix)
{
	uint32_t wordLen = strlen(szWord);
	uint32_t suffixLen = strlen(szSuffix);

	if(wordLen >= suffixLen)
	{
		return (0 == memcmp(&szWord[wordLen - suffixLen], szSuffix, suffixLen));
	}

	return false;
}

/**
 * Checks if a word has a valid g extension
 * @param szWord
 * @return
 */
static bool validExtension(const char* szWord)
{
	uint8_t i;
	for(i = 0; i < MAX_EXT_AVAIL; i++)
	{
		if(endsWith(szWord, gaszFileExtensions[i])) return true;
	}

	return false;
}

/**
 * Scan files in a given directory and returns the selection.
 * If the selection is a directory, it does it recursively
 *
 * @return
 */
static ut_fs_navigate chooseFile()
{
	DIR	 st_usb_dir;
	FILINFO st_usb_finfo;
	FRESULT eRes;
	ut_menu filesMenu;
	char aszFiles[MENU_MAX_ITEMS][MAX_COLUMN + 1];

	/* Clean */
	memset(aszFiles, 0, MENU_MAX_ITEMS * (MAX_COLUMN + 1));

	/* Initialize menu */
	ut_menu_init(&filesMenu);
	/* No header */
	filesMenu.boShowTitle = false;
	filesMenu.maxItemsPerPage = MAX_ROW;

	/* Open dir */
	eRes = R_tfat_f_opendir(&st_usb_dir, gszCurFile);
	if(eRes == TFAT_FR_OK)
	{
		/* Check if it is on root */
		if(strlen(gszCurFile) > 0)
		{
			filesMenu.items[filesMenu.numItems].text = "..";
			filesMenu.numItems++;
		}

		/* Populate menu */
		while(true)
		{
			eRes = R_tfat_f_readdir(&st_usb_dir, &st_usb_finfo);
			/* Check for end of dir */
			if(eRes != TFAT_FR_OK || st_usb_finfo.fname[0] == 0) { break; }
			if(st_usb_finfo.fname[0] == '.') { continue; } /* Ignore dot entry */
			if(strstr(st_usb_finfo.fname, "SYSTEM")) { continue; }

			/* Copy to menu */
			if(st_usb_finfo.fattrib & TFAT_AM_DIR)
			{
				sprintf(aszFiles[filesMenu.numItems], "/%.*s", MAX_COLUMN, st_usb_finfo.fname);
			}
			else if(validExtension(st_usb_finfo.fname))
			{
				sprintf(aszFiles[filesMenu.numItems], "%.*s", MAX_COLUMN, st_usb_finfo.fname);
			}
			else
			{
				/* Not a valid file */
				continue;
			}

			filesMenu.items[filesMenu.numItems].text = aszFiles[filesMenu.numItems];
			filesMenu.numItems++;

			if(filesMenu.numItems == MENU_MAX_ITEMS) { break; }
		}

		/* Show menu */
		if(ut_menu_browse(&filesMenu, portMAX_DELAY) >= 0)
		{
			if(filesMenu.items[filesMenu.selectedItem].text[0] == '/')
			{
				/* Is a dir, recursively */
				strcat(gszCurFile, filesMenu.items[filesMenu.selectedItem].text);
				return NAVIGATE_CONTINUE;
			}
			else if(filesMenu.items[filesMenu.selectedItem].text[0] == '.')
			{
				/* It should rise up a level */
				char* last = ut_strrstr(gszCurFile, "/");
				*last = 0;
				return NAVIGATE_CONTINUE;
			}

			/* Is a file - end of routine */
			strcat(gszCurFile, filesMenu.items[filesMenu.selectedItem].text);
			return NAVIGATE_END;
		}
	}

	/* Operation was cancelled */
	gszCurFile[0] = 0;
	return NAVIGATE_CANCELLED;
}

// ***********************************************************************
// Public functions
// ***********************************************************************

/**
 * Choose G file from USB.
 * @param pContext
 * @return
 */
ut_state ut_state_choose_file(ut_context* pContext)
{
	FATFS  st_usb_fatfs;
	DIR	 st_usb_dir;
	FRESULT eRes;
	//char szFullPath[MAX_FILE_PATH_SIZE];

	/* Root dir */
	memset(gszCurFile, 0, sizeof(gszCurFile));
	strcpy(gszCurFile, USB_ROOT);

	/* Check if usb is mounted */
	eRes = R_tfat_f_opendir(&st_usb_dir, USB_ROOT);
	if(eRes != TFAT_FR_OK)
	{
		/* Ask for user to insert usb */
		ut_lcd_clear(SCREEN_HEADER_ID);
		ut_lcd_clear(SCREEN_MAIN_ID);
		ut_lcd_drawString(SCREEN_MAIN_ID, 1, 0, "  INSIRA USB  ", false);
		ut_lcd_output();

		/* Wait for USB to mount */
		ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
		eRes = R_tfat_f_mount(0, &st_usb_fatfs);
		eRes = R_tfat_f_opendir(&st_usb_dir, USB_ROOT);
	}

	/* Fat is mounted */
	ut_lcd_clear(SCREEN_MAIN_ID);

	/* Read */
	ut_lcd_enableFooter(false);
	ut_lcd_enableHeader(false);
	ut_lcd_output();

	/* Try to selected a file */
	ut_fs_navigate eErr;
	do
	{
		/* Just a delay */
		vTaskDelay(10 / portTICK_PERIOD_MS);
		/* Navigate through folders */
		eErr = chooseFile();
	} while(eErr == NAVIGATE_CONTINUE);

	/* Go back to menu */
	return STATE_MAIN_MENU;
}
