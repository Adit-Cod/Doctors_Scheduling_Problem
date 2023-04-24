/******************************************************************************
*****                            HEAP HEADER                              *****
******************************************************************************/

/*------ standard includes -------*/
#include<stdlib.h>
#include<stdio.h>
#include<stdint.h>
#include<string.h>
/*------ module includes --------*/
#include"clinic_config.h"

/*=============================================================================
 =======                    PROTOTYPES OF PUBLIC FUNCTIONS              =======
 =============================================================================*/
void insert(patient_record_t* patient, patient_record_t* data, uint32_t *count,uint8_t flag);
void print_heap(patient_record_t* patient,patient_record_t* patient1,uint32_t count, uint32_t count1);

patient_record_t get_max(patient_record_t* patient);
patient_record_t extract_max(patient_record_t* patient, uint32_t *count);