#ifndef server_h
#define server_h

#include "signal.h++"
#include "netdevice.h++"
#include "gateway.h++"

#include <forward_list>
#include <thread>


class ThreadPool {
public:
	explicit ThreadPool(std::size_t threads = 10) : thread_cnt_{threads}, threads_{} {
	}

	template <typename Function>
	void run(Function&& f) {
		for (std::size_t i{0}; i != thread_cnt_; ++i)
			threads_.emplace_front(f);

		for (auto& thread : threads_)
			thread.join();
	}

protected:
	std::size_t thread_cnt_;
	std::forward_list<std::thread> threads_;
};

class SingleThreaded {
public:
	template <typename Function>
	void run(Function&& f) {
		f();
	}
};

template <typename GatewayProtocol, typename ControllerProtocol, typename ThreadingPolicy>
class server : protected ThreadingPolicy {
public:
	explicit server() = delete;
	explicit server(server const&) = delete;
	explicit server(server&&) = delete;
	explicit server(gnutls::credentials&&, gnutls::priorities&&);
	~server();

	server& operator=(server const&) = delete;

protected:
	void send_local(boost::asio::const_buffers_1 buffer) { std::cerr << "send_local" << std::endl; }
	void send_remote(boost::asio::const_buffers_1 buffer) {
		std::cerr << "send_remote: " << std::this_thread::get_id() << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(3));
	}
	void received_local(boost::asio::const_buffers_1 buffer) { std::cerr << "received_local" << std::endl; }
	void received_remote(boost::asio::const_buffers_1 buffer) { std::cerr << "received_remote" << std::endl; }

	using send_local_type = decltype(std::bind(&server::send_local, std::declval<server*>(), std::placeholders::_1));
	using send_remote_type = decltype(std::bind(&server::send_remote, std::declval<server*>(), std::placeholders::_1));
	using received_local_type = decltype(std::bind(&server::received_local, std::declval<server*>(), std::placeholders::_1));
	using received_remote_type = decltype(std::bind(&server::received_remote, std::declval<server*>(), std::placeholders::_1));

	using netdevice_type = netdevice<send_remote_type, received_local_type>;
	using gateway_type = gateway<GatewayProtocol, send_local_type, received_remote_type>;

public:
	using session_builder_type = typename gateway_type::session_builder_type;

protected:
	gnutls::credentials credentials_;
	gnutls::priorities priorities_;

	boost::asio::io_service io_; //!< the io service

	class signal signal_; //!< handles all kind of signals
	netdevice_type netdevice_; //!< handles packets from virtual network device
	gateway_type gateway_;
	session_builder_type session_builder_;
};


#ifndef IN_IDE_PARSERER
#include "server.t++"
#endif

#endif
