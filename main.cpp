#include <memory>
#include <cstdint>
#include <iostream>

#include <evhttp.h>

//#include <jansson.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <json.hpp>


using json =     nlohmann::json;
int main()
{
    json.
    if (!event_init())
    {
        std::cerr << "Failed to init libevent." << std::endl;
        return -1;
    }
    char const SrvAddress[] = "127.0.0.1";
    std::uint16_t SrvPort = 5555;
    std::unique_ptr<evhttp, decltype(&evhttp_free)> Server(evhttp_start(SrvAddress, SrvPort), &evhttp_free);
    if (!Server)
    {
        std::cerr << "Failed to init http server." << std::endl;
        return -1;
    }
    void (*OnReq)(evhttp_request *req, void *) = [] (evhttp_request *req, void *)
    {

        struct evbuffer *requestBuffer;
        size_t requestLen;
        char *requestDataBuffer;

        json_t *requestJSON;
        json_error_t error;

        // Error buffer
        char errorText[1024];

        // Process Request
        requestBuffer = evhttp_request_get_input_buffer(req);
        requestLen = evbuffer_get_length(requestBuffer);

        requestDataBuffer = (char *)malloc(sizeof(char) * requestLen);
        memset(requestDataBuffer, 0, requestLen);
        evbuffer_copyout(requestBuffer, requestDataBuffer, requestLen);

        printf("%s\n", evhttp_request_uri(req));

        requestJSON = json_loadb(requestDataBuffer, requestLen, 0, &error);
        free(requestDataBuffer);


        auto *OutBuf = evhttp_request_get_output_buffer(req);
        if (!OutBuf)
            return;
        evbuffer_add_printf(OutBuf, "Hello ворлд !");
        evhttp_send_reply(req, HTTP_OK, "", OutBuf);
    };
    evhttp_set_gencb(Server.get(), OnReq, nullptr);
    if (event_dispatch() == -1)
    {
        std::cerr << "Failed to run messahe loop." << std::endl;
        return -1;
    }
    return 0;
}
