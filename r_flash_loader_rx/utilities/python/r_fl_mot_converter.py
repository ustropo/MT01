#!/usr/bin/env python
'''
/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only 
* intended for use with Renesas products. No other uses are authorized. This 
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE 
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS 
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE 
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer *
* Copyright (C) 2013 Renesas Electronics Corporation. All rights reserved.    
*******************************************************************************/
/****************************************************************************
* File Name		: FL_MOT_Converter.py
* Description   : This application takes an S-Record file and converts it into
*                 a Load Image to be used with the Renesas FlashLoader project.
*                 The code is setup so that if you are just wanting to change
*                 the size of entries in a structure then the only thing
*                 you need to change is the number associated with the 
*                 entry in the dictionaries below (FL_LI_FORMAT and 
*                 FL_BH_FORMAT).  You can edit the image header valid_mask
*                 value using the -m option. You can edit the block header
*                 valid_mask value by changing FL_BH_VALID_MASK.  If you
*                 want to edit more than the size of entries then you will
*                 need to dig in deeper.
*
* Notes         : For seeing the format of a S-Record file I recommend looking
*                 here : http://www.amelek.gda.pl/avr/uisp/srecord.htm
******************************************************************************/  
/******************************************************************************
* History 		: MM.DD.YYYY Version Information
*               : 03.18.2010 Ver. 1.00 First Release - BCH
*               : 09.20.2010 Ver. 2.00 Cleaned up code, made easier to 
*                                      modify - BCH
*               : 03.01.2012 Ver. 3.00 Added '-m' option to set the app's 
*                                      valid mask value. The default value is 
*                                      0xAA. If the read value does not match
*                                      the expected value then an error 
*                                      message is output.
******************************************************************************/
'''
#Used for getting input arguments and exiting
import sys
#Used to split extension off of the input filename (that's it)
import os
#Used for string operations
import string
#binascii is used for converting from ASCII to binary
import binascii
#crcmod is used for doing the CRC16
import crcmod
#Used for unpacking FL_CurAppHeader from S-Record file (like C style struct)
from struct import *
#This allows us to define the 'struct' that we use later
from collections import namedtuple

