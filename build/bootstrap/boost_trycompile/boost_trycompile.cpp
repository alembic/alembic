#include <boost/thread.hpp>



#include <fstream>
#include <iostream>
#include <string>

using std::ostream;
using std::ofstream;
using std::cout;

void hello()
{
    std::cout << "Yes we don't have no libboost_thread today." << std::endl;
}

int main(int argc, char *argv[])
{

#if defined(BOOST_ALL_DYN_LINK) || defined(BOOST_SYSTEM_DYN_LINK)
    std::cout << "ERROR: Boost is compiled as dynamically linked. We're expecting statically linked" << std::endl;
#endif

    boost::thread bt( hello );
    bt.join();

    return 0;
}

