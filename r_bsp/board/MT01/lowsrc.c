/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No 
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all 
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, 
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM 
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES 
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS 
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of 
* this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer 
*
* Copyright (C) 2013 Renesas Electronics Corporation. All rights reserved.    
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : lowsrc.c
* Description  : Functions to support stream I/O
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 26.10.2011 1.00     First Release
*         : 15.01.2013 1.10     Added braces to if() statements. 
*         : 25.06.2013 1.20     Made minor changes to code to remove 'loss of precision' and 'unused parameter'
*                               compiler messages.
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "r_bsp_config.h"

/* Do not include this file if stdio is disabled in r_bsp_config. */
#if (BSP_CFG_IO_LIB_ENABLE == 1)

#include <string.h>
#include <stdio.h>
#include <stddef.h>

/* BCH - 01/15/2013 */
/* 1335: Function parameters identifiers are missing below. This is not going to be changed to keep this as close to 
         default lowsrc.c as possible.
   0602: Defect for macro names with '_[A-Z]' is also being suppressed since these are defaut names from toolchain.  
   3447: External linkage is not needed for these toolchain supplied library functions.
   2217: Line length could be changed but will not to keep file as close to original as possible. 
   3227: Not going to add const since this is toolchain library function. 
   3206: Not going to change parameters since this is toolchain library function.
   4130: Not going to change since this is provided toolchain library code.
   3397: Not going to change since this is provided toolchain library code.
   3400: Not going to change since this is provided toolchain library code. 
   0861: Not going to change since this is provided toolchain library code. 
   4107: Not going to change since this is provided toolchain library code. 
   3003: Not going to change since this is provided toolchain library code. 
   3777: Not going to change since this is provided toolchain library code. */
/* PRQA S 1335, 0602, 2217, 3447, 3227, 3206, 4130, 3397, 3400, 0861, 4107, 3003, 3777 ++ */

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/*Number of I/O Stream*/
#define IOSTREAM 20

/* file number */
#define STDIN  0                    /* Standard input (console)        */
#define STDOUT 1                    /* Standard output (console)       */
#define STDERR 2                    /* Standard error output (console) */

#define FLMIN  0                    /* Minimum file number     */
#define _MOPENR 0x1
#define _MOPENW 0x2
#define _MOPENA 0x4
#define _MTRUNC 0x8
#define _MCREAT 0x10
#define _MBIN   0x20
#define _MEXCL  0x40
#define _MALBUF 0x40
#define _MALFIL 0x80
#define _MEOF   0x100
#define _MERR   0x200
#define _MLBF   0x400
#define _MNBF   0x800
#define _MREAD  0x1000
#define _MWRITE 0x2000
#define _MBYTE  0x4000
#define _MWIDE  0x8000
/* File Flags */
#define O_RDONLY 0x0001 /* Read only                                       */
#define O_WRONLY 0x0002 /* Write only                                      */
#define O_RDWR   0x0004 /* Both read and Write                             */
#define O_CREAT  0x0008 /* A file is created if it is not existed          */
#define O_TRUNC  0x0010 /* The file size is changed to 0 if it is existed. */
#define O_APPEND 0x0020 /* The position is set for next reading/writing    */
                        /* 0: Top of the file 1: End of file               */

/* Special character code */
#define CR 0x0d                     /* Carriage return */
#define LF 0x0a                     /* Line feed       */

#if defined( __RX )
const long _nfiles = IOSTREAM; /* The number of files for input/output files */
#else
const int _nfiles = IOSTREAM;  /* The number of files for input/output files */
#endif
char flmod[IOSTREAM];          /* The location for the mode of opened file.  */

unsigned char sml_buf[IOSTREAM];

#define FPATH_STDIN     "C:\\stdin"
#define FPATH_STDOUT    "C:\\stdout"
#define FPATH_STDERR    "C:\\stderr"

/* H8 Normal mode ,SH and RX */
#if defined( __2000N__ ) || defined( __2600N__ ) || defined( __300HN__ ) || defined( _SH )
/* Output one character to standard output */
extern void charput(char);
/* Input one character from standard input */
extern char charget(void);
/* Output one character to the file        */
extern char fcharput(char, unsigned char);
/* Input one character from the file       */
extern char fcharget(char*, unsigned char);
/* Open the file */
extern char fileopen(char*, unsigned char, unsigned char*);
/* Close the file */
extern char fileclose(unsigned char);
/* Move the file offset */
extern char fpseek(unsigned char, long, unsigned char);
/* Get the file offset */
extern char fptell(unsigned char, long*);

