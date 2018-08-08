#include <iostream>
#include <sstream>
#include <fstream>
#include "Config.h"
#include "Logger.h"

void Config::add(const std::string & key, const std::string & value)
{
    keyValueMap[key] = value;
}

void Config::clear()
{
    keyValueMap.clear();
}

void Config::print()
{
    for(auto & kv : keyValueMap) {
        std::cout << kv.first << ": " << kv.second << std::endl;
    }
}

void Config::log(Logger & logger)
{
    for(auto & kv : keyValueMap) {
        logger.normal() << kv.first << ": " << kv.second;
    }
}

std::string Config::lookup(const std::string & key)
{
    return keyValueMap[key];
}

std::string Config::lookup(const std::string & key,
                           const std::string & defaultValue)
{
    auto at = keyValueMap.find(key);
    if(at == keyValueMap.end()) {
        return defaultValue;
    }
    else {
        return at->second;
    }
}

template<>
std::string Config::get(const std::string & key)
{
    return lookup(key);
}

template<>
float Config::get(const std::string & key)
{
    return std::stof(lookup(key));
}

template<>
double Config::get(const std::string & key)
{
    return std::stod(lookup(key));
}

template<>
int Config::get(const std::string & key)
{
    return std::stoi(lookup(key));
}

template<>
long Config::get(const std::string & key)
{
    return std::stol(lookup(key));
}

template<>
unsigned long Config::get(const std::string & key)
{
    return std::stoul(lookup(key));
}

template<>
bool Config::get(const std::string & key)
{
    auto value = lookup(key);
    std::transform(value.begin(), value.end(), value.begin(), tolower);
    return (value == "t" ||
            value == "true" ||
            value == "1" ||
            value == "yes");
}

bool loadConfigFromKeyValueFile(const std::string & fileName, Config & config)
{
    config.clear();

    std::ifstream ifs(fileName);

    if(!ifs.good()) {
        std::cerr << "Error opening config file '" << fileName << "'" << std::endl;
        return false;
    }

    std::string line;
    std::string::size_type pos;

    while(std::getline(ifs, line)) {
        // Strip comments
        pos = line.find("#");
        if(pos != std::string::npos) { line.erase(pos); }

        // Split key/value around =, igoring lines without =
        pos = line.find("=");
        if(pos == std::string::npos) { continue; }

        auto key = line.substr(0, pos);
        auto value = line.substr(pos + 1);

        config.add(key, value);
    }

    return true;
}

