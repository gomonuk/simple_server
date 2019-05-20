#include <memory>
#include <cstdint>
#include <iostream>

#include <evhttp.h>
#include <jansson.h>
#include <cstring>
#include <vector>

#define HEADER_BUFFER_SIZE 1024
int VECTOR_SIZE = 0;


struct messagePost
{
    int count;
    char message[2048];

};
std::vector<messagePost> messagesPostKeep;

void messageAppend( char *message){
    bool needAppend = true;

    for (messagePost &m : messagesPostKeep) {
        if (strcmp(m.message, message) == 0 ) {
            m.count += 1;
            needAppend = false;
        }
    }

    if (needAppend){
        messagePost m = messagePost();
        messagesPostKeep.push_back(m);
        messagesPostKeep[VECTOR_SIZE].count = 1;
        strcpy(messagesPostKeep[VECTOR_SIZE].message, message);
        VECTOR_SIZE += 1;
    }

    for (messagePost &m : messagesPostKeep) {
        std::cerr << m.message << " " << m.count << std::endl;
    }}


void OnGET (evhttp_request *req, void *) {
    auto *OutBuf = evhttp_request_get_output_buffer(req);
    if (!OutBuf)
        return;
    evbuffer_add_printf(OutBuf, "<html><body><center><h1>Hello Wotld!</h1></center></body></html>");
    evhttp_send_reply(req, HTTP_OK, "OK", OutBuf);
};


void OnPOST (evhttp_request *req, void *) {
    auto *OutBuf = evhttp_request_get_output_buffer(req);
    if (!OutBuf)
        return;

    // прием json

    json_t *requestJSONobj;
    struct evbuffer *requestBuffer;
    size_t requestLen;
    char *requestDataBuffer;
    json_error_t error;


    requestBuffer = evhttp_request_get_input_buffer(req);
    requestLen = evbuffer_get_length(requestBuffer);
    requestDataBuffer = (char *) malloc(sizeof(char) * requestLen);
    evbuffer_copyout(requestBuffer, requestDataBuffer, requestLen);
    requestJSONobj = json_loadb(requestDataBuffer, requestLen, 0, &error);

    messageAppend(requestDataBuffer);

    // отправка json
    json_t *responseJSON;
    json_t *message;

    char responseHeader[HEADER_BUFFER_SIZE];

    char *responseData;
    int responseLen;
    struct evbuffer *responseBuffer;

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
    evhttp_send_reply(req, HTTP_OK, "ОК", OutBuf);
}


int main() {
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
    } else {
        std::cerr << "Server run http://" << SrvAddress << ":" << SrvPort << std::endl;
    }

    void (*Router)(evhttp_request *req, void *) = [](evhttp_request *req, void *) {
            switch (evhttp_request_get_command(req)) {
                case EVHTTP_REQ_GET:
                    std::cerr << "EVHTTP_REQ_GET" << std::endl;
                    OnGET(req, nullptr);
                    break;
                case EVHTTP_REQ_POST:
                    std::cerr << "EVHTTP_REQ_POST" << std::endl;
                    OnPOST(req, nullptr);
                    break;
                case EVHTTP_REQ_HEAD:    std::cerr << "HEAD" << std::endl;    break;
                case EVHTTP_REQ_PUT:     std::cerr << "PUT" << std::endl;     break;
                case EVHTTP_REQ_DELETE:  std::cerr << "DELETE" << std::endl;  break;
                case EVHTTP_REQ_OPTIONS: std::cerr << "OPTIONS" << std::endl; break;
                case EVHTTP_REQ_TRACE:   std::cerr << "TRACE" << std::endl;   break;
                case EVHTTP_REQ_CONNECT: std::cerr << "CONNECT" << std::endl; break;
                case EVHTTP_REQ_PATCH:   std::cerr << "PATCH" << std::endl;   break;
                default:                 std::cerr << "unknown" << std::endl; break;
            }
        };

    evhttp_set_gencb(Server.get(), Router, nullptr);
    if (event_dispatch() == -1) {
        std::cerr << "Failed to run loop." << std::endl;
        return -1;
    }

    return 0;
}
