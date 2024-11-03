/*
 * scheduler.h
 *
 *  Created on: 29 Oct 2024
 *      Author: HPVictus
 */

#ifndef INC_SCHEDULER_H_
#define INC_SCHEDULER_H_

#include "stdint.h"
#include "stdlib.h"
#include "main.h"

#define TICK 10
typedef struct sTask {
    void (*pTask)(void);       // Con trỏ hàm của nhiệm vụ
    uint32_t Delay;            // Độ trễ (tính bằng ticks) trước khi chạy lần đầu
    uint32_t Period;           // Chu kỳ (tính bằng ticks) cho các lần chạy tiếp theo
    uint8_t RunMe;             // Biến đếm để xác định khi nào nhiệm vụ cần được thực thi
    uint32_t TaskID;           // ID của nhiệm vụ
    struct sTask* next;
} sTask;

#define SCH_MAX_TASKS 10      // Số nhiệm vụ tối đa trong scheduler

// Các hàm của scheduler
extern uint32_t IDtoPrint;
// Khởi tạo scheduler
void SCH_Init(void);
// Thêm nhiệm vụ vào scheduler
uint32_t SCH_Add_Task(void (*pFunction)(), uint32_t DELAY, uint32_t PERIOD);
// Cập nhật thời gian đợi của các nhiệm vụ trong scheduler
void SCH_Update(void);
// Điều phối nhiệm vụ - thực thi các nhiệm vụ đã đến thời gian
uint8_t SCH_Dispatch_Tasks(void);
// Xóa nhiệm vụ dựa trên taskID
uint8_t SCH_Delete_Task(const uint32_t taskID);






















//////////////////////////////////////////////////////////////////////
#define ERROR_SCH_TOO_MANY_TASKS           					1
#define ERROR_SCH_CANNOT_DELETE_TASK       					2
#define ERROR_SCH_WAITING_FOR_SLAVE_TO_ACK 					3
#define ERROR_SCH_WAITING_FOR_START_COMMAND_FROM_MASTER 	4
#define ERROR_SCH_ONE_OR_MORE_SLAVES_DID_NOT_START 			5
#define ERROR_SCH_LOST_SLAVE               					6
#define ERROR_SCH_CAN_BUS_ERROR           					7
#define ERROR_I2C_WRITE_BYTE_AT24C64       					8

// Bật tính năng báo lỗi
#define SCH_REPORT_ERRORS
extern uint8_t Error_code_G;
///////////////////////////////////////////////////////////////////////
// Đưa hệ thống vào chế độ ngủ để tiết kiệm năng lượng (tùy chọn)
void SCH_Go_To_Sleep(void);
#define RETURN_ERROR 		9
#define RETURN_NORMAL 		10
// Báo cáo trạng thái lỗi của hệ thống (tùy chọn)
void SCH_Report_Status(void);

#endif /* INC_SCHEDULER_H_ */
