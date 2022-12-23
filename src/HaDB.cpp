//
// Created by vanilor on 23/12/22.
//

#include <sys/stat.h>
#include <stdexcept>

#include "HaUtils.h"
#include "HaDB.h"
#include "HaTable.h"


struct stat info;

HaDB::HaDB(const std::string& fsRoot) {

    setRoot(fsRoot);
    // @TODO Load configuration ?
}

HaDB HaDB::setName(const std::string& newName) {

    if(!isResourceNameValid(newName))
        throw std::invalid_argument("Database name must accepts only letters, numbers, and -_ characters");

    this->dbName = newName;
    return *this;
}

void HaDB::setRoot(const std::string &fsRoot) {

    if(!isHaDB(fsRoot))
    {
        throw std::invalid_argument("Data folder is not a Hatofi database");
    }
    this->root = fsRoot;
}

std::string HaDB::getRoot() {

    return this->root;
}

HaDB HaDB::addTable(const std::string& tableName) {

    if(dirExists(this->getRoot() + "/" + tableName))
    {
        throw std::invalid_argument("Table "+tableName+" already exists");
    }

    HaTable newTable = HaTable(tableName);
    newTable.publish(this->getRoot());

    this->tables.push_back(newTable);

    return *this;
}

HaDB HaDB::delTable(std::string tableName) {

    return *this;
}

std::vector<HaTable> HaDB::getTables() {

    return {};
}