#include "../include/logger_wrapper.hpp"

#include <ranges>
#include <iostream>
#include <algorithm>

#include <spdlog/sinks/basic_file_sink.h>

logger_wrapper::logger_wrapper(std::shared_ptr<spdlog::logger> &&logger) {
    try {
        logger_ = std::move(logger);
        assert(logger_ != nullptr);

        logger_->set_error_handler([](const std::string &error) {
            std::cerr << error << std::endl;
        });
    }
    catch (const spdlog::spdlog_ex &ex) {
        std::cerr << "Logger initializing error!" << std::endl;
    }
}

logger_wrapper::~logger_wrapper() {
    logger_->flush();
    spdlog::drop(logger_->name());
}

const std::shared_ptr<spdlog::logger> &logger_wrapper::logger() const {
    return logger_;
}

std::shared_ptr<spdlog::logger> logger_wrapper::get_logger_by_name(const std::string &name_of_logger) {
    auto logger = spdlog::get(name_of_logger);
    if (!logger) {
        throw std::runtime_error("Invalid name of logger!");
    }
    return logger;
}

void logger_wrapper::log_message_in_multiple_logger(const std::vector<std::string> &names_of_loggers,
                                                    std::string &&message, spdlog::level::level_enum &&level) {
    std::ranges::for_each(names_of_loggers, [&](const std::string &logger_name) {
        auto logger = logger_wrapper::get_logger_by_name(logger_name);
        logger->log(level, message);
    });
}
