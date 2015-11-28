/*
 * ut_context.h
 *
 *  Created on: Oct 29, 2015
 *      Author: Fernando
 */

#ifndef STATES_UT_CONTEXT_H_
#define STATES_UT_CONTEXT_H_

#define DEFAULT_CONTEXT_DATA_LEN	128

typedef unsigned char  ut_tag;
typedef unsigned char  ut_len;
typedef unsigned char* ut_value;

/**
 * Used to share data among different states
 */
typedef struct
{
	ut_tag tag;
	ut_len len;
	unsigned char value[DEFAULT_CONTEXT_DATA_LEN];
} ut_context;

/**
 * Some methods to work with contexts / tlv objects
 */


#endif /* STATES_UT_CONTEXT_H_ */
