#ifndef gateway_h
#define gateway_h

#include <iterator>
#include <vector>
#include <list>
#include <forward_list>
#include <algorithm>
#include <type_traits>
#include <boost/asio/ip/tcp.hpp>
#include "dccp.h++"
#include "session.h++"

template<typename...>
class gateway;

template<>
class gateway<> {
public:
	explicit gateway(boost::asio::io_service& io) : io_(io) {
	}

protected:
	boost::asio::io_service& io_;
};

template<typename Protocol, typename... Protocols>
class gateway<Protocol, Protocols...> : public gateway<Protocols...> {
public:
	explicit gateway(boost::asio::io_service& io, std::vector<typename Protocol::endpoint> const& e, std::vector<typename Protocols::endpoint> const&... args) :
	gateway<Protocols...>(io, args...) {
		using namespace std;

		for_each(begin(e), end(e), [&](auto const& e) {
			acceptors_.emplace_front(io, e);
			auto& acceptor(acceptors_.front());
			acceptor.native_non_blocking(true);
			auto socket(make_shared<typename Protocol::socket>(io));
			acceptor.async_accept(*socket, std::bind(&gateway::session, this, ref(acceptor), socket, placeholders::_1));
		});
	}

	void session(typename Protocol::acceptor& acceptor,std::shared_ptr<typename Protocol::socket> socket, boost::system::error_code const& error) {
		using namespace std;
		typedef typename Protocol::socket socket_type;

		socket->native_non_blocking(true);
		auto it = sessions_.emplace(end(sessions_), move(*socket));
		it->unregister([this, it](){ sessions_.erase(it); });

		{
			auto socket(make_shared<typename Protocol::socket>(this->io_));
			acceptor.async_accept(*socket, std::bind(&gateway::session, this, ref(acceptor), socket, placeholders::_1));
		}
	}

protected:
	std::forward_list<typename Protocol::acceptor> acceptors_;
	std::list<class session<Protocol>> sessions_;
};

#endif
