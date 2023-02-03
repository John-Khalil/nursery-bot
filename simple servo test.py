import websocket
import json
import time

def on_message(ws, message):
    print("Received message: ", message)

def on_error(ws, error):
    print("Error: ", error)

def on_close(ws):
    print("Connection closed")

def on_open(ws):
    while True:
        message = {"instruction":1024,"servoPosition":2400}
        ws.send("data:"+json.dumps(message)+"<@>")
        time.sleep(5)
        message = {"instruction":1024,"servoPosition":500}
        ws.send("data:"+json.dumps(message)+"<@>")
        time.sleep(5)


if __name__ == "__main__":
    websocket.enableTrace(True)
    ws = websocket.WebSocketApp("ws://192.168.1.18:80",
                              on_message = on_message,
                              on_error = on_error,
                              on_close = on_close)
    ws.on_open = on_open
    ws.run_forever()
