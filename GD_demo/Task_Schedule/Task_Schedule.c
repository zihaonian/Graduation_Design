#include "Task_Schedule.h"
#include "usart.h"
void Task_Start(TASK_SCHEDULE_t* TaskSchedule, Task_Number_t task_number)
{
    uint8_t indx = 0;
 
    for(indx = 0; indx < TASK_MAX_NO ; indx++)
    {
        if(task_number == TaskSchedule[indx].task_number)
        {
            TaskSchedule[indx].task_switch = TASK_ON;
            TaskSchedule[indx].task_tickCnt = TaskSchedule[indx].task_tickInit;
            break;
        }
    }
    
}

void Task_Stop(TASK_SCHEDULE_t* TaskSchedule,Task_Number_t task_number)
{
    uint8_t indx = 0;
    
    for(indx = 0; indx < TASK_MAX_NO; indx++)
    {
        if(task_number == TaskSchedule[indx].task_number)
        {
            TaskSchedule[indx].task_switch = TASK_OFF;
            TaskSchedule[indx].task_tickCnt = 0;
            break;
        }
    }
}


void Start_Schedule(TASK_SCHEDULE_t* TaskSchedule)
{
    uint8_t indx;
    while(1)
    {
        for(indx = 0; indx < TASK_MAX_NO; indx++)
        {
            switch(TaskSchedule[indx].task_status)
            {
                case TASK_OK:
                {
                    break;
                }
                case TASK_ERROR:
                {
                    printf("TASK_ERROR");
                    break;
                }
                case TASK_READY:
                {
                    if(TaskSchedule[indx].taskFunc != NULL)
                    {
                        TaskSchedule[indx].task_status = TaskSchedule[indx].taskFunc();
 
                        switch(TaskSchedule[indx].task_type)
                        {
                            case TASK_CYCLC:
                            {
                                TaskSchedule[indx].task_tickCnt = TaskSchedule[indx].task_tickInit;
                                break;
                            }
                            case TASK_EVENT:
                            {
                                TaskSchedule[indx].task_switch = TASK_OFF;
                                break;
                            }
                        }
                    }
                    break;
                }
            }
        }
    }   
}

void ScheduleSystickHander(TASK_SCHEDULE_t* TaskSchedule)
{
    uint8_t indx = 0;
 
    for(indx = 0; indx < TASK_MAX_NO; indx++)
    {
        if(TaskSchedule[indx].task_switch == TASK_ON && TaskSchedule[indx].task_status != TASK_READY)
        {
            if(TaskSchedule[indx].task_tickCnt > 0)
            {
                TaskSchedule[indx].task_tickCnt--;
            }
 
            if(TaskSchedule[indx].task_tickCnt == 0)
            {
                TaskSchedule[indx].task_status = TASK_READY;
            }
 
        }
    }
}

