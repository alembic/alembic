#include "PathUtil.h"


#include <boost/tokenizer.hpp>


void TokenizePath(const std::string & path, std::vector<std::string> & result)
{
    typedef boost::char_separator<char> Separator;
    typedef boost::tokenizer<Separator> Tokenizer;
    
    Tokenizer tokenizer(path, Separator("/"));
    
    for (Tokenizer::iterator I = tokenizer.begin(); I != tokenizer.end(); ++I)
    {
        if ((*I).empty()) continue;
        result.push_back((*I));
    }
}
