#ifndef server_h
#define server_h

#include "gateway.h++"
#include "netdevice.h++"
#include <boost/asio/io_service.hpp>
#include <utility>
#include <thread>
#include <algorithm>
#include <vector>

template<typename... Protocols>
class server {
public:
	template <typename... Gateway_Args>
	server(std::size_t const threads, Gateway_Args&&... gateway_args) :
		io_(),
		threads_(),
		netdevice_(io_),
		gateway_(io_, gateway_args...) {
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
