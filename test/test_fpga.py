import sys, time
import numpy as np
import serial
from PIL import Image, ImageFilter

PORT = "COM7"
BAUD = 115200       # must match the UART Lite baud
CLK_HZ = 81.25e6    # ui_clk


def preprocess(path):
    a = np.array(Image.open(path).convert("L"), dtype=np.uint8)
    if a.mean() > 127:
        a = 255 - a
    ys, xs = np.where(a > a.max() * 0.25)
    if len(ys) == 0:
        sys.exit("no digit found")
    a = a[ys.min():ys.max() + 1, xs.min():xs.max() + 1]

    h, w = a.shape
    k = max(2, int(max(h, w) / 14) | 1)          # odd kernel size
    img = Image.fromarray(a).filter(ImageFilter.MaxFilter(k))

    s = 20.0 / max(h, w)
    nh, nw = max(1, round(h * s)), max(1, round(w * s))
    a = np.array(img.resize((nw, nh), Image.LANCZOS))

    out = np.zeros((28, 28), dtype=np.uint8)
    y0, x0 = (28 - nh) // 2, (28 - nw) // 2
    out[y0:y0 + nh, x0:x0 + nw] = a
    out = (out.astype(np.float32) * 255 / max(out.max(), 1)).astype(np.uint8)
    return out


def main():
    if len(sys.argv) < 2:
        sys.exit("usage: python predict.py image.png")

    x = preprocess(sys.argv[1])
    Image.fromarray(x).resize((224, 224), Image.NEAREST).save("sent.png")

    s = serial.Serial(PORT, BAUD, timeout=5)
    s.reset_input_buffer()

    t0 = time.time()
    s.write(x.tobytes())
    r = s.read(5)
    total = time.time() - t0
    s.close()

    if len(r) != 5:
        sys.exit("timeout: no reply from FPGA")

    digit = r[0]
    cycles = int.from_bytes(r[1:], "little")
    fpga_us = cycles / CLK_HZ * 1e6

    print(f"digit:        {digit}")
    print(f"FPGA time:    {fpga_us:.1f} us  ({cycles} cycles)")
    print(f"round trip:   {total:.2f} s (UART transfer)")


if __name__ == "__main__":
    main()