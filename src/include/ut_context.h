/*
 * ut_context.h
 *
 *  Created on: Oct 29, 2015
 *      Author: Fernando
 */

#ifndef STATES_UT_CONTEXT_H_
#define STATES_UT_CONTEXT_H_

#define DEFAULT_CONTEXT_DATA_LEN	512

typedef unsigned char  ut_tag;
typedef unsigned char  ut_len;
typedef unsigned char* ut_value;

/**
 * Struct to handle TLV objects
 */
typedef struct
{
	ut_tag tag;
	ut_len len;
	ut_value value;
} ut_tlv;

/**
 * Used to share data among different states
 */
typedef struct
{
	const char* name;
	unsigned int  numItems;
	unsigned char abData[DEFAULT_CONTEXT_DATA_LEN];
	ut_tlv object;
} ut_context;

/**
 * Some methods to work with contexts / tlv objects
 */


#endif /* STATES_UT_CONTEXT_H_ */