#This is the overall class that processes an S-Record file
class FL_MOT_Converter:
    
    #Number of bytes used for checksum in S-Record file
    CHECKSUM_BYTES = 1

    #If the user wants to change the size of the fields in the Load Image Header or Data Block
    #Header then they can do this below.  There is a Python dictionary for each header below
    #with the name of the structure entry and the number of bytes associated with that entry.
    #If for instance you wanted to change the 'image_id' field to be 2 bytes you would change
    #the FL_LI_FORMAT definition below to 'image_id':2

    #An array for holding Load Image size format
    FL_LI_FORMAT = {'valid_mask':1, 'image_ID':1, 'version_num':1, 'load_image_size':4, 'max_block_size':4, 'image_crc':2, 'raw_crc':2, 'start_address':4, 'successfully_stored':4 }
    #Number of bytes in Load File Header
    FL_LI_STRUCT_SIZE = sum([i for i in FL_LI_FORMAT.values()])

    #An array for holding Block Header size format
    FL_BH_FORMAT = {'valid_mask':1, 'sequence_ID':2, 'flash_address':4, 'data_size':4, 'data_crc':2, 'next_block_address':4}
    #Number of bytes in Data Block Header
    FL_BH_STRUCT_SIZE = sum([i for i in FL_BH_FORMAT.values()])    

    #Other defines for Load Image Headers and Data Block Headers
    FL_BH_VALID_MASK = "BB"
        
    #Holds current sequence number for record
    sequence_number = 0

    #Initializer function for this class.  It takes in parameters
    #and initialilzes class variables.  It also configures the CRC
    #calculator that will be used.
    def __init__(self, input_file, output_file, max_rec_size, fill_space, header_loc, in_valid_mask):
        self.mot_filename = input_file
        self.out_filename = output_file
        self.max_block_size = max_rec_size
        self.max_fill_space = fill_space
        self.header_location = header_loc
        self.input_valid_mask = in_valid_mask
        self.header_bytes_left = self.FL_LI_STRUCT_SIZE
        self.fileheader = ""
        self.filesize = 0
        #Used for CRC - CCITT - x^16 + x^12 + x^5 + 1
        self.g16 = 0x11021
        self.crc_init = 0x1D0F
        #CRC used for the entire file - Image CRC
        self.file_crc = crcmod.Crc(self.g16, self.crc_init, 0)
        #CRC used for each block
        self.crc = crcmod.mkCrcFun(self.g16,self.crc_init,0)       
        
    #If an error is found in the S-Record file then this function is called
    def found_error(self):
        print "Each line in a S-Record should start with a 'S'"
        print "The file you input had a line that started without"
        print "an 'S'.  Please check to make sure you have a valid"
        print "S-Record file."
        sys.exit()

    #This function packages up a Data Block and writes it to the output file
    def write_record(self,output_file, current_buffer, msb_start_address):
        
        #Print valid mask 0xBB
        write_str = binascii.unhexlify(self.switch_endian(self.FL_BH_VALID_MASK))
        
        #Write Sequence ID
        msb_sequence_num = ("%0" + str(self.FL_BH_FORMAT['sequence_ID']*2) + "x") % self.sequence_number
        #Switch to LSB
        lsb_sequence_num = self.switch_endian(msb_sequence_num)
        #Write 'sequence_ID' to file
        write_str += binascii.unhexlify(lsb_sequence_num)
        #increment sequence number        
        self.sequence_number += 1
        
        #Print start address for block
        #Pad address if not correct number of chars
        msb_start_address = ("0"*((self.FL_BH_FORMAT['flash_address']*2) - len(msb_start_address))) + msb_start_address
        #Switch to LSB
        lsb_start_address = self.switch_endian(msb_start_address)
        #Write 'flash_address' field to file
        write_str += binascii.unhexlify(lsb_start_address)
        
        #Print size of data block in bytes        
        msb_size_string = ("%0" + str(self.FL_BH_FORMAT['data_size']*2) + "x") % (len(current_buffer)/2)
        #Switch to LSB
        lsb_size_string = self.switch_endian(msb_size_string)
        #Write 'data_size' field to file
        write_str += binascii.unhexlify(lsb_size_string)
                          
        #Print CRC of data - Using CCITT - x^16 + x^12 + x^5 + 1        
        msb_crc_out = ("%0" + str(self.FL_BH_FORMAT['data_crc']*2) + "x") % self.crc(binascii.unhexlify(current_buffer))
        #Switch to LSB
        lsb_crc_out = msb_crc_out[2:] + msb_crc_out[:2]
        #Write 'data_crc' to file
        write_str += binascii.unhexlify(lsb_crc_out)
        
        #Print empty record for MCU to fill in for 'next block header address'
        #The 'join' command below will join strings of FF together to make FFFF...
        #for however many bytes I need
        write_str += binascii.unhexlify(''.join(self.FL_BH_FORMAT['next_block_address']*['FF']))
        
        #Print data
        write_str += binascii.unhexlify(current_buffer)
        
        #Write new block to file
        output_file.write(write_str)
        
        #Update file CRC
        self.file_crc.update(write_str)
        
        #Update filesize
        self.filesize += (len(current_buffer)/2) + self.FL_BH_STRUCT_SIZE
    
    #This function handles switching MSB to LSB and vice versa
    def switch_endian(self,temp):
        #Check to see if argument is 1 byte long, if so send it back
        if(len(temp) == 1):
            return temp
        #Check to make sure length is even
        if(len(temp)%2 == 1):
            print 'Switching endian failed.  Input should always have even number length'
            sys.exit()
        #Switch endian
        temp_length = len(temp)
        #Do first iteration
        temp_length = temp_length - 2
        switched = temp[temp_length:]
        while(temp_length > 0):
            #Append next byte
            switched = switched + temp[temp_length-2:temp_length]
            temp_length = temp_length - 2
        return switched
    
    #This function is called to process the S-Record file
    def Process(self):
        
        #Open input file
        try:
            mot_file = open(self.mot_filename, "r")
        except:
            print 'Error opening input file ' , self.mot_filename
            sys.exit()
        
        #Open a new file for output
        try:
            out_file = open(self.out_filename, "wb")
        except:
            print 'Error opening output file ' , self.out_filename
            sys.exit()
        
        #Write as much of the load file header as we can.  We'll
        #come back and write the rest at the end.
        #Print holders for Valid Mask, Image ID, Version #, Size of Load Image
        #Using .join() to make variable length string of all F's
        out_file.write(binascii.unhexlify(''.join((self.FL_LI_FORMAT['valid_mask'] 
                                                   + self.FL_LI_FORMAT['image_ID'] 
                                                   + self.FL_LI_FORMAT['version_num'] 
                                                   + self.FL_LI_FORMAT['load_image_size'])
                                                  *['FF'])))
        #Print max block header size
        msb_max_block_size = ("%0" + str(self.FL_LI_FORMAT['max_block_size']*2) + "x") % self.max_block_size
        #Switch to LSB
        lsb_max_block_size = self.switch_endian(msb_max_block_size)
        #Write out 'max_block_size'
        out_file.write(binascii.unhexlify(lsb_max_block_size))

        #Print holders for Image CRC, Raw CRC, Address in Ext Memory,
        #and Successfully Stored.  Using .join() again
        out_file.write(binascii.unhexlify(''.join((self.FL_LI_FORMAT['image_crc'] 
                                                   + self.FL_LI_FORMAT['raw_crc'] 
                                                   + self.FL_LI_FORMAT['start_address']
                                                   + self.FL_LI_FORMAT['successfully_stored'])
                                                  *['FF'])))
        
        #Process each line in the file
        prev_address = 0
        address = 0
        num_bytes = 0
        start_address = ""
        cur_buffer = ""
        prev_num_bytes = 0
        cur_num_bytes = 0
        for line in mot_file:
            #Test to see if each line starts with 'S'
            if line.startswith('S') == False:
                self.found_error()            
            
            #Get address for this line
            #S3 means 4-byte address
            if line.startswith('S3') == True:        
                address_start_byte = 4
                data_start_byte = 12
                address_size_bytes = 4
            #S2 means 3-byte address
            elif line.startswith('S2') == True:        
                address_start_byte = 4
                data_start_byte = 10
                address_size_bytes = 3
            #S1 means 2-byte address
            elif line.startswith('S1') == True:        
                address_start_byte = 4
                data_start_byte = 8
                address_size_bytes = 2
            #You can add more elif statements here for handling other
            #S-Records.  There are S0-S9.  I only handle the ones I need
            else:
                continue
                
            #Read the address for this S-Record line
            address = int(line[address_start_byte:data_start_byte],16)
            
            #Get number of bytes on the line
            cur_num_bytes = int(line[2:address_start_byte],16)
            
            #Get number of bytes between this record and last (0 means they are sequential)
            bytes_between = address - prev_address - prev_num_bytes + (address_size_bytes + self.CHECKSUM_BYTES)
            
            #Get file header if this is the place for it
            if address <= self.header_location and self.header_location < (address + cur_num_bytes - address_size_bytes - self.CHECKSUM_BYTES):
                #All or part of the file header is in this buffer
                
                #How far into buffer does the file header start
                offset_in_buffer = self.header_location - address
                
                #How many bytes are left after the start of the file load header in buffer
                buffer_bytes_left = (address + cur_num_bytes - address_size_bytes - self.CHECKSUM_BYTES) - self.header_location
                
                if  buffer_bytes_left >= self.header_bytes_left:
                    #We can get the whole (or rest) of the file header now
                    self.fileheader += line[data_start_byte+(offset_in_buffer*2):data_start_byte+(offset_in_buffer*2)+(self.header_bytes_left*2)]
                    
                    self.header_bytes_left = 0
                else:
                    #We can only get part of the file header this time
                    self.fileheader += line[data_start_byte+(offset_in_buffer*2):data_start_byte+(offset_in_buffer*2)+(buffer_bytes_left*2)]
                    
                    self.header_bytes_left -= buffer_bytes_left
                    self.header_location += buffer_bytes_left
        
            #Check if first line of file
            if len(cur_buffer) == 0:
                cur_buffer += line[data_start_byte:len(line)-((self.CHECKSUM_BYTES*2)+1)]
                #Get start address
                start_address = line[address_start_byte:data_start_byte]
            #Check to see if address is sequential or within max_fill_space
            elif bytes_between <= self.max_fill_space and (len(cur_buffer)/2) + bytes_between + 1 < self.max_block_size:
                #Add 0xFF's in empty space to join S-Records that are not sequential
                if(bytes_between > 0):
                    while(bytes_between > 0):
                        cur_buffer += 'FF'
                        bytes_between -= 1
                #Check to see if adding this record will go over the max Data size, if so split it
                if((len(cur_buffer)/2) + cur_num_bytes - address_size_bytes - self.CHECKSUM_BYTES > self.max_block_size):
                    num_bytes_left = self.max_block_size - (len(cur_buffer)/2)
                    #Multiple num_bytes_left by 2 since data is in ASCII hex
                    cur_buffer += line[data_start_byte:data_start_byte + (2*num_bytes_left)]
                    #Output previous record
                    self.write_record(out_file, cur_buffer, start_address)
                    #Start new record header
                    cur_buffer = line[data_start_byte + (2*num_bytes_left):len(line)-((self.CHECKSUM_BYTES*2)+1)]
                    #Get new start address
                    if(address_size_bytes == 4):
                        start_address = "%08x" % (address + num_bytes_left)
                    elif(address_size_bytes == 3):
                        start_address = "%06x" % (address + num_bytes_left)
                    else:
                        start_address = "%04x" % (address + num_bytes_left)
                else:
                    cur_buffer += line[data_start_byte:len(line)-((self.CHECKSUM_BYTES*2)+1)]
            #If not sequential, and not first line, then this is new block
            else:
                #Useful debug printout
                #Print 'new record ' + hex(address) + ' difference is ' + str(address - prev_address - prev_num_bytes + (address_size_bytes + self.CHECKSUM_BYTES))

                #Output previous record
                self.write_record(out_file, cur_buffer, start_address)                        
                
                #Start new record header
                cur_buffer = line[data_start_byte:len(line)-((self.CHECKSUM_BYTES*2)+1)]
                #Get new start address
                start_address = line[address_start_byte:data_start_byte]
            
            #Update for next line
            prev_num_bytes = cur_num_bytes
            
            #Update previous address so you can check if next S-Record is sequential
            prev_address = address
            
        #Output last buffer, if there is one
        if(len(cur_buffer) > 0):
            #output previous record
            self.write_record(out_file, cur_buffer, start_address)
            
        #Check to make sure LoadFileHeader was found
        if self.header_bytes_left > 0:
            print 'Error - The Load Image Header was not found for this application.'
            print 'Look at structure of Load Image Header for what is supposed to be found'
            sys.exit()
        else:
            #Process file header and write to file
            self.ProcessHeader(out_file)        
        
        #Close output file
        out_file.close()
        
        print "S-Record file converted successfully."
        print "Output file is " + self.out_filename
        print "Size of entire Load Image is " + str(self.filesize) + " bytes"

    #Not all of the information for the Load Image Header is known when we first start processing
    #the file.  This function is called after the file is processed so that we know all the 
    #information we need (image_ID, version_num, file_crc)
    def ProcessHeader(self, output_file):
        #This is used to 'define a structure' so that we can take the data and split it
        #into its individual parts easily.  This would be similar to having a structure pointer
        #in C and pointing it to a block of memory that you knew represented a structure.  The
        #entries in this string need to be in the same exact order as you have in the 
        #C structure.  For instance 'valid_mask' is the first entry and 'image_ID' is the 2nd.
        FL_Struct_LI = 'valid_mask image_ID version_num load_image_size max_block_size image_crc raw_crc start_address successfully_stored'

        #This builds the format string needed. B = byte, H = 2 bytes, L = 4 bytes per entry.
        #Output will produce something like this '<BBBLLHHLL'
        FL_LI_FORMAT_STRING = "<"
        for entry in FL_Struct_LI.split(' '):
            if self.FL_LI_FORMAT[entry] == 1:
                FL_LI_FORMAT_STRING += 'B'
            elif self.FL_LI_FORMAT[entry] == 2:
                FL_LI_FORMAT_STRING += 'H'
            elif self.FL_LI_FORMAT[entry] == 4:
                FL_LI_FORMAT_STRING += 'L'
            else:
                print 'Error - This code only supports even sized structure objects for Load Image Headers and Data Block Headers';
                sys.exit()        

        #Use string to define entries in structure
        LoadImageHeader = namedtuple('LoadImageHeader', FL_Struct_LI)
        #Use structure to get values from the data 'blob'
        my_header = LoadImageHeader._make(unpack(FL_LI_FORMAT_STRING,binascii.unhexlify(self.fileheader)))

        #Check Valid Mask to make sure this is actually a valid header
        if my_header.valid_mask != self.input_valid_mask:
            print 'Error - Valid mask in Application Header did not match the value it was supposed to be.'
            print 'Expected Value = ' + hex(self.input_valid_mask) + " Actual Value = " + hex(my_header.valid_mask)
            sys.exit()
        
        #Go back and write the Load File Header
        output_file.seek(0)
        #Write valid mask
        output_file.write(binascii.unhexlify(self.switch_endian(("%0" + str(self.FL_LI_FORMAT['valid_mask']*2) + "x") % my_header.valid_mask)))
        #Write Image ID
        output_file.write(binascii.unhexlify(self.switch_endian(("%0" + str(self.FL_LI_FORMAT['image_ID']*2) + "x") % my_header.image_ID)))
        #Write Version Number
        output_file.write(binascii.unhexlify(self.switch_endian(("%0" + str(self.FL_LI_FORMAT['version_num']*2) + "x") % my_header.version_num)))

        #We need to switch the endian on these next entries because they are MSB on the PC
        #Add LoadImageHeader size to filesize        
        self.filesize += self.FL_LI_STRUCT_SIZE
        output_file.write(binascii.unhexlify(self.switch_endian(("%0" + str(self.FL_LI_FORMAT['load_image_size']*2) + "x") % self.filesize)))
        
        #Write Max Block Size
        output_file.write(binascii.unhexlify(self.switch_endian(("%0" + str(self.FL_LI_FORMAT['max_block_size']*2) + "x") % self.max_block_size)))
        
        #Write Image CRC
        output_file.write(binascii.unhexlify(self.switch_endian(("%0" + str(self.FL_LI_FORMAT['image_crc']*2) + "x") % self.file_crc.crcValue)))
        
        #Write raw CRC
        output_file.write(binascii.unhexlify(self.switch_endian(("%0" + str(self.FL_LI_FORMAT['raw_crc']*2) + "x") % my_header.raw_crc)))

