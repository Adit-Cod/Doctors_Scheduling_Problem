/******************************************************************************
*****                       DOCTOR'S OFFICE                               *****
******************************************************************************/
/*!
***     \file       core_main.c
***    
***     \author     Aditya Subramanian
***                 Email.. aditya.subramanian3005@gmail.com
***
***     \brief      This module is intended to act as core driver to the Doctor's Office
***     \par        File_description
***                 Main file of the Doctor's Office 
***                 Doctor and Patient driver is within the scope of this file.                                 
***
*\n*/
/*****************************************************************************/

/*=============================================================================
=======                            INCLUDES                             =======
=============================================================================*/

/*------ standard includes -------*/

/*------ project includes --------*/
#include"core_main.h"
/*------ module includes --------*/

/*=============================================================================
=======               DEFINES & MACROS FOR GENERAL PURPOSE              =======
=============================================================================*/

/*=============================================================================
=======                       CONSTANTS  &  TYPES                       =======
=============================================================================*/

/* Main Heap = Waiting Room */
patient_record_t Patient_Record[CLINIC_CAPACITY];
/* Cafe Heap = waiting Room */
patient_record_t PatientOverflow_Queue[CAFE_HEAP_SIZE];
/* Array to hold the Patients Record */
patient_record_t PatientRecord_Holder[NO_OF_DOCTORS];
/* Array holding pre-empted patients */
patient_record_t PatientPreempt_queue[NO_OF_DOCTORS];
/* Pointer to hold VIP patient Record should pre-emption take place*/
patient_record_t* patient_preempthandle; 

/* Mutex Defines */
pthread_mutex_t lock_doctor;
pthread_mutex_t lock_patient;
pthread_mutex_t time_lock;
pthread_mutex_t file_lock;

sem_t sem_patientAvailable;
/* Global variables and flags*/
static   uint32_t timer  = ZERO;
uint8_t  volatile threadIdToWakeUp = -1;
uint8_t  volatile flager = ZERO;
uint32_t count  = ZERO;
uint32_t count1 = ZERO; 
uint32_t volatile doc_dropflag = ZERO;
uint32_t volatile patient_servicingcount = ZERO;
uint32_t volatile patient_preemptCount   = ZERO; 
uint32_t volatile total_patient_count    = ZERO; 


/*=============================================================================
=======                       PRIVATE FUNCTION DECLARATIONS             =======
=============================================================================*/
static void timer_compute(uint32_t* timer);
/*=============================================================================
=======                              METHODS                            =======
=============================================================================*/

/*---------------------------------------------------------------------------*/
/*     FUNCTION: *timer_func()
 */
/*!    \brief    Timer Thread to compute and manage overall application time.
 *
 *
 *     \param   timer 
 *    
 *     \returns  void
 *     \pre
 *     \post
 *
 *     \warning
 *//*------------------------------------------------------------------------*/

void *timer_func(void *arg)
{
    pthread_mutex_lock(&time_lock);
    uint32_t  *timer = (uint32_t *)arg;
    pthread_mutex_unlock(&time_lock);

    #ifdef DEBUG_TRACE_ENABLED
    printf("Timer:%d\n",*timer);
    #endif

    while(1)
    {
        pthread_mutex_lock(&time_lock);
        if((*timer) == ZERO)
        pthread_exit(NULL);
        else
        (*timer)--;
        pthread_mutex_unlock(&time_lock);
        sleep(ONE);
    }
}

/*---------------------------------------------------------------------------*/
/*     FUNCTION: *Doctor_func()
 */
/*!    \brief    Doctor Thread/Threads to compute and manage overall Doctor Practise
 *
 *
 *     \param    void
 *    
 *     \returns  void
 *     \pre
 *     \post
 *
 *     \warning
 *//*------------------------------------------------------------------------*/

