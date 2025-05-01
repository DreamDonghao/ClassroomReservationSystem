//
// Created by donghao on 25-4-26.
//
#include <main.h>
#include <mysqlx/xdevapi.h>
web::Front front;

int main() {
    try {
        mysqlx::Session sess("mysqlx://root:123456@127.0.0.1:33060/test");
        sess.sql("insert into user (name,classroom) values (?,?)")
                .bind("haha", 10)
                .execute();
    } catch (const mysqlx::Error &err) {
        std::cerr << "Error: " << err.what() << std::endl;
    }

    return 0;
}

int f() {
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

    // 自动加载与文件同名的路由,没有指定正确文件类型默认返回html
    CROW_ROUTE(app, "/<string>")([](const std::filesystem::path &fileName)-> const std::string & {
        if (fileName.extension().string() == ".html") {
            return front.getHtml(fileName.filename());
        } else if (fileName.extension().string() == ".css") {
            return front.getCss(fileName.filename());
        } else if (fileName.extension().string() == ".js") {
            return front.getJs(fileName.filename());
        } else {
            return front.getHtml(fileName.filename().string() + ".html");
        }
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
