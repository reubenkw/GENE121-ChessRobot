from PIL import ImageGrab
from PIL import ImageStat
import pyautogui
import time

folderAddress = "C:/Users/Reube/Desktop/CPP to RC testing/"

def waitForInFile(desiredFirstElement):
    firstElement = str(desiredFirstElement + 1)      #insures initialization does not cause equality
    while int(firstElement) != desiredFirstElement:
        time.sleep(2)
        with open(folderAddress + "IPC/IPC_CPP_to_RC.txt","r") as fin:
            firstElement = fin.readline()

numCPP = 0
waitForInFile(numCPP)
numCPP += 1

pyautogui.hotkey("win", "ctrlleft", "left")
time.sleep(2)
robotScrnBox = bbox=(30, 500, 375, 675)
image = ImageGrab.grab(robotScrnBox)
stat = ImageStat.Stat(image)
initPVM = sum(stat.mean) / 3.0

while True:
    print("Waiting for RC")
    while True:
        image = ImageGrab.grab(robotScrnBox)

        stat = ImageStat.Stat(image)
        pixelValueMean = stat.mean
        print(pixelValueMean)

        if ((sum(pixelValueMean) / 3.0) < (initPVM - 2)):
            break
        time.sleep(4)

    pyautogui.click(140, 34) #robot dropdown
    time.sleep(1)
    for x in range(0,8):
        pyautogui.hotkey("down")
    pyautogui.hotkey("enter")
    pyautogui.hotkey("down")
    pyautogui.hotkey("down")
    pyautogui.hotkey("enter")
    time.sleep(3)

    pyautogui.doubleClick(50, 190)  #rc data
    time.sleep(1)
    pyautogui.click(35, 155)        #RC->CPP file
    time.sleep(1)
    pyautogui.click(435, 120)       #upload file to computer
    time.sleep(1)
    pyautogui.click(615, 400)       #desktop
    time.sleep(1)
    pyautogui.hotkey("down")
    time.sleep(1)
    pyautogui.hotkey("right")
    time.sleep(1)
    pyautogui.hotkey("right")
    time.sleep(1)
    pyautogui.hotkey("enter")

    print("Waiting for CPP")

    pyautogui.hotkey("win", "ctrlleft", "right")

    waitForInFile(numCPP)
    numCPP += 1

    pyautogui.hotkey("win", "ctrlleft", "left")

    time.sleep(2)

    pyautogui.click(530, 115)   #download to robot
    time.sleep(1)
    pyautogui.click(400, 80)    #search bar
    time.sleep(1)
    pyautogui.typewrite("C:/Users/Reube/Desktop/CPP to RC testing/IPC")
    time.sleep(1)
    pyautogui.click(275, 170)    #CPP->RC file in finder
    time.sleep(1)
    pyautogui.hotkey("enter")
    time.sleep(1)
    pyautogui.click(535, 570)    #close window
    time.sleep(1)
    pyautogui.click(775, 64)     #download to robot
    time.sleep(5)
    pyautogui.click(440, 640)     #start button

    time.sleep(5)
