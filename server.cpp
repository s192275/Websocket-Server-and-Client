#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <iostream>
#include <string>
#include <chrono>

namespace beast = boost::beast;          // from <boost/beast.hpp>
namespace http = beast::http;            // from <boost/beast/http.hpp>
namespace websocket = beast::websocket;  // from <boost/beast/websocket.hpp>
namespace net = boost::asio;             // from <boost/asio.hpp>
using tcp = net::ip::tcp;                // from <boost/asio/ip/tcp.hpp>

void do_session(tcp::socket socket) {
    try {
        // Construct the WebSocket stream from the socket
        websocket::stream<tcp::socket> ws{ std::move(socket) };

        // Accept the WebSocket handshake
        ws.accept();

        for (int i = 0; i< 100000; i++ ) {
            // This buffer will hold the incoming message
            beast::flat_buffer buffer;

            // Measure the time before sending the response
            auto start_time = std::chrono::steady_clock::now();

            // Read a message into our buffer
            ws.read(buffer);
            std::string message = beast::buffers_to_string(buffer.data());
            
            std::string message_to_send = std::to_string(i);
            ws.write(net::buffer(message_to_send));

            
            // Measure the time after sending the response
            auto end_time = std::chrono::steady_clock::now();
            std::chrono::duration<double> elapsed = end_time - start_time;

            // Print the elapsed time
            std::cout << "Mesaj : " << message << "Islenmesi icin gecen sure : " << elapsed.count() << " saniye" << std::endl;
        }
    }
    catch (beast::system_error const& se) {
        // This indicates that the session was closed
        if (se.code() != websocket::error::closed)
            std::cerr << "Error: " << se.code().message() << std::endl;
    }
    catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

int main() {
    try {
        std::cout << "Client baglantisi baslatiliyor..." << std::endl;
        // The io_context is required for all I/O
        net::io_context ioc;

        // These objects perform our I/O
        tcp::acceptor acceptor{ ioc, {tcp::v4(), 8080} };

        while (true) {
            // This will receive the new connection
            tcp::socket socket{ ioc };

            // Block until we get a connection
            acceptor.accept(socket);

            // Start a session on this connection
            do_session(std::move(socket));
        }
    }
    catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}

