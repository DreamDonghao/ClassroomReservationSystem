//
// Created by donghao on 25-4-29.
//
#ifndef DATABASE_H
#define DATABASE_H
#include <sqlite3.h>
#include <filesystem>


namespace mdb {
    // 用来管理数据库
    class database {
    public:
        explicit database(const std::filesystem::path &db_path);
    private:
        sqlite3 *db = nullptr;
        sqlite3_stmt *stmt = nullptr;
    };

}

#endif //DATABASE_H
