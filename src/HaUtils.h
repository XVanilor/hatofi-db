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
unsigned int getRawDataLengthFromBase64(std::string base64_in);
std::string getStringDistribFolderNameForLen(unsigned int len, StringRepartitionQuartiles* ref);
template<typename T>
static inline double Lerp(T v0, T v1, T t);
template<typename T>
static inline std::vector<T> Quantile(const std::vector<T> &inData, const std::vector<T> &probs);
std::vector<std::string> get_sample_file_content(const std::string& file_path);

/**
 * Returns a map of Q1,med,Q3 based on sample strings lengths repartition to optimize data classification based on their
 * length
 *
 * @params std::vector<std::string> sample
 * @return std::map<std::string, int> as {"q1": x, "med": y, "q3": z} of Q1, mediane and Q3
 */
StringRepartitionQuartiles* get_string_optimal_repartition_quartile(const std::string& file_path);

#endif //HATOFI_DB_HAUTILS_H