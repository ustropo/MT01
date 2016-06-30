#!/usr/bin/env python
'''
/******************************************************************************
* DISCLAIMER:
* The software supplied by Renesas Technology Europe Ltd is
* intended and supplied for use on Renesas Technology products.
* This software is owned by Renesas Technology Europe, Ltd. Or
* Renesas Technology Corporation and is protected under applicable
* copyright laws. All rights are reserved.
*
* THIS SOFTWARE IS PROVIDED "AS IS". NO WARRANTIES, WHETHER EXPRESS,
* IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* APPLY TO THIS SOFTWARE. RENESAS TECHNOLOGY AMERICA, INC. AND
* AND RENESAS TECHNOLOGY CORPORATION RESERVE THE RIGHT, WITHOUT
* NOTICE, TO MAKE CHANGES TO THIS SOFTWARE. NEITHER RENESAS
* TECHNOLOGY AMERICA, INC. NOR RENESAS TECHNOLOGY CORPORATION SHALL,
* IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
* CONSEQUENTIAL DAMAGES FOR ANY REASON WHATSOEVER ARISING OUT OF THE
* USE OR APPLICATION OF THIS SOFTWARE.
******************************************************************************/
/* Copyright (C) 2010. Renesas Electronics America, All Rights Reserved */
/*FILE COMMENT******************************* Technical reference data ****
* File Name		: FL_SerialFlashLoader.py
* Description   : This application implements the communications protocols
*                 from the FlashLoader project.  The communications medium
*                 is asynchronous serial.  The commands that are supported
*                 are transferring a new load image (load), erasing a 
*                 Load Image block (erase), and getting information about
*                 what is currently stored on the Device (info).
******************************************************************************/  
/******************************************************************************
* History 		: MM.DD.YYYY Version Information
*               : 03.18.2010 Ver. 1.00 First Release - BCH
*               : 09.29.2010 Ver. 2.00 Cleaned up code, made easier to 
*               :                      modify - BCH
*FILE COMMENT END*************************************************************/
'''
#For communicating over serial
import serial
#System specific parameters and functions
import sys
#For string use
import string
#binascii is used for converting from ASCII to binary
import binascii
#Used for unpacking FL_CurAppHeader from S-Record file (like C style struct)
from struct import *
#This allows us to define the 'struct' that we use later
from collections import namedtuple

