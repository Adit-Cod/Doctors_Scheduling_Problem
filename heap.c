/******************************************************************************
*****                       DOCTOR'S OFFICE                               *****
******************************************************************************/
/*!
***     \file       heap.c
***    
***     \author     Aditya Subramanian
***                 Email.. aditya.subramanian3005@gmail.com
***
***     \brief      This module is intended to act as driver to model binary heap
***     \par        File_description
***                 All Heap Functionalities are handled within the scope of this file.                                  
***
*\n*/
/*****************************************************************************/

/*=============================================================================
=======                            INCLUDES                             =======
=============================================================================*/

/*------ standard includes -------*/

/*------ project includes --------*/

/*------ module includes --------*/
#include"heap.h"
/*=============================================================================
=======               DEFINES & MACROS FOR GENERAL PURPOSE              =======
=============================================================================*/

/*=============================================================================
=======                       CONSTANTS  &  TYPES                       =======
=============================================================================*/

/*=============================================================================
=======                       PRIVATE FUNCTION DECLARATIONS             =======
=============================================================================*/

static uint32_t parent(uint32_t i);
static uint32_t left_child(uint32_t i);
static uint32_t right_child(uint32_t i);
static void swap(patient_record_t *x, patient_record_t *y);
static void max_heapify(patient_record_t* patient, uint32_t i, uint32_t n);

/*=============================================================================
=======                              METHODS                            =======
=============================================================================*/

/*---------------------------------------------------------------------------*/
/*     FUNCTION: parent(uint32_t i)
 */
/*!    \brief   Parent 
 *
 *
 *     \param   void 
 *    
 *     \returns  uint32_t index
 *     \pre
 *     \post
 *              Returns the index of the parent node
 *     \warning
 *//*------------------------------------------------------------------------*/

static uint32_t parent(uint32_t index) 
{
    return (index - ONE) / TWO;
}

/*---------------------------------------------------------------------------*/
/*     FUNCTION: left_child (uint32_t index)
 */
/*!    \brief    left_child
 *
 *
 *     \param    uint32_t index 
 *    
 *     \returns  uint32_t index
 *     \pre
 *     \post
 *              Returns the index of the left child node
 *     \warning
 *//*------------------------------------------------------------------------*/

static uint32_t left_child(uint32_t index) 
{
    return TWO*index + ONE;
}

/*---------------------------------------------------------------------------*/
/*     FUNCTION: right_child (uint32_t i)
 */
/*!    \brief    right_child returns index of the node in heap. 
 *
 *
 *     \param   uint32_t index 
 *    
 *     \returns  uint32_t index
 *     \pre
 *     \post
 *              Returns the index of the right child node
 *     \warning
 *//*------------------------------------------------------------------------*/

static uint32_t right_child(uint32_t index)
{
    return TWO*index + TWO;
}

/*---------------------------------------------------------------------------*/
/*     FUNCTION: swap
 */
/*!    \brief    Responsible to swap two nodes given to the function. 
 *
 *
 *     \param   patient_record_t *x, patient_record_t *y
 *    
 *     \returns  void
 *     \pre
 *     \post
 *              Returns the index of the left child node
 *     \warning
 *//*------------------------------------------------------------------------*/

static void swap(patient_record_t *node1, patient_record_t *node2) 
{
    patient_record_t temp = *node1;
    *node1 = *node2;
    *node2 = temp;
}

/*---------------------------------------------------------------------------*/
/*     FUNCTION: max_heapify
 */
/*!    \brief    Rearrange the nodes in the heap to put the highest priority patient on top. 
 *
 *
 *     \param   patient_record_t* patient, uint32_t i, uint32_t n
 *    
 *     \returns  void
 *     \pre
 *     \post
 *              
 *     \warning
 *//*------------------------------------------------------------------------*/

