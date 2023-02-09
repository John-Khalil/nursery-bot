# import websocket
# import json
# import time

# def on_message(ws, message):
#     print("Received message: ", message)

# def on_error(ws, error):
#     print("Error: ", error)

# def on_close(ws):
#     print("Connection closed")

# def on_open(ws):
#     while True:
#         message = {"instruction":1024,"servoPosition":2400}
#         ws.send("data:"+json.dumps(message)+"<@>")
#         time.sleep(5)
#         message = {"instruction":1024,"servoPosition":500}
#         ws.send("data:"+json.dumps(message)+"<@>")
#         time.sleep(5)


# if __name__ == "__main__":
#     websocket.enableTrace(True)
#     ws = websocket.WebSocketApp("ws://192.168.1.18:80",
#                               on_message = on_message,
#                               on_error = on_error,
#                               on_close = on_close)
#     ws.on_open = on_open
#     ws.run_forever()

import websocket
import json
import time




def on_message(ws, message):
    print("--------Received message: ", message)
    # Deserialize the received JSON message
    # received_message = json.loads(message)

    # # Extract values from the JSON object
    # servo = received_message["medo"]

    # # Print the extracted values
    # print("medo:", servo)
 

def on_error(ws, error):
    print("Error: ", error)

def on_close(ws):
    print("Connection closed")

def on_open(ws):
    message = {"auth":"test0"}
    ws.send(json.dumps(message))
    # while True:
    input_text =json.dumps({"servo":1024,"pos":0,"pos2":180})
    ws.send(""+input_text+"")



# if name == "main":
#     websocket.enableTrace(True)
#     ws = websocket.WebSocketApp("ws://xtensa32plus.ddns.net:51",
#                               on_message = on_message,
#                               on_error = on_error,
#                               on_close = on_close)
#     ws.on_open = on_open ##law ha receive yeb2a : on_message 
#     ws.run_forever()
websocket.enableTrace(True)
ws = websocket.WebSocketApp("ws://xtensa32plus.ddns.net:51",
                            on_message = on_message,
                            on_error = on_error,
                            on_close = on_close)
ws.on_open = on_open ##law ha receive yeb2a : on_message 
ws.run_forever()

