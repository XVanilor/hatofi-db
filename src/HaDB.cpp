//
// Created by vanilor on 23/12/22.
//

#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <fmt/core.h>

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

    HaTable* newTable = new HaTable(tableName);
    this->tables.push_back(newTable);

    return newTable;
}

void HaDB::delTable(std::string tableName) {

    return;
}

std::vector<HaTable*> HaDB::getTables() {

    return this->tables;
}

void HaDB::publish() {

    for(HaTable* t : tables)
    {
        printf("Making %s...\n", t->getName().c_str());

        if(dirExists(this->getRoot() + "/" + t->getName()))
        {
            throw std::invalid_argument("Table "+t->getName()+" already exists");
        }

        t->publish(this->getRoot());
    }
}

void HaDB::load(const std::string& file)
{
    // Get current date
    const std::string curDate = currentDate();
    std::string leakUuid;

    // Start i/o operations
    std::ios::sync_with_stdio(false);
    std::ifstream ifs(file);

    std::string line;

    int i = 0;
    while(std::getline(ifs, line))
    {
        // Get leak UUID
        if(i++ == 0)
        {
            leakUuid = line;
            continue;
        }

        std::ofstream dataOutFile;
        std::ofstream entityOutFile;

        // Data splitting
        std::vector<std::string> columns = tokenize(line, ":");

        std::string dataOutFileName = fmt::format("{}/dataclass/{}/{}/{}/{}.md5", this->getRoot(), columns[1], columns[3].substr(0,2), columns[3].substr(2,2),columns[3]);
        dataOutFile.open(dataOutFileName);
        dataOutFile << fmt::format("b64:{}\n", columns[2]);
        dataOutFile.close();

        std::string entityOutFileName = fmt::format("{}/entity/{}/{}/{}.md5", this->getRoot(), columns[0].substr(0,2), columns[0].substr(2,2),columns[0]);
        entityOutFile.open(entityOutFileName, std::ios_base::app); // append instead of overwrite
        entityOutFile << fmt::format("{} {} {}\n", columns[3], curDate, leakUuid);
        entityOutFile.close();
    }
}