//
// Created by Vanilor on 23/12/22.
//

#include "lib/CLI11.hpp"

#include "HaDB.h"
#include "Log.h"

int main(int argc, char** argv) {

    // Application init
    CLI::App app{"App description"};
    std::string data_dir = std::filesystem::current_path().string();
    app.add_option("-d,--data-directory", data_dir, "Database directory")->required();

    /** Database generation **/
    CLI::App* genSub = app.add_subcommand("gen", "Generate Hatofi architecture based on config file");
    std::string config_file;
    genSub->add_option("-c,--config", config_file, "Config file to load")->required();

    /** Data loading **/
    CLI::App* loadSub = app.add_subcommand("load", "Load data into Hatofi database");
    std::string file_loaded;
    loadSub->add_option("-i,--input", file_loaded, "File to be loaded")->required();

    /** Data Query */
    CLI::App* querySub = app.add_subcommand("query", "Query data (exact or partial) from Hatofi DB");
    CLI::App* queryOpt = querySub->add_option_group("queryOpt");

    bool queryPartialMatch{false};
    bool queryExactMatch{true}; // Default is full-exact search because it's faster
    auto partialFlagOpt = queryOpt->add_flag("-p,--partial", queryPartialMatch, "Search for partial match in base64 data (may be slower than exact match)");
    auto exactFlagOpt = queryOpt->add_flag("-e,--exact", queryExactMatch,"Search for exact and full string (performs hash search)");

    partialFlagOpt->excludes(exactFlagOpt);
    exactFlagOpt->excludes(partialFlagOpt);

    std::string dt;
    std::string search_string;
    queryOpt->add_option("dataclass", dt, "You string to look for in database' data");
    queryOpt->add_option("searchString", search_string, "You string to look for in database' data");

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
    printf("Data directory: %s\n", real_data_dir);

    HaDB db = HaDB(real_data_dir);

    if(app.got_subcommand("gen"))
    {
        log_info("Loading configuration...");
        db.fromConfig(config_file);
        log_info("Configuration successfully loaded");
    }
    else if(app.got_subcommand("load"))
    {
        try {

            log_info("Loading data...");
            db.load(file_loaded);
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
        std::string task_uuid;
        if (queryPartialMatch)
            task_uuid = db.query(dt, search_string, HaDB::MATCH_TYPE::PARTIAL);
        else
            task_uuid = db.query(dt, search_string, HaDB::MATCH_TYPE::EXACT);

        printf("%s\n",task_uuid.c_str());
    }

    return 0;
}