#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/array.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>
#include <memory>
#include <semaphore>

#include "Socket.hpp"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>

using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
//using multi_buffer = basic_multi_buffer< std::allocator< char>>;

class BSocket: public Socket{
public:
	BSocket(); // Constructor
	~BSocket(); // Destructor
	void switch_to_ws() override;
	[[nodiscard]] std::pair<int, std::string> ws_request(const std::string& msg) override;
	void ws_request_async(const std::string& msg, std::function<void(int, const std::string&)> callback) override;
	void ws_response_async(int status, const std::string &resp) override;
	void connect_async();
	void on_resolve(const boost::system::error_code& ec, boost::asio::ip::basic_resolver<boost::asio::ip::tcp>::results_type results);
	void on_connect(const boost::system::error_code& ec); 
	void on_handshake(const boost::system::error_code& ec); 
	void connect_sync();
private:
	net::io_context ioc_async;
	ssl::context ctx_async;
	tcp::resolver resolver_async;
	websocket::stream<beast::ssl_stream<tcp::socket>>* m_ws;
	websocket::stream<beast::ssl_stream<tcp::socket>>* m_ws_async;
	std::thread m_io_thread;
	bool async_flag = false;
	bool q_empty = true;
	boost::asio::io_context::work work_guard;
};
