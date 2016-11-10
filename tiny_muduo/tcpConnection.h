#pragma once
#include "nonCopyable.h"
#include "memory"
namespace cm 
{
	class TcpConnetion :public NonCopyable, 
						public std::enable_shared_from_this<TcpConnetion> {
		
	};
}