from PIL import Image
import threading
import pystray
import requests
import json
import time
import webbrowser

ICON = Image.open("icon.png")
ADDR = "192.168.0.115"

Active = True
Mutex = threading.Lock()
Tray = pystray.Icon("GFG", ICON, "Desk Controller", menu=pystray.Menu())


def AppIsActive():
    global Active
    Mutex.acquire()
    active = Active
    Mutex.release()
    return active


def AppExit():
    global Active
    Tray.stop()
    Mutex.acquire()
    Active = False
    Mutex.release()


def CmdDriveTo(height):
    try:
        data = {"height": height}
        requests.post("http://" + ADDR + "/drive/height", data=json.dumps(data))
    except:
        print("Drive error")


def CmdPanel():
    webbrowser.open("http://" + ADDR)


def Reload():
    # try:
    print("Request GET /health")
    resp = requests.get("http://" + ADDR + "/health")
    print("Response " + str(resp.status_code) + " " + resp.text)

    data = json.loads(resp.text)

    items = []
    items = items + [pystray.MenuItem("Panel", CmdPanel)]
    items = items + [pystray.Menu.SEPARATOR]

    for preset in data["presets"]:
        items = items + [pystray.MenuItem(str(preset), CmdPanel)]

    items = items + [pystray.Menu.SEPARATOR]
    items = items + [pystray.MenuItem("Exit", AppExit)]

    Tray.menu = tuple(items)
    print("Reload ok")
    # except:
    print("Reload failure")


def TrayTask():
    global Tray
    Reload()
    Tray.run()
    pass


def PowerTask():
    while AppIsActive():
        print("Power task!")
        time.sleep(10)


trayThread = threading.Thread(target=TrayTask)
powerThread = threading.Thread(target=PowerTask)

trayThread.start()
powerThread.start()

trayThread.join()
powerThread.join()
