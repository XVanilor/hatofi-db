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
    const std::string LOG_DATE_FORMAT = "%Y-%m-%d";

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

    void loadFromFile(const std::string& file, bool force);
    void loadRaw(const std::string& dataclass, const std::string& value);
    void log(const std::string& dataclass, const std::string& value, const std::string& sourceUUID, std::string curDate);
    void query(const std::string& dataclass, std::string searchString);
    void query_by_hash(const std::string& dataclass, const std::string& searchHash);
    std::filesystem::directory_iterator getDataLinks(const std::string& dataclass, const std::string& md5Hash);
    std::string getLogs(const std::string& dataclass, const std::string& md5Hash);
    bool updateEntry(const std::string& dataclass, const std::string& md5Hash, const std::string& newDataclass, const std::string& newValue);

private:
    std::string dbName;
    std::string root;
    std::vector<HaTable*> tables;

    std::string getDataPath(const std::string& dataclass, const std::string& dataMD5);
    bool createEntry(const std::string& dataclass, const std::string& dataBase64, std::string dataMD5);

};

#endif //HATOFI_DB_HADB_H