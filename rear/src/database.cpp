//
// Created by donghao on 25-4-29.
//

#include <database.h>
#include <iostream>

mdb::database::database(const std::filesystem::path &db_path) {
    try {
        if (!std::filesystem::exists(db_path)) {
           throw std::runtime_error("Database does not exist");
        }
    }catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    sqlite3_open(db_path.string().c_str(), &db);
}
