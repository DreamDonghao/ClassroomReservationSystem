//
// Created by donghao on 25-4-26.
//

#include "Front.h"

std::string &tp::Front::getHtml(const std::string &name) {
    getFileContent(htmlRootPath + name);
    return content;
}

std::string &tp::Front::getCss(const std::string &name) {
    getFileContent(cssRootPath + name);
    return content;
}

std::string &tp::Front::getJs(const std::string &name) {
    getFileContent(jsRootPath + name);
    return content;
}

void tp::Front::getFileContent(const std::filesystem::path &path)  {
    const std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        std::cout<<path.string()<<std::endl;
        throw std::runtime_error("Could not open file");
    }
    std::ostringstream oss;
    oss << file.rdbuf();
    content =  oss.str();
}
