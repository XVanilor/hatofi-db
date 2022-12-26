//
// Created by vanilor on 23/12/22.
//

#include <string>
#include <regex>
#include <sys/stat.h>
#include <iomanip>

bool isResourceNameValid(const std::string& resourceName)
{
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
bool dirExists(const std::string& path)
{
    struct stat info{};

    int statRC = stat(path.c_str(), &info );
    if( statRC != 0 )
    {
        return false;
    }

    return (info.st_mode & S_IFDIR) != 0;
}

bool isHaDB(const std::string& path)
{
    if(!dirExists(path))
        return false;
    // TODO
    return true;
}

std::string toHex(int number, bool upper_case = false)
{
    std::stringstream ss;
    ss << std::hex << number; // int decimal_value
    std::string hexRes = ss.str();

    if(upper_case)
        std::transform(hexRes.begin(), hexRes.end(),hexRes.begin(), ::toupper);

    return hexRes;
}

std::vector<std::string> tokenize(const std::string& s, const std::string& del = " ")
{
    std::vector<std::string> res;

    int start, end = -1*del.size();
    do {
        start = end + del.size();
        end = s.find(del, start);
        res.push_back(s.substr(start, end - start));
    } while (end != -1);

    return res;
}

std::string currentDate()
{
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d", &tstruct);

    return buf;
}