void *Doctor_func(void *arg) 
{
    uint8_t Doc_review = -1;
    uint8_t temp_lock = -1;
    uint32_t Time_tracker;
    uint32_t Doc_Id = *(uint32_t *)arg;
    patient_record_t patient_handler;
    patient_record_t patient_temp;
    patient_record_t Vip_Patient;

    #ifdef TRACE_LOG_INFO
    printf("Hi This is Doc %d \n",Doc_Id);
    #endif
    
    /*Doctor is ready to service! */
    while(1)
    {   
        /*Sleep for x Seconds before you can look for patients. To avoid race condition */
        sleep(TWO);

        sem_wait(&sem_patientAvailable);

        /*Does not make sense for doctors to stay if P patients are served. Doctor can go home! */ 
        if(total_patient_count == PATIENT_COUNT)
        {
            #ifdef TRACE_LOG_INFO
            printf("Doc Will Exit with ID %d \n",Doc_Id);
            #endif
            break;
        }
        
        /* Check to see if there is a patient in waiting room or pre-empted room */
        if((count > ZERO) || (patient_preemptCount > ZERO)) 
        {
            /*Lock the patient service count. This prevents multiple doctors overriding at the same time */
            pthread_mutex_lock(&lock_patient);
            patient_servicingcount++;
            pthread_mutex_unlock(&lock_patient);

            #ifdef DEBUG_TRACE_ENABLED
            printf(" Count: %d patient_preemptCount %d with doc id %d \n",count,patient_preemptCount,Doc_Id);
            #endif

            /* Lock the room for data extraction. If the doctor has a preempted patient waiting for him with higher prio. Pick! */
            pthread_mutex_lock(&lock_doctor);
            if( Patient_Record[ZERO].priority_id > PatientPreempt_queue[Doc_Id-ONE].priority_id )
            {
                /* Sanitary Check incase another doctor context switches and comes down to this point! */
                if(total_patient_count == PATIENT_COUNT)
                {
                    pthread_mutex_unlock(&lock_doctor);

                    #ifdef TRACE_LOG_INFO
                    printf("Doc %d is exiting here!\n",Doc_Id);
                    #endif

                    break;
                }
                /* Extract from Heap at waiting room */ 
                patient_handler = extract_max(Patient_Record,&count);
                #ifdef TRACE_DEBUG_ENABLED 
                printf("Present extracted patient %d\n", patient_handler.patient_id);
                #endif

                /* Within the same context. If one patient is pulled in for servicing, it makes room in waiting room. pull highest
                   priority guy back into waiting room*/
                if(count1 > ZERO )
                {
                    flager = ONE;         
                    patient_temp = extract_max(PatientOverflow_Queue,&count1);
                    count = CLINIC_CAPACITY-ONE;
                    insert(Patient_Record,&patient_temp,&count,flager);
                    
                    #ifdef TRACE_LOG_INFO
                    printf("A patient with id %d is moved from the cafe to the clinic \n", patient_temp.patient_id);
                    #endif
                }
            }
            else
            {
                /* Here to pull back patient to get done with it! */
                patient_handler = PatientPreempt_queue[Doc_Id-1];
                #ifdef TRACE_LOG_INFO
                printf("OK, I have retrieved a patient who was with doc with Id %d and Prio %d\n",patient_handler.patient_id,
                patient_handler.priority_id);
                #endif
                patient_preemptCount--;
                /* Just to avoid repeated pick*/
                PatientPreempt_queue[Doc_Id-1].priority_id = ZERO;
                sem_post(&sem_patientAvailable); 
            }         
            pthread_mutex_unlock(&lock_doctor);

            pthread_mutex_lock(&time_lock);
            patient_handler.time_start = timer;
            pthread_mutex_unlock(&time_lock);
            
            /* Handling Specialist for every fourth patient */
            if(!(patient_handler.patient_id % 4))
            {
                #ifdef TRACE_LOG_INFO
               printf("**This patient %d has to go to the specialist! %d \n",patient_handler.patient_id,patient_handler.doc_Id);
               #endif
               patient_handler.is_withSpecialist = TRUE;
               /* If patient goes to a specialist. Mimic the behavior by putting the same doctor for a longer duration. 
               Preventing from picking new patient!*/
               patient_handler.ailment+=7;
            }

            patient_handler.doc_Id = Doc_Id;
            pthread_mutex_lock(&lock_patient);
            PatientRecord_Holder[Doc_Id-ONE] = patient_handler;
            pthread_mutex_unlock(&lock_patient);
            
            /* All attributes of patient is now available with the doctor to start servicing! */
            while (ONE) 
            {

                /* Key functionality to check for pre-emption*/
                pthread_mutex_lock(&lock_patient);
                temp_lock = threadIdToWakeUp;
                pthread_mutex_unlock(&lock_patient);

                if(temp_lock == patient_handler.doc_Id)
                { 
                    #ifdef DEBUG_TRACE_ENABLED
                    printf("Preemption in play\n");
                    #endif

                    uint8_t temp; 
                    pthread_mutex_lock(&time_lock);
                    temp = timer;
                    pthread_mutex_unlock(&time_lock);

                    patient_handler.time_left = patient_handler.time_start - timer;
                    #ifdef TRACE_LOG_INFO
                    printf("Time left for patient %d is %d\n",patient_handler.patient_id,patient_handler.time_left);
                    #endif

                    patient_handler.temp_ailment = patient_handler.ailment;
                    patient_handler.ailment = (patient_handler.time_left/10);
                    patient_handler.temp_priority = patient_handler.priority_id;
                    patient_handler.priority_id = PLATINUM_PATIENT;
                    patient_handler.is_preempted = TRUE;                    
                    pthread_mutex_lock(&lock_doctor);
                    patient_preemptCount++;
                    PatientPreempt_queue[Doc_Id - ONE] = patient_handler;
                    pthread_mutex_unlock(&lock_doctor);

                    /* Execute VIP patient within this Context. */
 
                    patient_handler = *(patient_preempthandle);
                    pthread_mutex_lock(&time_lock);
                    patient_handler.time_start = timer;
                    pthread_mutex_unlock(&time_lock);
                    patient_handler.doc_Id = Doc_Id;
                    sleep(patient_handler.ailment);
                    #ifdef TRACE_LOG_INFO
                    printf("I have currently serviced a VIP patient with ID: %d and priority %d\n",patient_handler.patient_id,
                    patient_handler.priority_id);
                    #endif
                    threadIdToWakeUp = -1;
                    break;
                }
                else
                {
                    /* Mimic sleep behavior by keeping doctor busy for ailment seconds */
                    /* Access timer */
                    pthread_mutex_lock(&time_lock);
                    Time_tracker = timer;
                    pthread_mutex_unlock(&time_lock);

                    if((patient_handler.time_start - Time_tracker ) == (patient_handler.ailment*AILMENT_SCALE))
                    {
                        if(patient_handler.is_preempted == TRUE)
                        {
                            /*Restore back to original priority */
                            patient_handler.priority_id = patient_handler.temp_priority;
                            patient_handler.ailment = patient_handler.temp_ailment;
                        }
                        #ifdef TRACE_LOG_INFO
                        printf("##Patient %d Has been serviced to 0 by Doc %d \n",patient_handler.patient_id,patient_handler.doc_Id);
                        #endif 
                        break;
                    }
                } 
            }
            pthread_mutex_lock(&file_lock);
            
            srand(time(ZERO));
            Doc_review = (rand() % 10) + ONE;
            /* Generate a random response as review for the serviced doctor and store for file write! */
            patient_handler.Doc_review = Doc_review;
            file_write(&patient_handler);
            total_patient_count++;

            pthread_mutex_unlock(&file_lock);

            if(PATIENT_COUNT == total_patient_count)
            {
               #ifdef TRACE_LOG_ERROR
               printf("2:Total Patient count at this stage %d with doc Id %d\n",total_patient_count,patient_handler.doc_Id);
               #endif
               /* Wake up other doctors and ask them to go home! */
               for(int i = ZERO ; i < NO_OF_DOCTORS;i++)
               sem_post(&sem_patientAvailable);
               
               #ifdef TRACE_LOG_ERROR
               printf("Doc will exit with ID %d\n",Doc_Id);
               #endif
              
               break;
            }

            pthread_mutex_lock(&lock_doctor);
            patient_servicingcount--;
            pthread_mutex_unlock(&lock_doctor);
        }
        else
        {
            /* Here until timer runs out as no patients to service. Sanitary exit point! */
            if(timer == ZERO )
            pthread_exit(NULL);
        }
    }
    #ifdef DEBUG_TRACE_ENABLED
    printf("The following doctor has exited %d\n",Doc_Id);
    #endif
}

