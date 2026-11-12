#include "xparameters.h"
#include "xil_types.h"
#include "xil_cache.h"
#include "xtmrctr.h"
#include "xdigit_mlp.h"

extern char inbyte(void);
extern void outbyte(char c);

#define N_IN 784
#define IMG_ADDR (XPAR_MIG_7SERIES_0_BASEADDR + 0x100000)

int main(void) {
    XDigit_mlp ip;
    XTmrCtr tmr;
    volatile u8 *img = (volatile u8 *)IMG_ADDR;

    Xil_DCacheDisable();

    if (XDigit_mlp_Initialize(&ip, XPAR_DIGIT_MLP_0_DEVICE_ID) != XST_SUCCESS) return 1;
    if (XTmrCtr_Initialize(&tmr, XPAR_TMRCTR_0_DEVICE_ID) != XST_SUCCESS) return 1;

    while (1) {
        for (int i = 0; i < N_IN; i++) img[i] = (u8)inbyte();

        XDigit_mlp_Set_img(&ip, (u64)(UINTPTR)img);

        XTmrCtr_Reset(&tmr, 0);
        XTmrCtr_Start(&tmr, 0);
        XDigit_mlp_Start(&ip);
        while (!XDigit_mlp_IsDone(&ip));
        XTmrCtr_Stop(&tmr, 0);

        u32 cycles = XTmrCtr_GetValue(&tmr, 0);
        u32 digit  = XDigit_mlp_Get_return(&ip);

        outbyte((char)digit);
        for (int k = 0; k < 4; k++) outbyte((char)(cycles >> (8 * k)));
    }
}
