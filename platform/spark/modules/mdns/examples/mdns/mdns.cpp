#include "MDNS.h"
#include "application.h"

#include <string>
#include <vector>

#define HTTP_PORT 80
#define ALT_HTTP_PORT 8080

std::string hostname(System.deviceID().c_str());
MDNS mdns(hostname);

TCPServer server = TCPServer(HTTP_PORT);
TCPServer altServer = TCPServer(ALT_HTTP_PORT);

void
setup()
{
    server.begin();
    altServer.begin();

    std::vector<std::string> subServices;

    // udp/tcp, protocol type, service name, txt entries, subservices
    mdns.addService(MDNS::Protocol::TCP, "_customhttp", "Core 1", HTTP_PORT, {"normal"}, {"printer"});

    mdns.addService(MDNS::Protocol::TCP, "_http", "Core alt", ALT_HTTP_PORT, {"alt"});

    bool success = mdns.begin(true);
}

void
loop()
{
    mdns.processQueries();

    TCPClient client = server.available();

    if (client) {
        while (client.read() != -1)
            ;

        client.write("HTTP/1.1 200 Ok\n\n<html><body><h1>Ok!</h1></body></html>\n\n");
        client.flush();
        delay(5);
        client.stop();
    }

    TCPClient altClient = altServer.available();

    if (altClient) {
        while (altClient.read() != -1)
            ;

        altClient.write("HTTP/1.1 200 Ok\n\n<html><body><h1>Alternative port ok!</h1></body></html>\n\n");
        altClient.flush();
        delay(5);
        altClient.stop();
    }
}
