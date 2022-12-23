//
// Created by vanilor on 23/12/22.
//

#ifndef HATOFI_DB_HAUTILS_H
#define HATOFI_DB_HAUTILS_H


bool isResourceNameValid(const std::string& resourceName);
bool dirExists(const std::string& path);
bool isHaDB(const std::string& path);
std::string toHex(int number, bool upper_case = false);

#endif //HATOFI_DB_HAUTILS_H