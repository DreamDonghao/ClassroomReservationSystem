//
// Created by donghao on 25-4-26.
//
#include <crow.h>
#include <sqlite3.h>
#include <string>
#include <fstream>

std::string getFile(const std::filesystem::path& filepath) {
    const std::ifstream file(filepath, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file");
    }
    std::ostringstream oss;
    oss << file.rdbuf();
    return oss.str();
}

int main() {

    crow::SimpleApp app;

    CROW_ROUTE(app, "/")([]()->std::string {
        return getFile("front/templates/index.html");
    });

    app.port(18080).multithreaded().run();

    return 0;
}
