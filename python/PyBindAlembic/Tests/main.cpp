#include <Python.h>
#include <testAlembicImport.h>
#include <string.h>
#include <boost/python.hpp>

#ifdef PLATFORM_WINDOWS
#include <winsock.h>
#endif


#define TEST(x) if (argc < 2 || !strcmp (argv[1], #x)) x();

void
runTests (int argc, char *argv[])
{
    Py_Initialize();

    // Test 1: Alembic CoreAbstract Type binding
    {
        std::string code =
            "import testTypes\n";

        PyRun_SimpleString (code.c_str());
    }

    // Test 2: Python import binding.
    // We export an alembic cache file with c++ api.
    TEST(testAlembicImport);

    // We then import the cache using python api
    // and check the values read are the same.
    {
        std::string code =
            "from testAlembicImport import*\n"
            "print 'Testing Python import binding for scalar data types'\n"
            "importCache( 'testImportScalarProperties.abc' )\n"
            "print 'passed'\n"
//            "print 'Testing Python import binding for small array data types'\n"
//            "importCache( 'testImportSmallArrayProperties.abc' )\n"
//            "print 'passed'\n"
            "print 'Testing Python import binding for array data types'\n"
            "importCache( 'testImportArrayProperties.abc' )\n"
            "print 'passed'\n";

        PyRun_SimpleString (code.c_str());
    }

    // Test 3: Python export binding.
    // We export an alembic cache file using python api
    // and read it again using python api
    {
        std::string code =
            "from testAlembicImport import*\n"
            "from testAlembicExport import*\n"
            "print 'Testing Python export binding for scalar data types'\n"
            "exportCache( 'testExportScalarProperties.abc', 0 )\n"
            "importCache( 'testExportScalarProperties.abc' )\n"
            "print 'passed'\n"
//            "print 'Testing Python export binding for small array data types'\n"
//            "exportCache( 'testExportSmallArrayProperties.abc', 1 )\n"
//            "importCache( 'testExportSmallArrayProperties.abc' )\n"
//            "print 'passed'\n"
            "print 'Testing Python export binding for array data types'\n"
            "exportCache( 'testExportArrayProperties.abc', 2 )\n"
            "importCache( 'testExportArrayProperties.abc' )\n"
            "print 'passed'\n";

        PyRun_SimpleString (code.c_str());
    }

    // Test 4: Export/Import a polygon cube
    // An animated cube is exported and imported
    {
        std::string code =
            "import testCacheCube\n"
            "import testCacheCube2\n";

        PyRun_SimpleString (code.c_str());
    }

    // Test 5: Geom, Material and Collection bindings, iterators
    {
        std::string code =
            "import testIterators\n"
            "import testWrap\n"
            "import testAbcGeomBinding\n"
            "import testMaterial\n"
            "import testCollections\n";

        PyRun_SimpleString (code.c_str());
    }

    // Test 6: Instance, Hash, Reference
    {
        std::string code =
            "import testInstance\n"
            "import testHash\n"
            "import testReference\n";

        PyRun_SimpleString (code.c_str());
    }

    Py_Finalize();
}

std::string
stripLastPath (std::string path)
{
    std::size_t lastSlash = path.find_last_of('\\');
    if (lastSlash != std::string::npos)
        return path.substr (0, lastSlash);
    return "";
}

int
main (int argc, char *argv[])
{
#ifdef PLATFORM_WINDOWS
    WSADATA info;
    if (WSAStartup(MAKEWORD(2,0), &info)) {
    //    throw "Could not start WSA";
    }

    //
    // Until we have something like the zenowrapper on Windows, we need to set
    // the PYTHONPATH manually.
    //
    char exePath[_MAX_PATH];
    GetModuleFileName (0, exePath, _MAX_PATH);
    std::string startDir = stripLastPath (exePath);
    // Find the build root by cutting everything after the second-last slash.
    std::string buildRoot = stripLastPath (stripLastPath (startDir));
    // Set the python path
    char* startPythonPath = getenv ("PYTHONPATH");
    std::string pythonPath = "PYTHONPATH=" + buildRoot + "\\win32\\PyModules";
    if (startPythonPath)
        pythonPath += ";" + std::string (startPythonPath);
    putenv (pythonPath.c_str());
#endif

    runTests (argc, argv);
    return 0;
}
