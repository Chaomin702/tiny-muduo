#pragma once
#include <memory>
#include <map>
#include <set>
#include <unordered_set>
#include "callback.h"
#include "circularBuffer.h"
#include "tcpConnection.h"
#include "eventLoop.h"
#include "dbg.h"
namespace cm {
	typedef std::weak_ptr<TcpConnection> WeakTcpConnectionPtr;
	struct Entry {
		Entry(const WeakTcpConnectionPtr& weakconn)
			: weakConn_(weakconn) {}
		~Entry() {
			TcpConnetionPtr conn = weakConn_.lock();
			if (conn) {
				conn->getloop()->runInLoop(std::bind(&TcpConnection::activeClose, conn));
				log_info("%s timeout, force close!", conn->name().c_str());
			 }
		}
		WeakTcpConnectionPtr weakConn_;
	};
	
	typedef std::shared_ptr<Entry> EntryPtr;
	typedef std::weak_ptr<Entry> WeakEntryPtr;
	typedef std::unordered_set<EntryPtr> Bucket;
	typedef cm::circular_buffer<Bucket> WeakConnectionList;
}