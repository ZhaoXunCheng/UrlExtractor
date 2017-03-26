#include "proxy_url_extractor.h"
#include <fstream>
#include <vector>
#include "tokener.h"


template< class _StringVector,
class StringType,
class _DelimType>
    inline void StringSplit(
    const StringType& str,
    const _DelimType& delims,
    unsigned int maxSplits,
    _StringVector& ret)
{
    unsigned int numSplits = 0;


    size_t start, pos;
    start = 0;

    do
    {
        pos = str.find_first_of( delims, start );

        if ( pos == start )
        {
            ret.push_back(StringType());
            start = pos + 1;
        }
        else if ( pos == StringType::npos || ( maxSplits && numSplits + 1== maxSplits ) )
        {
            ret.push_back(StringType());
            *(ret.rbegin()) = StringType(str.data() + start, str.size() - start);
            break;
        }
        else
        {
            ret.push_back(StringType());
            *(ret.rbegin()) = StringType(str.data() + start, pos - start);
            start = pos + 1;
        }

        ++numSplits;

    }
    while ( pos != StringType::npos );
}


ProxyURLExtractor::ProxyURLExtractor()
{
}

bool ProxyURLExtractor::Initialize( const std::string& param_keys_path )
{
    std::ifstream ifs;
    ifs.open(param_keys_path.data(), std::fstream::in);
    typedef std::vector<std::string> stringvector;
    stringvector keysvect;

    while (!ifs.eof()) {
        std::string line;
        getline(ifs, line);
        if (ifs.fail() && !ifs.eof()) {
            fprintf(stderr, "SubUrlExtractor::LoadParamKeysFile readfile_error=[%s] error!!", param_keys_path.data());
            ifs.close();
            return false;
        }
        if (line.empty()) continue;

        keysvect.clear();
        StringSplit(line, ",", static_cast<unsigned int>(-1), keysvect);
        assert(keysvect.size() >= 1);
        keys_set_.insert(keysvect.begin(), keysvect.end());
        keys_set_.erase("");
    }

    ifs.close();

    return true;
}

std::string ProxyURLExtractor::Extract( const std::string& raw_url )
{
    std::string sub_url;
    ProxyURLExtractor::Extract(keys_set_, raw_url, sub_url);
    return sub_url;
}

void ProxyURLExtractor::Extract( const KeyItems& keys, const std::string& raw_url, std::string& sub_url )
{
    Tokener token(raw_url);
    Tokener temp;
    token.skipTo('?');
    token.next();
    std::string key;
    while (!token.isEnd()) {
        while(token.current() == '&'){
            token.next();
        }
        std::string temp_str = token.nextString('&');
        if(temp_str.length() == 0){
            temp_str = token.nextString();
        }
        temp.reset(temp_str.c_str(), temp_str.length());

        key = temp.nextString('=');
		if (keys.find(key) != keys.end()) {
            sub_url = temp.nextString();
            break;
        }
    }
}

std::string ProxyURLExtractor::Extract( const KeyItems& keys, const std::string& raw_url )
{
    std::string sub_url;
    ProxyURLExtractor::Extract(keys, raw_url, sub_url);
    return sub_url;
}
