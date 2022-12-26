//
// Created by vanilor on 23/12/22.
//

#ifndef HATOFI_DB_HAUTILS_H
#define HATOFI_DB_HAUTILS_H

#include <vector>

bool isResourceNameValid(const std::string& resourceName);
bool dirExists(const std::string& path);
bool isHaDB(const std::string& path);
std::string toHex(int number, bool upper_case = false);
std::vector<std::string> tokenize(const std::string& s, const std::string& del = " ");
std::string currentDate();

#endif //HATOFI_DB_HAUTILS_H