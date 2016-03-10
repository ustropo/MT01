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
#include "interpreter_if.h"

#include "lcd_menu.h"
#include "lcd.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// ***********************************************************************
// Global variables
// ***********************************************************************
FATFS  st_usb_fatfs;
DIR	   st_usb_dir;
char gszCurFile[MAX_FILE_PATH_SIZE];
extern bool drivemountFlag;

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

static const char* gszFileMenuTitle = "ESCOLHA UM ARQUIVO";

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

/**
 * Get first occurrence of find in string in reverse mode.
 * It is the last occurrence of find in string.
 *
 * @param string	String to be searched.
 * @param find		String to search.
 * @return Pointer to the last occurrence.
 */
static char * ut_strrstr(char *string, char *find)
{
  char *cp;
  uint32_t len = strlen(find);
  for (cp = string + strlen(string); cp >= string; cp--)
  {
    if (strncmp(cp, find, len) == 0)
    {
    	return cp;
    }
  }
  return NULL;
}

/**
 * Checks if a word has a valid g extension
 * @param szWord
 * @return
 */
static bool validExtension(const char* szWord)
{
	uint8_t i;
	uint32_t wordLen = strlen(szWord);
	uint32_t suffixLen = 0;

	for(i = 0; i < MAX_EXT_AVAIL; i++)
	{
		suffixLen = strlen(gaszFileExtensions[i]);
		if(wordLen >= suffixLen && !memcmp(&szWord[wordLen - suffixLen], gaszFileExtensions[i], suffixLen))
		{
			return true;
		}
	}

	return false;
}

/**
 * Sort array in ascending order.
 * @param pbArray	Array to be sorted.
 * @param iLen		Array length.
 */
static void sort_array(char* pbArray, int iLen)
{
	char tmp[MAX_COLUMN + 1];
	char* pbStrNxt;
	int j;
	int i;

	/* Sorting array */
	for(i = 0; i < iLen; ++i)
	{
		/* Next element */
		pbStrNxt = pbArray + (MAX_COLUMN + 1);
		for(j = i + 1; j < iLen; ++j)
		{
			/* Swap */
			if(strcmp(pbArray, pbStrNxt) > 0)
			{
				strncpy(tmp, pbArray, MAX_COLUMN);
				strncpy(pbArray, pbStrNxt, MAX_COLUMN);
				strncpy(pbStrNxt, tmp, MAX_COLUMN);
			}

			pbStrNxt += MAX_COLUMN + 1;
		}

		/* Next element */
		pbArray += MAX_COLUMN + 1;
	}
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
	uint8_t i;
	char aszFiles[MENU_MAX_ITEMS][MAX_COLUMN + 1];

	/* Clean */
	memset(aszFiles, 0, MENU_MAX_ITEMS * (MAX_COLUMN + 1));

	/* Initialize menu */
	ut_menu_init(&filesMenu);
	/* No header */
	filesMenu.title = gszFileMenuTitle;
	filesMenu.boShowTitle = true;
	filesMenu.maxItemsPerPage = MAX_ROW;

	/* Open dir */
	eRes = R_tfat_f_opendir(&st_usb_dir, gszCurFile);
	if(eRes == TFAT_FR_OK)
	{
		/* Check if it is on root */
		if(strlen(gszCurFile) > 0)
		{
			sprintf(aszFiles[filesMenu.numItems++], "..");
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

			if(++filesMenu.numItems == MENU_MAX_ITEMS) { break; }
		}

		/* Sort */
		sort_array(aszFiles[0], filesMenu.numItems);
		/* Fill menu */
		for(i = 0; i < filesMenu.numItems; ++i)
		{
			filesMenu.items[i].text = aszFiles[i];
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
				if(last != 0) *last = 0;
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
	FRESULT eRes;

	/* Root dir */
	memset(gszCurFile, 0, sizeof(gszCurFile));
	strcpy(gszCurFile, USB_ROOT);

	if (drivemountFlag)
	{
	/* Check if usb is mounted */
	eRes = R_tfat_f_opendir(&st_usb_dir, USB_ROOT);
	}
	else
	{
		ut_lcd_output_warning("NENHUM USB\n\
							   ENCONTRADO\n");

		vTaskDelay(1000 / portTICK_PERIOD_MS);

		return STATE_MAIN_MENU;
	}

	/* Fat is mounted */
	ut_lcd_clear();

	/* Read */
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
