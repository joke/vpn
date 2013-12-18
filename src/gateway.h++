#ifndef gateway_h
#define gateway_h

#include <boost/asio/ip/udp.hpp>
#include "session.h++"
#include <unordered_map>
#include <vector>

namespace {

template <typename Protocol, typename SendHandler, typename ReceiveHandler>
class gateway;

template <typename SendHandler, typename ReceiveHandler>
class gateway<boost::asio::ip::udp, SendHandler, ReceiveHandler> {
public:
	using protocol_type = boost::asio::ip::udp;
	using endpoint_type = typename protocol_type::endpoint;
	using session_type = session<protocol_type>;

	explicit gateway(boost::asio::io_service& io, SendHandler&& s, ReceiveHandler&& r) : io_(io), socket_(io_), send_{s}, receive_{r} {
		using namespace std;
		using namespace placeholders;
		//boost::asio::buffer(data, size), sender_endpoint, handler);

		auto buffer(make_shared<vector<uint8_t>>(1500));
		auto endpoint(make_shared<endpoint_type>());

		socket_.async_receive_from(boost::asio::buffer(*buffer), *endpoint, std::bind(&gateway::receive, this, _1, _2, buffer, endpoint));
	}

	void receive(boost::system::error_code const& error, std::size_t const bytes_transfered, std::shared_ptr<std::vector<std::uint8_t>> buffer, std::shared_ptr<endpoint_type> endpoint) {
		// check if endpoint is known
		// decode
	}

	//void send(boost::asio::const_buffers_1 const buffer, endpoint_type const& endpoint) {}
	//void receive(boost::asio::const_buffers_1 const buffer, endpoint_type const& endpoint) {}

	using endpoints_type = std::unordered_map<endpoint_type, session_type>;

	endpoints_type endpoints_;

protected:
	boost::asio::io_service& io_;
	protocol_type::socket socket_;
	SendHandler send_;
	ReceiveHandler receive_;
};

} // namespace: <>

#ifndef IN_IDE_PARSERER
#include "gateway.t++"
#endif

#endif
