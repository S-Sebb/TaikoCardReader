# -*- coding: utf-8 -*-
import os
import time

from pyWinhook import HookManager
from win32api import keybd_event
from win32con import KEYEVENTF_KEYUP, KEYEVENTF_EXTENDEDKEY
from win32gui import PumpMessages, PostQuitMessage


def press_f3():
    keybd_event(114, 0, KEYEVENTF_EXTENDEDKEY, 0)


class KeystrokeWatcher(object):
    def __init__(self):
        self.version_number = "3.6"
        self.hm = HookManager()
        self.hm.KeyDown = self.on_keyboard_event
        self.hm.HookKeyboard()
        self.last_stroke_time = None
        self.card_num = []
        if not os.path.exists("nijiiro_path.txt"):
            print("nijiiro_path.txt not found.\n"
                  "Program will quit in 10 seconds.")
            time.sleep(10)
            exit()
        with open("nijiiro_path.txt", "r") as f:
            self.nijiiro_path = f.read().strip()
        if not os.path.exists(os.path.join(self.nijiiro_path, "bnusio.dll")):
            print("Please make sure the path in nijiiro_path.txt points to correct Nijiiro install location.\n"
                  "Program will quit in 10 seconds.")
            time.sleep(10)
            exit()
        self.card_dat_filepath = os.path.join(self.nijiiro_path, "cards.dat")
        if not os.path.exists(self.card_dat_filepath):
            with open(self.card_dat_filepath, "w") as f:
                f.write("00000000000000000000")
        print("Program started. Version " + self.version_number)

    def on_keyboard_event(self, event):
        num_keys = [0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39]
        try:
            if event.KeyID in num_keys:
                self.record_keypress(event.KeyID)
        finally:
            return True

    def record_keypress(self, key):
        num_key_dict = {0x30: "0", 0x31: "1", 0x32: "2", 0x33: "3", 0x34: "4", 0x35: "5", 0x36: "6", 0x37: "7",
                        0x38: "8", 0x39: "9"}
        current_stroke_time = time.time()
        key_num = num_key_dict[key]
        if self.last_stroke_time is None:
            if key_num == "2":
                self.last_stroke_time = current_stroke_time
                self.card_num.append(key_num)
        else:
            if current_stroke_time - self.last_stroke_time > 0.05:
                if key_num == "2":
                    self.last_stroke_time = current_stroke_time
                    self.card_num = [key_num]
                else:
                    self.last_stroke_time = None
                    self.card_num = []
                    return
            else:
                self.card_num.append(key_num)
                self.last_stroke_time = current_stroke_time
        if len(self.card_num) >= 10:
            card_num = "".join(self.card_num)
            if len(card_num) == 10 and card_num.startswith("20000"):
                print("Card num detected: " + card_num)
                card_num = "0000000000" + card_num
                with open(self.card_dat_filepath, "w") as f:
                    f.write(card_num)
                press_f3()
            else:
                print("Invalid card num: " + card_num)
            self.card_num = []
            self.last_stroke_time = None

    def shutdown(self):
        PostQuitMessage(0)
        self.hm.UnhookKeyboard()


watcher = KeystrokeWatcher()
PumpMessages()
