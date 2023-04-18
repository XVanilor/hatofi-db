//
// Created by vanilor on 23/12/22.
//

#include <filesystem>
#include <fmt/core.h>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <utility>
#include "lib/crypto/md5.h"
#include "lib/uuid_v4.h"

#include "HaUtils.h"
#include "HaDB.h"
#include "HaTable.h"
#include "Log.h"

HaDB::HaDB(const std::string &fsRoot) {

    setRoot(fsRoot);
}

HaDB *HaDB::setName(const std::string &newName) {

    this->dbName = newName;
    return this;
}

void HaDB::setRoot(const std::string &fsRoot) {

    if (!isHaDB(fsRoot)) {
        throw std::invalid_argument("Data folder is not a Hatofi database");
    }
    this->root = fsRoot;
}

std::string HaDB::getRoot() {

    return this->root;
}

std::string HaDB::getDataPath(const std::string &dataclass, const std::string& dataMD5) {

    // Performs exact and full match search using MD5 hash
    // Get first 2 and 4th bytes of MD5 hash
    std::string root1 = dataMD5.substr(0, 2);
    std::string root2 = dataMD5.substr(2, 2);

    // File path
    return this->getRoot() + "/dataclass/" + dataclass + "/" + root1 + "/" + root2 + "/" + dataMD5;
}

HaTable *HaDB::addTable(HaTable *newTable) {
    this->tables.push_back(newTable);
    return newTable;
}

HaTable *HaDB::addEmptyTable(const std::string &tableName) {

    auto *newTable = new HaTable(tableName);
    this->tables.push_back(newTable);

    return newTable;
}

void HaDB::delTable(std::string tableName) {

    return;
}

std::vector<HaTable *> HaDB::getTables() {

    return this->tables;
}

HaDB *HaDB::loadConfig(const std::string &configFile) {
    std::ifstream ifs(configFile);

    std::string currentSectionName;
    auto *currentTable = new HaTable("default");

    std::string line;
    int i = 0;
    int tablesLoaded = 0;

    while (std::getline(ifs, line)) {
        i++;

        // Skip empty or commented lines
        if (line.empty() || line.starts_with("#"))
            continue;

            // Process YAML section name lines
        else if (line.starts_with("[") && line.ends_with("]")) {
            line.erase(0, 1);
            line.erase(line.length() - 1, 1);
            currentSectionName = line;

            if (currentSectionName == "table") {
                if (tablesLoaded == 0) {
                    // Skip first sample table
                    tablesLoaded++;
                    continue;
                }

                this->addTable(currentTable);
                currentTable = new HaTable("default");
                continue;
            }
            continue;
        }
            // Process key-value lines
        else if (line.find('=') != std::string::npos) // Check if line is formatted as key=value (contains an =)
        {
            std::string key = line.substr(0, line.find('='));
            std::string value = line.substr(line.find('=') + 1, line.length() - line.find('='));

            // HatofiDB config
            if (currentSectionName == "hatofi") {
                if (key == "name") {
                    this->setName(value);
                    continue;
                } else {
                    throw std::invalid_argument(
                            "Invalid config file at line " + std::to_string(i) + ": Unknown config key " +
                            currentSectionName);
                }
            }

                // Table config
            else if (currentSectionName == "table") {
                if (key == "name") {
                    currentTable->setName(value);
                } else if (key == "ns") {
                    currentTable->setNS(value);
                } else if (key == "maxDepth") {
                    if (!is_number(value)) {
                        throw std::invalid_argument(
                                "Invalid config file at line " + std::to_string(i) + ": maxDepth must be an integer");
                    }
                    currentTable->maxDepth = std::stoi(value);
                } else if (key == "bytesPerDepth") {
                    if (!is_number(value)) {
                        throw std::invalid_argument("Invalid config file at line " + std::to_string(i) +
                                                    ": bytesPerDepth must be an integer");
                    }
                    currentTable->maxDepth = std::stoi(value);
                } else if (key == "q1") {
                    if (!is_number(value)) {
                        throw std::invalid_argument(
                                "Invalid config file at line " + std::to_string(i) + ": q1 must be an integer");
                    }
                    double q1 = std::stoi(value);
                    if (currentTable->quartiles == nullptr)
                        currentTable->quartiles = new StringRepartitionQuartiles(q1, q1, q1 + 1);
                    currentTable->quartiles->q1 = q1;
                } else if (key == "q2") {
                    if (!is_number(value)) {
                        throw std::invalid_argument(
                                "Invalid config file at line " + std::to_string(i) + ": q2 must be an integer");
                    }
                    double q2 = std::stoi(value);
                    if (currentTable->quartiles == nullptr)
                        currentTable->quartiles = new StringRepartitionQuartiles(q2, q2, q2 + 1);
                    currentTable->quartiles->q2 = q2;
                } else if (key == "q3") {
                    if (!is_number(value)) {
                        throw std::invalid_argument(
                                "Invalid config file at line " + std::to_string(i) + ": q3 must be an integer");
                    }
                    double q3 = std::stoi(value);
                    if (currentTable->quartiles == nullptr)
                        currentTable->quartiles = new StringRepartitionQuartiles(q3 - 1, q3 - 1, q3);
                    currentTable->quartiles->q3 = q3;
                }
            }
                // Unknown. Throws error
            else {
                throw std::invalid_argument(
                        "Invalid config file at line " + std::to_string(i) + ": Unknown section " + currentSectionName);
            }
        } else {
            throw std::invalid_argument("Invalid config file at line " + std::to_string(i) + ": Invalid line");
        }
    }

    // Load last table
    if (tablesLoaded > 0) {
        this->addTable(currentTable);
    }

    ifs.close();

    return this;
}

