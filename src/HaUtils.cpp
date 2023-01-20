//
// Created by vanilor on 23/12/22.
//

#include <openssl/evp.h>
#include <regex>
#include <string>
#include <sys/stat.h>

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

std::string currentDate(const std::string& format)
{
    time_t     now = time(0);
    struct tm  tstruct{};
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), format.c_str(), &tstruct);

    return buf;
}

std::string exec(std::string cmd)
{
    std::array<char, 128> buffer{};
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
    {
        result += buffer.data();
    }
    // Remove EOL of cmd output
    result.erase(std::remove(result.begin(), result.end(), '\n'),result.end());

    return result;
}

bool is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

bool is_valid_uuid(const std::string& s)
{
    static const std::regex e("[a-fA-F0-9]{8}-[a-fA-F0-9]{4}-4[a-fA-F0-9]{3}-[89aAbB][a-fA-F0-9]{3}-[a-fA-F0-9]{12}");
    return regex_match(s, e);
}