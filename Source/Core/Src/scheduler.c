/*
 * scheduler.c
 *
 *  Created on: 29 Oct 2024
 *      Author: HPVictus
 */


#include "scheduler.h"


sTask SCH_tasks_G[SCH_MAX_TASKS];//Khai báo đây để bao đóng
uint8_t current_index_task = 0;//Bình thường là khai báo ở trong HÀM


// Đang hơi xung đột kiểu DỮ LIỆU stdint với unsigned -> nên dùng một cái thôi



// Khởi tạo scheduler
void SCH_Init(void) {
    unsigned char i;
    for (i = 0; i < SCH_MAX_TASKS; i++) {
        SCH_Delete_Task(i);     // Xóa các nhiệm vụ khỏi mảng
    }
    Error_code_G = 0;           // Đặt mã lỗi về 0
    //Timer_init();               // Khởi tạo bộ đếm thời gian
    //Watchdog_init();            // Khởi tạo watchdog
}


// Thêm nhiệm vụ vào scheduler
unsigned char SCH_Add_Task(void (*pFunction)(), unsigned int DELAY, unsigned int PERIOD) {
    unsigned char Index = 0;
    // Tìm vị trí trống trong mảng nhiệm vụ
    while ((SCH_tasks_G[Index].pTask != 0) && (Index < SCH_MAX_TASKS)) {
        Index++;
    }
    // Kiểm tra nếu mảng đã đầy
    if (Index == SCH_MAX_TASKS) {
        Error_code_G = ERROR_SCH_TOO_MANY_TASKS; // Đặt mã lỗi
        return SCH_MAX_TASKS;
    }
    // Thêm nhiệm vụ vào mảng
    SCH_tasks_G[Index].pTask = pFunction;
    SCH_tasks_G[Index].Delay = DELAY;
    SCH_tasks_G[Index].Period = PERIOD;
    SCH_tasks_G[Index].RunMe = 0;

    //SCH_tasks_G[Index].TaskID =
    return Index;
}

// Cập nhật thời gian đợi của các nhiệm vụ trong scheduler
void SCH_Update(void) {
    unsigned char Index;
    for (Index = 0; Index < SCH_MAX_TASKS; Index++) {
        if (SCH_tasks_G[Index].pTask) {      // Nếu nhiệm vụ tồn tại
            if (SCH_tasks_G[Index].Delay == 0) {
                SCH_tasks_G[Index].RunMe += 1;    // Đánh dấu nhiệm vụ đã đến hạn chạy
                if (SCH_tasks_G[Index].Period) {
                    SCH_tasks_G[Index].Delay = SCH_tasks_G[Index].Period; // Cập nhật chu kỳ
                }
            } else {
                SCH_tasks_G[Index].Delay -= 1;    // Giảm thời gian chờ
            }
        }
    }
}

// Điều phối nhiệm vụ - thực thi các nhiệm vụ đã đến thời gian
void SCH_Dispatch_Tasks(void) {
    unsigned char Index;
    for (Index = 0; Index < SCH_MAX_TASKS; Index++) {
        if (SCH_tasks_G[Index].RunMe > 0) {
            (*SCH_tasks_G[Index].pTask)();       // Thực thi nhiệm vụ
            SCH_tasks_G[Index].RunMe -= 1;       // Đặt lại cờ
            // Nếu nhiệm vụ chỉ chạy một lần, xóa khỏi mảng
            if (SCH_tasks_G[Index].Period == 0) {
                SCH_Delete_Task(Index);
            }
        }
    }
}

// Xóa nhiệm vụ dựa trên taskID
unsigned char SCH_Delete_Task(const unsigned int taskID) {
    unsigned char Return_code;
    if (SCH_tasks_G[taskID].pTask == 0) {
        Error_code_G = ERROR_SCH_CANNOT_DELETE_TASK;
        Return_code = RETURN_ERROR;
    } else {
        Return_code = RETURN_NORMAL;
    }
    SCH_tasks_G[taskID].pTask = 0x0000;
    SCH_tasks_G[taskID].Delay = 0;
    SCH_tasks_G[taskID].Period = 0;
    SCH_tasks_G[taskID].RunMe = 0;
    return Return_code;
}

//Chuyển MCU vào SLEEP
void SCH_Go_To_Sleep(void) {
    // Chuyển MCU vào chế độ ngủ (tuỳ chọn)
}

/////////////////////////////////////////////////////////////////
uint8_t Error_code_G = 0;
uint8_t Last_error_code_G = 0;
uint32_t Error_tick_count_G = 0;
uint8_t Error_code_G;

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
