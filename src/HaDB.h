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
    void query(const std::string& dataclass, std::string searchString);
    void query_by_hash(const std::string& dataclass, const std::string& searchHash);
    std::filesystem::directory_iterator getDataLinks(const std::string& dataclass, const std::string& md5Hash);
    std::string getLogs(const std::string& dataclass, const std::string& md5Hash);

private:
    std::string dbName;
    std::string root;
    std::vector<HaTable*> tables;

    std::string getDataPath(const std::string& dataclass, const std::string& dataMD5);

};

#endif //HATOFI_DB_HADB_H