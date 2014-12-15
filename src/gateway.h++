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
	explicit gateway(boost::asio::io_service& io, gnutls::credentials const& c) : io_(io), credentials_(c) {
	}

protected:
	boost::asio::io_service& io_;
	gnutls::credentials const& credentials_;

protected:
	void connect();
};

template<typename Protocol, typename... Protocols>
class gateway<Protocol, Protocols...> : public gateway<Protocols...> {
public:
	explicit gateway(boost::asio::io_service& io, gnutls::credentials const& c, std::vector<typename Protocol::endpoint> const& e, std::vector<typename Protocols::endpoint> const&... args) :
	gateway<Protocols...>(io, c, args...) {
		using namespace std;

		for_each(begin(e), end(e), [&](auto const& e) {
			acceptors_.emplace_front(io, e);
			auto& acceptor(acceptors_.front());
			acceptor.native_non_blocking(true);
			auto socket(make_shared<typename Protocol::socket>(io));
			acceptor.async_accept(*socket, std::bind(&gateway::session, this, ref(acceptor), socket, placeholders::_1));
		});
	}

	void connect(std::vector<typename Protocol::endpoint> const& endpoints) {
		typedef typename Protocol::socket socket_type;
		socket_type socket(this->io_);

		// 			socket.open(dccp::v4());

		socket.connect(endpoints.front());
		socket.native_non_blocking(true);

		auto it = sessions_.emplace(end(sessions_), std::move(socket), this->credentials_, false);
		it->unregister([this, it](){ sessions_.erase(it); });
		it->start();
	}

	using  gateway<Protocols...>::connect;

protected:
	void session(typename Protocol::acceptor& acceptor,std::shared_ptr<typename Protocol::socket> socket, boost::system::error_code const& error) {
		using namespace std;
		typedef typename Protocol::socket socket_type;

		{
			auto socket(make_shared<typename Protocol::socket>(this->io_));
			acceptor.async_accept(*socket, std::bind(&gateway::session, this, ref(acceptor), socket, placeholders::_1));
		}

		std::cerr << "______________________________ QUEUE size: " << sessions_.size() << std::endl;

		socket->native_non_blocking(true);
		auto it = sessions_.emplace(end(sessions_), move(*socket), this->credentials_, true);
		it->unregister([this, it](){ sessions_.erase(it); });
		it->start();
	}

protected:
	std::forward_list<typename Protocol::acceptor> acceptors_;
	std::list<class session<Protocol>> sessions_;
};

#endif
