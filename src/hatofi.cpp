//
// Created by Vanilor on 23/12/22.
//

#include "lib/CLI11.hpp"

#include "HaDB.h"
#include "HaUtils.h"
#include "Log.h"

int main(int argc, char** argv) {

    const std::string BUILD_VERSION = "1.1.1";

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
    bool force_file_load = false;
    loadSub->add_option("-i,--input", file_loaded, "File to be loaded")->required();
    loadSub->add_flag("-f,--force", force_file_load, "Force file to be loaded in case of conflict");

    /** Data Query */
    CLI::App* querySub = app.add_subcommand("query", "Query data (exact or partial) from Hatofi DB");
    CLI::App* queryOpt = querySub->add_option_group("search_type");

    bool queryPartialMatch{false};
    bool queryExactMatch{true}; // Default is full-exact search because it's faster
    auto partialFlagOpt = queryOpt->add_flag("-p,--partial", queryPartialMatch, "Search for partial match in base64 data (may be slower than exact match)");
    auto exactFlagOpt = queryOpt->add_flag("-e,--exact", queryExactMatch,"Search for exact and full string (performs hash search)");

    partialFlagOpt->excludes(exactFlagOpt);
    exactFlagOpt->excludes(partialFlagOpt);

    std::string dt;
    std::string search_string;
    queryOpt->add_option("dataclass", dt, "You string to look for in database' data")->required();
    queryOpt->add_option("searchString", search_string, "You string to look for in database' data")->required();

    /** Get heuristics for data optimal repartition based on string length on each line **/
    CLI::App* strLengthRepartSub = app.add_subcommand("analyze", "Get quartiles required for data organization optimization based on string length");
    std::string sample_file_path;
    strLengthRepartSub->add_option("-i,--input", sample_file_path, "Sample file to laod. Each sample data MUST be on a single line")->required();

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

    HaDB db = HaDB(real_data_dir);

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
        db.fromConfig(config_file);
        log_info("Configuration successfully loaded");
    }
    else if(app.got_subcommand("load"))
    {
        try {

            log_info("Loading data...");
            db.load(file_loaded, force_file_load);
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
    else if(app.got_subcommand("analyze"))
    {
        StringRepartitionQuartiles* quartiles = get_string_optimal_repartition_quartile(sample_file_path);
        printf("%f\n%f\n%f\n", quartiles->q1, quartiles->med, quartiles->q3);
    }
    else
    {
        log_error("Please enter a command or use -h,--help");
    }

    return 0;
}