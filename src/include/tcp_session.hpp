#pragma once

#include <string_view>

#include <boost/asio.hpp>
#include <boost/beast.hpp>

class tcp_session {
public:
    tcp_session(const std::string & host, std::uint16_t port, boost::asio::io_context &io) :
            host_(host),
            port_(std::to_string(port)),
            io_(io),
            stream_(io_) {
        stream_.expires_never();
    }

    ~tcp_session() {
        close_connection();
    }

    std::error_code try_connect() {
        boost::beast::error_code ec;
        boost::asio::ip::tcp::resolver resolver(io_);
        stream_.connect(resolver.resolve(host_, port_), ec);
        return ec;
    }

    template<class T>
    boost::beast::http::response<boost::beast::http::dynamic_body> send_and_get_response(boost::beast::http::request<T> &request) {
        if(!stream_.socket().is_open()) {
            try_connect();
        }
        boost::beast::http::write(stream_, request);
        static boost::beast::flat_buffer buffer;
        boost::beast::http::response<boost::beast::http::dynamic_body> response;
        boost::beast::http::read(stream_, buffer, response);
        return response;
    }

    void close_connection() {
        boost::beast::error_code ec;
        if(stream_.socket().is_open()) {
            stream_.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
        }
    }

private:
    std::string host_;
    std::string port_;
    boost::asio::io_context &io_;
    boost::beast::tcp_stream stream_;
};

