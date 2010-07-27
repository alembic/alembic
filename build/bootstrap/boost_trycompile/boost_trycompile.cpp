//
// tests compile of boost with:
// 
//  iostreams
//  program_options
//  filesystem 
//  regex
//  system
//
// not tested:
//  python

#include <boost/iostreams/stream.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/system/system_error.hpp>
#include <boost/regex.hpp>


// Can test with find_package(python) 
// #include <boost/python.hpp>
// #include <Python.h>
// using namespace boost::python;

#include <fstream>
#include <iostream>

using std::ostream;
using std::ofstream;
using std::cout;

namespace bfs = boost::filesystem;
namespace po = boost::program_options;
namespace bio = boost::iostreams;
using bio::stream;

int main(int argc, char *argv[])
{
// uncomment to test the try compile failing
//    void *****

#if defined(BOOST_ALL_DYN_LINK) || defined(BOOST_SYSTEM_DYN_LINK)
    std::cout << "ERROR: Boost is compiled as dynamically linked. We're expecting statically linked" << std::endl;
#endif

    // test iostream
    ofstream ofs("sample.txt");
    ofs << "Hello, World!\n";
    ofs.flush();
    ofs.close();

    // test filesystem 
    bfs::path rp;

    // test regex
    static const boost::regex GENERIC_TYPE_BAR_NAME_RE("([[:word:]]*)\\|([[:word:]]*)" );

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

