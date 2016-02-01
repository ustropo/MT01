#include "platform.h"
#include "eeprom.h"
#include "r_vee_if.h"

uint32_t configVar[VAR_MAX] = {
	1,
	1,
	0,
	1,
	10,
	1500,
	7500,
	0,
	0,
	0,
	0
};

/***********************************************************************************************************************
* Function Name: VEE_OperationDone_Callback
* Description  : Callback function from VEE that signifies that VEE operation
*                has finished.
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
void VEE_OperationDone_Callback(void)
{

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