/*---------------------------------------------------------------------------*/
/*     FUNCTION: *Patient_func()
 */
/*!    \brief    Patient Thread/Threads to compute and manage overall Patient Sequence
 *
 *
 *     \param    void
 *    
 *     \returns  void
 *     \pre
 *     \post
 *
 *     \warning
 *//*------------------------------------------------------------------------*/

void *Patient_Func(void *arg)
{
    patient_record_t* Patient = (patient_record_t *)arg;
    uint8_t temp;
    uint8_t kill_flag = ZERO;
    uint32_t temp_service; 
        
    #ifdef DEBUG_TRACE_ENABLED
    printf("Patient called with ID %d\n",Patient->patient_id);
    #endif
    
    /*Generate a random Ailment in allignment with clininc.h */
    srand(time(0));
    temp = (rand() % 5) + 1;

    Patient->ailment = temp;
    
    /* Generate a random priority in allignment with clininc.h */
    srand(time(0));
    temp = (rand() % 4) + 1;

    Patient->priority_id = temp;

    pthread_mutex_lock(&lock_patient);
    temp_service = patient_servicingcount;
    pthread_mutex_unlock(&lock_patient); 
    
    #ifdef DEBUG_TRACE_ENABLED
    printf("patient service Count %d, prio id %d\n",temp_service,Patient->priority_id);
    #endif
    
    /* If new patient is VIP and All Doctors are busy. Pre-empt! */
    pthread_mutex_lock(&lock_patient);
    if( ((Patient->priority_id) == VIP_PATIENT) && (patient_servicingcount == NO_OF_DOCTORS))
    {
        for(int i=ZERO; i<NO_OF_DOCTORS; i++)
        {
            if((PatientRecord_Holder[i].priority_id < Patient->priority_id) && (PatientRecord_Holder[i].is_withSpecialist == FALSE))
            {
                threadIdToWakeUp = i+ ONE;
                patient_preempthandle = Patient;
                #ifdef TRACE_LOG_INFO
                printf("I am sending in this patient with Id %d into Doctor's Office with prio %d \n",patient_preempthandle->patient_id,
                patient_preempthandle->priority_id);
                #endif
                kill_flag = ONE;
            }
        }
    }
    pthread_mutex_unlock(&lock_patient);
    
    /* If Clinic is full and the last patient is of lower priority than current patient. Swap the patient out and insert current patient*/
    pthread_mutex_lock(&lock_doctor);
    if((count == CLINIC_CAPACITY) && (Patient->priority_id > Patient_Record[CLINIC_CAPACITY - 1].priority_id)  && (kill_flag != 1))
    {
        flager = ZERO; 
        patient_record_t low_priority_patient = Patient_Record[CLINIC_CAPACITY-1];
        insert(PatientOverflow_Queue,&low_priority_patient,&count1,flager);
        count--;
        #ifdef TRACE_LOG_INFO
        printf("Patient %d with priority %d has pre-empted Patient %d with priority %d and pushed into Cafe\n",
        Patient->patient_id, Patient->priority_id,low_priority_patient.patient_id,low_priority_patient.priority_id);
        #endif  
    }
    pthread_mutex_unlock(&lock_doctor);
    
    /*Regular insertion of patient into either waiting Room or Cafe */
    pthread_mutex_lock(&lock_doctor);
    if((count<CLINIC_CAPACITY) && (kill_flag != 1))
    {
        kill_flag = ZERO;
        flager = ONE;
        
        insert(Patient_Record,Patient,&count,flager);
        #ifdef TRACE_LOG_INFO
        printf("Patient %d inserted in the clinic \n", Patient->patient_id);
        #endif
    }
    else if((count >= CLINIC_CAPACITY) && (kill_flag != 1))
    { 
        kill_flag = ZERO;
        flager = ZERO; 
        insert(PatientOverflow_Queue,Patient,&count1,flager);
        #ifdef TRACE_LOG_INFO
        printf("Patient %d inserted in the cafe \n", Patient->patient_id);
        #endif
    }
    else{/*Do Nothing. Exit Thread! */}
    pthread_mutex_unlock(&lock_doctor);
    sem_post(&sem_patientAvailable);   
}

