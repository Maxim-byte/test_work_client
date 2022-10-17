#pragma once

#include <vector>

#include <boost/asio.hpp>

struct server_data {
    std::string host;
    std::uint16_t port;
};

struct logger_config {
    std::string name_of_file_logger;
    std::string name_of_console_logger;
    std::vector<std::string> names_of_loggers;
};
