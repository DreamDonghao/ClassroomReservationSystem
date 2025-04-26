//
// Created by donghao on 25-4-26.
//

#ifndef FRONT_H
#define FRONT_H
#include <string>
#include <filesystem>
#include <fstream>
#include <iostream>
namespace tp {
   class Front {
    public:
        void setHtmlRootPath(std::string path) { htmlRootPath = std::move(path); };
        void setCssRootPath(std::string path) { cssRootPath = std::move(path); };
        void setJsRootPath(std::string path) { jsRootPath = std::move(path); };
        const std::string &getHtmlRootPath() const { return htmlRootPath; };
        const std::string &getCssPath() const { return cssRootPath; };
        const std::string &getJsPath() const { return jsRootPath; };

        std::string &getHtml(const std::string &name);
        std::string &getCss(const std::string &name);
        std::string &getJs(const std::string &name);

    private:
        std::string htmlRootPath;
        std::string cssRootPath;
        std::string jsRootPath;

        std::string content;

        void getFileContent(const std::filesystem::path &path);
    };
}

#endif //FRONT_H
