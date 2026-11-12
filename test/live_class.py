import collections
import threading
import time

import cv2
import numpy as np
import serial

PORT = "COM7"
BAUD = 115200          # must match the UART Lite baud
CLK_HZ = 81.25e6     # ui_clk
BOX = 300            # capture box size in pixels

state = {"frame": None, "prev": None, "digit": None, "us": 0.0, "rt": 0.0,
         "status": "", "dark": True, "run": True}


def preprocess(roi, dark_ink):
    g = cv2.GaussianBlur(cv2.cvtColor(roi, cv2.COLOR_BGR2GRAY), (5, 5), 0)
    mode = cv2.THRESH_BINARY_INV if dark_ink else cv2.THRESH_BINARY
    _, b = cv2.threshold(g, 0, 255, mode + cv2.THRESH_OTSU)

    # drop small noise blobs
    n, lab, stats, _ = cv2.connectedComponentsWithStats(b)
    keep = np.zeros_like(b)
    for i in range(1, n):
        if stats[i, cv2.CC_STAT_AREA] >= 60:
            keep[lab == i] = 255
    ys, xs = np.where(keep > 0)
    if len(ys) < 100 or len(ys) > 0.4 * keep.size:
        return None

    k = keep[ys.min():ys.max() + 1, xs.min():xs.max() + 1]
    side = max(k.shape)

    area = cv2.countNonZero(k)
    edge = area - cv2.countNonZero(cv2.erode(k, np.ones((3, 3), np.uint8)))
    t = 2 * area / max(edge, 1)
    h, w = k.shape
    s = 20.0 / max(h, w)
    nh, nw = max(1, round(h * s)), max(1, round(w * s))
    k = cv2.resize(k, (nw, nh), interpolation=cv2.INTER_AREA)

    out = np.zeros((28, 28), dtype=np.uint8)
    y0, x0 = (28 - nh) // 2, (28 - nw) // 2
    out[y0:y0 + nh, x0:x0 + nw] = k
    return (out.astype(np.float32) * 255 / max(out.max(), 1)).astype(np.uint8)


def worker(ser):
    hist = collections.deque(maxlen=5)
    while state["run"]:
        roi = state["frame"]
        if roi is None:
            time.sleep(0.01)
            continue

        x = preprocess(roi, state["dark"])
        state["prev"] = x
        if x is None:
            state["digit"] = None
            hist.clear()
            time.sleep(0.05)
            continue

        t0 = time.time()
        ser.write(x.tobytes())
        r = ser.read(5)
        if len(r) != 5:
            state["status"] = "timeout: press board reset"
            ser.reset_input_buffer()
            continue

        state["status"] = ""
        state["rt"] = time.time() - t0
        state["us"] = int.from_bytes(r[1:], "little") / CLK_HZ * 1e6
        hist.append(r[0])
        state["digit"] = collections.Counter(hist).most_common(1)[0][0]


def main():
    ser = serial.Serial(PORT, BAUD, timeout=5)
    ser.reset_input_buffer()
    cap = cv2.VideoCapture(0, cv2.CAP_DSHOW)
    if not cap.isOpened():
        raise SystemExit("camera not found")

    th = threading.Thread(target=worker, args=(ser,), daemon=True)
    th.start()

    while True:
        ok, frame = cap.read()
        if not ok:
            break
        h, w = frame.shape[:2]
        x0, y0 = (w - BOX) // 2, (h - BOX) // 2
        state["frame"] = frame[y0:y0 + BOX, x0:x0 + BOX].copy()

        cv2.rectangle(frame, (x0, y0), (x0 + BOX, y0 + BOX), (0, 255, 0), 2)

        p = state["prev"]
        if p is not None:
            small = cv2.resize(p, (112, 112), interpolation=cv2.INTER_NEAREST)
            frame[8:120, 8:120] = cv2.cvtColor(small, cv2.COLOR_GRAY2BGR)

        d = state["digit"]
        label = "-" if d is None else str(d)
        cv2.putText(frame, label, (x0 + BOX + 30, y0 + 140),
                    cv2.FONT_HERSHEY_SIMPLEX, 5, (0, 255, 0), 8)
        cv2.putText(frame, f"FPGA {state['us']:.0f} us | trip {state['rt'] * 1000:.0f} ms",
                    (10, h - 40), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 255, 255), 2)
        mode = "dark ink on light" if state["dark"] else "light on dark"
        cv2.putText(frame, f"{mode}  [i] toggle  [q] quit  {state['status']}",
                    (10, h - 12), cv2.FONT_HERSHEY_SIMPLEX, 0.55, (0, 255, 255), 1)

        cv2.imshow("FPGA digit recognition", frame)
        key = cv2.waitKey(1) & 0xFF
        if key == ord("q"):
            break
        if key == ord("i"):
            state["dark"] = not state["dark"]

    state["run"] = False
    th.join()
    ser.close()
    cap.release()
    cv2.destroyAllWindows()


if __name__ == "__main__":
    main()