if __name__ == '__main__':
    from optparse import OptionParser
    
    parser = OptionParser(
        description = "FlashLoader S-Record Converter"
    )
    
    parser.add_option("-i", "--input",
        dest="mot_filename",
        action="store",
        help="The path to the file you want to convert.",
        default = "",
        metavar="FILE"
    )
    
    parser.add_option("-o", "--output",
        dest="out_filename",
        action="store",
        help="Name of the output file.",
        default = "",
        metavar="OUTPUT"
    )
    
    parser.add_option("-d", "--data_size",
        dest="max_block_size",
        action="store",
        type='int',
        help="Set max size in bytes for Data section in record [default=2048]",
        default = 2048,
        metavar="MAXBLOCKSIZE"
    )
    
    parser.add_option("-f", "--fill_space",
        dest="max_fill_space",
        action="store",
        type='int',
        help="Max bytes between 2 records to fill with 0xFF's and join data [default=64]",
        default = 64,
        metavar="FILLSPACE"
    )
    
    parser.add_option("-l", "--location",
        dest="header_location",
        action="store",
        type='int',
        help="Flash location for application load file header [default=0xFFFFFE00]",
        default=0xFFFFFE00,
        metavar="HEADERLOC"
    )
    
    parser.add_option("--formatting",
        dest="want_formatting",
        action="store_true",
        help="Displays information on how the binary file is structured.",
        default=False
    )

    parser.add_option("-m", "--mask",
        dest="input_valid_mask",
        action="store",
        type='int',
        help="Set the value you used for the valid mask [default=0xAA]",
        default=0xAA,
        metavar="VALIDMASK"
    )

    if len(sys.argv) == 1:
        parser.print_help()
        sys.exit()
    else:
        (options, args) = parser.parse_args()

    if options.want_formatting == True:
        #Give information on file structure
        print 'The format of the output binary file is: 1 Load File Header followed by n Blocks.'
        print 'n is the number of Blocks needed to represent S-Record file.'
        print ''
        print 'Structure of a Load File Header:'
        print '| Valid Mask             | ' + str(FL_MOT_Converter.FL_LI_FORMAT['valid_mask']) + ' Byte(s)  | Always 0x' + options.input_valid_mask + ', marks valid Load File Header'
        print '| Image ID               | ' + str(FL_MOT_Converter.FL_LI_FORMAT['image_ID']) + ' Byte(s)  | Identifies application'
        print '| Version #              | ' + str(FL_MOT_Converter.FL_LI_FORMAT['version_num']) + ' Byte(s)  | Identifies version of application'
        print '| Size of Load Image     | ' + str(FL_MOT_Converter.FL_LI_FORMAT['load_image_size']) + ' Byte(s)  | Size of image as in external memory'
        print '| Max Block Size         | ' + str(FL_MOT_Converter.FL_LI_FORMAT['max_block_size']) + ' Byte(s)  | Max size of block'
        print '| Image CRC              | ' + str(FL_MOT_Converter.FL_LI_FORMAT['image_crc']) + ' Byte(s)  | CRC of data as in ext memory, CCITT'
        print '| Raw CRC                | ' + str(FL_MOT_Converter.FL_LI_FORMAT['raw_crc']) + ' Byte(s)  | CRC of image as in MCU flash, CCITT'
        print '| 1st Block Header Addr  | ' + str(FL_MOT_Converter.FL_LI_FORMAT['start_address']) + ' Byte(s)  | Location of first block header in ext memory'
        print '| Successfully Stored    | ' + str(FL_MOT_Converter.FL_LI_FORMAT['successfully_stored']) + ' Byte(s)  | Identifies successfully downloaded image (written by MCU)'
        print ''
        print 'Structure of a Block Header:'
        print '| Valid Mask             | ' + str(FL_MOT_Converter.FL_BH_FORMAT['valid_mask']) + ' Byte(s)  | Always 0x' + FL_MOT_Converter.FL_BH_VALID_MASK + ', marks new block header'
        print '| Sequence ID            | ' + str(FL_MOT_Converter.FL_BH_FORMAT['sequence_ID']) + ' Byte(s)  | Identifier for this block'
        print '| Flash Address          | ' + str(FL_MOT_Converter.FL_BH_FORMAT['flash_address']) + ' Byte(s)  | The starting address for the data'
        print '| Size of Data           | ' + str(FL_MOT_Converter.FL_BH_FORMAT['data_size']) + ' Byte(s)  | Number of bytes of Data'
        print '| CRC-16                 | ' + str(FL_MOT_Converter.FL_BH_FORMAT['data_crc']) + ' Byte(s)  | CRC of Data, CCITT - x^16 + x^12 + x^5 + 1'
        print '| Next Header Address    | ' + str(FL_MOT_Converter.FL_BH_FORMAT['next_block_address']) + ' Byte(s)  | Address of next block header in external memory'
        print '| Data                   | 0-4 GBytes | Data'
        print ''
        print 'NOTE: All binary data is stored LSB'
        print ''
        
        
    if len(options.mot_filename) == 0:
        #No input file
        print 'Error - No input file!'
        parser.print_help()
        sys.exit()
        
    if len(options.out_filename) == 0:
        #No output file was given, use modified input filename
        #This fuction will give path without extension in 'start' (and extension) in 'ext'
        start, ext = os.path.splitext(options.mot_filename)
        #Add some other extension
        options.out_filename = start + ".bch"
        
    fl_m = FL_MOT_Converter(options.mot_filename, options.out_filename, options.max_block_size, options.max_fill_space, options.header_location, options.input_valid_mask)
    
    fl_m.Process()
        
        
