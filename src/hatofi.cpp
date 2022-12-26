//
// Created by Vanilor on 23/12/22.
//

#include "lib/CLI11.hpp"

#include "HaDB.h"

int main(int argc, char** argv) {

    // Application init
    CLI::App app{"App description"};
    std::string data_dir = std::filesystem::current_path().string();
    app.add_option("-d,--data-directory", data_dir, "Database directory");

    /** Database generation **/
    CLI::App* genSub = app.add_subcommand("gen", "Generate Hatofi architecture based on config file");

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

    if(app.got_subcommand("gen"))
    {
        db.publish();
    }
    else if(app.got_subcommand("load"))
    {
        db.load(file_loaded);
    }

    return 0;
}