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
    enum MATCH_TYPE {
        EXACT,
        PARTIAL
    };

    const size_t MIN_SEARCH_LEN = 7; // Minimum recommended is 7, as it will match at least 4 base64 encoded characters

    explicit HaDB(const std::string& fsRoot);

    HaDB* setName(const std::string& newName);
    void setRoot(const std::string& fsRoot);
    std::string getRoot();

    HaDB* loadConfig(const std::string& configFile);
    void publish();

    HaTable* addTable(HaTable* newTable);
    HaTable* addEmptyTable(const std::string& tableName);
    void delTable(std::string tableName);
    std::vector<HaTable*> getTables();

    void load(const std::string& file, bool force);
    std::string query(const std::string& dataclass, std::string searchString, MATCH_TYPE matchType);

private:
    std::string dbName;
    std::string root;
    std::vector<HaTable*> tables;

};

#endif //HATOFI_DB_HADB_H