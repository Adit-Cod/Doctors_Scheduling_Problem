/******************************************************************************
*****                       DOCTOR'S OFFICE                               *****
******************************************************************************/
/*!
***     \file       File_System.c
***    
***     \author     Aditya Subramanian
***                 Email.. aditya.subramanian3005@gmail.com
***
***     \brief      This module is intended to perform writing to file system
***     \par        File_description
***                 Maintain Patient Records at the end of the day 
***                                                  
***
*\n*/
/*****************************************************************************/

/*=============================================================================
=======                            INCLUDES                             =======
=============================================================================*/

/*------ standard includes -------*/

/*------ project includes --------*/

/*------ module includes --------*/
#include "file_system.h"

/*=============================================================================
=======               DEFINES & MACROS FOR GENERAL PURPOSE              =======
=============================================================================*/

/*=============================================================================
=======                       CONSTANTS  &  TYPES                       =======
=============================================================================*/
pthread_mutex_t lock_file;
FILE *fp;
/*=============================================================================
=======                       PRIVATE FUNCTION DECLARATIONS             =======
=============================================================================*/

/*=============================================================================
=======                              METHODS                            =======
=============================================================================*/

/*---------------------------------------------------------------------------*/
/*     FUNCTION: file_init()
 */
/*!    \brief    File Init.
 *
 *
 *     \param   void 
 *    
 *     \returns  void
 *     \pre
 *     \post
 *              Create a file pointer and set to writing mode
 *     \warning
 *//*------------------------------------------------------------------------*/

void file_init(void)
{
    fp = fopen("patient_Record.txt", "a+");
    if(fp)
    pthread_mutex_init(&lock_file, NULL);
}

/*---------------------------------------------------------------------------*/
/*     FUNCTION: file_write()
 */
/*!    \brief    File Init.
 *
 *
 *     \param    patient_record_t* patient
 *    
 *     \returns  bool
 *     \pre
 *     \post
 *               Write to File system once processed. 
 *     \warning
 *//*------------------------------------------------------------------------*/

bool file_write(patient_record_t* patient)
{
    int temp; 
    time_t t;   
    time(&t);
    printf("Patient Id:%d, Patient Ailment: %d, Patient Priority: %d, Doc Id: %d,current_time %s\n",patient->patient_id,patient->ailment,patient->priority_id,patient->doc_Id,
    ctime(&t));
    temp = fprintf(fp,"Patient Id:%d, Patient Ailment: %d, Patient Priority: %d, Doc Id: %d, Doc Review %d, current_time %s\n",patient->patient_id,patient->ailment,patient->priority_id,patient->doc_Id,
    patient->Doc_review,ctime(&t));

    if(temp > 0)
    return TRUE;

    return FALSE;
}

/*---------------------------------------------------------------------------*/
/*     FUNCTION: close_fileTransaction(void)
 */
/*!    \brief    File Close
 *
 *
 *     \param    void
 *    
 *     \returns  void
 *     \pre
 *     \post
 *               Close the file thats opened.  
 *     \warning
 *//*------------------------------------------------------------------------*/

void close_fileTransaction(void)
{
    fclose(fp);
}
