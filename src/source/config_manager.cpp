#include "../include/config_manager.hpp"

#include <fstream>
#include <iostream>
#include <exception>

#include <boost/asio.hpp>

config_manager::config_manager() {
    std::ifstream config_stream(path_to_config_.data());
    assert(config_stream.is_open());

    try {
        nlohmann::json config = nlohmann::json::parse(config_stream);

        time_to_flush_logs_in_s_ = config["time_to_flush_logs_s"].get<std::uint8_t>();
        assert(time_to_flush_logs_in_s_ != 0);

        //may be 0, if don't try to reconnect
        time_reconnect_in_s_ = config["times_to_reconnect_s"].get<std::uint8_t>();

        server_data_.port = config["server_data"]["port"].get<std::uint16_t>();
        server_data_.host = config["server_data"]["host"].get<std::string>();

        boost::system::error_code ec;
        boost::asio::ip::make_address_v4(server_data_.host, ec);
        assert(!ec && "Host is inconvertible to ipV4!");

        logger_config_.name_of_file_logger = config["logging"]["file_logger"].get<std::string>();
        logger_config_.name_of_console_logger = config["logging"]["console_logger"].get<std::string>();

        assert(logger_config_.name_of_console_logger != logger_config_.name_of_file_logger);
        logger_config_.names_of_loggers.push_back(logger_config_.name_of_console_logger);
        logger_config_.names_of_loggers.push_back(logger_config_.name_of_file_logger);

    } catch (const std::exception & ex) {
        std::cerr << "Can't init config_manager: " << ex.what() << std::endl;
        std::exit(1);
    }
}

const server_data &config_manager::get_network_config() const {
    return server_data_;
}

const logger_config &config_manager::get_logger_config() const {
    return logger_config_;
}

std::uint8_t config_manager::get_flush_period_time() const {
    return time_to_flush_logs_in_s_;
}

std::uint8_t config_manager::get_time_to_reconnect() const {
    return time_reconnect_in_s_;
}
