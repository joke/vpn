#ifndef server_h
#define server_h

#include "gateway.h++"
#include "netdevice.h++"
#include "convert.h++"
#include <boost/asio/io_service.hpp>
#include <utility>
#include <thread>
#include <algorithm>
#include <vector>

template<typename... Protocols>
class server {
public:
	template <typename... Gateway_Args>
	server(std::size_t const threads, std::string name, gnutls::credentials const& cre, Gateway_Args&&... gateway_args) :
		io_(),
		threads_(),
		netdevice_(io_, name, hex_to_address(cre.fingerprint()).to_v6(), std::bind(&server::to_gateway, this, std::placeholders::_1, std::placeholders::_2)),
		gateway_(io_, cre, gateway_args...) {
			threads_.reserve(threads);
			netdevice_.startup();
			gateway_.startup();
	}

	void run() {
		if (threads_.capacity() > 0) {
			for (std::size_t begin(0), end(threads_.capacity()); begin != end; ++end)
				threads_.emplace_back(std::bind(static_cast<std::size_t(boost::asio::io_service::*)()>(&boost::asio::io_service::run), std::ref(io_)));
			for (auto& thread : threads_)
				thread.join();
		} else
			io_.run();
	}

	bool to_gateway(std::uint64_t const prefix, std::shared_ptr<std::vector<std::uint8_t>> const data) {
		return gateway_.send(prefix, data);
	}

	void to_netdevice(boost::asio::const_buffers_1 buf) {
	}

	template <typename T>
	void connect(T const& t) {
		gateway_.connect(t);
	}

private:
	boost::asio::io_service io_;
	std::vector<std::thread> threads_;
	netdevice<server> netdevice_;
	gateway<Protocols...> gateway_;
};

#ifndef IN_IDE_PARSERER
#include "server.t++"
#endif

#endif
