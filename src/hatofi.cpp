//
// Created by Vanilor on 23/12/22.
//

#include <cstdlib>
#include <climits>
#include "lib/CLI11.hpp"

#include "HaDB.h"
#include "HaUtils.h"
#include "Log.h"

int main(int argc, char** argv) {

    const std::string BUILD_VERSION = "2.3.3";

    /** Application init **/
    CLI::App app{"HatofiDB - Hashmap To Filesystem Database\n    > Created by Matthieu \"Vanilor\" Herbette\n    > Find the repo at https://github.com/XVanilor/hatofi-db/\n"};
    std::string data_dir = std::filesystem::current_path().string();
    bool show_version = false;

    CLI::App* startOpt = app.add_option_group("init");
    auto nominalStartOpt = startOpt->add_option("-d,--data-directory", data_dir, "Database directory");
    auto showVersionOpt = startOpt->add_flag("-v,--version", show_version, "Print binary version");

    showVersionOpt->excludes(nominalStartOpt);
    nominalStartOpt->excludes(showVersionOpt);

    /** Database generation **/
    CLI::App* genSub = app.add_subcommand("gen", "Generate Hatofi architecture based on config file");
    std::string config_file;
    genSub->add_option("-c,--config", config_file, "Config file to load")->required();

    /** Data loading **/
    CLI::App* loadSub = app.add_subcommand("load", "Load data into Hatofi database");

    std::string file_loaded;
    bool force_load = false;
    loadSub->add_flag("-f,--force", force_load, "Force file to be loaded in case of conflict");

    CLI::App* loadSubSource = loadSub->add_option_group("loadSubSource");
    loadSubSource->add_option("-i,--input", file_loaded, "File to be loaded");

    std::string loadSubSourceRawDt;
    std::string loadSubSourceRawData;
    CLI::App* loadSubSourceRaw = loadSubSource->add_option_group("loadSubSourceRaw");
    loadSubSourceRaw->add_option("-D,--dataclass", loadSubSourceRawDt, "Dataclass of new entry")->required();
    loadSubSourceRaw->add_option("-T,--text", loadSubSourceRawData, "Raw data to insert")->required();

    // Only accepts options from loadSubSource or loadSubSourceRaw
    loadSubSource->required()->require_option(1);

    /** Data Query */
    CLI::App* querySub = app.add_subcommand("query", "Query data from Hatofi DB");
    std::string dt;
    std::string search_plaintext;
    std::string search_md5;
    std::string md5Hash;

    CLI::App* querySearchApp = querySub->add_subcommand("search", "Search if a given data exists in database");
    querySearchApp->add_option("-D,--dataclass", dt, "Dataclass to search in")->required();

    // Add an option to search by hash OR by plain text value, exclusively
    auto* querySearchTypeGroup = querySearchApp->add_option_group("searchByWhat");
    querySearchTypeGroup->add_option("-H,--hash", search_md5, "Search for a data using it's MD5 hash");
    querySearchTypeGroup->add_option("-T,--text", search_plaintext, "Search for a data using it's plain text value");
    querySearchTypeGroup->require_option(1); // Require one and only one of those arguments

    CLI::App* queryLinksApp = querySub->add_subcommand("links", "Get linked data hashes");
    queryLinksApp->add_option("-D,--dataclass", dt, "Dataclass to search in")->required();
    queryLinksApp->add_option("-H,--hash", md5Hash, "Data to get links from")->required();

    CLI::App* queryLogsApp = querySub->add_subcommand("logs", "Get data import logs");
    queryLogsApp->add_option("-D,--dataclass", dt, "Dataclass to search in")->required();
    queryLogsApp->add_option("-H,--hash", md5Hash, "Data to get logs from")->required();


    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        return app.exit(e);
    }

    char* real_data_dir = realpath(data_dir.c_str(), nullptr);
    if(real_data_dir == nullptr)
    {
        log_error("Data directory "+data_dir+" is invalid or does not exists");
        return 1;
    }
    data_dir.clear();
    if(!show_version)
        printf("Data directory: %s\n", real_data_dir);

    HaDB* db = new HaDB(real_data_dir);
    // Config was already generated in this directory and is an active HatofiDB instance
    if(std::filesystem::exists(std::string(real_data_dir) + "/.dbmeta"))
        db->loadConfig(std::string(real_data_dir) + "/.dbmeta");

    if(show_version)
    {
        printf("HatofiDB %s\n", BUILD_VERSION.c_str());
        printf("    > Created by Matthieu \"Vanilor\" Herbette\n");
        printf("    > Find the public repo at https://github.com/XVanilor/hatofi-db\n");
        exit(0);
    }
    else if(app.got_subcommand("gen"))
    {
        log_info("Loading configuration...");
        db = new HaDB(real_data_dir);
        db->loadConfig(config_file);
        db->publish();
        log_info("Configuration successfully loaded");
    }
    else if(app.got_subcommand("load"))
    {
        try {

            log_info("Loading data...");
            if(loadSubSourceRawDt.empty() && !file_loaded.empty())
            {
                db->loadFromFile(file_loaded, force_load);
            }
            else
            {
                db->loadRaw(loadSubSourceRawDt, loadSubSourceRawData);
            }
            log_info("Data loaded successfully");

        } catch (const std::invalid_argument& e)
        {
            log_error(e.what());
            exit(1);
        }
        catch (const std::runtime_error& e)
        {
            log_error(e.what());
            exit(2);
        }
    }

    else if(app.got_subcommand("query"))
    {
        if(querySub->got_subcommand("links"))
        {
            for (const auto & entry : db->getDataLinks(dt, md5Hash))
            {
                char real_link_path[PATH_MAX];

                // We will try to extract dataclass from real file path as it's 3 times parent older in fs hierarchy
                realpath(entry.path().c_str(), real_link_path);
                std::filesystem::path p = real_link_path;
                std::string dt_name = p.parent_path().parent_path().parent_path().filename().string();

                // Get data base64 value
                std::string b64_value = "::";
                std::string real_link_path_str = real_link_path;
                std::ifstream input(real_link_path_str + "/" + entry.path().filename().string() + ".md5");
                for( std::string line; getline( input, line ); )
                {
                    if(line.find("b64:") != std::string::npos)
                        b64_value = line + ":";
                }
                std::cout << dt_name + ":" + b64_value + entry.path().filename().string() << std::endl;
            }
            return 0;
        }
        else if (querySub->got_subcommand("logs"))
        {
            std::cout << db->getLogs(dt, md5Hash) << std::endl;
        }
        // Default is plain text query search
        else
        {
            // Query search has been performed by hash
            if(!search_md5.empty())
            {
                db->query_by_hash(dt, search_md5);
            }
            else
            {
                db->query(dt, search_plaintext);
            }
        }
    }
    else
    {
        log_error("Please enter a command or use -h,--help");
    }

    return 0;
}