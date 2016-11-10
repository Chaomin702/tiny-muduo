#pragma once
#include <memory>
#include <functional>
#include "timeStamp.h"
namespace cm 
{
	class TcpConnection;
	class Buffer;
	using TimerCallback = std::function<void()>;
	using TcpConnetionPtr = std::shared_ptr<TcpConnection>;
	using ConnectionCallback = std::function<void (const TcpConnetionPtr&)>;
	using MessageCallback = std::function<void(const TcpConnetionPtr&, Buffer *buf, TimeStamp)>;
	using CloseCallback = std::function < void(const TcpConnetionPtr&)>;
}