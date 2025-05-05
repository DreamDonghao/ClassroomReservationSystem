//
// Created by donghao on 25-4-26.
//
#include <main.h>
web::Front front;

int main() {
    //mysqlx::Session session;

    mysqlx::Session sess("mysqlx://root:123456@127.0.0.1:33060/crs");


    // 设置html文件根目录
    front.setHtmlRootPath("front/templates");
    // 设置css文件根目录
    front.setCssRootPath("front/static/css");
    // 设置js文件根目录
    front.setJsRootPath("front/static/js");

    crow::App<CORS> app;

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
    std::unordered_map<std::string, std::string> users = {
        {"catgirl", "123456"},
        {"admin", "admin"}
    };

    CROW_ROUTE(app, "/login").methods("POST"_method)
        ([&](const crow::request& req) {
            auto result = sess.sql("select ")
            auto body = crow::json::load(req.body);
            if (!body) {
                crow::json::wvalue res;
                res["success"] = false;
                res["error"] = "Invalid JSON";
                return crow::response(400, res);
            }

            std::string username = body["username"].s();
            std::string password = body["password"].s();

            crow::json::wvalue res;
            if (users.contains(username) && users[username] == password) {
                res["success"] = true;
                res["message"] = "登录成功喵~";
                return crow::response(200, res);
            } else {
                res["success"] = false;
                res["message"] = "用户名或密码错误喵！";
                return crow::response(401, res);
            }
        });

    // 获取教室信息
    CROW_ROUTE(app, "/api/getClassrooms")
    ([&] {
        auto result = sess.sql("select buildingNumber,floorNumber,classroomNumber,reservationStatu from classrooms").
                execute();
        std::vector<crow::json::wvalue> classrooms;
        for (mysqlx::Row row: result) {
            crow::json::wvalue classroom;
            classroom["buildingNumber"] = row[0].get<std::string>();
            classroom["floorNumber"] = row[1].get<std::string>();
            classroom["classroomNumber"] = row[2].get<std::string>();
            std::string reservationStatuStr;
            try {
                std::stringstream ss;
                ss << row[3];
                reservationStatuStr = ss.str();
            } catch (...) {
                reservationStatuStr = "{}";
            }
            classroom["reservationStatu"] = crow::json::load(reservationStatuStr);
            classrooms.emplace_back(classroom);
        }
        crow::json::wvalue res;
        res["classrooms"] = std::move(classrooms);
        return res;
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