/*---------------------------------------------------------------------------*/
/*     FUNCTION: Main Function
 */
/*!    \brief   Main Driver Code to Initiate the clinic practise
 *
 *
 *     \param    void
 *    
 *     \returns  void
 *     \pre
 *     \post
 *
 *     \warning
 *//*------------------------------------------------------------------------*/

void main()
{
    /*sanitary Process!*/
    memset(&Patient_Record,ZERO,CLINIC_CAPACITY*sizeof(patient_record_t)); 
    memset(&PatientOverflow_Queue,ZERO,CAFE_HEAP_SIZE*sizeof(patient_record_t));
    memset(&PatientRecord_Holder,ZERO,NO_OF_DOCTORS*sizeof(patient_record_t));
    memset(&PatientPreempt_queue,ZERO,NO_OF_DOCTORS*sizeof(patient_record_t));
    /*Set up File System */
    file_init();

    pthread_mutex_init(&time_lock,NULL);
    pthread_mutex_init(&lock_doctor,NULL);
    pthread_mutex_init(&lock_patient,NULL);
    pthread_mutex_init(&file_lock,NULL);
    
    /*Initialize the semaphores */
    sem_init(&sem_patientAvailable,ZERO,PATIENT_COUNT);

    /*Array to maintain the Doctor Threads */
    pthread_t Doctors[NO_OF_DOCTORS];
    /*Array to maintain the Patient Threads */
    pthread_t Patients[PATIENT_COUNT];

    pthread_t Timer;
    timer_compute(&timer);

    int Doctor_Id[NO_OF_DOCTORS];
    int Patient_Id[PATIENT_COUNT];

    pthread_create(&Timer,NULL,&timer_func, (void *)&timer);
    /*Create the thread for doctors to get started! */
    for (int i = ZERO; i < NO_OF_DOCTORS; i++) 
    {
        Doctor_Id[i] = i + ONE;
        pthread_create(&Doctors[i], NULL, Doctor_func, (void *)&Doctor_Id[i]);
    }
    /*Now that the doctor thread is created; each doctor is in waiting for the patient to come in! */
    while(1)
    {
        for(int i = ZERO ; i< PATIENT_COUNT; i++)
        {
            int temp;
            srand(time(0));
            /*Generate a Random Value randing 4-10 */
            temp = (rand() % 7) + 4;
            sleep(temp);
            /*Create Patient Thread after Random Time between 4-10 units */
            Patient_Record[i].patient_id = i+1;
            pthread_create(&Patients[i],NULL,Patient_Func,(void *)&Patient_Record[i]);
        }

        for(int i=0; i<PATIENT_COUNT; i++)
        {
            pthread_join(Patients[i],NULL);
        } 

        /* Note: Handling the end of day condition. TIME TO GO HOME!*/
        while(timer != ZERO);
        
        /* Sanitary Closure: Well at this time, if doctors are still waiting on a patient. That's no use. Because time is up. Kill them! */
        
       for (int i = ZERO; i < NO_OF_DOCTORS; i++)
       {
            pthread_cancel(Doctors[i]);
       } 
        for(int j = 0 ; j< NO_OF_DOCTORS; j++)
        {
            pthread_join(Doctors[j],NULL);
        }
        break;
    }

    pthread_mutex_destroy(&time_lock);
    pthread_mutex_destroy(&lock_patient);
    pthread_mutex_destroy(&lock_doctor);
    pthread_mutex_destroy(&file_lock);

    sem_destroy(&sem_patientAvailable);

    printf("file close called");
    close_fileTransaction();
    
    #ifdef DEBUG_TRACE_ENABLED
    //print_heap(Patient_Record,PatientOverflow_Queue,count, count1);
    #endif
}

/*---------------------------------------------------------------------------*/
/*     FUNCTION: timer_compute()
 */
/*!    \brief    Computes the time based on approximate worst case Scenario 
 *
 *
 *     \param    void
 *    
 *     \returns  void
 *     \pre
 *     \post
 *
 *     \warning
 *//*------------------------------------------------------------------------*/
static void timer_compute(uint32_t* timer)
{
    *timer = ( (PATIENT_COUNT*PATIENT_ARRIVAL_TIME) + (PATIENT_COUNT*AIL_CHECKUP) + 
               (PATIENT_COUNT*SPECIALIST_TIME)/PROBABILITY_OF_SPECIALIST ) + ARBIT_TIME;
    printf("Timer:%d\n",*timer);
}