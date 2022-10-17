#pragma once

#include <vector>
#include <memory>

#include <spdlog/spdlog.h>

#include "config_structs.hpp"

class logger_wrapper {
public:
    explicit logger_wrapper(std::shared_ptr<spdlog::logger> &&logger);

    ~logger_wrapper();

    [[nodiscard]] const std::shared_ptr<spdlog::logger> &logger() const;

    static std::shared_ptr<spdlog::logger> get_logger_by_name(const std::string &name_of_logger);

    static void log_message_in_multiple_logger(const std::vector<std::string> &names_of_loggers, std::string &&message, spdlog::level::level_enum &&level);

private:
    std::shared_ptr<spdlog::logger> logger_;
};
