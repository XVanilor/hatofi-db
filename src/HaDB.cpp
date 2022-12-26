//
// Created by vanilor on 23/12/22.
//

#include <filesystem>
#include <sys/stat.h>
#include <stdexcept>

#include "HaUtils.h"
#include "HaDB.h"
#include "HaTable.h"

HaDB::HaDB(const std::string& fsRoot) {

    setRoot(fsRoot);
    // @TODO Load configuration ?
}

HaDB* HaDB::setName(const std::string& newName) {

    this->dbName = newName;
    return this;
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

HaTable* HaDB::addTable(const std::string& tableName) {

    if(dirExists(this->getRoot() + "/" + tableName))
    {
        throw std::invalid_argument("Table "+tableName+" already exists");
    }

    HaTable* newTable = new HaTable(tableName);
    this->tables.push_back(newTable);

    return newTable;
}

void HaDB::delTable(std::string tableName) {

    return;
}

std::vector<HaTable*> HaDB::getTables() {

    return {};
}

void HaDB::publish() {

    for(HaTable* t : tables)
    {
        printf("Making %s...\n", t->getName().c_str());
        t->publish(this->getRoot());
    }
}
