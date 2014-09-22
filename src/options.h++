#ifndef options_h
#define options_h

#include <boost/program_options/variables_map.hpp>

class testy {
public:
// 	testy(testy const&) = delete;
	testy() : key("xx") { std::cerr << "_______________ testy(): " << key << " _____________" << std::endl; }
	testy(testy const& t) : key(t.key) { std::cerr << "_______________ testy(&): " << key << " _____________" << std::endl; }
	testy(testy&& t) : key(t.key) { std::cerr << "_______________ testy(&&): " << key << " _____________" << std::endl; }
	explicit testy(std::string const& a) : key(a) { std::cerr << "_______________ testy(string): " << key << " _____________" << std::endl; }
	testy& operator=(testy const& t) { std::cerr << "_______________ operator=(&): " << key << " _____________" << std::endl; }
	testy& operator=(testy&& t) { std::cerr << "_______________ operator=(&&): " << key << " _____________" << std::endl; }
	std::string key;
};


namespace cfg {

extern boost::program_options::variables_map parameters;

//! parses command line using boost program_options
//! \brief parses command line
//! \param[in] argc number of commandline arguments
//! \param[in] argv command line
void parse_command_line(int const argc, char const* const* const argv);

} // namespace: cfg

#endif
