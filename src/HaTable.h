//
// Created by vanilor on 23/12/22.
//

#ifndef HATOFI_DB_HATABLE_H
#define HATOFI_DB_HATABLE_H

#include <string>

class HaTable {

public:
    int maxDepth = 2;
    int bytesPerDepth = 2;

    explicit HaTable(const std::string& newName);
    std::string getName();
    HaTable* setNS(const std::string& newNs);
    bool publish(const std::string& rootFs);
    bool del();

private:
    void createDirectoryLevel(const std::string& parentDir, int currentDepth, int maxDepth);

private:
    std::string tableName;

    /**
     * Table namespace is a directory where table is located
     * Example: If table name is "data" and namespace is "test", table data will be stored in /test/data
     */
    std::string ns;
};

#endif //HATOFI_DB_HATABLE_H