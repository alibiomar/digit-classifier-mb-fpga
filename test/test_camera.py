import cv2

for idx in (0, 1, 2):
    for name, be in (("DSHOW", cv2.CAP_DSHOW), ("MSMF", cv2.CAP_MSMF)):
        cap = cv2.VideoCapture(idx, be)
        ok = cap.isOpened() and cap.read()[0]
        print(idx, name, "OK" if ok else "fail")
        cap.release()