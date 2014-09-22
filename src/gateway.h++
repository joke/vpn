#ifndef gateway_h
#define gateway_h

#include <boost/asio/ip/udp.hpp>
#include "session.h++"
#include <unordered_map>
#include <vector>


template <typename Protocol, typename SendHandler, typename ReceiveHandler>
class gateway;

template <typename SendHandler, typename ReceiveHandler>
class gateway<boost::asio::ip::udp, SendHandler, ReceiveHandler> {
public:
	using protocol_type = boost::asio::ip::udp;
	using endpoint_type = typename protocol_type::endpoint;
	using session_builder_type = session_builder<protocol_type>;
	using session_type = typename session_builder_type::session_type;

	explicit gateway(boost::asio::io_service& io, session_builder_type const& sb, SendHandler&& s, ReceiveHandler&& r) : io_(io), session_builder_(sb), socket_(io_), send_{s}, receive_{r} {
		using namespace std;
		using namespace placeholders;
		//boost::asio::buffer(data, size), sender_endpoint, handler);

		socket_.open(protocol_type::v6());
		socket_.bind(protocol_type::endpoint(boost::asio::ip::address::from_string("::"), 54321));
		socket_.native_non_blocking(true);


		auto buffer(make_shared<vector<uint8_t>>(1500));
		auto endpoint(make_shared<endpoint_type>());

		socket_.async_receive_from(boost::asio::buffer(*buffer), *endpoint, std::bind(&gateway::receive, this, _1, _2, buffer, endpoint));
	}

	void receive(boost::system::error_code const& error, std::size_t const bytes_transfered, std::shared_ptr<std::vector<std::uint8_t>> buffer, std::shared_ptr<endpoint_type> endpoint) {
		using namespace std;

		auto match = endpoints_.find(*endpoint);
		if (match != std::end(endpoints_)) {
			std::cerr << "_______________________________________1 match " << std::endl;

			match->second.decrypt(boost::asio::buffer(*buffer, bytes_transfered));

		} else {
			std::cerr << "_______________________________________2 no match" << std::endl;
			auto position = endpoints_.emplace(*endpoint, session_builder_.construct(true));
			if (position.second)
				position.first->second.decrypt(boost::asio::buffer(*buffer, bytes_transfered));

		}

		{
			using namespace std::placeholders;

			auto buffer(make_shared<vector<uint8_t>>(1500));
			auto endpoint(make_shared<endpoint_type>());
			socket_.async_receive_from(boost::asio::buffer(*buffer), *endpoint, std::bind(&gateway::receive, this, _1, _2, buffer, endpoint));
		}
	}

	//void send(boost::asio::const_buffers_1 const buffer, endpoint_type const& endpoint) {}
	//void receive(boost::asio::const_buffers_1 const buffer, endpoint_type const& endpoint) {}

	using endpoints_type = std::map<endpoint_type, session_type>;

	endpoints_type endpoints_;

protected:
	boost::asio::io_service& io_;
	session_builder_type const& session_builder_;
	protocol_type::socket socket_;
	SendHandler send_;
	ReceiveHandler receive_;
};


#ifndef IN_IDE_PARSERER
#include "gateway.t++"
#endif

#endif