void HaDB::publish() {

    // Create config file using YAML
    std::ofstream configFile;
    configFile.open(this->getRoot() + "/.dbmeta");
    configFile << "[hatofi]\n";
    configFile << "name=" + this->dbName + "\n";

    // Create tables
    for (HaTable *t: this->tables) {
        if (dirExists(this->getRoot() + "/" + t->getFullName())) {
            log_warn("Table " + t->getName() + " already exists and will not be overwrite");
            continue;
        }

        log_info("Making " + t->getName() + "...");

        configFile << "[table]\n";
        configFile << "ns=" + t->getNS() + "\n";
        configFile << "name=" + t->getName() + "\n";
        configFile << "maxDepth=" + std::to_string(t->maxDepth) + "\n";
        configFile << "bytesPerDepth=" + std::to_string(t->bytesPerDepth) + "\n";
        if (t->quartiles != nullptr) {
            configFile << "q1=" + std::to_string(t->quartiles->q1) + "\n";
            configFile << "q2=" + std::to_string(t->quartiles->q2) + "\n";
            configFile << "q3=" + std::to_string(t->quartiles->q3) + "\n";
        }

        // Load table into HaDB
        if (t->maxDepth == 2)
            log_info(t->getName() + " table maxDepth: 2 (default)");
        if (t->bytesPerDepth == 2)
            log_info(t->getName() + " table bytesPerDepth: 2 (default)");

        t->publish(this->getRoot());
    }

    configFile.close();
}