#This is the class that handles communication requests
class FL_Uploader:
    
    # DEFINEs associated with communications protocol 
    # These first two are for initializing communications 
    FL_COM_INIT_1 =             (0xBC)
    FL_COM_INIT_2 =             (0xCD)
    # Acknowledge command 
    FL_COM_ACK  =               (0xB1)
    # Error has occurred 
    FL_COM_ERROR =              (0xB2)
    # Tells host that transmitted block was corrupted, send again
    FL_COM_RESEND =             (0xB3)
    # Tells MCU that host has sent all records 
    FL_COM_DONE =               (0xB4)

    # Command from host for new image to be downloaded 
    FL_COM_OP_NEW_IMAGE =       (0xD1)
    # Command from host for info on current images 
    FL_COM_OP_INFO_REQ  =       (0xD2)
    # Command from host to erase a load block 
    FL_COM_OP_ERASE_BLOCK =     (0xD3)

    
    # Reply that image is stored, just needs to be flashed 
    FL_COM_REP_NEEDS_FLASH =    (0xC1)
    # Reply that the requested image is already running 
    FL_COM_REP_ALREADY_RUNNING =(0xC2)
    #Reply that there was not enough room for the new image
    FL_COM_REP_NO_ROOM =        (0xC3)
    # Reply that max_block_size is too large 
    FL_COM_REP_BLOCK_TOO_LARGE= (0xC4)
    # Reply that image is to large to store 
    FL_COM_REP_IMAGE_TOO_LARGE= (0xC5)
    
    # Timeout value for initial connection to chip (in seconds)
    FL_COM_TIMEOUT_INIT =       (10)
    # Timeout value for communications after device has responded (in seconds)
    FL_COM_TIMEOUT_REG  =       (70)


    #If the user wants to change the size of the fields in the Load Image Header or Data Block
    #Header then they can do this below.  There is a Python dictionary for each header below
    #with the name of the structure entry and the number of bytes associated with that entry.
    #If for instance you wanted to change the 'image_id' field to be 2 bytes you would change
    #the FL_LI_FORMAT definition below to 'image_id':2

    #An array for holding Load Image size format
    FL_LI_FORMAT = {'valid_mask':1, 'image_ID':1, 'version_num':1, 'load_image_size':4, 'max_block_size':4, 'image_crc':2, 'raw_crc':2, 'start_address':4, 'successfully_stored':4 }
    #Number of bytes in Load File Header
    FL_LI_STRUCT_SIZE = sum([i for i in FL_LI_FORMAT.values()])
    #This builds the format string needed. B = byte, H = 2 bytes, L = 4 bytes per entry.
    FL_STRUCT_LI = 'valid_mask image_ID version_num load_image_size max_block_size image_crc raw_crc start_address successfully_stored'
    #Output will produce something like this '<BBBLLHHLL'
    FL_LI_FORMAT_STRING = "<"
    for entry in FL_STRUCT_LI.split(' '):
        if FL_LI_FORMAT[entry] == 1:
            FL_LI_FORMAT_STRING += 'B'
        elif FL_LI_FORMAT[entry] == 2:
            FL_LI_FORMAT_STRING += 'H'
        elif FL_LI_FORMAT[entry] == 4:
            FL_LI_FORMAT_STRING += 'L'
        else:
            print 'Error - This code only supports even sized structure objects for Load Image Headers';
            sys.exit()

    #An array for holding Block Header size format
    FL_BH_FORMAT = {'valid_mask':1, 'sequence_ID':2, 'flash_address':4, 'data_size':4, 'data_crc':2, 'next_block_address':4}
    #Number of bytes in Data Block Header
    FL_BH_STRUCT_SIZE = sum([i for i in FL_BH_FORMAT.values()])    
    #This builds the format string needed. B = byte, H = 2 bytes, L = 4 bytes per entry.
    FL_STRUCT_BH = 'valid_mask sequence_ID flash_address data_size data_crc next_block_address'
    #Output will produce something like this '<BHLLHL'
    FL_BH_FORMAT_STRING = "<"
    for entry in FL_STRUCT_BH.split(' '):
        if FL_BH_FORMAT[entry] == 1:
            FL_BH_FORMAT_STRING += 'B'
        elif FL_BH_FORMAT[entry] == 2:
            FL_BH_FORMAT_STRING += 'H'
        elif FL_BH_FORMAT[entry] == 4:
            FL_BH_FORMAT_STRING += 'L'
        else:
            print 'Error - This code only supports even sized structure objects for Data Block Headers';
            sys.exit()
        
    def __init__(self, port_num=0, op="info", filep="", q=False, eb=-1):
        #Which communications port to use
        self.port = port_num
        #What operation to perform
        self.command = op
        #What file to load (if loading)
        self.filepath = filep
        #Whether or not to print messages of what is going on
        self.quiet = q
        #Which load block to erase
        self.erase_block = eb
        #Open and configure the serial port
        # open first serial port
        self.ser = serial.Serial()
        #Used for processing load image headers
        self.LoadImageHeader = namedtuple('LoadImageHeader', self.FL_STRUCT_LI)
        #Used for processing block headers
        self.BlockHeader = namedtuple('BlockHeader', self.FL_STRUCT_BH)
    
    def execute(self):
        #Assign port to use
        self.ser.port = self.port
        #Set baudrate
        self.ser.baudrate = 115200
        #Set timeout
        self.ser.timeout = self.FL_COM_TIMEOUT_INIT
        
        #Now send what command we want to run    
        if self.command == 'info':
            #Get information on load images on MCU
            self.command_info_request()
                            
        elif self.command == 'load':
            #Want to send new load image
            self.command_new_image()
        
        elif self.command == 'erase':
            #Want to erase load block
            self.command_erase_block()
        
        print 'DONE.'
        
        #Close port
        self.ser.close()             

    #Print information about the current setup
    def __str__(self):
        ret = 'FlashLoader Uploader Object\n\r'
        ret += 'Port is ' + str(self.ser.portstr) + '\n\r'
        ret += 'Filepath is ' + self.filepath + '\n\r'
        ret += 'Command is ' + self.command + '\n\r'
        return ret

    #Read some bytes from serial port
    def read_bytes(self, num_bytes):
        #Setup read
        self.RxBuf = ''
        self.RxBuf = self.ser.read(num_bytes)
        
        #Check for timeout
        if len(self.RxBuf) == 0:
            #Handle timeout
            self.timeout_error()

    #A timeout has occurred, exit
    def timeout_error(self):
        # Show error
        print '*** Error - Timeout on Receive ***'
        
        # Close Port
        self.ser.close()
        
        # Exit
        sys.exit()

    #Lets the Device know that a new command is coming in
    def send_initialization(self):
        #Try to open desired communication port
        try:
            self.ser.open()
        except serial.SerialException as inst:
            print 'Error - ', inst
            sys.exit()
        
        if self.quiet == False:
            print 'Opened communications on ' + self.ser.name + ' at ' + str(self.ser.baudrate) + ' baud'  
        
        #Start communications with MCU
        #Send first communications init message
        self.ser.write(chr(self.FL_COM_INIT_1))
        #Send second communications init message
        self.ser.write(chr(self.FL_COM_INIT_2))
        
        if self.quiet == False:            
            print '-Sent Communication Initialization Messages'

    #This command requests the Device to erase a given Load Image partition
    def command_erase_block(self):
        #Check to make sure valid erase block # was given.
        #Number cannot be negative and is limited to 1 byte (0 - 255)
        if self.erase_block < 0 or self.erase_block > 255:
            print 'Error - Invalid block # (must be in range 0 - 255)'
            return
        
        #Send init messages to MCU
        self.send_initialization()
        
        #Send erase request to MCU
        self.ser.write(chr(self.FL_COM_OP_ERASE_BLOCK))
        
        if self.quiet == False:
            print '-Sent Erase Block Request.  Waiting for response.'
            
        #Now wait for ACK
        self.read_bytes(1)
        
        #Check response
        if self.RxBuf[0] != chr(self.FL_COM_ACK):
            print '-->Received Error Response - Invalid command (block erase)'
            return
        elif self.RxBuf[0] == chr(self.FL_COM_ACK):
            if self.quiet == False:
                print '-->Received ACK - Ready for block number to erase.'
        else:
            print 'Error - Erase Block - Received unknown response from MCU'
            return            
        
        #Send which load block to erase
        self.ser.write(chr(self.erase_block))
        
        #Set longer timeout value for further communications
        self.ser.timeout = self.FL_COM_TIMEOUT_REG
        
        if self.quiet == False:
                print '-Sent request to erase block 0x%02X' % self.erase_block
                       
        #Now wait for reply
        self.read_bytes(1)
        
        #Check response
        if self.RxBuf[0] != chr(self.FL_COM_ACK):
            print '-->Received Error Response - Invalid erase block number'
        elif self.RxBuf[0] == chr(self.FL_COM_ACK):
            print '-->Received ACK - Erase Done.'
        else:
            print 'Error - Erase Block - Received unknown response from MCU'            
        
    #This command requests information from the Device about what images
    #it currently has.
    def command_info_request(self):
        #Send init messages to MCU
        self.send_initialization()
        
        #Send information request to MCU
        self.ser.write(chr(self.FL_COM_OP_INFO_REQ))
        
        if self.quiet == False:
            print '-Sent Information Request.  Waiting for response.'        
        
        #Now wait for information
        #First will be a byte that tells how many more bytes are coming
        self.read_bytes(1)
        
        if self.quiet == False:
            print '-->Repsonse Good - Now receive ' + str(ord(self.RxBuf)) + ' bytes'            
        
        #Get number of LI headers
        numLIH = ord(self.RxBuf) / self.FL_LI_STRUCT_SIZE
        
        #Receive headers
        self.read_bytes(ord(self.RxBuf))
        
        if self.quiet == False:
            print '-->Received Headers. Decoding and printing...'
        
        i = 0
        while i < numLIH:                
            #Decode current image header
            header = self.LoadImageHeader._make(unpack(self.FL_LI_FORMAT_STRING, self.RxBuf[self.FL_LI_STRUCT_SIZE*i:(self.FL_LI_STRUCT_SIZE*i)+self.FL_LI_STRUCT_SIZE]))
            if i == 0:
                #First header is current image
                print 'Current Running Image Info:'
            else:
                #Rest of images are load images on external memory
                print 'External Memory - Load Image ' + str(i)
                
            print '    Valid Mask           = ' + ("0x%0" + str(self.FL_LI_FORMAT['valid_mask']*2) + "x") % header.valid_mask
            print '    Image ID             = ' + ("0x%0" + str(self.FL_LI_FORMAT['image_ID']*2) + "x") % header.image_ID
            print '    Version #            = ' + ("0x%0" + str(self.FL_LI_FORMAT['version_num']*2) + "x") % header.version_num
            print '    Size of Load Image   = ' + ("0x%0" + str(self.FL_LI_FORMAT['load_image_size']*2) + "x") % header.load_image_size
            print '    Max Block Size       = ' + ("0x%0" + str(self.FL_LI_FORMAT['max_block_size']*2) + "x") % header.max_block_size
            print '    Image CRC            = ' + ("0x%0" + str(self.FL_LI_FORMAT['image_crc']*2) + "x") % header.image_crc
            print '    Raw CRC (as in MCU)  = ' + ("0x%0" + str(self.FL_LI_FORMAT['raw_crc']*2) + "x") % header.raw_crc
            print '    Start Address        = ' + ("0x%0" + str(self.FL_LI_FORMAT['start_address']*2) + "x") % header.start_address
            print '    Successfully Stored? = ' + ("0x%0" + str(self.FL_LI_FORMAT['successfully_stored']*2) + "x") % header.successfully_stored
            
            i += 1
        
        print ''
        print 'All load images have been downloaded.'

    #This command requests that the Device download a new Load Image
    def command_new_image(self):
                
        #Try to open load file
        try:
            rd_file = open(self.filepath, "rb")
        except:
            print 'Error - ', self.filepath, ' is not a valid file.'
            return
        
        #Send init messages to MCU
        self.send_initialization()
        
        #Send new image request to MCU
        self.ser.write(chr(self.FL_COM_OP_NEW_IMAGE))
        
        if self.quiet == False:
            print '-Sent New Image Upload Request.  Waiting for response.'
        
        #Now wait for information
        #First there will be an acknowledgement to send the header
        self.read_bytes(1)
        
        if self.quiet == False:
            print '-->Received response, 0x' + self.RxBuf[0].encode("hex")
        
        #Check for acknowledgement
        if self.RxBuf[0] != chr(self.FL_COM_ACK):
            print 'Error - Acknowledgement not received'
            rd_file.close()
            sys.exit()
        
        if self.quiet == False:
            print '-->Received Acknowledgement!'
            print '-Sending new image header.'
        
                
        #Now send new header
        try:
            self.ser.write(rd_file.read(self.FL_LI_STRUCT_SIZE))
        except:
            print 'Error in file ', self.filepath
            rd_file.close()
            sys.exit()
            
        #Set longer timeout value for further communications
        self.ser.timeout = self.FL_COM_TIMEOUT_REG
        
        #Wait for response
        self.read_bytes(1)
            
        #Check response
        if self.RxBuf[0] == chr(self.FL_COM_ACK):
            #Wait for sequence ID
            self.read_bytes(self.FL_BH_FORMAT['sequence_ID'])
            print '-->Reply=Ready for image, start with block 0x' + self.RxBuf[1].encode("hex") + self.RxBuf[0].encode("hex")
        elif self.RxBuf[0] == chr(self.FL_COM_REP_ALREADY_RUNNING):
            print '-->Reply=The input load image is already running on the MCU.'
            sys.exit()
        elif self.RxBuf[0] == chr(self.FL_COM_REP_NEEDS_FLASH):
            print '-->Reply=The MCU has the image, it just needs to flash it in.'
            sys.exit()
        elif self.RxBuf[0] == chr(self.FL_COM_REP_NO_ROOM):
            print '-->Reply=Not enough room to store new image.'
            sys.exit()
        elif self.RxBuf[0] == chr(self.FL_COM_REP_BLOCK_TOO_LARGE):
            print '-->Reply=Max block size is too large.  Convert S-Record file again with a smaller max block size.'
            sys.exit()
        elif self.RxBuf[0] == chr(self.FL_COM_REP_IMAGE_TOO_LARGE):
            #Now get largest image size
            size = 0
            self.read_bytes(4)
            size |= ord(self.RxBuf[0])
            size |= ord(self.RxBuf[1]) << 8
            size |= ord(self.RxBuf[2]) << 16
            size |= ord(self.RxBuf[3]) << 24
            print '-->Reply=The load image is too large and cannot be stored. Max size per image is %d bytes (decimal)' % size            
            sys.exit()
        else:
            print '-->Reply=Unknown Command'
            print 'Error - Unknown command received.'
            rd_file.close()
            sys.exit()        
        
        #Get sequence ID
        seqIDString = ""
        i = self.FL_BH_FORMAT['sequence_ID']-1
        while i >= 0:
            seqIDString += '%02x' % ord(self.RxBuf[i])
            i -= 1
        seq_id = int(seqIDString,16)
        
        #Find requested block in file
        self.FindBlock(seq_id, rd_file)                
        
        #Send block header                    
        try:
            #Read current block header
            tx_buf = rd_file.read(self.FL_BH_STRUCT_SIZE)
        except:
            print 'Error - Invalid File'
            rd_file.close()
            sys.exit()
        
        #Send block until you reach EOF()
        while tx_buf != "":

            #Decode block header
            bheader = self.BlockHeader._make(unpack(self.FL_BH_FORMAT_STRING, tx_buf))
            
            if self.quiet == False:            
                print '-Sending block with sequence ID ' + "0x%04x" % bheader.sequence_ID
            
            #Send block header
            self.ser.write(tx_buf)
            
            #Wait for response
            self.read_bytes(1)
            
            if self.RxBuf[0] != chr(self.FL_COM_ACK):
                print 'Error - Unknown Response from MCU - Was sending blocks, expected ACK'
                rd_file.close()
                sys.exit()
            
            try:
                #Read in data
                tx_buf = rd_file.read(bheader.data_size)
            except:
                print 'Error - Invalid File'
                rd_file.close()
                sys.exit()
            
            #Send data to MCU
            self.ser.write(tx_buf)
            
            #Wait for response
            #Will send ACK and next seq_id if it transmitted successfully
            #Will send RESEND and same seq_id if error occurred during transmission
            self.read_bytes(1 + self.FL_BH_FORMAT['sequence_ID'])
            
            if self.RxBuf[0] == chr(self.FL_COM_RESEND):
                #Move file back and resend again
                rd_file.seek(-1*(bheader.data_size + self.FL_BH_STRUCT_SIZE),1)
                if self.quiet == False:
                    print '-->Reply = Error, resend block'
            elif self.RxBuf[0] == chr(self.FL_COM_ACK):
                #Data was received without error, move to next seq_id
                seqIDString = ""
                i = self.FL_BH_FORMAT['sequence_ID']
                while i > 0:
                    seqIDString += '%02x' % ord(self.RxBuf[i])
                    i -= 1
                re_seq_id = int(seqIDString,16)
                if re_seq_id != (bheader.sequence_ID + 1):
                    print 'Error - MCU requested non-incremental sequence ID during block transfers'
                    rd_file.close()
                    sys.exit()
                if self.quiet == False:
                    print '-->Reply = Block transmitted fine, move to next'
            else:
                #Unknown response
                print 'Error - Unknown response from MCU - Expected ACK or RESEND'
                rd_file.close()
                sys.exit()
            
            try:
                #Read current block header
                tx_buf = rd_file.read(self.FL_BH_STRUCT_SIZE)
            except:
                print 'Error - Invalid File'
                rd_file.close()
                sys.exit()

        
        #File has been sent, let MCU know it is finished
        #The MCU is expecting another header so we fill a buffer the size of
        #header with FL_COM_DONE's
        self.ser.write(binascii.unhexlify(''.join(self.FL_BH_STRUCT_SIZE*["%02x" % self.FL_COM_DONE])))
        
        #Receive final DONE signal
        self.read_bytes(1)
        
        #Check for error
        if self.RxBuf[0] != chr(self.FL_COM_DONE):
            print 'Error - Unexpected response when trying to finish - Expected FL_COM_DONE but got ' + "0x%02x" % ord(self.RxBuf[0])
            rd_file.close()
            sys.exit()
        
        print 'File transferred successfully!'
                
        #Close file
        rd_file.close()
            
    #Finds a Data Block in a file you want to send.  This is used 
    #for when processing a retry.
    def FindBlock(self, seq_id, file):
        
        #Read first block header
        try:
            bheader_str = file.read(self.FL_BH_STRUCT_SIZE)
        except:
            print 'Error - Invalid File - No block headers.'
            sys.exit()
        
        #Check read
        if len(bheader_str) < self.FL_BH_STRUCT_SIZE:
            print 'Error - Invalid File - Sequence ID not found'
            sys.exit()
        
        #Decode block header
        bheader = self.BlockHeader._make(unpack(self.FL_BH_FORMAT_STRING, bheader_str))            

        #Loop through file looking for the sequence_ID sent in
        while seq_id != bheader.sequence_ID:
            
            #This is not the block we want, so skip to next block
            try:
                
                #Move forward in file to next block
                file.seek(bheader.data_size, 1)                
                
                #Read next header
                bheader_str = file.read(self.FL_BH_STRUCT_SIZE)
                
                #Check read
                if len(bheader_str) < self.FL_BH_STRUCT_SIZE:
                    print 'Error - Invalid File - Sequence ID not found'
                    sys.exit()
                    
                #Decode block header
                bheader = self.BlockHeader._make(unpack(self.FL_BH_FORMAT_STRING, bheader_str))
            except:
                print 'Error - Invalid File'
                sys.exit()
            
        #Adjust file being read back to start of this block
        file.seek(-1 * self.FL_BH_STRUCT_SIZE, 1)
        
        



