#pragma once
#include <utility>
#include <memory>
#include <string>
#include <vector>
#include <functional>

class Socket{
public:
	virtual ~Socket();
	virtual void switch_to_ws() = 0;
	[[nodiscard]] virtual std::pair<int, std::string> ws_request(const std::string& msg) = 0;
	virtual void ws_request_async(const std::string& msg, std::function<void(int, const std::string&)> callback) = 0;
	virtual void ws_response_async(int status, const std::string& resp) = 0;
protected:
	const std::string host = "test.deribit.com";
	const std::string port = "443";
	std::vector<std::pair<int, std::string>> m_response_buffer;
};
