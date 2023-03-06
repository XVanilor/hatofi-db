//
// Created by vanilor on 23/12/22.
//

#ifndef HATOFI_DB_HAUTILS_H
#define HATOFI_DB_HAUTILS_H

#include <vector>
#include <map>

class StringRepartitionQuartiles {

public:
    StringRepartitionQuartiles(const unsigned int& q1, const unsigned int& q2, const unsigned int& q3);

    unsigned int q1{};
    unsigned int q2{};
    unsigned int q3{};
};

/**
 * @see https://stackoverflow.com/a/36851059
 */
class NotImplemented : public std::logic_error
{
public:
    NotImplemented() : std::logic_error("Function not yet implemented") { };
};

bool isResourceNameValid(const std::string& resourceName);
bool dirExists(const std::string& path);
bool isHaDB(const std::string& path);
std::string toHex(int number, bool upper_case = false);
std::vector<std::string> tokenize(const std::string& s, const std::string& del = " ");
std::string currentDate(const std::string& format);
std::string exec(const std::string& cmd);
bool is_number(const std::string& s);
bool is_valid_uuid(const std::string& s);

#endif //HATOFI_DB_HAUTILS_H