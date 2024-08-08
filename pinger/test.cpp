#include "Pinger.hpp"
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>

int main()
{
    pinger::PingerConfig config;
    config.count = -1;
    config.destination_address = "google.com";

    auto callback = [](bool is_online) {
        std::cerr << (is_online ? "ONLINE" : "OFFLINE") << '\n';
    };

    pinger::Pinger pinger{config, callback};
    pinger.start();
    std::this_thread::sleep_for(std::chrono::seconds(5));
}
