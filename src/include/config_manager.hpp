#pragma once

#include <filesystem>
#include <string_view>

#include <nlohmann/json.hpp>

#include "config_structs.hpp"

class config_manager {
public:

    static config_manager &instance() {
        static config_manager config_manager;
        return config_manager;
    }

    [[nodiscard]] const server_data &get_network_config() const;

    [[nodiscard]] const logger_config &get_logger_config() const;

    [[nodiscard]] std::uint8_t get_flush_period_time() const;

    [[nodiscard]] std::uint8_t get_time_to_reconnect() const;

private:
    config_manager();

private:
    server_data server_data_{};
    logger_config logger_config_{};

    std::uint8_t time_to_flush_logs_in_s_;
    std::uint8_t time_reconnect_in_s_;

    static constexpr std::string_view path_to_config_ = "./../../config/common.json";
};