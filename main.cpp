#include <memory>
#include <cstdint>
#include <iostream>

#include <evhttp.h>
#include <jansson.h>
#include <cstring>

#define HEADER_BUFFER_SIZE 1024
#define ERROR_RESPONSE_SIZE 1024

int main()
{
    if (!event_init()) {
        std::cerr << "Failed to init libevent." << std::endl;
        return -1;
    }

    char const SrvAddress[] = "127.0.0.1";
    std::uint16_t SrvPort = 5555;
    std::unique_ptr<evhttp, decltype(&evhttp_free)> Server(evhttp_start(SrvAddress, SrvPort), &evhttp_free);

    if (!Server) {
        std::cerr << "Failed to init http server." << std::endl;
        return -1;
    }

    void (*OnReq)(evhttp_request *req, void *) = [] (evhttp_request *req, void *) {
        auto *OutBuf = evhttp_request_get_output_buffer(req);
        if (!OutBuf)
            return;
//        evbuffer_add_printf(OutBuf, "<html><body><center><h1>Hello Wotld!</h1></center></body></html>");
//        evhttp_send_reply(req, HTTP_OK, "", OutBuf);

        // прием json

        json_t *requestJSONobj;
        struct evbuffer *requestBuffer;
        size_t requestLen;
        char *requestDataBuffer;
        json_error_t error;


        requestBuffer = evhttp_request_get_input_buffer(req);
        requestLen = evbuffer_get_length(requestBuffer);
        requestDataBuffer = (char *)malloc(sizeof(char) * requestLen);
        evbuffer_copyout(requestBuffer, requestDataBuffer, requestLen);
        requestJSONobj = json_loadb(requestDataBuffer, requestLen, 0, &error);


        // отправка json
        json_t *responseJSON;
        json_t *message;

        char responseHeader[HEADER_BUFFER_SIZE];

        char *responseData;
        int responseLen;
        struct evbuffer *responseBuffer;

        // Create JSON response data
//        responseJSON = json_object();
//
//        message = json_string("Hello World");
//        json_object_set_new(responseJSON, "message", message);

        // dump JSON to buffer and store response length as string
        responseData = json_dumps(requestJSONobj, JSON_INDENT(3));
        responseLen = static_cast<int>(strlen(responseData));
        snprintf(responseHeader, HEADER_BUFFER_SIZE, "%d", responseLen);
        json_decref(requestJSONobj);

        // create a response buffer to send reply
        responseBuffer = evbuffer_new();

        // add appropriate headers
        evhttp_add_header(req->output_headers, "Content-Type", "application/json");
        evhttp_add_header(req->output_headers, "Content-Length", responseHeader);

//        evbuffer_add(responseBuffer, responseData, responseLen);

        evbuffer_add_printf(OutBuf, responseData);
        evhttp_send_reply(req, HTTP_OK, "", OutBuf);


        // send the reply
//        evhttp_send_reply(request, 200, "OK", responseBuffer);


    };

    evhttp_set_gencb(Server.get(), OnReq, nullptr);
    if (event_dispatch() == -1) {
        std::cerr << "Failed to run messahe loop." << std::endl;
        return -1;
    }

    return 0;
}
