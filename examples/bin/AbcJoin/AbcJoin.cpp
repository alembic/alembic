
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <experimental/string_view>
#include <Alembic/AbcCoreHDF5/All.h>
#include <Alembic/AbcCoreOgawa/All.h>
#include "anim.hpp"
#include "mod.hpp"

using std::experimental::string_view;

namespace dataModel
{
void sort();
bool duplicates();
void reset();
bool empty();
void scale(double const);
}

namespace animGather
{
string_view load(
        bool const noLoadOpt,
        std::vector<std::string> const& animFileNames,
        AnimFiles& animFiles);
Alembic::Abc::IArchive getArchive(std::string const& animFileName);
}

namespace levCheck1
{
void traverse(
        AnimFiles const& animFiles,
        std::vector<std::string> const& modFileNames,
        std::vector<std::string>& msgs);
void traverse(
        AnimFiles const& animFiles,
        std::vector<std::pair<std::string, std::string>> const& modAnimFileNames,
        std::vector<std::string>& msgs);
}

namespace levOpt1
{
void traverse(AnimFiles const& animFiles);
}

namespace levOpt2
{
void traverse(AnimFiles const& animFiles);
}

namespace levOpt3
{
void traverse(AnimFiles const& animFiles);
}

static unsigned long int const levOutMax = 3UL;
static unsigned long int const levOptMax = 4UL;
static unsigned long int const levCheckMax = 2UL;

unsigned char levOpt(levOptMax);

AnimConfig animConfig;
ModConfig modConfig;

template <typename TV>
bool compareFirsts(
        typename TV::value_type const& a,
        typename TV::value_type const& b);

namespace animOut
{
void merge(
        bool const dupAnim,
        std::vector<std::string> const& modFileNames,
        std::vector<double> const& offsets,
        std::vector<std::string> const& animFileNames,
        AnimFiles const& animFiles,
        bool const difAnim,
        Alembic::Abc::OArchive& archOut);
}

namespace animTran
{
void transferAnimObject(
        Alembic::Abc::IObject const& parentAnim,
        Alembic::Abc::OObject const& parentOut);
}

namespace levOpt4
{
void traverse(
        AnimFiles const& animFiles,
        std::vector<std::string> const& modFileNames);
void traverse(
        AnimFiles const& animFiles,
        std::vector<std::pair<std::string, std::string>> const& modAnimFileNames);
}

namespace modGather
{
string_view dataLoad(std::vector<std::string> const& modFiles);
}

namespace levCheck2
{
void traverse(
        AnimFiles const& animFiles,
        std::vector<std::string> const& modFileNames,
        std::vector<std::string>& msgs);
void traverse(
        AnimFiles const& animFiles,
        std::vector<std::pair<std::string, std::string>> const& modAnimFileNames,
        std::vector<std::string>& msgs);
}

static inline void printHelp()
{
    std::cout << "AbcJoin.exe -m fileMod.abc -a fileAnim.abc -o fileOut.abc\n";
    std::cout << "-m --model fileMod.abc\n";
    std::cout << "-a --animation fileAnim.abc\n";
    std::cout << "-f --offset 0.0\n";
    std::cout << "-o --out fileOut.abc\n";
    std::cout << "-p --positions\n";
    std::cout << "-pn --positions-name rest\n";
    std::cout << "-n --normals\n";
    std::cout << "-gn --generate-normals\n";
    std::cout << "-nn --normals-name rnml\n";
    std::cout << "-fns --form-name-spaces\n";
    std::cout << "-gpn --group-by-instance\n";
    std::cout << "-nrp --no-replace\n";
    std::cout << "-fnm --force-no-match\n";
    std::cout << "-pnm --pass-no-match\n";
    std::cout << "-nlp --no-load-opt\n";
    std::cout << "-nbc --no-block-check\n";
    std::cout << "-prn --project-name PRJ\n";
    std::cout << "-sf --scale-factor 1.0\n";
    std::cout << "-tlv --out-level " << levOutMax;
    std::cout << "\n-plv --opt-level " << levOptMax;
    std::cout << "\n-clv --check-level " << levCheckMax;
    std::cout << "\n-nuv --no-uvs\n";
    std::cout << "-fcn --force-join\n";
    std::cout << "-h --help";
}

template <typename TV>
TV unique(TV r)
{
    if (r.empty())
    {
        return r;
    }
    std::sort(r.begin(), r.end());
    return TV(r.begin(), std::unique(r.begin(), r.end()));
}