/* RX */
#elif defined( __RX )
/* Output one character to standard output */
extern void charput(unsigned char);
/* Input one character from standard input */
extern unsigned char charget(void);

/* H8 Advanced mode */
#elif defined( __2000A__ ) || defined( __2600A__ ) || defined( __300HA__ ) || defined( __H8SXN__ ) || defined( __H8SXA__ ) || defined( __H8SXM__ ) || defined( __H8SXX__ )
/* Output one character to standard output */
extern void charput(char);
/* Input one character from standard input */
extern char charget(void);
/* Output one character to the file        */
extern char fcharput(char, unsigned char);
/* Input one character from the file       */
extern char fcharget(char*, unsigned char);
/* Open the file */
/* Specified as the number of register which stored paramter is 3 */
extern char __regparam3 fileopen(char*, unsigned char, unsigned char*);
/* Close the file */
extern char fileclose(unsigned char);
/* Move the file offset */
extern char fpseek(unsigned char, long, unsigned char);
/* Get the file offset */
extern char fptell(unsigned char, long*);

/* H8300 and H8300L */
#elif defined( __300__ ) || defined( __300L__ )
/* Output one character to standard output */
extern void charput(char);
/* Input one character from standard input */
extern char charget(void);
/* Output one character to the file        */
extern char fcharput(char, unsigned char);
/* Input one character from the file       */
extern char fcharget(char*, unsigned char);
/* Open the file */
/* Specified as the number of register which stored paramter is 3 */
extern char __regparam3 fileopen(char*, unsigned char, unsigned char*);
/* Close the file */
extern char fileclose(unsigned char);
/* Move the file offset */
/* Move the file offset */
extern char __regparam3 fpseek(unsigned char, long, unsigned char);
/* Get the file offset */
extern char fptell(unsigned char, long*);
#endif

#include <stdio.h>
FILE *_Files[IOSTREAM]; // structure for FILE
char *env_list[] = {            // Array for environment variables(**environ)
    "ENV1=temp01",
    "ENV2=temp02",
    "ENV9=end",
    '\0'                        // Terminal for environment variables
};

char **environ = env_list;

/* This macro is used to suppress compiler messages about a parameter not being used in a function. The nice thing
 * about using this implementation is that it does not take any extra RAM or ROM.
 */
#define     INTERNAL_NOT_USED(p)    ((void)(p))

/****************************************************************************/
/* _INIT_IOLIB                                                              */
/*  Initialize C library Functions, if necessary.                           */
/*  Define USES_SIMIO on Assembler Option.                                  */
/****************************************************************************/
void _INIT_IOLIB( void )
{
    /* A file for standard input/output is opened or created. Each FILE     */
    /* structure members are initialized by the library. Each _Buf member   */
    /* in it is re-set the end of buffer pointer.                           */

    /* Standard Input File                                                  */
    if( freopen( FPATH_STDIN, "r", stdin ) == NULL )
    {
        stdin->_Mode = 0xffff;  /* Not allow the access if it fails to open */
    }
    stdin->_Mode  = _MOPENR;            /* Read only attribute              */
    stdin->_Mode |= _MNBF;              /* Non-buffering for data           */
    stdin->_Bend = stdin->_Buf + 1;  /* Re-set pointer to the end of buffer */

    /* Standard Output File                                                 */
    if( freopen( FPATH_STDOUT, "w", stdout ) == NULL ) 
    {
        stdout->_Mode = 0xffff; /* Not allow the access if it fails to open */
    }
    stdout->_Mode |= _MNBF;             /* Non-buffering for data           */
    stdout->_Bend = stdout->_Buf + 1;/* Re-set pointer to the end of buffer */
    
    /* Standard Error File                                                  */
    if( freopen( FPATH_STDERR, "w", stderr ) == NULL )
    {
        stderr->_Mode = 0xffff; /* Not allow the access if it fails to open */
    }
    stderr->_Mode |= _MNBF;             /* Non-buffering for data           */
    stderr->_Bend = stderr->_Buf + 1;/* Re-set pointer to the end of buffer */
}

