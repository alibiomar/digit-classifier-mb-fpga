#include <cstdio>
#include "digit_mlp.h"

#define N_TEST 1000

static unsigned char X[N_TEST][784];
static unsigned char y[N_TEST];

int main() {
    FILE *f = fopen("test_set.bin", "rb");
    if (!f) { printf("test_set.bin not found\n"); return 1; }
    fread(X, 1, N_TEST * 784, f);
    fread(y, 1, N_TEST, f);
    fclose(f);

    int ok = 0;
    for (int i = 0; i < N_TEST; i++)
        if (digit_mlp(X[i]) == y[i]) ok++;

    printf("acc %d/%d = %.2f%%\n", ok, N_TEST, 100.0 * ok / N_TEST);
    return 0;
}
