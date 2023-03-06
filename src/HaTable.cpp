//
// Created by vanilor on 23/12/22.
//

#include <algorithm>
#include <regex>
#include <filesystem>
#include <valarray>

#include "HaUtils.h"
#include "HaTable.h"

HaTable::HaTable(const std::string &newName) {

    this->setName(newName);
}

HaTable* HaTable::setName(const std::string& newName)
{
    if(!isResourceNameValid(newName))
        throw std::invalid_argument("Invalid table name "+newName);

    this->tableName = newName;
    return this;
}

std::string HaTable::getName() {
    return this->tableName;
}

bool HaTable::publish(const std::string& rootFs)
{
    // Create table structure
    std::replace(this->ns.begin(), this->ns.end(), '.', '/');
    std::string tableRootDir = rootFs + "/" + this->ns + '/' + this->tableName;
    std::filesystem::create_directories(tableRootDir);

    // Create directory architecture
    createDirectoryLevel(tableRootDir, 1, this->maxDepth);

    return true;
}

void HaTable::createDirectoryLevel(const std::string& parentDir, int currentDepth, int maxDirDepth)
{
    // Create subfolders
    for (int j = 0; j < pow(16, this->bytesPerDepth); j++) {

        std::string i_str = toHex(j);
        if((int)i_str.length() < this->bytesPerDepth)
            i_str.insert(0, this->bytesPerDepth - i_str.length(), '0');

        std::string subDir = parentDir + "/" + i_str;
        std::filesystem::create_directory(subDir);

        if(currentDepth < maxDirDepth)
        {
            createDirectoryLevel(subDir, currentDepth + 1, maxDirDepth);
        }
    }
}

bool HaTable::del() {
    throw NotImplemented();
}

HaTable* HaTable::setNS(const std::string &newNs) {

    std::string pattern = "^[a-zA-Z0-9\\.]*$";
    const std::regex e(pattern);
    if(!std::regex_match(newNs, e))
    {
        throw std::invalid_argument("Invalid table namespace "+newNs);
    }

    this->ns = newNs;
    return this;
}

std::string HaTable::getNS() {

    return this->ns;
}