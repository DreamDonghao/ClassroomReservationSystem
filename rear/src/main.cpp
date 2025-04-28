#include <main.h>

web::Front front;

int main() {
    // 设置html文件根目录
    front.setHtmlRootPath("front/templates");
    // 设置css文件根目录
    front.setCssRootPath("front/static/css");
    // 设置js文件根目录
    front.setJsRootPath("front/static/js");

    crow::SimpleApp app;

    //添加跟路由
    CROW_ROUTE(app, "/")([]()-> std::string {
        return front.getHtml("index.html");
    });
    
    // 自动加载与文件同名的路由
    CROW_ROUTE(app, "/<string>")([](const std::filesystem::path &fileName)-> const std::string & {
        if (fileName.extension().string() == ".html") {
            return front.getHtml(fileName.filename());
        } else if (fileName.extension().string() == ".css") {
            return front.getCss(fileName.filename());
        } else if (fileName.extension().string() == ".js") {
            return front.getJs(fileName.filename());
        }
        static std::string message("No such file or directory:");
        message += fileName.filename().string();
        return message;
    });

    // 停止服务线程，当输入stop时关闭服务
    std::thread stopServe([&]() {
        std::string cmd;
        while (std::cin >> cmd) {
            if (cmd == "stop") {
                break;
            }
        }
        app.stop();
    });
    //以不阻塞主线程的方式加入线程
    stopServe.detach();

    //启动web应用
    app.port(18080).multithreaded().run();

    return 0;
}
