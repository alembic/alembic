#include <boost/program_options.hpp>


// Can test with find_package(python)
// #include <boost/python.hpp>
// #include <Python.h>
// using namespace boost::python;

#include <fstream>
#include <iostream>

using std::ostream;
using std::ofstream;
using std::cout;

namespace po = boost::program_options;

int main(int argc, char *argv[])
{

#if defined(BOOST_ALL_DYN_LINK) || defined(BOOST_SYSTEM_DYN_LINK)
    std::cout << "ERROR: Boost is compiled as dynamically linked. We're expecting statically linked" << std::endl;
#endif

    // test program_options
    po::options_description desc( "boost_testcompile options" );
    desc.add_options()

        ( "help,h", "prints this help message" )

        ;

    po::variables_map vm;
    po::store( po::command_line_parser( argc, argv ).
               options( desc ).run(), vm );
    po::notify( vm );

    //-*************************************************************************

    if ( vm.count( "help" ) )
    {
        std::cout << desc << std::endl;
        return -1;
    }

    return 0;
}

