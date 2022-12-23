//
// Created by vanilor on 23/12/22.
//

#ifndef HATOFI_DB_HATABLE_H
#define HATOFI_DB_HATABLE_H

#include <string>

class HaTable {

public:
    std::string tableName;
    int maxDepth = 2;
    int bytesNumberPerDepth = 1;

    explicit HaTable(const std::string& newName);
    bool publish(const std::string& rootFs);
    void createDirectoryLevel(const std::string& parentDir, int currentDepth);
    bool del();

};

#endif //HATOFI_DB_HATABLE_H
