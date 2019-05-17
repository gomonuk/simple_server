#include <memory>
#include <cstdint>
#include <iostream>

#include <evhttp.h>

int main()
{
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

        struct evbuffer* buf = nullptr;
        size_t len = 0;
        char* data = nullptr;

        if (req == NULL) return; // req is null after a timeout

        // get the event buffer containing POST body
        buf = evhttp_request_get_input_buffer(req);

        // get the length of POST body
        len = evbuffer_get_length(buf);

        // create a char array to extract POST body
        data = (char *)malloc(len + 1);
        data[len] = 0;

        // copy POST body into your char array
        evbuffer_copyout(buf, data, len);


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
