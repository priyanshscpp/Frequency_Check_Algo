#include <iostream>
#include <string>
#include "Api.hpp"
#include <utility>
#include <nlohmann/json.hpp>
#include <chrono>

void place_order(int &orders, Api& api){
	std::string inst = "ETH-PERPETUAL";
	int price = 1000, quant = 10;
	std::ostringstream order_query;
	order_query << R"({
        	"jsonrpc": "2.0",
        	"method": "private/buy",
        	"params": {
            		"instrument_name": ")" << inst << R"(",
            		"type": "limit",
            		"price": )" << price << R"(,
            		"amount": )" << quant << R"(
        	},
        	"id": "1"
    	})";
	std::pair<int, std::string> reply = api.api_private(order_query.str());
	if(reply.first){
		std::cout << "Error occured\n";
		return;
	}
	std::cout << "Order [" << orders++ << "] Placed\n";
	return;
}

void place_order_async(int& orders, Api& api){
	std::string inst = "ETH-PERPETUAL";
	int price = 1000, quant = 10;
	std::ostringstream order_query;
	order_query << R"({
        	"jsonrpc": "2.0",
        	"method": "private/buy",
        	"params": {
            		"instrument_name": ")" << inst << R"(",
            		"type": "limit",
            		"price": )" << price << R"(,
            		"amount": )" << quant << R"(
        	},
        	"id": "1"
    	})";
	std::string temp = order_query.str();
	api.api_private_async(temp);
	std::cout << "Order [" << orders++ << "] Placed\n";
	return;
}

void clear_orders(Api& api){
	std::string req = R"({
  		"method": "private/enable_cancel_on_disconnect",
  		"params": {},
  		"jsonrpc": "2.0",
  		"id": 1
	})";

	std::pair<int, std::string> reply = api.api_private(req);
	if(reply.first){
		std::cout << "Error occured\n";
		return;
	}
	std::cout << "> Cancel on Disconnect Enabled\n";
	return;
}

int main(){
	const std::string summary = R"({
  		"method": "private/get_account_summaries",
  		"params": {},
  		"jsonrpc": "2.0",
  		"id": 0
	})";

	Api api = Api();
	std::pair<int, std::string> reply = api.api_private(summary);
	if(reply.first){
		std::cout << "Error occured\n";
	}
	
	nlohmann::json obj = nlohmann::json::parse(reply.second);
	//std::cout << obj.dump(4) << '\n';
	std::cout << "> Starting test...\n";
	std::cout << "> Setting up cancel on disconnect\n";
	clear_orders(api);
	

	int orders = 0, _duration = 0, cnt = 20;
	while(orders < cnt){ 
		// Start the clock
    		auto start = std::chrono::high_resolution_clock::now();
		
		place_order(orders, api); 
   		
		// Stop the clock
    		auto end = std::chrono::high_resolution_clock::now();

		// Calculate duration
    		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    		// Output result
    		std::cout << "Execution time: " << duration.count() << " us\n";
		
		_duration += duration.count();
	}
	// Avg latency
	std::cout << "Avg Latency: " << (1.0 * _duration) / cnt << "us\n";
	std::cout << "Enter to exit...\n";
	std::string inp; std::cin >> inp;
	return 0;
}
