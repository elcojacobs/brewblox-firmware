
#include "RecurringTask.hpp"
#include "graphics/graphics.hpp"
#include "graphics/widgets.hpp"
#include "websocketserver.hpp"

#include <boost/asio.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

#include <thread>
std::shared_ptr<listener> webSocketServer;
net::io_context ioc{1};

int main()
{
    namespace beast = boost::beast;         // from <boost/beast.hpp>
    namespace http = beast::http;           // from <boost/beast/http.hpp>
    namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
    namespace net = boost::asio;            // from <boost/asio.hpp>
    using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

    webSocketServer = std::make_shared<listener>(ioc, tcp::endpoint{net::ip::make_address("0.0.0.0"), 7377});
    webSocketServer->run();

    auto graphics = Graphics::getInstance();

    static auto timeSetter = RecurringTask(ioc, boost::asio::chrono::milliseconds(1000),
                                           RecurringTask::IntervalType::FROM_EXPIRY,
                                           [&graphics]() {
                                               auto tickMinutes = boost::asio::chrono::system_clock::now().time_since_epoch() / asio::chrono::minutes(1);
                                               auto minutes = tickMinutes % (60);

                                               auto tickHours = boost::asio::chrono::system_clock::now().time_since_epoch() / asio::chrono::hours(1);
                                               auto hours = tickHours % (24) + 2;
                                               graphics.bar.setTime(hours, minutes);
                                           });
    timeSetter.start();

    static auto graphicsLooper = RecurringTask(ioc, boost::asio::chrono::milliseconds(10),
                                               RecurringTask::IntervalType::FROM_EXPIRY,
                                               []() {
                                                   lv_task_handler();
                                               });
    graphicsLooper.start();

    static auto displayTick = RecurringTask(ioc, boost::asio::chrono::milliseconds(10),
                                            RecurringTask::IntervalType::FROM_EXPIRY,
                                            []() {
                                                lv_tick_inc(10);
                                            });
    displayTick.start();

    static std::array<NormalWidget, 5> sensorWidgets{{
        NormalWidget(graphics.grid, "Widget 1", "IPA", "21.0"),
        NormalWidget(graphics.grid, "Widget 2", "Blond", "21.0"),
        NormalWidget(graphics.grid, "Widget 3", "Lager", "5.1"),
        NormalWidget(graphics.grid, "Widget 4", "Stout", "23.1"),
        NormalWidget(graphics.grid, "Widget 5", "Wit", "21.4"),
    }};
    static auto widget6 = PidWidget(graphics.grid);
    widget6.setBar1(25);
    widget6.setBar2(-80);

    ioc.run();
}