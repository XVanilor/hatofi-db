//
// Created by vanilor on 23/12/22.
//

#ifndef HATOFI_DB_HADB_H
#define HATOFI_DB_HADB_H

#include <string>
#include <vector>

#include "HaTable.h"

class HaDB {

public:

    explicit HaDB(const std::string& fsRoot);

    HaDB setName(const std::string& newName);
    void setRoot(const std::string& fsRoot);
    std::string getRoot();

    HaDB addTable(const std::string& tableName);
    HaDB delTable(std::string tableName);
    std::vector<HaTable> getTables();

private:
    std::string dbName;
    std::string root;
    std::vector<HaTable> tables;

};


#endif //HATOFI_DB_HADB_H