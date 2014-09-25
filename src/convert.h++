#ifndef convert_h
#define convert_h

#include <string>
#include <boost/asio/ip/udp.hpp>

//! convert binary data to hex string
std::string to_hex(unsigned char const* const data, std::size_t const s, bool uppercase = false);

//! convert first bytes of binary data to uint64_t
std::uint64_t to_uint64_t(unsigned char const* const data);

//! read address from stream
template <typename _CharT, typename _Traits>
std::basic_istream<_CharT, _Traits>& operator>>(std::basic_istream<_CharT, _Traits>&, boost::asio::ip::address&);

//! write address to stream
template <typename _CharT, typename _Traits>
std::basic_istream<_CharT, _Traits>& operator<<(std::basic_istream<_CharT, _Traits>&, boost::asio::ip::address&);

namespace boost {
namespace serialization {

//! the serialization function for boost
template <typename Archive, typename InternetProtocol>
void serialize(Archive& ar, boost::asio::ip::basic_endpoint<InternetProtocol>&, unsigned int const file_version);

//! save part of serialization function
template <typename Archive, typename InternetProtocol>
void save(Archive& ar, boost::asio::ip::basic_endpoint<InternetProtocol> const&, unsigned int const version);

//! load part of serialization function
template <typename Archive, typename InternetProtocol>
void load(Archive& ar, boost::asio::ip::basic_endpoint<InternetProtocol>&, unsigned int const version);

} // namespace: serialization
} // namespace: boost


#ifndef IN_IDE_PARSERER
#include "convert.t++"
#endif

#endif
