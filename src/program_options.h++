#ifndef program_options_h
#define program_options_h

#include <boost/program_options/variables_map.hpp>

namespace cfg {

extern boost::program_options::variables_map configuration;

//! parses command line using boost program_options
//! \brief parses command line
//! \param[in] argc number of commandline arguments
//! \param[in] argv command line
void parse_command_line(int const argc, char const* const* const argv);

} // namespace: cfg

#endif
