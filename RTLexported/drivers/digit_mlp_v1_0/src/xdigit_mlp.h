// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2023.1 (64-bit)
// Tool Version Limit: 2023.05
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2023 Advanced Micro Devices, Inc. All Rights Reserved.
// 
// ==============================================================
#ifndef XDIGIT_MLP_H
#define XDIGIT_MLP_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/
#ifndef __linux__
#include "xil_types.h"
#include "xil_assert.h"
#include "xstatus.h"
#include "xil_io.h"
#else
#include <stdint.h>
#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stddef.h>
#endif
#include "xdigit_mlp_hw.h"

/**************************** Type Definitions ******************************/
#ifdef __linux__
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
#else
typedef struct {
    u16 DeviceId;
    u64 Control_BaseAddress;
} XDigit_mlp_Config;
#endif

typedef struct {
    u64 Control_BaseAddress;
    u32 IsReady;
} XDigit_mlp;

typedef u32 word_type;

/***************** Macros (Inline Functions) Definitions *********************/
#ifndef __linux__
#define XDigit_mlp_WriteReg(BaseAddress, RegOffset, Data) \
    Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))
#define XDigit_mlp_ReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))
#else
#define XDigit_mlp_WriteReg(BaseAddress, RegOffset, Data) \
    *(volatile u32*)((BaseAddress) + (RegOffset)) = (u32)(Data)
#define XDigit_mlp_ReadReg(BaseAddress, RegOffset) \
    *(volatile u32*)((BaseAddress) + (RegOffset))

#define Xil_AssertVoid(expr)    assert(expr)
#define Xil_AssertNonvoid(expr) assert(expr)

#define XST_SUCCESS             0
#define XST_DEVICE_NOT_FOUND    2
#define XST_OPEN_DEVICE_FAILED  3
#define XIL_COMPONENT_IS_READY  1
#endif

/************************** Function Prototypes *****************************/
#ifndef __linux__
int XDigit_mlp_Initialize(XDigit_mlp *InstancePtr, u16 DeviceId);
XDigit_mlp_Config* XDigit_mlp_LookupConfig(u16 DeviceId);
int XDigit_mlp_CfgInitialize(XDigit_mlp *InstancePtr, XDigit_mlp_Config *ConfigPtr);
#else
int XDigit_mlp_Initialize(XDigit_mlp *InstancePtr, const char* InstanceName);
int XDigit_mlp_Release(XDigit_mlp *InstancePtr);
#endif

void XDigit_mlp_Start(XDigit_mlp *InstancePtr);
u32 XDigit_mlp_IsDone(XDigit_mlp *InstancePtr);
u32 XDigit_mlp_IsIdle(XDigit_mlp *InstancePtr);
u32 XDigit_mlp_IsReady(XDigit_mlp *InstancePtr);
void XDigit_mlp_EnableAutoRestart(XDigit_mlp *InstancePtr);
void XDigit_mlp_DisableAutoRestart(XDigit_mlp *InstancePtr);
u32 XDigit_mlp_Get_return(XDigit_mlp *InstancePtr);

void XDigit_mlp_Set_img(XDigit_mlp *InstancePtr, u64 Data);
u64 XDigit_mlp_Get_img(XDigit_mlp *InstancePtr);

void XDigit_mlp_InterruptGlobalEnable(XDigit_mlp *InstancePtr);
void XDigit_mlp_InterruptGlobalDisable(XDigit_mlp *InstancePtr);
void XDigit_mlp_InterruptEnable(XDigit_mlp *InstancePtr, u32 Mask);
void XDigit_mlp_InterruptDisable(XDigit_mlp *InstancePtr, u32 Mask);
void XDigit_mlp_InterruptClear(XDigit_mlp *InstancePtr, u32 Mask);
u32 XDigit_mlp_InterruptGetEnabled(XDigit_mlp *InstancePtr);
u32 XDigit_mlp_InterruptGetStatus(XDigit_mlp *InstancePtr);

#ifdef __cplusplus
}
#endif

#endif
