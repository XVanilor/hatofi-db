//
// Created by vanilor on 23/12/22.
//

#include <map>
#include <regex>
#include <string>
#include <sys/stat.h>
#include <cmath>
#include <fstream>
#include <iostream>

#include "HaUtils.h"

StringRepartitionQuartiles::StringRepartitionQuartiles(const unsigned int& q1, const unsigned int& q2, const unsigned int& q3)
{
    this->q1 = q1;
    this->q2 = q2;
    this->q3 = q3;
}

bool isResourceNameValid(const std::string &resourceName) {
    std::string pattern = "^[a-zA-Z0-9_-]{1,255}$";
    const std::regex e(pattern);
    return std::regex_match(resourceName, e);
}

/******************************************************************************
 * Checks to see if a directory exists. Note: This method only checks the
 * existence of the full path AND if path leaf is a dir.
 *
 * @return  >0 if dir exists AND is a dir,
 *           0 if dir does not exist OR exists but not a dir,
 *          <0 if an error occurred (errno is also set)
 * @see     https://stackoverflow.com/a/52043954
 *****************************************************************************/
bool dirExists(const std::string &path) {
    struct stat info{};

    int statRC = stat(path.c_str(), &info);
    if (statRC != 0) {
        return false;
    }

    return (info.st_mode & S_IFDIR) != 0;
}

bool isHaDB(const std::string &path) {
    if (!dirExists(path))
        return false;
    // TODO
    return true;
}

std::string toHex(int number, bool upper_case) {
    std::stringstream ss;
    ss << std::hex << number; // int decimal_value
    std::string hexRes = ss.str();

    if (upper_case)
        std::transform(hexRes.begin(), hexRes.end(), hexRes.begin(), ::toupper);

    return hexRes;
}

std::vector<std::string> tokenize(const std::string &s, const std::string &del) {
    std::vector<std::string> res;

    int start, end = -1 * del.size();
    do {
        start = end + del.size();
        end = s.find(del, start);
        res.push_back(s.substr(start, end - start));
    } while (end != -1);

    return res;
}

std::string currentDate(const std::string &format) {
    time_t now = time(0);
    struct tm tstruct{};
    char buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), format.c_str(), &tstruct);

    return buf;
}

std::string exec(const std::string &cmd) {
    std::array<char, 128> buffer{};
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    // Remove EOL of cmd output
    result.erase(std::remove(result.begin(), result.end(), '\n'), result.end());

    return result;
}

bool is_number(const std::string &s) {
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

bool is_valid_uuid(const std::string &s) {
    static const std::regex e("[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}");
    return regex_match(s, e);
}

unsigned int getRawDataLengthFromBase64(std::string base64_in)
{
    return (3 * base64_in.size() / 4) - std::count(base64_in.begin(), base64_in.end(), '=');
}

std::string getStringDistribFolderNameForLen(unsigned int len, StringRepartitionQuartiles* ref)
{
    if(len <= ref->q1)
        return "_lower_or_equal_than_"+std::to_string(ref->q1);
    else if(len > ref->q1 && len <= ref->q2)
        return "_lower_or_equal_than_"+std::to_string(ref->q2);
    else if(len > ref->q2 && len <= ref->q3)
        return "_lower_or_equal_than_"+std::to_string(ref->q3);
    else
        // rawStrLen is > q3
        return "_more_than_"+std::to_string(ref->q3);
}

template<typename T>
static inline double Lerp(T v0, T v1, T t)
{
    return (1 - t)*v0 + t*v1;
}


/**
 * @see https://stackoverflow.com/questions/11964552/finding-quartiles
 * @usage   std::vector<double> in = { 1,2,3,4,5,6,7,8,9,10,11 };
 *          auto quartiles = Quantile<double>(in, { 0.25, 0.5, 0.75 });
 * @tparam T
 * @param inData
 * @param probs
 * @return
 */
template<typename T>
static inline std::vector<T> Quantile(const std::vector<T> &inData, const std::vector<T> &probs)
{
    if (inData.empty()) {
        return std::vector<T>();
    }

    if (1 == inData.size()) {
        return std::vector<T>(1, inData[0]);
    }

    std::vector<T> data = inData;
    std::sort(data.begin(), data.end());
    std::vector<T> quantiles;

    for (size_t i = 0; i < probs.size(); ++i) {
        T poi = Lerp<T>(-0.5, data.size() - 0.5, probs[i]);

        size_t left = std::max(int64_t(std::floor(poi)), int64_t(0));
        size_t right = std::min(int64_t(std::ceil(poi)), int64_t(data.size() - 1));

        T datLeft = data.at(left);
        T datRight = data.at(right);

        T quantile = Lerp<T>(datLeft, datRight, poi - left);

        quantiles.push_back(quantile);
    }

    return quantiles;
}

std::vector<std::string> get_sample_file_content(const std::string& file_path)
{
    std::vector<std::string> samples;
    std::ifstream in(file_path.c_str());
    if(!in)
        throw std::invalid_argument("[ERROR] Cannot open nor read sample file");

    std::string str;
    while (std::getline(in, str))
    {
        if(str.size() > 0)
            samples.push_back(str);
    }
    in.close();
    return samples;
}

StringRepartitionQuartiles* get_string_optimal_repartition_quartile(const std::string& file_path)
{

    std::vector<std::string> samples = get_sample_file_content(file_path);

    std::vector<double> string_lengths;
    for(const auto& str: samples)
        string_lengths.push_back(str.size());

    sort(string_lengths.begin(), string_lengths.end());
    auto quartiles = Quantile<double>(string_lengths, { 0.25, 0.5, 0.75 });
    return new StringRepartitionQuartiles(int(quartiles[0]), int(quartiles[1]), int(quartiles[2]));
}
