from PIL import Image
import threading
import pystray
import requests
import json
import time
import webbrowser
from functools import partial

ICON = Image.open("icon.png")
ADDR = "192.168.0.115"

ThreadsAlive = True


def POST(path, body=None):
    try:
        path = "http://" + ADDR + path
        resp = None

        if body is not None:
            body = json.dumps(body)
            print("GET " + path + " " + body)
            resp = requests.post(path, data=body)
        else:
            print("GET " + path)
            resp = requests.post(path)

        print(str(resp.status_code) + " " + resp.text)
        return resp
    except:
        print("POST failed")
        return None


def GET(path):
    try:
        path = "http://" + ADDR + path
        print("GET " + path)
        resp = requests.get(path)
        print(str(resp.status_code) + " " + resp.text)
        return resp
    except:
        print("GET failed")
        return None


def CmdExit(icon, item):
    print("CMD: Exit")
    icon.stop()
    ThreadsAlive = False


def CmdPanel(icon, item):
    print("CMD: Panel")
    webbrowser.open("http://" + ADDR)


def CmdDriveTo(height, icon, item):
    return POST("/drive/height", {"height": height})


def Reload(icon, item):
    presets = []

    try:
        resp = GET("/health")
        data = json.loads(resp.text)
        presets = data["config"]["drive"]["presets"]
    except:
        pass

    menu = (pystray.MenuItem(text="LeftClick", action=CmdPanel, default=True, visible=False),)

    menu = menu + (pystray.MenuItem("Panel", CmdPanel),)
    menu = menu + (pystray.MenuItem("Reload", Reload),)

    menu = menu + (pystray.Menu.SEPARATOR,)

    for preset in presets:
        menu = menu + (pystray.MenuItem(str(preset), partial(CmdDriveTo, preset)),)

    menu = menu + (pystray.Menu.SEPARATOR,)

    menu = menu + (pystray.MenuItem('Exit', CmdExit),)

    icon.menu = pystray.Menu(*menu)


# Create the icon
Icon = pystray.Icon("Desk Controller", ICON)

Reload(Icon, None)


def PowerTask():
    while ThreadsAlive:
        print("Power thread!")
        time.sleep(10)


def TrayTask():
    Icon.run()


powerThread = threading.Thread(target=PowerTask)
trayThread = threading.Thread(target=TrayTask)
powerThread.start()
trayThread.start()
powerThread.join()
trayThread.join()
