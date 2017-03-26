#ifndef PROXY_URL_EXTRACTOR_H_
#define PROXY_URL_EXTRACTOR_H_

#include<string>
#include<set>


class ProxyURLExtractor
{
public:
    typedef std::set<std::string/*proxy key*/> KeyItems;

public:
    ProxyURLExtractor();

        
    bool Initialize(const std::string& rule_file);

        
    std::string Extract(const std::string& url);

    static void Extract(const KeyItems& keys, const std::string& raw_url, std::string& sub_url);
    static std::string Extract(const KeyItems& keys, const std::string& raw_url);

private:

    KeyItems keys_set_;
};


#endif //PROXY_URL_EXTRACTOR_H_
