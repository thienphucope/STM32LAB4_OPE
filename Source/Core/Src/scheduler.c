/*
 * scheduler.c
 *
 *  Created on: 29 Oct 2024
 *      Author: HPVictus
 */


#include "scheduler.h"


sTask* headtask;
uint32_t taskcount;
uint32_t waittime;
uint32_t availableID;
uint32_t IDtoPrint; //to save ID just ran task to Print after calling Dispatch

uint8_t Error_code_G;


void SCH_Init(void){
	headtask = NULL;
	taskcount = 0;
	waittime = 0;
	availableID = 1;
	IDtoPrint = 0;
}



void enqueue(sTask* newtask){
	if (headtask == NULL){
		headtask = newtask;
		return;
	}

	sTask* now = headtask;
	sTask* pre = NULL;

	while (now != NULL)
	{
		if (now->Delay > newtask->Delay)
		{
			if (now == headtask)
			{
				newtask->next = headtask;
				headtask = newtask;
				now->Delay -= newtask->Delay;
			}
			else
			{
				newtask->next = now;
				pre->next = newtask;
				now->Delay -= newtask->Delay;
			}
			return;
		}


		newtask->Delay -= now->Delay;
		pre = now;
		now = now->next;
	}


	if (now == NULL)
		{
			pre->next = newtask;
			return;
		}



}
uint32_t SCH_Add_Task(void (*pFunction)(), uint32_t DELAY, uint32_t PERIOD){

	if (taskcount > SCH_MAX_TASKS) {
		Error_code_G = ERROR_SCH_TOO_MANY_TASKS;
	}
	taskcount++;
	sTask* newtask = malloc(sizeof(sTask));
	if (newtask == NULL) return 0;

	newtask->pTask = pFunction;
	newtask->Delay = DELAY / TICK;
	newtask->Period = PERIOD / TICK;
	newtask->TaskID = availableID++;
	newtask->next = NULL;

	enqueue(newtask);

	return newtask->TaskID;
}

void SCH_Update(void){
	if (headtask == NULL){
		waittime = (taskcount > 0) ? waittime + 1 : 0;
		return;
	}
	waittime++;
	if (headtask->Delay > 0){
		int temp = headtask->Delay - waittime;
		if (temp >= 0) {
			headtask->Delay = temp;
			waittime = 0;
		}
		else
		{
			headtask->Delay = 0;
			waittime = 0 - temp;
		}
	}

	return;

}

uint8_t SCH_Dispatch_Tasks(void){
	if (headtask == NULL || headtask->Delay > 0) return 0;
	sTask* run = headtask;
	headtask = headtask->next;
	run->next = NULL;
	run->Delay = run->Period;

	run->pTask();

	IDtoPrint = run->TaskID; // để mà in ra

	if (run->Period != 0) enqueue(run);
	else free(run);
	return 1;
}

uint8_t SCH_Delete_Task(uint32_t deltaskID){
	if (deltaskID <= 0 && deltaskID >= SCH_MAX_TASKS) {
		Error_code_G = ERROR_SCH_CANNOT_DELETE_TASK;
		return 0;
	}
	sTask* now = headtask;
	sTask* pre = NULL;
	if (deltaskID == 1)
	{
		taskcount--;
		headtask = headtask->next;
		now->next->Delay += now->Delay;
		free(now);
		return 1;
	}
	now = headtask;
	pre = NULL;
	while (now->TaskID != deltaskID && now != NULL){
		pre = now;
		now = now->next;
	}
	if (now == NULL) {
		Error_code_G = ERROR_SCH_CANNOT_DELETE_TASK;
	}

	if (now->next != NULL) now->next->Delay += now->Delay;
	if (pre != NULL) pre->next = now->next;
	free(now);
	taskcount--;
	return 1;
}

// Đang hơi xung đột kiểu DỮ LIỆU stdint với unsigned -> nên dùng một cái thôi
// Chỉnh sửa lại Update với Add để cho nó O(1) nhưng mà xoá thêm liên tục nó bị chậm :))

























/////////////////////////////////////////////////////////////////
uint8_t Error_code_G = 0;
uint8_t Last_error_code_G = 0;
uint32_t Error_tick_count_G = 0;


// Hàm báo cáo trạng thái lỗi
/*void SCH_Report_Status(void) {
#ifdef SCH_REPORT_ERRORS
    // Kiểm tra nếu có mã lỗi mới
    if (Error_code_G != Last_error_code_G) {
        // Giả định sử dụng logic âm cho LED (0 sáng, 1 tắt)
        Error_port = 255 - Error_code_G;
        Last_error_code_G = Error_code_G;

        // Đặt thời gian cho mã lỗi hiển thị (giả định 60,000 ticks = 1 phút nếu tick là 1 ms)
        if (Error_code_G != 0) {
            Error_tick_count_G = 60000;  // Hiển thị lỗi trong 1 phút
        } else {
            Error_tick_count_G = 0;      // Không có lỗi, dừng hiển thị
        }
    } else {
        // Nếu vẫn còn thời gian hiển thị lỗi, giảm bộ đếm
        if (Error_tick_count_G != 0) {
            if (--Error_tick_count_G == 0) {
                Error_code_G = 0;        // Đặt lại mã lỗi sau khi hết thời gian
            }
        }
    }
#endif
}
*/
