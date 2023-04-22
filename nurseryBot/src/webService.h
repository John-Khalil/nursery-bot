#include <stdint.h>
#include <functional>
#include <vector>
#include <memory>
#include <algorithm>
#include <string>
#include <type_traits>
// #include "stringFunctions.cpp"
// #include "consoleLogger.h"

#include <ESPAsyncWebServer.h>

namespace web{

class service{

    public:

        std::vector<std::function<void(uint8_t*)>>readCallbackList;    
        std::vector<AsyncWebSocketClient *>webSocketClientList;

        // AsyncWebSocketClient *webSocketClient;


        AsyncWebServer *server;
        AsyncWebSocket *ws; 


        #define HTTP_ACK "ACK"
        std::string httpResponse=HTTP_ACK;



        service &onData(const std::function<void(uint8_t*)>onReadCallback){
            readCallbackList.push_back(onReadCallback);
            return (*this);
        }

        service &httpSetResponse(uint8_t* data){
            httpResponse=(char*)data;
            return (*this);
        }

        service &httpSetResponse(char* data){
            httpResponse=data;
            return (*this);
        }

        service &httpSetResponse(std::string data){
            httpResponse=data;
            return (*this);
        }

        service &send(uint8_t* data){
            for(auto webSocketClient:webSocketClientList){
                if(webSocketClient->status() == AwsClientStatus::WS_CONNECTED)
                    webSocketClient->text(std::string((char*)data).c_str()); 
            }
            return (*this);
        }

        service &send(char* data){
            for(auto webSocketClient:webSocketClientList){
                if(webSocketClient->status() == AwsClientStatus::WS_CONNECTED)
                    webSocketClient->text(std::string(data).c_str()); 
            }
            return (*this);
        }

        service &send(std::string data){
            for(auto webSocketClient:webSocketClientList){
                if(webSocketClient->status() == AwsClientStatus::WS_CONNECTED)
                    webSocketClient->text(data.c_str());
            }
            return (*this);
        }



        service &setup(uint16_t port,uint8_t* path=(uint8_t*)"/"){
            server=new AsyncWebServer(port);
            ws=new AsyncWebSocket((char*)path);

            ws->onEvent([&](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {

                if (type == WS_EVT_CONNECT) {
                    webSocketClientList.push_back(client);
                } else if (type == WS_EVT_DISCONNECT) {

                } else if (type == WS_EVT_DATA) {
                    String body = "";
                    for (size_t i = 0; i < len; i++) {
                        body += (char)data[i];
                    }
                    for(auto readCallback:readCallbackList)
                        readCallback((uint8_t*)body.c_str());
                    
                }
            });
            server->addHandler(ws);

            server->on((char*)path, (HTTP_GET), [&](AsyncWebServerRequest *request){
                
                AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", httpResponse.c_str());
                response->addHeader("Access-Control-Allow-Origin", "*");
                request->send(response);
                httpResponse=HTTP_ACK;
            });

            server->onRequestBody([&](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
                // Handle the incoming data chunk by chunk
                // 'data' is a pointer to the incoming data buffer
                // 'len' is the length of the data buffer
                // 'index' is the starting index of the data in the buffer
                // 'total' is the total length of the request body
                
                // Convert the incoming data to a String
                String body = "";
                for (size_t i = 0; i < len; i++) {
                    body += (char)data[i];
                }

                // Check if all data has been received
                if (index + len == total) { // indicates the end of data transmition 
                    
                    for(auto readCallback:readCallbackList)
                        readCallback((uint8_t*)body.c_str());

                    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", httpResponse.c_str());
                    response->addHeader("Access-Control-Allow-Origin", "*");
                    request->send(response);
                    httpResponse=HTTP_ACK;
                }
            });



            // server->on((char*)path, (HTTP_GET|HTTP_POST), [&](AsyncWebServerRequest *request){
            //     request->onData([&](uint8_t *data, size_t len){
            //         Serial.print("queryParam : ");

            //         Serial.println(data);
            //     });
            //     request->send(200, "text/plain",httpResponse.c_str());
            //     httpResponse=HTTP_ACK;
            // });
            

            server->begin();

            return (*this);
        }

        service(uint16_t port,uint8_t* path=(uint8_t*)"/"){
            setup(port,path);
        }

        service(uint16_t port,char* path="/"){
            setup(port,(uint8_t*)path);
        }

        service(uint16_t port,std::string path="/"){
            setup(port,(uint8_t*)path.c_str());
        }

        ~service(){
            delete server;
            delete ws;
        }


};

}