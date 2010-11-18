#include "ProcArgs.h"

#include <boost/tokenizer.hpp>

#include <vector>

//INSERT YOUR OWN TOKENIZATION CODE AND STYLE HERE
ProcArgs::ProcArgs(RtString paramStr)
: frame(0.0)
, fps(24.0)
, shutterOpen(0)
, shutterClose(0)
{
    typedef boost::char_separator<char> Separator;
    typedef boost::tokenizer<Separator> Tokenizer;
    
    std::vector<std::string> tokens;
    std::string params(paramStr);
    
    Tokenizer tokenizer(params, Separator(" "));
    for (Tokenizer::iterator I = tokenizer.begin(); I != tokenizer.end(); ++I)
    {
        if ((*I).empty()) continue;
        tokens.push_back((*I));
    }
    
    for (size_t i = 0; i < tokens.size(); ++i)
    {
        if (tokens[i] == "-frame")
        {
            ++i;
            if (i < tokens.size())
            {
                frame = atof(tokens[i].c_str());
            }
        }
        else if (tokens[i] == "-fps")
        {
            ++i;
            if (i < tokens.size())
            {
                fps = atof(tokens[i].c_str());
            }
        }
        else if (tokens[i] == "-shutterOpen")
        {
            ++i;
            if (i < tokens.size())
            {
                shutterOpen = atof(tokens[i].c_str());
            }
        }
        else if (tokens[i] == "-shutterClose")
        {
            ++i;
            if (i < tokens.size())
            {
                shutterClose = atof(tokens[i].c_str());
            }
        }
        else if (tokens[i] == "-filename")
        {
            ++i;
            if (i < tokens.size())
            {
                filename = tokens[i];
            }
        }
        else if (tokens[i] == "-objectpath")
        {
            ++i;
            if (i < tokens.size())
            {
                objectpath = tokens[i];
            }
        }
    }
}