template <typename TS>
static inline std::vector<std::pair<TS, TS>> zip(
    std::vector<TS> const& va,
    std::vector<TS> const& vb)
{
    std::vector<std::pair<TS, TS>> vp;
    typename TS::size_type const sz = vb.size();
    if (va.size() != sz)
    {
        return {};
    }
    vp.reserve(sz);
    for (std::size_t i = 0; i < sz; ++i)
    {
        vp.emplace_back(std::make_pair(va[i], vb[i]));
    }
    return vp;
}

template <typename TS>
static inline std::vector<std::pair<TS, TS>> unique(
    std::vector<TS> const& va,
    std::vector<TS> const& vb)
{
    return unique(zip(va, vb));
}

static inline void optimize(
        bool const dupAnim,
        std::vector<std::string> const& modFileNames,
        AnimFiles const& animFiles,
        std::vector<std::string> const& animFileNames)
{
    switch (levOpt)
    {
        default:
        case 4:
            if (dupAnim)
            {
                levOpt4::traverse(animFiles,
                        unique(modFileNames, animFileNames));
            }
            else
            {
                levOpt4::traverse(animFiles, modFileNames);
            }
        case 3:
            levOpt3::traverse(animFiles);
        case 2:
            levOpt2::traverse(animFiles);
        case 1:
            levOpt1::traverse(animFiles);
        case 0:
            break;
    }
}

static inline std::vector<std::string> validate(
        bool const dupAnim,
        std::vector<std::string> const& modFileNames,
        unsigned char const levCheck,
        AnimFiles const& animFiles,
        std::vector<std::string> const& animFileNames)
{
    std::vector<std::string> msgs;
    if (dupAnim)
    {
        std::vector<std::pair<std::string, std::string>> const& up =
                unique(modFileNames, animFileNames);
        levCheck1::traverse(animFiles, up, msgs);
        if (levCheck > 1)
        {
            levCheck2::traverse(animFiles, up, msgs);
        }
    }
    else
    {
        levCheck1::traverse(animFiles, modFileNames, msgs);
        if (levCheck > 1)
        {
            levCheck2::traverse(animFiles, modFileNames, msgs);
        }
    }
    return msgs;
}

static inline void report(std::vector<std::string> const& msgs)
{
    std::vector<std::string>::const_iterator mit = msgs.cbegin();
    std::cerr << *mit;
    ++mit;
    std::for_each(mit, msgs.cend(), [](std::string const& m)
            { std::cerr << "\n" + m; });
}

