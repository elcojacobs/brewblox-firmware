
#include "graphics/graphics.hpp"
#include "graphics/widgets.hpp"
#include "websocketserver.hpp"
#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

#include <thread>
std::shared_ptr<listener> webSocketServer;
int main()
{
    namespace beast = boost::beast;         // from <boost/beast.hpp>
    namespace http = beast::http;           // from <boost/beast/http.hpp>
    namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
    namespace net = boost::asio;            // from <boost/asio.hpp>
    using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
    // auto const address = net::ip::make_address("0.0.0.0");
    // auto const port = static_cast<unsigned short>(7376);
    // auto const threads = 1;

    net::io_context ioc{1};

    webSocketServer = std::make_shared<listener>(ioc, tcp::endpoint{net::ip::make_address("0.0.0.0"), 7376});
    webSocketServer->run();

    // Run the I/O service on the requested number of threads
    // std::vector<std::thread> v;
    // v.reserve(threads - 1);
    // for (auto i = threads - 1; i > 0; --i)
    //     v.emplace_back([&ioc] { ioc.run(); });
    // server->run();

    auto graphics = Graphics::getInstance();

    static std::array<NormalWidget, 5> sensorWidgets{{
        NormalWidget(graphics.grid, "Widget 1", "IPA", "21.0"),
        NormalWidget(graphics.grid, "Widget 2", "Blond", "21.0"),
        NormalWidget(graphics.grid, "Widget 3", "Lager", "5.1"),
        NormalWidget(graphics.grid, "Widget 4", "Stout", "23.1"),
        NormalWidget(graphics.grid, "Widget 5", "Wit", "21.4"),
    }};

    while (true) {
        using namespace std::chrono_literals;
        lv_task_handler();
        lv_tick_inc(100);
        ioc.run_for(100ms);
        lv_obj_invalidate(graphics.grid);
    }
}