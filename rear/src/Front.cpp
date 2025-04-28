#include "Front.h"
#include <fstream>

const std::string &web::Front::getHtml(const std::filesystem::path &name) {
    return getFileContent(m_htmlRootPath / name);
}

const std::string &web::Front::getCss(const std::filesystem::path &name) {
    return getFileContent(m_cssRootPath / name);
}

const std::string &web::Front::getJs(const std::filesystem::path &name) {
    return getFileContent(m_jsRootPath / name);
}

const std::string &web::Front::getFileContent(const std::filesystem::path &path) {
    // 先读取文件到缓冲区
    if (loadFileToContent(path)) {
        // 共享锁,能有多个线程同时读取对象,读取对象时无法写入
        std::shared_lock lock(m_mutex);
        return m_content[path.filename().string()];
    }
    // 无法加载文件时输出错误信息
    static std::string message;
    message = "No find file:" + path.filename().string() + "\nNo find directory:" + path.string();
    return message;
}

bool web::Front::loadFileToContent(const std::filesystem::path &path) {
    // 缓冲区内没有该文件时读取文件到缓冲区
    if (!m_content.contains(path.filename().string())) {
        // 记录文件加入缓冲区记录队列
        m_loadFilelog.push(path.filename().string());
        // ReSharper disable once CppLocalVariableMayBeConst
        std::ifstream file(path, std::ios::in | std::ios::binary);
        //当文件无法打开时,返回false
        if (!file.is_open()) {
            return false;
        }
        std::ostringstream oss;
        oss << file.rdbuf();

        // 独占锁,同时只能有一个线程写入对象。写入时无法读取
        std::unique_lock lock(m_mutex);
        m_content[path.filename().string()] = std::move(oss.str());
        m_charNum += m_content[path.filename().string()].size();
        // 缓冲区溢出时删除最早加入的文件
        while (m_charNum > m_maxCharNum) {
            m_charNum -= m_content[m_loadFilelog.back()].size();
            m_content.erase(m_loadFilelog.back());
            m_loadFilelog.pop();
        }
    }
    return true;
}