if __name__ == '__main__':
    from optparse import OptionParser
    
    parser = OptionParser(
        description = "FlashLoader Uploader - Upload new load image to MCU over Asynchronous Serial"
    )
    
    parser.add_option("-f", "--file",
        dest="filename",
        action="store",
        help="The path to the file you want to upload.",
        default = None,
        metavar="FILE"
    )
    
    parser.add_option("-p", "--port",
        dest="port_num",
        action="store",
        type= 'int',
        help="The port number to use for communications",
        default = None,
        metavar="Port#"
    )
    
    parser.add_option("-c", "--command",
        dest="command",
        action="store",
        type="choice",
        choices=("info","load","erase"),
        help="The command you want to execute [info, load, erase]",
        default="info",
        metavar="command"
    )
    
    parser.add_option("-q", "--quiet",
        dest="quiet",
        action="store_true",
        help="If specified, messages will be suppressed.",
        default=False
    )
    
    parser.add_option("-b", "--block",
        dest="erase_block",
        action="store",
        type= 'int',
        help="The load block you want to erase",
        default = -1,
        metavar="BLOCK"
    )
    
    if len(sys.argv) == 1:
        parser.print_help()
        sys.exit()
    else:
        (options, args) = parser.parse_args()
        
    #Initialize class
    fl = FL_Uploader(options.port_num, options.command, options.filename, options.quiet, options.erase_block)        
            
    #Do command
    fl.execute()
    
            
    

    
