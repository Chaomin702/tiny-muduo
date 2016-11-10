#pragma once
#include <memory>
#include <functional>
namespace cm 
{
	class TcpConnection;
	using TimerCallback = std::function<void()>;
	using TcpConnetionPtr = std::shared_ptr<TcpConnection>;
	using ConnectionCallback = std::function<void (const TcpConnetionPtr&)>;
	using MessageCallback = std::function<void(const TcpConnetionPtr&, const char*data, ssize_t len)>;
	using CloseCallback = std::function < void(const TcpConnetionPtr&)>;
}