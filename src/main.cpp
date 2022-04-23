#include <client.h>
#include <iostream>
#include <thread>

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

        std::string message;
        //wait for connect
        if (client.connect(endpoint_iterator))
        {

            std::thread t([&io_context]()
                          { io_context.run(); });

            // std::time_t time = std::time(nullptr);
            // std::cout << "[" << std::ctime(&time) << "] ";
            while (std::getline(std::cin, message))
            {
                std::cout << "\x1b[1A" // move cursor up one 
                          << "\x1b[2K"; // delete the entire line
                std::cout << "[" << Client::getCurrentTime() << "] " << message << std::endl;

                if (message == "/exit")
                    break;
                client.send_message(message);
            }

            client.close();
            t.join();
            std::cout << "session is over\n";
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}