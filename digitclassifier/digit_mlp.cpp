#include "digit_mlp.h"
#include "weights.h"

int digit_mlp(const unsigned char img[N_IN]) {
#pragma HLS INTERFACE m_axi port=img offset=slave bundle=gmem depth=784
#pragma HLS INTERFACE s_axilite port=img
#pragma HLS INTERFACE s_axilite port=return

    unsigned char buf[N_IN];
    for (int i = 0; i < N_IN; i++) {
#pragma HLS PIPELINE II=1
        buf[i] = img[i];
    }

    signed char h8[N_HID];
    for (int j = 0; j < N_HID; j++) {
        int acc = B1[j];
        for (int i = 0; i < N_IN; i++) {
#pragma HLS PIPELINE II=1
            acc += (int)buf[i] * (int)W1[j * N_IN + i];
        }
        if (acc < 0) acc = 0;
        acc >>= SHIFT;
        if (acc > 127) acc = 127;
        h8[j] = (signed char)acc;
    }

    int best = 0;
    int best_v = -2147483647;
    for (int k = 0; k < N_OUT; k++) {
        int acc = B2[k];
        for (int j = 0; j < N_HID; j++) {
            acc += (int)h8[j] * (int)W2[k * N_HID + j];
        }
        if (acc > best_v) { best_v = acc; best = k; }
    }
    return best;
}
