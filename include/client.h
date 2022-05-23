#pragma once

#include <queue>
#include <vector>
#include <boost/asio.hpp>
#include <iomanip>

using namespace boost::asio::ip;
namespace io = boost::asio;

class Client
{
public:
    Client(io::io_context &io_context);
    bool connect(const tcp::resolver::iterator &endpoint_iterator, boost::system::error_code &ec);
    void send_message(const std::string &message);
    void close();
    bool isClosed() const;
    static std::_Put_time<char> getCurrentTime();

private:
    void do_read_header();
    void do_read_body();
    void do_write();
    void closeSocket();

    io::io_context &io_context;
    tcp::socket socket;
    std::queue<std::string> writeMessages;

    enum{header_length = 8};
    char header[header_length];
    std::vector<char> incomingData;

    bool connectionClosed;
};
