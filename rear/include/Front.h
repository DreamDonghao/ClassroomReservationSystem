//
// Created by donghao on 25-4-26.
//

#ifndef FRONT_H
#define FRONT_H
#include <string>
#include <filesystem>
#include <unordered_map>
#include <queue>
#include <shared_mutex>
namespace web {
    // 用来管理前端文件内容
    // 具有缓冲文件功能,避免重复读取文件
    // 当缓冲区到达最大时，会通过删除最早加入的文件避免缓冲区溢出
    class Front {
    public:
        Front(): m_maxCharNum(1000000000000) {
        };

        // 设置缓冲区最大字节数
        void setMaxCharNum(const unsigned long long maxCharNum) { m_maxCharNum = maxCharNum; };

        // 设置html文件根目录
        void setHtmlRootPath(std::filesystem::path path) { m_htmlRootPath = std::move(path); };

        // 设置css文件根目录
        void setCssRootPath(std::filesystem::path path) { m_cssRootPath = std::move(path); };

        // 设置js文件根目录
        void setJsRootPath(std::filesystem::path path) { m_jsRootPath = std::move(path); };

        // 获取当前html文件根目录
        [[nodiscard]] const std::filesystem::path &getHtmlRootPath() const { return m_htmlRootPath; };

        // 获取当前css文件根目录
        [[nodiscard]] const std::filesystem::path &getCssPath() const { return m_cssRootPath; };

        // 获取当前js文件根目录
        [[nodiscard]] const std::filesystem::path &getJsPath() const { return m_jsRootPath; };

        // 获取html文件内容
        const std::string &getHtml(const std::filesystem::path &name);

        // 获取css文件内容
        const std::string &getCss(const std::filesystem::path &name);

        // 获取js文件内容
        const std::string &getJs(const std::filesystem::path &name);

    private:
        // 线程锁,用来给数据读写加锁,防止线程数据竞争
        mutable std::shared_mutex m_mutex;

        // html文件根目录
        std::filesystem::path m_htmlRootPath;

        // css文件根目录
        std::filesystem::path m_cssRootPath;

        // js文件根目录
        std::filesystem::path m_jsRootPath;

        // 加载缓冲区记录
        std::queue<std::string> m_loadFilelog;

        // 缓冲区最大字节数
        unsigned long long m_maxCharNum;

        // 缓冲区当前字节数
        unsigned long long m_charNum = 0;

        // 缓冲区内容
        std::unordered_map<std::string, std::string> m_content;

        // 获取文件内容
        const std::string& getFileContent(const std::filesystem::path &path);

        // 加载文件到缓冲区
        bool loadFileToContent(const std::filesystem::path &path);
    };
}

#endif //FRONT_H
