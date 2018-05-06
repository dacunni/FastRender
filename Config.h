#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <map>
#include <string>

class Config
{
    public:
        Config() = default;
        virtual ~Config() = default;

        template<typename T> T get(const std::string & key);

        void add(const std::string & key, const std::string & value);
        void clear();
        void print();

    protected:
        using KeyValueMap = std::map<std::string, std::string>;

        std::string lookup(const std::string & key);
        std::string lookup(const std::string & key,
                           const std::string & defaultValue);

        KeyValueMap keyValueMap;
};

bool loadConfigFromKeyValueFile(const std::string & fileName, Config & config);

#endif
