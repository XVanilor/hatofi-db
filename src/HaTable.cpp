//
// Created by vanilor on 23/12/22.
//

#include <regex>
#include <sys/types.h>
#include <sys/stat.h>
#include <filesystem>
#include <valarray>

#include "HaUtils.h"
#include "HaTable.h"

HaTable::HaTable(const std::string &newName) {

    if(!isResourceNameValid(newName))
        throw std::invalid_argument("Invalid table name HaTable");

    this->tableName = newName;
}


bool HaTable::publish(const std::string& rootFs)
{
    std::string tableRootDir = rootFs + "/" + this->tableName;
    std::filesystem::create_directory(tableRootDir);

    createDirectoryLevel(tableRootDir, 1);

    return true;
}

void HaTable::createDirectoryLevel(const std::string& parentDir, int currentDepth)
{
    // Create subfolder
    for (int j = 0; j < pow(16, this->bytesNumberPerDepth); j++) {

        std::string i_str = toHex(j);
        if(i_str.length() < this->bytesNumberPerDepth)
            i_str.insert(0, this->bytesNumberPerDepth - i_str.length(), '0');
        //printf("i: %s\n", i_str.c_str());

        std::string subDir = parentDir + "/" + i_str;
        std::filesystem::create_directory(subDir);

        if(currentDepth < this->maxDepth)
        {
            createDirectoryLevel(subDir, currentDepth + 1);
        }
    }
}

bool HaTable::del() {
    return false;
}