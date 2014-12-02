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
};

template<typename Protocol, typename... Protocols>
class gateway<Protocol, Protocols...> : public gateway<Protocols...> {
public:
	explicit gateway(boost::asio::io_service& io, gnutls::credentials const& c, std::vector<typename Protocol::endpoint> const& e, std::vector<typename Protocols::endpoint> const&... args) :
	gateway<Protocols...>(io, c, args...) {
		using namespace std;

		if (!cfg::configuration["connect"].empty()) {

			typedef typename Protocol::socket socket_type;
			socket_type socket(this->io_);

			std::cerr << "_________________:::" << cfg::configuration["connect"].as<vector<dccp::endpoint>>().front() << endl;
// 			socket.open(dccp::v4());
// 			socket.native_non_blocking(true);
			socket.connect(cfg::configuration["connect"].as<vector<dccp::endpoint>>().front());
			int a = 0;

			auto it = sessions_.emplace(end(sessions_), move(socket), this->credentials_, false);
			it->unregister([this, it](){ sessions_.erase(it); });
			it->start();
		} else {

			for_each(begin(e), end(e), [&](auto const& e) {
				acceptors_.emplace_front(io, e);
				auto& acceptor(acceptors_.front());
				acceptor.native_non_blocking(true);
				auto socket(make_shared<typename Protocol::socket>(io));
				acceptor.async_accept(*socket, std::bind(&gateway::session, this, ref(acceptor), socket, placeholders::_1));
			});

		}
	}

	void session(typename Protocol::acceptor& acceptor,std::shared_ptr<typename Protocol::socket> socket, boost::system::error_code const& error) {
		using namespace std;
		typedef typename Protocol::socket socket_type;

		std::cerr << "______________________________ QUEUE size: " << sessions_.size() << std::endl;

		socket->native_non_blocking(true);
		auto it = sessions_.emplace(end(sessions_), move(*socket), this->credentials_, true);
		it->unregister([this, it](){ sessions_.erase(it); });
		it->start();

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
