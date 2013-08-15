#ifndef controller_protocol_h
#define controller_protocol_h
#include <boost/asio.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/serialization/list.hpp>
#include <list>

#include "convert.h++"


namespace {

//! class used as interface between commandline
class query {
public:
	enum class action { nodelist = 0, nodeadd = 1 , nodedel = 2 };

	//! serialize content
	template<class Archive>
	void serialize(Archive&, unsigned int const);

	action action_; //!< action to perform
	std::list<boost::asio::ip::udp::endpoint> endpoints_; //!< list of all the endpoints
};

} // namespace: <>

#ifndef IN_IDE_PARSERER
#include "controller_protocol.t++"
#endif

#endif
