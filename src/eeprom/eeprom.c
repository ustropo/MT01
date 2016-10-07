#include "platform.h"
#include "eeprom.h"
#include "r_vee_if.h"
#include <string.h>

//#define VEE_DEMO_ERASE_FIRST

static void eepromFormat(void);

enum { READY, NOT_READY } sample_state;

/*! configVarOxInit - Constante inicial de parametrização para Oxicorte */
const float configVarOxInit[OX_CONFIG_MAX] = {
	5,                               //!< Altura de perfuração
	1,                               //!< Altura de corte
	5000,                            //!< Velocidade de corte
	3,                               //!< Tempo de aquecimento
	2                                //!< Tempo de Perfuração
};

/*! configVarPlInit - Constante inicial de parametrização para Plasma */
const float configVarPlInit[PL_CONFIG_MAX] = {
	5,                               //!< Altura de perfuração
	1,                               //!< Altura de corte
	5000,                            //!< Velocidade de corte
	1,                               //!< Tempo de Perfuração
	150                              //!< Tensao do THC
};

/*! configVarJogInit - Constante inicial de velocidade de jog*/
const float configVarJogInit[JOG_MAX] = {
	7500,                              //!< Velocidade inicial de jog rapido
	1500,                              //!< Velocidade inicial de jog lento
};

/*! configVarJogInit - Constante inicial de config de maquina*/
const float configVarMaqInit[CFG_MAQUINA_MAX - 1] = {
	15,                               //!< Altura de deslocamento
};

uint32_t configFlagsInit[FLAG_MAX] = {0,1};
uint32_t configFlags[FLAG_MAX];

float configVarOx[OX_CONFIG_MAX];
float configVarPl[PL_CONFIG_MAX];
float configVarMaq[CFG_MAQUINA_MAX - 1]; // retirado o modo maquina
float configVarParMaq[CFG_PAR_MAQ_MAX];
float configVarJog[JOG_MAX];

float zeroPieceInit[3] = {0,0,0};
float zeroPiece[3];

vee_record_t dataRecord;

void eepromInit(void)
{
#if defined(VEE_DEMO_ERASE_FIRST)
	eepromFormat();
#else
	R_VEE_Open();
	eepromReadConfig(CONFIGVAR_OX);
	eepromReadConfig(CONFIGVAR_PL);
	eepromReadConfig(CONFIGVAR_JOG);
	eepromReadConfig(CONFIGVAR_MAQ);
	eepromReadConfig(CONFIGVAR_PAR_MAQ);
	eepromReadConfig(CONFIGFLAG);
	eepromReadConfig(ZEROPIECE);
#endif
}

