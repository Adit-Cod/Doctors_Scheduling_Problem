/******************************************************************************
*****                            CLINIC CONFIG HEADER                     *****
******************************************************************************/

#ifndef __CLINIC_CONFIG_H__
#define __CLINIC_CONFIG_H__

/*------ standard includes -------*/
#include <stdint.h>
#include <stdbool.h>
/*------ module includes --------*/

/*=============================================================================
=======                       DEFINES                                   =======
=============================================================================*/
#define NO_OF_DOCTORS          10
#define CLINIC_CAPACITY        6
#define PATIENT_COUNT          50
#define MAX_HEAP_SIZE          CLINIC_CAPACITY
#define CAFE_HEAP_SIZE        (PATIENT_COUNT-MAX_HEAP_SIZE)
#define TRUE                   1
#define FALSE                  0
#define ZERO                   0
#define ONE                    1
#define TWO                    2
#define THREE                  3
#define FOUR                   4
#define VIP_PATIENT            4
#define PLATINUM_PATIENT       3
/* Scaled as per patient.h */
#define AILMENT_SCALE             4 
#define SPECIALIST_TIME           7
#define PROBABILITY_OF_SPECIALIST 4
/*Worst Case scenario where each patient arrives in 10 units difference */
#define PATIENT_ARRIVAL_TIME      10
/* Additional buffer time for system */
#define ARBIT_TIME                10

/*Define for Tracing */
#define TRACE_LOG_INFO
//#define DEBUG_TRACE_ENABLED  
#define TRACE_LOG_ERROR

/*=============================================================================
 =======                             TYPES                              =======
 =============================================================================*/

typedef struct 
{
    bool is_preempted;
    bool is_withSpecialist;
    uint8_t Doc_review;
    uint32_t patient_id; 
    uint32_t priority_id;
    uint32_t ailment;
    uint32_t doc_Id;
    uint32_t time_left;
    uint32_t time_start;
    uint32_t temp_ailment;
    uint32_t temp_priority;
}patient_record_t;
#endif

