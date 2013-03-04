#include <fstream>

#include "cppunit/TextOutputter.h"
#include "cppunit/extensions/TestFactoryRegistry.h"
#include "cppunit/ui/text/TestRunner.h"

using namespace SPI_CppUnit_v1;

int main(void)

{

    TextUi::TestRunner runner;

    runner.addTest(TestFactoryRegistry::getRegistry().makeTest());

    std::string path = __FILE__;
    path = path.substr(0, path.size() - 8); // - Main.cpp
    path += "../out/TestResults.txt";

    std::ofstream stream(path.c_str());
    TextOutputter* outputter(new TextOutputter(&runner.result(), stream));

    runner.setOutputter(outputter);

    return runner.run() ? 0 : 1;
}
