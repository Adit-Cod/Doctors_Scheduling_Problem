/******************************************************************************
*****                           FILE SYSTEM HEADER                        *****
******************************************************************************/
#ifndef __FILE_SYSTEM__H__
#define __FILE_SYSTEM__H__

/*------ standard includes -------*/
#include<pthread.h>
#include<stdio.h>
#include<stdbool.h>
#include<time.h>

/*------ module includes --------*/
#include "clinic_config.h"

/*=============================================================================
 =======                    PROTOTYPES OF PUBLIC FUNCTIONS              =======
 =============================================================================*/
bool file_write(patient_record_t* patient);
void file_init(void);
void close_fileTransaction(void);

#endif