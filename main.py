from KKT_Module.ksoc_global import kgl
from KKT_Module.Configs import SettingConfigs
from KKT_Module.SettingProcess.SettingProccess import SettingProc, ConnectDevice, ResetDevice
from KKT_Module.DataReceive.DataReciever import RawDataReceiver, HWResultReceiver, FeatureMapReceiver,MultiResult4168BReceiver
import time
import numpy as np
import tensorflow as tf
from tensorflow.keras.models import Sequential, load_model
from tensorflow.keras.layers import Conv2D, MaxPooling2D, Flatten, Dense
from sklearn.model_selection import train_test_split
import serial
from msvcrt import getch, kbhit

# 設定串行通信參數
arduino = serial.Serial(port='COM7', baudrate=9600, timeout=.1)

def send_person_count(count):
    if count<0: count = 0
    byte_str = bytes(str(count), 'utf-8') + b'\n'
    arduino.write(byte_str)


def connect():
    connect = ConnectDevice()
    connect.startUp()                       # Connect to the device
    reset = ResetDevice()
    reset.startUp()                         # Reset hardware register

def startSetting():
    SettingConfigs.setScriptDir("K60168-Test-00256-008-v0.0.8-20230717_480cm")  # Set the setting folder name
    ksp = SettingProc()                 # Object for setting process to setup the Hardware AI and RF before receive data
    ksp.startUp(SettingConfigs)             # Start the setting process
    # ksp.startSetting(SettingConfigs)        # Start the setting process in sub_thread

def startLoop():
    frame_count = 0
    mean = 0
    model = load_model('my_model.h5')
    model.load_weights('./weight_rditrain125.h5')

    # Receiver for getting Raw data
    R = FeatureMapReceiver(chirps=32)       # Receiver for getting RDI PHD map
    R.trigger(chirps=32)                             # Trigger receiver before getting the data
    time.sleep(0.5)
    print('# ======== Start getting gesture ===========')

    while True:
        res = R.getResults()                # Get data from receiver
        if res is None:
            continue
        frame = list(res[0])
        new_frame = frame/np.max(frame)
        new_frame = np.array(new_frame)
        new_frame = new_frame.reshape(1, 32, 32, 1)  # Reshape for model input
        
        predicted_objects = model.predict(new_frame)
        
        frame_count += 1
        mean += round(predicted_objects[0][0])
        frame_unit = 25
        if frame_count == frame_unit:
            frame_count = 0
            final_people = mean // frame_unit
            send_person_count(final_people)
            mean = 0
        
        if kbhit():
            if getch() == b'q':
                break


def main():
    kgl.setLib()

    # kgl.ksoclib.switchLogMode(True)

    connect()                               # First you have to connect to the device

    startSetting()                         # Second you have to set the setting configs

    startLoop()                             # Last you can continue to get the data in the loop

if __name__ == '__main__':
    main()
