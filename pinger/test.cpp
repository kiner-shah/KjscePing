#include "Pinger.hpp"
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <future>

int main()
{
    pinger::PingerConfig config;
    config.count = 4;
    config.destination_address = "google.com";

    auto callback = [](bool is_online) {
        std::cerr << (is_online ? "ONLINE" : "OFFLINE") << '\n';
    };

    pinger::Pinger pinger{config, callback};
    auto f = std::async(std::launch::async, [&pinger]() {
        pinger.start();
        });
    auto status = f.wait_for(std::chrono::seconds(30));
    if (status == std::future_status::timeout || status == std::future_status::ready)
    {
        pinger.stop();
    }
}