static void max_heapify(patient_record_t* patient, uint32_t i, uint32_t n)
{
    // find left child node
    uint32_t left = left_child(i);

    // find right child node
    uint32_t right = right_child(i);

    // find the largest among 3 nodes
    uint32_t largest = i;

    // check if the left node is larger than the current node
    if (left <= n && patient[left].priority_id > patient[largest].priority_id) {
        largest = left;
    }

    // check if the right node is larger than the current node
    if (right <= n && patient[right].priority_id > patient[largest].priority_id) {
        largest = right;
    }

    // swap the largest node with the current node 
    // and repeat this process until the current node is larger than 
    // the right and the left node
    if (largest != i) 
    {
        patient_record_t temp = patient[i];
        patient[i] = patient[largest];
        patient[largest] = temp;
        max_heapify(patient,largest,n);
    }
}

/*---------------------------------------------------------------------------*/
/*     FUNCTION: insert
 */
/*!    \brief    Insert an element in the Heap.  
 *
 *
 *     \param   patient_record_t* patient, patient_record_t* data, uint32_t *count,uint8_t flag
 *    
 *     \returns  void
 *     \pre
 *     \post
 *              
 *     \warning
 *//*------------------------------------------------------------------------*/

void insert(patient_record_t* patient, patient_record_t* data, uint32_t *count,uint8_t flag) 
{
    if(flag == TRUE)
    {
        if (*count >= MAX_HEAP_SIZE) 
        {
            printf("Full, Cannot insert!\n");
            return;
        }
    }
   
    // first insert the time at the last position of the array 
    // and move it up
    patient[*count] = *data;
    *count = *count + 1;
    #ifdef DEBUG_TRACE_ENABLED   
    printf("Flag = %d Current_Count = %d\n", flag, *count);
    #endif
    // move up until the heap property satisfies
    int i = *count - 1;
    while (i != 0 && patient[parent(i)].priority_id < patient[i].priority_id) 
    {
        swap(&patient[parent(i)], &patient[i]);
        i = parent(i);
    }
}

/*---------------------------------------------------------------------------*/
/*     FUNCTION: get_max
 */
/*!    \brief   Returns the  maximum item of the heap.  
 *
 *
 *     \param   patient_record_t* patient
 *    
 *     \returns  patient_record_t get_max
 *     \pre
 *     \post
 *              
 *     \warning
 *//*------------------------------------------------------------------------*/

patient_record_t get_max(patient_record_t* patient) 
{
    return patient[ZERO];
}

/*---------------------------------------------------------------------------*/
/*     FUNCTION: extract_max
 */
/*!    \brief   Deletes the max item and return  
 *
 *     \param   patient_record_t* patient
 *    
 *     \returns  patient_record_t get_max
 *     \pre
 *     \post
 *                
 *     \warning
 *//*------------------------------------------------------------------------*/

patient_record_t extract_max(patient_record_t* patient, uint32_t *count) 
{

    patient_record_t max_item = *patient;
    #ifdef DEBUG_TRACE_ENABLED
    printf("Count %d in extract Max called by %d\n",*count,max_item.doc_Id);
    #endif
    // replace the first item with the last item
    
    if (*count > ZERO) {
        patient[ZERO] = patient[*count - ONE];
        *count = *count - ONE;
    }

    // maintain the heap property by heapifying the 
    // first item
    if(*count!=ZERO)
    max_heapify(patient, ZERO, *count);
    //printf("Moving outta extract max with flagger\n");
    return max_item;
}

/*---------------------------------------------------------------------------*/
/*     FUNCTION: print_heap
 */
/*!    \brief   Prints the items in a heap 
 *
 *     \param   patient_record_t* patient,patient_record_t* patient1, uint32_t count, uint32_t count1
 *    
 *     \returns  void
 *     \pre
 *     \post
 *                
 *     \warning
 *//*------------------------------------------------------------------------*/

void print_heap(patient_record_t* patient,patient_record_t* patient1, uint32_t count, uint32_t count1) 
{
    printf("%d\n",count);
    for (int i = ZERO; i < count; i++) {
        printf("Patient Id %d Priority Id %d \n", patient[i].patient_id,patient[i].priority_id);
    }
    for(int j = ZERO ; j < count1;j++)
    {
        printf("Patient Id %d Priority Id %d \n", patient1[j].patient_id,patient1[j].priority_id);
    }
}