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


def POST(path, body):
    try:
        path = "http://" + ADDR + path
        body = json.dumps(body)
        print("GET " + path + " " + body)
        resp = requests.post(path, data=body)
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
        presets = data["presets"]
    except:
        pass

    menu = (pystray.MenuItem(text="LeftClick", action=CmdPanel, default=True, visible=False),)

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

Icon.run()
