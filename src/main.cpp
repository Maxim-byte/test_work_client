#include <random>
#include <thread>
#include <iostream>

#include <boost/asio.hpp>
#include <boost/beast.hpp>

#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "include/logger_wrapper.hpp"
#include "include/config_manager.hpp"
#include "include/tcp_session.hpp"

namespace {
    constexpr std::uint16_t sleep_time_reconnect_ms = 500;
}

int main() {

    //read config
    const auto &config_manager = config_manager::instance();

    //setup logger
    auto file_logger = logger_wrapper(spdlog::basic_logger_mt<spdlog::async_factory>(
            config_manager.get_logger_config().name_of_file_logger, "./../logs/common.log"));

    auto console_logger = logger_wrapper(spdlog::stdout_color_mt(config_manager.get_logger_config().name_of_console_logger));

    spdlog::flush_every(std::chrono::seconds(config_manager.get_flush_period_time()));

    //init random engine
    std::random_device r;
    std::default_random_engine e1(r());
    std::uniform_int_distribution<std::uint16_t> uniform_dist(0, 1023);

    boost::beast::http::request<boost::beast::http::string_body> request{boost::beast::http::verb::get, "/v1/square", 11};
    request.set(boost::beast::http::field::host, config_manager.get_network_config().host);
    request.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    request.keep_alive(true);

    boost::asio::io_context io;
    boost::asio::signal_set signals(io, SIGTERM);
    bool need_to_stop_application = false;
    signals.async_wait([&](const boost::system::error_code &error, int signal_number) {
        console_logger.logger()->info("Closing application...");
        io.stop();
        need_to_stop_application = true;
    });
    std::thread io_thread([&]() {
        io.run();
    });

    tcp_session session(config_manager.get_network_config().host, config_manager.get_network_config().port, io);
    auto ec = session.try_connect();
    std::uint8_t time_already_try_to_reconnect = 0;
    std::cout << (ec.operator bool()) << std::endl;

    auto try_reconnect_if_error = [&](std::error_code &ec) mutable {
        while (ec) {
            if (time_already_try_to_reconnect >= 2 * config_manager.get_time_to_reconnect()) {
                logger_wrapper::log_message_in_multiple_logger(config_manager.get_logger_config().names_of_loggers,
                                                               std::string("Can't reconnect... : ") + ec.message(),
                                                               spdlog::level::critical);
                return false;
            }
            logger_wrapper::log_message_in_multiple_logger(config_manager.get_logger_config().names_of_loggers,
                                                           std::string("Reconnecting... : ") + ec.message(), spdlog::level::warn);
            ec = session.try_connect();
            ++time_already_try_to_reconnect;
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time_reconnect_ms));
        }
        return true;
    };

    if (!try_reconnect_if_error(ec)) {
        return -1;
    }

    std::stringstream ss;
    while (!need_to_stop_application) {
        ss.str("");
        request.body() = std::to_string(uniform_dist(e1));
        request.prepare_payload();
        try {
            ss << session.send_and_get_response(request);
            logger_wrapper::log_message_in_multiple_logger(config_manager.get_logger_config().names_of_loggers,
                                                           std::string("Succsesfully send and receive data: ") + ss.str(),
                                                           spdlog::level::info);
        } catch (const std::exception &ex) {
            logger_wrapper::log_message_in_multiple_logger(config_manager.get_logger_config().names_of_loggers,
                                                           std::string("Can't send request: ") + ex.what(), spdlog::level::warn);
            ec = session.try_connect();
            if (!try_reconnect_if_error(ec)) {
                session.close_connection();
                return -1;
            }
        }
    }
    io_thread.join();
    return 0;
}