void eepromWriteConfig(uint8_t varType)
{
    uint32_t ret;
    switch (varType)
    {
    	case CONFIGVAR_OX:  dataRecord.ID = CONFIGVAR_OX;
        				 dataRecord.pData = (uint8_t*)configVarOx;
        				 dataRecord.size =sizeof(configVarOx);
        				 break;
    	case CONFIGVAR_PL:  dataRecord.ID = CONFIGVAR_PL;
        				 dataRecord.pData = (uint8_t*)configVarPl;
        				 dataRecord.size =sizeof(configVarPl);
        				 break;
    	case CONFIGVAR_JOG:  dataRecord.ID = CONFIGVAR_JOG;
        				 dataRecord.pData = (uint8_t*)configVarJog;
        				 dataRecord.size =sizeof(configVarJog);
        				 break;
    	case CONFIGVAR_MAQ:  dataRecord.ID = CONFIGVAR_MAQ;
        				 dataRecord.pData = (uint8_t*)configVarMaq;
        				 dataRecord.size =sizeof(configVarMaq);
        				 break;
    	case CONFIGVAR_PAR_MAQ:  dataRecord.ID = CONFIGVAR_PAR_MAQ;
        				 dataRecord.pData = (uint8_t*)configVarParMaq;
        				 dataRecord.size =sizeof(configVarParMaq);
        				 break;
    	case CONFIGFLAG: dataRecord.ID = CONFIGFLAG;
						 dataRecord.pData = (uint8_t*)configFlags;
						 dataRecord.size =sizeof(configFlags);
						 break;
    	case ZEROPIECE:  dataRecord.ID = ZEROPIECE;
						 dataRecord.pData = (uint8_t*)&zeroPiece;
						 dataRecord.size =sizeof(zeroPiece);
						 break;
    	default:		 break;
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
    	case CONFIGVAR_OX: dataRecord.ID = CONFIGVAR_OX; break;
    	case CONFIGVAR_PL: dataRecord.ID = CONFIGVAR_PL; break;
    	case CONFIGVAR_JOG: dataRecord.ID = CONFIGVAR_JOG; break;
    	case CONFIGVAR_MAQ: dataRecord.ID = CONFIGVAR_MAQ; break;
    	case CONFIGVAR_PAR_MAQ: dataRecord.ID = CONFIGVAR_PAR_MAQ; break;
    	case CONFIGFLAG: dataRecord.ID = CONFIGFLAG; break;
    	case ZEROPIECE: dataRecord.ID = ZEROPIECE; break;
    	default: break;
    }
	ret = R_VEE_Read(&dataRecord);
	/* Check result */
	if( ret == VEE_NOT_FOUND )
	{
		eepromFormat();
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
    	case CONFIGVAR_OX: memcpy(configVarOx,dataRecord.pData,sizeof(configVarOx)); break;
    	case CONFIGVAR_PL: memcpy(configVarPl,dataRecord.pData,sizeof(configVarPl)); break;
    	case CONFIGVAR_JOG: memcpy(configVarJog,dataRecord.pData,sizeof(configVarJog)); break;
    	case CONFIGVAR_MAQ: memcpy(configVarMaq,dataRecord.pData,sizeof(configVarMaq)); break;
    	case CONFIGVAR_PAR_MAQ: memcpy(configVarParMaq,dataRecord.pData,sizeof(configVarParMaq)); break;
    	case CONFIGFLAG: memcpy(&configFlags,dataRecord.pData,sizeof(configFlags)); break;
    	case ZEROPIECE: memcpy(&zeroPiece,dataRecord.pData,sizeof(zeroPiece)); break;
    	default: break;
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

void eepromConsistencyCheck(void)
{
	uint8_t i;
	for (i = 0; i < OX_CONFIG_MAX; i++)
	{
		if (configVarOx[i] > ox_init_max[i] || configVarOx[i] < ox_init_min[i])
		{
			eepromFormat();
		}
	}
	for (i = 0; i < PL_CONFIG_MAX; i++)
	{
		if (configVarPl[i] > pl_init_max[i] || configVarPl[i] < pl_init_min[i])
		{
			eepromFormat();
		}
	}
}

static void eepromFormat(void)
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
		memcpy(configVarOx,configVarOxInit,sizeof(configVarOx));
		memcpy(configVarPl,configVarPlInit,sizeof(configVarPl));
		memcpy(configVarJog,configVarJogInit,sizeof(configVarJog));
		memcpy(&configFlags,&configFlagsInit,sizeof(configFlags));
		memcpy(&zeroPiece,&zeroPieceInit,sizeof(zeroPiece));
		memcpy(configVarMaq,configVarMaqInit,sizeof(configVarMaq));
		R_VEE_Open();
		eepromWriteConfig(CONFIGVAR_OX);
		eepromWriteConfig(CONFIGVAR_PL);
		eepromWriteConfig(CONFIGVAR_JOG);
		eepromWriteConfig(CONFIGVAR_MAQ);
		eepromWriteConfig(CONFIGVAR_PAR_MAQ);
		eepromWriteConfig(CONFIGFLAG);
		eepromWriteConfig(ZEROPIECE);
}

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