/****************************************************************************/
/* _CLOSEALL                                                                */
/****************************************************************************/
void _CLOSEALL( void )
{
    long i;

    for( i=0; i < _nfiles; i++ )
    {
        /* Checks if the file is opened or not                               */
        if( _Files[i]->_Mode & (_MOPENR | _MOPENW | _MOPENA ) )
        {
            fclose( _Files[i] );    /* Closes the file                       */
        }
    }
}

/**************************************************************************/
/*       open:file open                                                   */
/*          Return value:File number (Pass)                               */
/*                       -1          (Failure)                            */
/**************************************************************************/
#if defined( __RX )
long open(const char *name,                  /* File name                 */
     long  mode,                             /* Open mode                 */
     long  flg)                              /* Open flag                 */
#else
int open(char *name,                         /* File name                 */
     int  mode,                              /* Open mode                 */
     int  flg)                               /* Open flag                 */
#endif
{
    /* This code is only used to remove compiler info messages about these parameters not being used. */
    INTERNAL_NOT_USED(flg);

    if( strcmp( name, FPATH_STDIN ) == 0 )      /* Standard Input file?   */
    {
        if( ( mode & O_RDONLY ) == 0 )
        { 
            return -1;
        }
        flmod[STDIN] = (char)mode;
        return STDIN;
    }
    else if( strcmp( name, FPATH_STDOUT ) == 0 )/* Standard Output file?  */
    {
        if( ( mode & O_WRONLY ) == 0 )
        { 
            return -1;
        }
        flmod[STDOUT] = (char)mode;
        return STDOUT;
    }
    else if(strcmp(name, FPATH_STDERR ) == 0 )  /* Standard Error file?   */
    {
        if( ( mode & O_WRONLY ) == 0 ) 
        {
            return -1;
        }
        flmod[STDERR] = (char)mode;
        return STDERR;
    }
    else 
    {
        return -1;                             /*Others                  */
    }
}

#if defined( __RX )
long close( long fileno )
#else
int close( int fileno )
#endif
{
    /* This code is only used to remove compiler info messages about these parameters not being used. */
    INTERNAL_NOT_USED(fileno);

    return 1;
}

/**************************************************************************/
/* write:Data write                                                       */
/*  Return value:Number of write characters (Pass)                        */
/*               -1                         (Failure)                     */
/**************************************************************************/
#if defined( __RX )
long write(long  fileno,             /* File number                       */
      const unsigned char *buf,       /* The address of destination buffer */
      long  count)                   /* The number of chacter to write    */
#else
int write(int  fileno,               /* File number                       */
      char *buf,                     /* The address of destination buffer */
      int  count)                    /* The number of chacter to write    */
#endif
{
    long    i;                          /* A variable for counter         */
    unsigned char    c;                 /* An output character            */

    /* Checking the mode of file , output each character                  */
    /* Checking the attribute for Write-Only, Read-Only or Read-Write     */
    if((flmod[fileno]&O_WRONLY) || (flmod[fileno]&O_RDWR))
    {
        if( fileno == STDIN ) 
        {
            return -1;            /* Standard Input     */
        }
        else if( (fileno == STDOUT) || (fileno == STDERR) ) 
                                                /* Standard Error/output   */
        {
            for( i = count; i > 0; --i )
            {
                c = *buf++;
                charput(c);
            }
            return count;        /*Return the number of written characters */
        }
        else 
        {
            return -1;                  /* Incorrect file number          */
        }
    }
    else 
    {
        return -1;                      /* An error                       */
    }
}

#if defined( __RX )
long read( long fileno, unsigned char *buf, long count )
#else
int read( int fileno, char *buf, unsigned int count )
#endif
{
       long i;

       /* Checking the file mode with the file number, each character is input and stored the buffer */

       if((flmod[fileno]&_MOPENR) || (flmod[fileno]&O_RDWR))
       {
             for(i = count; i > 0; i--)
             {
                   *buf = charget();
                   if(*buf==CR)
                   {              /* Replace the new line character */
                         *buf = LF;
                   }
                   buf++;
             }
             return count;
       }
       else 
       {
             return -1;
       }
}

#if defined( __RX )
long lseek( long fileno, long offset, long base )
#else
long lseek( int fileno, long offset, int base )
#endif
{
    /* This code is only used to remove compiler info messages about these parameters not being used. */
    INTERNAL_NOT_USED(fileno);
    INTERNAL_NOT_USED(offset);
    INTERNAL_NOT_USED(base);

    return -1L;
}

#endif /* BSP_CFG_IO_LIB_ENABLE */
