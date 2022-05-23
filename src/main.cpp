#include <client.h>
#include <iostream>
#include <thread>
#include <exception>

int main(int argc, char *argv[])
{
    try
    {
        if (argc < 3)
        {
            std::cerr << "Usage chat_client <host> <port>";
            return 1;
        }

        io::io_context io_context;
        tcp::resolver resolver(io_context);
        auto endpoint_iterator = resolver.resolve(argv[1], argv[2]);
        Client client(io_context);
        boost::system::error_code ec;
        //wait for connect
        if (client.connect(endpoint_iterator, ec))
        {
            std::string message;
            std::thread t([&io_context]
                        { 
                             io_context.run(); 
                            
                        });

            while (std::getline(std::cin, message))
            {
                if(!client.isClosed() && message != "/exit"){
                    std::cout << "\x1b[1A" // move cursor up one 
                            << "\x1b[2K"; // delete the entire line
                    std::cout << "[" << Client::getCurrentTime() << "] " << message << std::endl;

                    client.send_message(message);
                }
                else
                    break;
            }

            client.close();
            t.join();
            std::cout << "session is over\n";
        }
        else
            std::cout << "Unable to establish connection: " << ec.message() << std::endl;

       
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}