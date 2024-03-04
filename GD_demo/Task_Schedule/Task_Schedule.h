#ifndef __TASK_SCHEDULE_H
#define __TASK_SCHEDULE_H 			   

#include "stdint.h"
#define TASK_MAX_NO  8
typedef enum
{
    TASK_NO1    =   0,
    TASK_NO2    =   1,
    TASK_NO3    =   2,
    TASK_NO4    =   3,
    TASK_NO5    =   4,
    TASK_NO6    =   5,
    TASK_NO7    =   6,
    TASK_NO8    =   7
    
}Task_Number_t;

typedef enum
{
    TASK_OFF    =  0,
    TASK_ON     =  1
    
}Task_Switch_t;

typedef enum
{
    TASK_EVENT =  0,
    TASK_CYCLC =  1
    
}Task_Type_t;

typedef enum
{
    TASK_OK     =  0,
    TASK_READY  =  1,
    TASK_ERROR  =  2
    
}Task_Status_t;

typedef struct 
{
    Task_Number_t   task_number;
    Task_Status_t   (*taskFunc)(void);
    Task_Type_t     task_type;
    Task_Switch_t   task_switch;
    Task_Status_t   task_status;
    uint16_t        task_tickCnt;
    uint16_t        task_tickInit;
 
}TASK_SCHEDULE_t;

void Task_Start(TASK_SCHEDULE_t* TaskSchedule,Task_Number_t task_number);
void Task_Stop(TASK_SCHEDULE_t* TaskSchedule,Task_Number_t task_number) ;
void Start_Schedule(TASK_SCHEDULE_t* TaskSchedule);
void ScheduleSystickHander(TASK_SCHEDULE_t* TaskSchedule);



//TASK_SCHEDULE_t TaskSchedule[]=
//{
//    {TASK_NO1,       Task1,    TASK_CYCLC,      TASK_ON,      TASK_OK,        0,     100     },
//    {TASK_NO2,       Task2,    TASK_CYCLC,      TASK_ON,      TASK_OK,        1,     100     },
//    {TASK_NO3,       Task3,    TASK_CYCLC,      TASK_ON,      TASK_OK,        2,     100     },
//    {TASK_NO4,       Task4,    TASK_CYCLC,      TASK_ON,      TASK_OK,        3,     100     },
//    {TASK_NO5,       Task5,    TASK_CYCLC,      TASK_ON,      TASK_OK,        4,     100     },
//    {TASK_NO6,       Task6,    TASK_CYCLC,      TASK_ON,      TASK_OK,        5,     100     },
//    {TASK_NO7,       Task7,    TASK_CYCLC,      TASK_ON,      TASK_OK,        6,     100     },
//    {TASK_NO8,       Task8,    TASK_CYCLC,      TASK_ON,      TASK_OK,        7,     100     },

//}; 

//Task_Status_t Task1(void);
//Task_Status_t Task2(void);
//Task_Status_t Task3(void);
//Task_Status_t Task4(void);
//Task_Status_t Task5(void);
//Task_Status_t Task6(void);
//Task_Status_t Task7(void);
//Task_Status_t Task8(void);

//Task_Status_t Task1(void)
//{
//    
//    return TASK_OK;
//}
//Task_Status_t Task2(void)
//{
//    
//    return TASK_OK;
//}
//Task_Status_t Task3(void)
//{
//    
//    return TASK_OK;
//}
//Task_Status_t Task4(void)
//{
//    
//    return TASK_OK;
//}
//Task_Status_t Task5(void)
//{
//    
//    return TASK_OK;
//}
//Task_Status_t Task6(void)
//{
//    
//    return TASK_OK;
//}
//Task_Status_t Task7(void)
//{
//    
//    return TASK_OK;
//}
//Task_Status_t Task8(void)
//{
//    
//    return TASK_OK;
//}
#endif





























