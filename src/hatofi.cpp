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
    CLI::App* sub = app.add_subcommand("gen", "This is a subcommand");
    std::string data_dir = std::filesystem::current_path().string();
    sub->add_option("-d,--data-directory", data_dir, "Database directory");

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        return app.exit(e);
    }

    printf("Data directory: %s\n", data_dir.c_str());

    HaDB db = HaDB(data_dir);
    db.addTable("test");

    return 0;
}