from PIL import Image
import pystray
import requests
import json


image = Image.open("icon.png")
ADDR = "192.168.0.115"


def after_click(icon, query):
    if str(query) == "GeeksforGeeks Website":
        print("The Best Place to learn anything Tech \
	Related -> https://www.geeksforgeeks.org/")
    # icon.stop()
    elif str(query) == "GeeksforGeeks Youtube":
        print("Youtube Channel of GeeksforGeeks \
	is -> https://www.youtube.com/@GeeksforGeeksVideos")
    # icon.stop()
    elif str(query) == "GeeksforGeeks LinkedIn":
        print("LinkedIn of GeeksforGeeks \
	is -> https://www.linkedin.com/company/geeksforgeeks/")
    elif str(query) == "Exit":
        icon.stop()


def DeskGoTo(height):
    data = {"height" : height}
    requests.post("http://" + ADDR + "/drive/height", data = json.dumps(data))

def DeskGo80():
    DeskGoTo(80)

def DeskGo85():
    DeskGoTo(85)

def DeskGo90():
    DeskGoTo(90)

def Exit():
    icon.stop()

icon = pystray.Icon("GFG", image, "GeeksforGeeks",
                    menu=pystray.Menu(
                        pystray.MenuItem("90",
                                         DeskGo90),
                        pystray.MenuItem("85",
                                         DeskGo85),
                        pystray.MenuItem("80",
                                         DeskGo80),
                        pystray.MenuItem("Exit", Exit)))

icon.run()
