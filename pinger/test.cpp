#include "Pinger.hpp"
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <future>

int main()
{
    using namespace std::chrono_literals;

    pinger::PingerConfig config;
    config.count = 20;
    config.destination_address = "google.com";

    auto callback = [](bool is_online) {
        std::cerr << (is_online ? "ONLINE" : "OFFLINE") << '\n';
    };

    pinger::Pinger pinger{config, callback};
    pinger.start();
    std::this_thread::sleep_for(10s);
    pinger.stop();
}
