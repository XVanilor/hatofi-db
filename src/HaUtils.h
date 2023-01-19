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
std::string exec(std::string cmd);
bool is_number(const std::string& s);
bool is_valid_uuid(const std::string& s);

#endif //HATOFI_DB_HAUTILS_H