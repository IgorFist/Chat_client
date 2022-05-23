#include <client.h>
#include <iostream>
#include <iomanip>
#include <chrono>

Client::Client(io::io_context &io_context) : io_context(io_context), socket(io_context), connectionClosed(false)
{
}

bool Client::connect(const tcp::resolver::iterator &endpoint_iterator, boost::system::error_code& ec)
{
    io::connect(socket, endpoint_iterator, ec);

    if (!ec)
    {
        do_read_header();
        return true;
    }
    return false;
}

void Client::send_message(const std::string &message)
{
    io::post(io_context, [this, message]()
    {
        std::stringstream ss;
        ss << std::setw(header_length) <<std::hex << message.size() << message;
        bool write_in_progress = !writeMessages.empty();
        writeMessages.push(ss.str());
        if(!write_in_progress)
        {
            do_write();
        } 
    });
}

void Client::close()
{
    io_context.post([this]()
    { 
        boost::system::error_code ec;
        socket.shutdown(tcp::socket::shutdown_both, ec);
    });
}

bool Client::isClosed() const
{
    return connectionClosed;
}

std::_Put_time<char> Client::getCurrentTime()
{
    time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    return std::put_time(localtime(&now), "%F %T");
}

void Client::do_read_header()
{

    io::async_read(socket, io::buffer(header), [this](const boost::system::error_code &ec, std::size_t)
    {
        if(!ec)
        {
            do_read_body();
        }
        else
        {

            connectionClosed = true;
            socket.close();
        } 
    });
}

void Client::do_read_body()
{
    size_t incomingDataSize;
    std::stringstream ss(std::string(header, header_length));
    ss >> std::hex >> incomingDataSize;
    incomingData.resize(incomingDataSize);

    io::async_read(socket, io::buffer(incomingData), [this](const boost::system::error_code &ec, std::size_t byteRetrieved)
    {
        if(!ec )
        {
            std::string messageRetrieved(incomingData.begin(), incomingData.end());    
            //\x1b[32m[ - green color, \x1b[39;49m - reset color      
            std::cout << "\x1b[32m[" << getCurrentTime() <<"] " << messageRetrieved << "\x1b[39;49m" << std::endl;
            do_read_header();
        }
        else
        {
            connectionClosed = true;
            socket.close();
        } 
    });
}

void Client::do_write()
{
    io::async_write(socket, io::buffer(writeMessages.front()), [this](const boost::system::error_code &ec, size_t)
    {
        if(!ec)
        {
            writeMessages.pop();
            if(!writeMessages.empty())
            {
                do_write();
            }
        }
        else
        {
            connectionClosed = true;
            socket.close();
        } 
    });
}

void Client::closeSocket()
{
    connectionClosed = true;
    socket.close();
}