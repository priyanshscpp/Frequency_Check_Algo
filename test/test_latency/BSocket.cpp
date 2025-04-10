#include "BSocket.hpp"

// Destructor
BSocket::~BSocket(){
	// Close the WebSocket connection
	try {
    		if(m_ws) m_ws -> close(boost::beast::websocket::close_code::normal);
		if(m_ws_async){ 
			m_ws_async->async_close(boost::beast::websocket::close_code::normal, [](const boost::system::error_code& ec) {
            			if (ec) {
                			std::cout << "Error during async_close: " << ec.message() << std::endl;
            			} else {
                			std::cout << "WebSocket closed successfully." << std::endl;
            			}
        		});
		}
		std::cout << "Connection Closed\n";
	} catch (const std::exception& e) {
    		std::cout << "Error during WebSocket close: " << e.what() << '\n';
	}
}

// Constructor
BSocket::BSocket(): ioc_async{}, ctx_async{ssl::context::tlsv12_client}, resolver_async{ioc_async}, m_ws{nullptr}, m_ws_async{nullptr}, work_guard(ioc_async){
	// Establish a sync connection
	connect_sync();
	std::cout << "Sync connection established\n";
	
	// Establish a async conncetion
	connect_async();
	std::cout << "Async connection established\n";
}

// Synchronous connection method
void BSocket::connect_sync(){
	net::io_context ioc;
	
	// The SSL context is required, and holds certificates
	ssl::context ctx{ssl::context::tlsv12_client};

	// These objects perform our I/O
        tcp::resolver resolver{ioc};

	m_ws = new websocket::stream<beast::ssl_stream<tcp::socket>>(ioc, ctx); 

        // Look up the domain name
        auto const results = resolver.resolve(host, port);
	
        // Make the connection on the IP address we get from a lookup
	net::connect(m_ws -> next_layer().next_layer(), results.begin(), results.end());

	m_ws -> next_layer().handshake(ssl::stream_base::client);
}

// Asynchronous connection method
void BSocket::connect_async() {
	m_io_thread = std::thread([this](){
		ioc_async.run();
		std::cout << "Exting no work to do\n";
	});
	// Start asynchronous resolve
	resolver_async.async_resolve(host, port,
            [this](const boost::system::error_code& ec, tcp::resolver::results_type results) {
                on_resolve(ec, results);
        });
}

// This function will be called once the resolver has finished resolving the domain
void BSocket::on_resolve(const boost::system::error_code& ec, boost::asio::ip::basic_resolver<boost::asio::ip::tcp>::results_type results) {
        if (ec) {
            std::cout << "Resolve failed: " << ec.message() << std::endl;
            return;
        }

        // Create the WebSocket stream with SSL
        m_ws_async = new websocket::stream<beast::ssl_stream<tcp::socket>>(ioc_async, ctx_async);

        // Asynchronously connect to the resolved address
        net::async_connect(m_ws_async -> next_layer().next_layer(), results.begin(), results.end(),
            [this](const boost::system::error_code& ec, boost::asio::ip::basic_resolver_iterator<boost::asio::ip::tcp> iterator) {
	    	const auto& endpoint = *iterator;
		std::cout << "Connected to " << endpoint.endpoint().address() << ":" << endpoint.endpoint().port() << std::endl;
               	on_connect(ec);
	});
}

// This function will be called once the connection has been made
void BSocket::on_connect(const boost::system::error_code& ec) {
        if (ec) {
            std::cout << "Connect failed: " << ec.message() << std::endl;
            return;
        }
	std::cout << "Performing SSL handshake\n";
        // Perform SSL handshake asynchronously
        m_ws_async -> next_layer().async_handshake(ssl::stream_base::client, [this](const boost::system::error_code& ec) {
                on_handshake(ec);
        });
}

// This function will be called once the handshake has been completed
void BSocket::on_handshake(const boost::system::error_code& ec) {
        if (ec) {
            std::cout << "Handshake failed: " << ec.message() << std::endl;
            return;
        }
	async_flag = true;
        std::cout << "BSocket Initialized and connected to " << host << " on port " << port << "!\n";	
}

// Sync req
[[nodiscard]] std::pair<int, std::string> BSocket::ws_request(const std::string& message){
	// Send the message
        m_ws_async -> write(net::buffer(message));
	
	// This buffer will hold the incoming message
        beast::flat_buffer buffer;

	// Read a message into our buffer
	m_ws_async -> read(buffer);
	std::string resp = beast::buffers_to_string(buffer.data());

	return make_pair(0, resp);
}

// Async req
void BSocket::ws_request_async(const std::string& message, std::function<void(int, const std::string&)> callback) { 
	// Send the message
	std::shared_ptr<std::string> _msg = std::make_shared<std::string>(message);

	auto start_time = std::chrono::high_resolution_clock::now();
	while(!q_empty);
	q_empty = false;
	m_ws_async -> async_write(boost::asio::buffer(*_msg), [this, _msg, callback, start_time](const boost::system::error_code &ec, std::size_t bytes_transferred){
		q_empty = true; // callback called write was successfull
		if (ec) {
			std::cout << "Error in async_write: " << ec.message() << std::endl;
			callback(1, ""); // Indicate failure
		} else {
			auto end_time = std::chrono::high_resolution_clock::now();				
    			auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
			std::cout << "Message written successfully and " << bytes_transferred << " bytes transferred in " << duration.count() << " us\n";
			callback(0, "Success\n");
		}
	});
}

// Async resp
void BSocket::ws_response_async(int status, const std::string &resp) { 
	m_response_buffer.push_back(make_pair(status, resp));
	std::cout << resp << '\n';	
	return;
}

void BSocket::switch_to_ws(){
	while(!async_flag); // Wait to initialize
	m_ws_async -> handshake(host, "/ws/api/v2");
	// Perform the websocket handshake
	if(m_ws){ 
		m_ws -> handshake(host, "/ws/api/v2");	
	} else { 
		std::cout << "Sync web socket not initialized\n"; 
		return;
	}
	std::cout << "Moved to WebSocket Connection\n";
}