void HaDB::load(const std::string &file, bool force = false) {
    // Get current date
    const std::string curDate = currentDate("%Y-%m-%d");
    std::string leakUuid;

    if (!std::filesystem::exists(file)) {
        throw std::invalid_argument("Input file " + file + " does not exists");
    }

    // Start i/o operations
    std::ios::sync_with_stdio(false);
    std::ifstream ifs(file);

    std::string line;

    // Get leak UUID in first line and check it\'s value
    std::getline(ifs, leakUuid);
    if (!is_valid_uuid(leakUuid)) {
        log_error("Leak UUID '" + leakUuid + "' in file " + file + " is invalid");
        exit(1);
    }

    // Register file hash & confirm startup
    std::ofstream fileOut;
    std::string inputFileMd5 = exec("md5sum '" + file + "' | cut -d' ' -f1");
    std::string inputFileSha256 = exec("sha256sum '" + file + "' | cut -d' ' -f1");
    std::string fileRegisterFileName = fmt::format("{}/file/{}/{}/{}.sha256",
                                                   this->getRoot(),
                                                   inputFileSha256.substr(0, 2),
                                                   inputFileSha256.substr(2, 2),
                                                   inputFileSha256
    );

    // Check if file was already registered
    if (std::filesystem::exists(fileRegisterFileName)) {
        if (force) {
            // Remove old log file if user asked to force importation
            remove(fileRegisterFileName.c_str());
        }
            // Check if file was fully imported
        else if (exec("grep '^status done$' '" + fileRegisterFileName + "'") == "status done") {
            throw std::invalid_argument("File was already registered in database");
        }
            /**
             *  File was registered but not completely imported
             *  It can be either an error in runtime (hatofi load process was aborted using kill)
             *  or that an another process with the same file is running
             */
        else {
            throw std::runtime_error(
                    "File was registered in database but import was not completed. Use -f or --force to force import (Warning: data may be loaded twice if another import process with this input file is running");
        }
    }

    fileOut.open(fileRegisterFileName, std::ios_base::app); // append instead of overwrite
    fileOut << fmt::format("md5 {}\nsha256 {}\ncreated_at {}\nstatus started\n",
                           inputFileMd5,
                           inputFileSha256,
                           currentDate("%Y-%m-%dT%H:%M:%S")
    );
    fileOut.close();

    while (std::getline(ifs, line)) {
        std::ofstream dataOutFile;
        std::ofstream logFile;

        // Data splitting
        std::vector<std::string> columns = tokenize(line, ":");
        std::string keyDt = columns[0];
        std::string keyMD5 = columns[1];
        std::string dt = columns[2];
        std::string data_base64 = columns[3];
        std::string dataMD5 = columns[4];

        if (!std::filesystem::exists(fmt::format("{}/dataclass/{}", this->getRoot(), dt))) {
            log_warn("Dataclass `" + dt + "` does not exist");
            continue;
        }

        std::string entryAbsLoc = fmt::format("{}/dataclass/{}/{}/{}/{}",
                                              this->getRoot(),
                                              dt,
                                              dataMD5.substr(0, 2),
                                              dataMD5.substr(2, 2),
                                              dataMD5
        );
        std::string keyAbsLoc = fmt::format("{}/dataclass/{}/{}/{}/{}",
                                            this->getRoot(),
                                            keyDt,
                                            keyMD5.substr(0, 2),
                                            keyMD5.substr(2, 2),
                                            keyMD5
        );
        // Create Entry folder architecture if it does not exist yet
        if (!std::filesystem::exists(entryAbsLoc)) {
            std::filesystem::create_directory(entryAbsLoc);

            // Put data definition into folder if not created yet
            std::string dataOutFileName = fmt::format("{}/{}.md5", entryAbsLoc, dataMD5);
            dataOutFile.open(dataOutFileName);
            dataOutFile << fmt::format("b64:{}\nmd5:{}\n", data_base64, dataMD5);
            dataOutFile.close();

            // Create links/ and logs/ subdirectory
            std::filesystem::create_directory(entryAbsLoc + "/links");
            std::filesystem::create_directory(entryAbsLoc + "/logs");
        }

        // Register entry log to acknowledge to we saw it
        // @TODO Handle log rotation
        std::string logFileName = fmt::format("{}/logs/import.log", entryAbsLoc);
        logFile.open(logFileName);
        logFile << dataMD5 + " " + curDate + " " + leakUuid + "\n";
        logFile.close();

        // Create bidirectional symlink only if it does not point to current data
        if (keyMD5 == dataMD5)
            continue;

        std::string keyRelativeLocation = fmt::format("../../../../../{}/{}/{}/{}",
                                                      keyDt,
                                                      keyMD5.substr(0, 2),
                                                      keyMD5.substr(2, 2),
                                                      keyMD5
        );
        std::string dataRelativeLocation = fmt::format("../../../../../{}/{}/{}/{}",
                                                       dt,
                                                       dataMD5.substr(0, 2),
                                                       dataMD5.substr(2, 2),
                                                       dataMD5
        );
        try {
            // Create key to entry symlink
            std::filesystem::create_directory_symlink(dataRelativeLocation, keyAbsLoc + "/links/" + dataMD5);
        } catch (std::filesystem::filesystem_error &error) {
            // Warn only if error is not "File exists"
            if (error.code().message().find("File exists") == std::string::npos)
                log_warn(error.what());
            // This will happen mostly in cases where symlink already exists. If not, you have another serious problem with your fs
        }
        try {
            // Create entry to key symlink
            std::filesystem::create_directory_symlink(keyRelativeLocation, entryAbsLoc + "/links/" + keyMD5);
        } catch (std::filesystem::filesystem_error &error) {
            // This will happen mostly in cases where symlink already exists. If not, you have another serious problem with your fs
            if (error.code().message().find("File exists") == std::string::npos)
                log_warn(error.what());
        }

    }

    // Confirm file loading completeness
    fileOut.open(fileRegisterFileName, std::ios_base::app); // append instead of overwrite
    fileOut << "status done\n";
    fileOut.close();
}

void HaDB::query(const std::string &dataclass, std::string searchString)
{
    std::string searchHash = md5(std::move(searchString));
    return this->query_by_hash(dataclass, searchHash);
}

void HaDB::query_by_hash(const std::string& dataclass, const std::string& searchHash)
{
    // File path
    std::string fPath = this->getDataPath(dataclass, searchHash) + "/" + searchHash + ".md5";

    if (!std::filesystem::exists(fPath)) {
        std::cerr << "Data does not exits" << std::endl;
        return;
    }

    std::string cmd0 = "cat '" + fPath + "'";
    system(cmd0.c_str());
}

std::filesystem::directory_iterator HaDB::getDataLinks(const std::string& dataclass, const std::string& md5Hash) {

    // Data path
    std::string dataPath = getDataPath(dataclass, md5Hash);

    if (!std::filesystem::exists(dataPath)) {
        std::cerr << "Data does not exits" << std::endl;
        return {};
    }

    return std::filesystem::directory_iterator(dataPath + "/links");
}

std::string HaDB::getLogs(const std::string& dataclass, const std::string& md5Hash) {

    std::string root1 = md5Hash.substr(0, 2);
    std::string root2 = md5Hash.substr(2, 2);
    // Data path
    std::string logPath = this->getRoot() + "/dataclass/" + dataclass + "/" + root1 + "/" + root2 + "/" + md5Hash;

    if (!std::filesystem::exists(logPath)) {
        std::cerr << "Data does not exits" << std::endl;
        return {};
    }

    return get_file_content(logPath + "/logs/import.log");
}