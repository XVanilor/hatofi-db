//
// Created by Vanilor on 23/12/22.
//

#include "lib/CLI11.hpp"
#include "HaDB.h"
#include "hatofi.h"

int main(int argc, char** argv) {

    // Application init
    CLI::App app{"App description"};

    /** Database generation **/
    CLI::App* genSub = app.add_subcommand("gen", "Generate Hatofi architecture based on config file");
    std::string data_dir = std::filesystem::current_path().string();
    genSub->add_option("-d,--data-directory", data_dir, "Database directory");

    /** Data loading **/
    CLI::App* loadSub = app.add_subcommand("load", "Loading data into Hatofi database");
    std::string file_loaded;
    loadSub->add_option("-f,--file", file_loaded, "File to be loaded");

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        return app.exit(e);
    }

    printf("Data directory: %s\n", data_dir.c_str());

    HaDB db = HaDB(data_dir);
    db.setName("dev");

    // @TODO Create template of default table with generated structure then copy into many tables instead of generating for all
    // Dataclass table
    db.addTable("email")->setNS("dataclass");
    db.addTable("password")->setNS("dataclass");

    // Entity table
    db.addTable("entity");

    // Input file table
    db.addTable("file");

    db.publish();

    return 0;
}