// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2023.1 (64-bit)
// Tool Version Limit: 2023.05
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2023 Advanced Micro Devices, Inc. All Rights Reserved.
// 
// ==============================================================
#ifndef __linux__

#include "xstatus.h"
#include "xparameters.h"
#include "xdigit_mlp.h"

extern XDigit_mlp_Config XDigit_mlp_ConfigTable[];

XDigit_mlp_Config *XDigit_mlp_LookupConfig(u16 DeviceId) {
	XDigit_mlp_Config *ConfigPtr = NULL;

	int Index;

	for (Index = 0; Index < XPAR_XDIGIT_MLP_NUM_INSTANCES; Index++) {
		if (XDigit_mlp_ConfigTable[Index].DeviceId == DeviceId) {
			ConfigPtr = &XDigit_mlp_ConfigTable[Index];
			break;
		}
	}

	return ConfigPtr;
}

int XDigit_mlp_Initialize(XDigit_mlp *InstancePtr, u16 DeviceId) {
	XDigit_mlp_Config *ConfigPtr;

	Xil_AssertNonvoid(InstancePtr != NULL);

	ConfigPtr = XDigit_mlp_LookupConfig(DeviceId);
	if (ConfigPtr == NULL) {
		InstancePtr->IsReady = 0;
		return (XST_DEVICE_NOT_FOUND);
	}

	return XDigit_mlp_CfgInitialize(InstancePtr, ConfigPtr);
}

#endif

