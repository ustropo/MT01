#include "platform.h"
#include "eeprom.h"
#include "r_vee_if.h"
#include <string.h>

//#define VEE_DEMO_ERASE_FIRST


extern float *velocidadeJog;

enum { READY, NOT_READY } sample_state;

float configVarInit[VAR_MAX] = {
	0,
	15,
	5,
	1.5,
	1,
	5000,
	1500,
	7500,
	150,
	2,
};

uint32_t configFlagsInit = 0;
uint32_t configFlags = 0;

float configVar[VAR_MAX];

float zeroPieceInit[3] = {0,0,0};
float zeroPiece[3];

vee_record_t dataRecord;

void eepromInit(void)
{
#if defined(VEE_DEMO_ERASE_FIRST)
    uint32_t loop1;
    uint32_t ret;
    /* Enable data flash access. */
    R_FlashDataAreaAccess(0xFFFF, 0xFFFF);

    for (loop1 = 0; loop1 < DF_NUM_BLOCKS; loop1++)
    {
        /* Erase data flash. */
        ret = R_FlashErase(BLOCK_DB0 + loop1);

        /* Check for errors */
        if(ret != FLASH_SUCCESS)
        {
            while(1)
            {
                /* Failure in erasing data flash. Something is not setup right. */
            }
        }

        /* Wait for flash operation to finish. */
        while(FLASH_SUCCESS != R_FlashGetStatus());
    }
#endif
	R_VEE_Open();
}

void eepromInitVar(void)
{
	velocidadeJog = &configVar[VELOC_JOG_LENTO];
}

void eepromWriteConfig(uint8_t varType)
{
    uint32_t ret;
    switch (varType)
    {
    	case CONFIGVAR:  dataRecord.ID = CONFIGVAR;
        				 dataRecord.pData = (uint8_t*)configVar;
        				 dataRecord.size =sizeof(configVar);
        				 break;
    	case CONFIGFLAG: dataRecord.ID = CONFIGFLAG;
						 dataRecord.pData = (uint8_t*)&configFlags;
						 dataRecord.size =sizeof(configFlags);
						 break;
    	case ZEROPIECE:  dataRecord.ID = ZEROPIECE;
						 dataRecord.pData = (uint8_t*)&zeroPiece;
						 dataRecord.size =sizeof(zeroPiece);
						 break;
    	default:
    }


    /* Generate check for data */
    ret = R_VEE_GenerateCheck(&dataRecord);
    /* Check result */
    if( ret != VEE_SUCCESS )
    {
        while(1)
        {
            /* Error */
        }
    }
	sample_state = NOT_READY;
	ret = R_VEE_Write(&dataRecord);
	/* Check result */
	if( ret != VEE_SUCCESS )
	{
	    while(1)
	    {
	        /* Error */
	    }
	}


    while(sample_state == NOT_READY)
       {
           /* Wait for write to finish. When write finishes it will call the VEE_OperationDone_Callback() callback
              function below. The user also has the option of just polling by disabling the callback functions in
              r_vee_config.h */
       }
}

void eepromReadConfig(uint8_t varType)
{
    uint32_t ret;

    switch (varType)
    {
    	case CONFIGVAR: dataRecord.ID = CONFIGVAR; break;
    	case CONFIGFLAG: dataRecord.ID = CONFIGFLAG; break;
    	case ZEROPIECE: dataRecord.ID = ZEROPIECE; break;
    	default:
    }
	ret = R_VEE_Read(&dataRecord);
	/* Check result */
	if( ret == VEE_NOT_FOUND )
	{
	    uint32_t loop1;
	    uint32_t ret;
	    /* Enable data flash access. */
	    R_FlashDataAreaAccess(0xFFFF, 0xFFFF);

	    for (loop1 = 0; loop1 < DF_NUM_BLOCKS; loop1++)
	    {
	        /* Erase data flash. */
	        ret = R_FlashErase(BLOCK_DB0 + loop1);

	        /* Check for errors */
	        if(ret != FLASH_SUCCESS)
	        {
	            while(1)
	            {
	                /* Failure in erasing data flash. Something is not setup right. */
	            }
	        }

	        /* Wait for flash operation to finish. */
	        while(FLASH_SUCCESS != R_FlashGetStatus());
	    }
		memcpy(configVar,configVarInit,sizeof(configVar));
		memcpy(&configFlags,&configFlagsInit,sizeof(configFlags));
		memcpy(&zeroPiece,&zeroPieceInit,sizeof(zeroPiece));
		R_VEE_Open();
	    eepromWriteConfig(CONFIGVAR);
	    eepromWriteConfig(CONFIGFLAG);
	    eepromWriteConfig(ZEROPIECE);
		return;
	}
	if( ret != VEE_SUCCESS )
	{
	    while(1)
	    {
	        /* Error */
	    }
	}
    R_VEE_ReleaseState();
    switch (varType)
    {
    	case CONFIGVAR: memcpy(configVar,dataRecord.pData,sizeof(configVar)); break;
    	case CONFIGFLAG: memcpy(&configFlags,dataRecord.pData,sizeof(configFlags)); break;
    	case ZEROPIECE: memcpy(&zeroPiece,dataRecord.pData,sizeof(zeroPiece)); break;
    	default:
    }

}


/***********************************************************************************************************************
* Function Name: VEE_OperationDone_Callback
* Description  : Callback function from VEE that signifies that VEE operation
*                has finished.
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
void VEE_OperationDone_Callback(void)
{
	sample_state = READY;
}
/***********************************************************************************************************************
End of VEE_OperationDone_Callback function
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name: FlashError
* Description  : This function is called either from the FlashAPI, or from a VEE interrupt to signal that an error has
*                occurred during a flash operation. Whatever the user decides to do in this case should be in this
*                function.
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
void FlashError(void)
{
    /* The user can check the g_vee_state variable to see what state the error occurred in. User could also have
       recovery code here. */
    while(1);
}