namespace
{
class Main
{
   public:
    Main(int const argc, char const* const argv[], int& r, bool& h)
    {
        if (parse(argc, argv, h) == EXIT_FAILURE)
        {
            r = EXIT_FAILURE;
            return;
        }
        if (h)
        {
            return;
        }
        if (check() == EXIT_FAILURE)
        {
            r = EXIT_FAILURE;
        }
    }
   private:
    int parse(int const argc, char const* const argv[], bool& help)
    {
        if (argc == 1)
        {
            printHelp();
            return EXIT_FAILURE;
        }
        for (int argi = 1; argi < argc; ++argi)
        {
            std::string const flag(argv[argi]);
            if (flag == "-fcn" || flag == "--force-join")
            {
                joinForce = true;
            }
            else if (flag == "-fns" || flag == "--form-name-spaces")
            {
                animConfig.formNamespaces = true;
                difAnim = true;
            }
            else if (flag == "-gpn" || flag == "--group-by-instance")
            {
                animConfig.groupByInstance = true;
                difAnim = true;
            }
            else if (flag == "-nbc" || flag == "--no-block-check")
            {
                noBlockCheck = true;
            }
            else if (flag == "-nlp" || flag == "--no-load-opt")
            {
                noLoadOpt = true;
            }
            else if (flag == "-fnm" || flag == "--force-no-match")
            {
                animConfig.forceNoMatch = true;
            }
            else if (flag == "-pnm" || flag == "--pass-no-match")
            {
                animConfig.passNoMatch = true;
            }
            else if (flag == "-nrp" || flag == "--no-replace")
            {
                animConfig.noReplace = true;
            }
            else if (flag == "-n" || flag == "--normals")
            {
                modConfig.outNormals = true;
            }
            else if (flag == "-gn" || flag == "--generate-normals")
            {
                modConfig.genNormals = true;
            }
            else if (flag == "-prn" || flag == "--project-name")
            {
                ++argi;
                if (argi < argc)
                {
                    modConfig.nameProject = argv[argi];
                }
                else
                {
                    std::cerr << "no project name";
                    return EXIT_FAILURE;
                }
            }
            else if (flag == "-p" || flag == "--positions")
            {
                modConfig.outPositions = true;
            }
            else if (flag == "-pn" || flag == "--positions-name")
            {
                modConfig.outPositions = true;
                ++argi;
                if (argi < argc)
                {
                    modConfig.namePositions = argv[argi];
                }
                else
                {
                    std::cerr << "no positions name";
                    return EXIT_FAILURE;
                }
            }
            else if (flag == "-nn" || flag == "--normals-name")
            {
                modConfig.outNormals = true;
                ++argi;
                if (argi < argc)
                {
                    modConfig.nameNormals = argv[argi];
                }
                else
                {
                    std::cerr << "no normals name";
                    return EXIT_FAILURE;
                }
            }
            else if (flag == "-m" || flag == "--model")
            {
                ++argi;
                if (argi < argc)
                {
                    modFileNames.emplace_back(argv[argi]);
                }
                else
                {
                    std::cerr << "no model file";
                    return EXIT_FAILURE;
                }
            }
            else if (flag == "-a" || flag == "--animation")
            {
                ++argi;
                if (argi < argc)
                {
                    animFileNames.emplace_back(argv[argi]);
                }
                else
                {
                    std::cerr << "no animation file";
                    return EXIT_FAILURE;
                }
            }
            else if (flag == "-f" || flag == "--offset")
            {
                ++argi;
                if (argi < argc)
                {
                    double const offset = strtod(argv[argi], nullptr);
                    if (offset == HUGE_VAL)
                    {
                        std::cerr << "wrong offset";
                        return EXIT_FAILURE;
                    }
                    offsets.emplace_back(offset);
                }
                else
                {
                    std::cerr << "no offset";
                    return EXIT_FAILURE;
                }
            }
            else if (flag == "-o" || flag == "--out")
            {
                ++argi;
                if (argi < argc)
                {
                    if (nameFileOut.empty())
                    {
                        nameFileOut = argv[argi];
                    }
                    else
                    {
                        std::cerr << "out flag can only be used once";
                        return EXIT_FAILURE;
                    }
                }
                else
                {
                    std::cerr << "no out file";
                    return EXIT_FAILURE;
                }
            }
            else if (flag == "-tlv" || flag == "--out-level")
            {
                ++argi;
                if (argi < argc)
                {
                    std::string const ts = argv[argi];
                    if (ts == "0")
                    {
                        levOut = 0;
                    }
                    else
                    {
                        unsigned long int const tcr = strtoul(ts.c_str(), nullptr, 0);
                        if (tcr == 0UL || tcr == ULONG_MAX)
                        {
                            std::cerr << "wrong out level";
                            return EXIT_FAILURE;
                        }
                        if (tcr > levOutMax)
                        {
                            std::cerr << "out level too big";
                            return EXIT_FAILURE;
                        }
                        levOut = tcr;
                    }
                }
                else
                {
                    std::cerr << "no out level";
                    return EXIT_FAILURE;
                }
            }
            else if (flag == "-plv" || flag == "--opt-level")
            {
                ++argi;
                if (argi < argc)
                {
                    std::string const ts = argv[argi];
                    if (ts == "0")
                    {
                        levOpt = 0;
                    }
                    else
                    {
                        unsigned long int const tcr = strtoul(ts.c_str(), nullptr, 0);
                        if (tcr == 0UL || tcr == ULONG_MAX)
                        {
                            std::cerr << "wrong opt level";
                            return EXIT_FAILURE;
                        }
                        if (tcr > levOptMax)
                        {
                            std::cerr << "opt level too big";
                            return EXIT_FAILURE;
                        }
                        levOpt = tcr;
                    }
                }
                else
                {
                    std::cerr << "no opt level";
                    return EXIT_FAILURE;
                }
            }
            else if (flag == "-clv" || flag == "--check-level")
            {
                ++argi;
                if (argi < argc)
                {
                    std::string const ts = argv[argi];
                    if (ts == "0")
                    {
                        levCheck = 0;
                    }
                    else
                    {
                        unsigned long int const tcr = strtoul(ts.c_str(), nullptr, 0);
                        if (tcr == 0UL || tcr == ULONG_MAX)
                        {
                            std::cerr << "wrong check level";
                            return EXIT_FAILURE;
                        }
                        if (tcr > levCheckMax)
                        {
                            std::cerr << "check level too big";
                            return EXIT_FAILURE;
                        }
                        levCheck = tcr;
                    }
                }
                else
                {
                    std::cerr << "no check level";
                    return EXIT_FAILURE;
                }
            }
            else if (flag == "-sf" || flag == "--scale-factor")
            {
                ++argi;
                if (argi < argc)
                {
                    if (scaleFactor == 1.0)
                    {
                        scaleFactor = strtod(argv[argi], nullptr);
                        if (scaleFactor == 0.0 || scaleFactor == HUGE_VAL)
                        {
                            std::cerr << "wrong scale factor";
                            return EXIT_FAILURE;
                        }
                    }
                    else
                    {
                        std::cerr << "scale factor flag can only be used once";
                        return EXIT_FAILURE;
                    }
                }
                else
                {
                    std::cerr << "no factor";
                    return EXIT_FAILURE;
                }
            }
            else if (flag == "-nuv" || flag == "--no-uvs")
            {
                modConfig.outUVs = false;
            }
            else if (flag == "-h" || flag == "--help")
            {
                printHelp();
                help = true;
                return 0;
            }
            else
            {
                if (flag[0] == '-')
                {
                    std::size_t const fl = flag.length();
                    if (fl == 1 || (fl == 2 && flag[1] == '-'))
                    {
                        std::cerr << "empty flag";
                    }
                    else
                    {
                        std::cerr << "unknown flag " << flag;
                    }
                }
                else
                {
                    std::cerr << "flags should begin with -";
                }
                return EXIT_FAILURE;
            }
        }
        return 0;
    }
    int check()
    {
        if (modFileNames.empty())
        {
            std::cerr << "no models\n";
            printHelp();
            return EXIT_FAILURE;
        }
        bool const lookDev =
                modConfig.outPositions ||
                modConfig.outNormals ||
                modConfig.genNormals;
        if (animFileNames.empty() && !lookDev)
        {
            std::cerr << "no animation\n";
            printHelp();
            return EXIT_FAILURE;
        }
        if (nameFileOut.empty())
        {
            std::cerr << "no out file\n";
            printHelp();
            return EXIT_FAILURE;
        }
        nubModFileNames = unique(modFileNames);
        for (std::string const& nubModFileName : nubModFileNames)
        {
            if (nubModFileName == nameFileOut)
            {
                std::cerr << "every model file has to be different from output file";
                return EXIT_FAILURE;
            }
        }
        if (animFileNames.empty() && lookDev)
        {
            animFileNames.emplace_back(nubModFileNames.front());
        }
        nubAnimFileNames = unique(animFileNames);
        for (std::string const& nubAnimFileName : nubAnimFileNames)
        {
            if (nubAnimFileName == nameFileOut)
            {
                std::cerr << "every animation file has to be different from output file";
                return EXIT_FAILURE;
            }
        }
        return 0;
    }
   public:
    std::vector<std::string> main() const
    {
        if (
                modConfig.outPositions ||
                modConfig.outNormals ||
                modConfig.genNormals)
        {
            if (1.0 != scaleFactor)
            {
                dataModel::scale(scaleFactor);
            }
        }
        string_view const badModFileName = modGather::dataLoad(nubModFileNames);
        if (!badModFileName.empty())
        {
            return {"not valid model " + badModFileName.to_string()};
        }
        if (dataModel::empty())
        {
            return {"no valid models"};
        }
        dataModel::sort();
        if (levCheck)
        {
            if (dataModel::duplicates())
            {
                return {"duplicate model object detected"};
            }
        }
        std::unique_ptr<AnimFiles> animFilesPtr = std::make_unique<AnimFiles>();
        AnimFiles& animFiles = *animFilesPtr;
        std::size_t const animFileNameSize = animFileNames.size();
        std::size_t const nubAnimFileNameSize = nubAnimFileNames.size();
        bool const dupAnim = nubAnimFileNameSize < animFileNameSize;
        if (!joinForce || animFileNameSize > 1)
        {
            bool const zero = offsets.empty();
            if (levCheck)
            {
                if (zero && dupAnim)
                {
                    return {"duplicate animation detected"};
                }
            }
            if (modFileNames.size() != animFileNameSize)
            {
                return {"model and animation count not the same"};
            }
            if (!zero && offsets.size() != animFileNameSize)
            {
                return {"offset and animation count not the same"};
            }
            string_view const badAnimFileName = animGather::load(noLoadOpt,
                    dupAnim ? nubAnimFileNames : animFileNames, animFiles);
            if (!badAnimFileName.empty())
            {
                return {"not valid animation " + badAnimFileName.to_string()};
            }
            if (dupAnim)
            {
                std::sort(animFiles.begin(), animFiles.end(),
                        compareFirsts<AnimFiles>);
            }
        }
        bool const merge = !joinForce || nubAnimFileNameSize > 1;
        if (merge)
        {
            if (!dupAnim)
            {
                if (animFiles.size() != modFileNames.size())
                {
                    return {"loaded animation and model count not the same"};
                }
            }
            optimize(dupAnim, modFileNames, animFiles, animFileNames);
            if (levCheck)
            {
                std::vector<std::string> msgs(validate(dupAnim, modFileNames,
                        levCheck, animFiles, animFileNames));
                if (!msgs.empty())
                {
                    if (!noBlockCheck)
                    {
                        return msgs;
                    }
                    report(msgs);
                }
            }
        }
        std::unique_ptr<std::ostringstream> archOutStream =
                std::make_unique<std::ostringstream>();
        std::ofstream archOutFile;
        {
            Alembic::Abc::IArchive archAnim;
            Alembic::Abc::OArchive archOut;
            if (0 == levOut)
            {
                archOut = Alembic::Abc::OArchive(
                        Alembic::AbcCoreHDF5::WriteArchive(), nameFileOut);
            }
            else
            {
                if (1 == levOut)
                {
                    archOut = Alembic::Abc::OArchive(
                            Alembic::AbcCoreOgawa::WriteArchive(), nameFileOut);
                }
                else
                {
                    Alembic::AbcCoreAbstract::MetaData metaData;
                    Alembic::AbcCoreOgawa::WriteArchive writeArchive;
                    Alembic::AbcCoreAbstract::ArchiveWriterPtr archWritePtr;
                    if (2 == levOut)
                    {
                        archOutFile.open(nameFileOut);
                        archWritePtr = writeArchive(&archOutFile, metaData);
                    }
                    else
                    {
                        archWritePtr = writeArchive(archOutStream.get(), metaData);
                    }
                    archOut = Alembic::Abc::OArchive(archWritePtr,
                            Alembic::Abc::kWrapExisting);
                }
            }
            if (merge)
            {
                animOut::merge(dupAnim, modFileNames, offsets, animFileNames,
                        animFiles, difAnim, archOut);
            }
            else
            {
                std::string const& animFileName = animFileNames.front();
                archAnim = animGather::getArchive(animFileName);
                if (!archAnim.valid())
                {
                    return {"not valid animation " + animFileName};
                }
                uint32_t const numTimeSamplings = archAnim.getNumTimeSamplings();
                for (uint32_t i = 1; i < numTimeSamplings; ++i)
                {
                    archOut.addTimeSampling(*archAnim.getTimeSampling(i));
                }
                animTran::transferAnimObject(archAnim.getTop(), archOut.getTop());
            }
        }
        animFilesPtr.reset();
        dataModel::reset();
        levOut3(archOutStream, archOutFile);
        return {};
    }
   private:
    void levOut3(
            std::unique_ptr<std::ostringstream>& archOutStream,
            std::ofstream& archOutFile) const
    {
        if (3 != levOut)
        {
            return;
        }
        std::string const& tmpStr = archOutStream->str();
        archOutStream.reset();
        archOutFile.open(nameFileOut);
        std::string::size_type const b = 2 << 20;
        std::string::size_type const s = tmpStr.size();
        char const* const d = tmpStr.data();
        if (s > b)
        {
            std::string::size_type const c = s / b;
            for (std::string::size_type i = 0; i < c; ++i)
            {
                archOutFile.write(d + i * b, b);
            }
            std::string::size_type const r = b * c;
            archOutFile.write(d + r, s - r);
        }
        else
        {
            archOutFile.write(d, s);
        }
    }
    std::vector<std::string> modFileNames, animFileNames,
            nubModFileNames, nubAnimFileNames;
    std::vector<double> offsets;
    std::string nameFileOut;
    double scaleFactor = 1.0;
    unsigned char levOut = levOutMax;
    unsigned char levCheck = levCheckMax;
    bool noBlockCheck = false;
    bool noLoadOpt = false;
    bool joinForce = false;
    bool difAnim = false;
};
}

int main(int const argc, char const* const argv[])
{
    int r = 0;
    bool h = false;
    Main const m(argc, argv, r, h);
    if (EXIT_FAILURE == r)
    {
        return EXIT_FAILURE;
    }
    if (h)
    {
        return 0;
    }
    std::vector<std::string> const& msgs = m.main();
    if (msgs.empty())
    {
        return 0;
    }
    report(msgs);
    return EXIT_FAILURE;
}
