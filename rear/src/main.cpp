//
// Created by donghao on 25-4-26.
//
#include <main.h>
web::Front front;

int main() {
    // 链接 MySQL 数据库
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

    // 登录请求路由
    CROW_ROUTE(app, "/api/login").methods("POST"_method)
    ([&](const crow::request &req) {
        const auto body = crow::json::load(req.body);
        //发送给前端的 json 内容
        crow::json::wvalue res;

        // 检测前端发送json文件的完整性
        if (!body || !body.has("studentNumber") || !body.has("password")) {
            res["success"] = false;
            res["message"] = "字段不完整喵！";
            return crow::response(400, res);
        }

        try {
            // 查询数据库中的哈希密码
            auto result = sess.sql("SELECT password FROM users WHERE studentNumber = ?")
                    .bind(std::string(body["studentNumber"].s())).execute();
            // 获取第一个满足条件的用户
            auto row = result.fetchOne();
            // 判断是否为空来检测是否存在满足条件的用户
            if (!row) {
                res["success"] = false;
                res["message"] = "用户不存在";
                return crow::response(404, res);
            }
            if (const std::string passwordHash = sha256(body["password"].s());
                row[0].get<std::string>() != passwordHash) {
                std::cout << row[0].get<std::string>() << std::endl;
                std::cout << passwordHash << std::endl;
                res["success"] = false;
                res["message"] = "密码错误";
                return crow::response(401, res);
            }
            res["success"] = true;
            res["message"] = "登录成功";
            return crow::response(200, res);
        } catch (const std::exception &e) {
            res["success"] = false;
            res["message"] = std::string("数据库问题") + e.what();
            return crow::response(500, res);
        }
    });

    // 注册请求路由
    CROW_ROUTE(app, "/api/register").methods("POST"_method)
    ([&](const crow::request &req) {
        const auto body = crow::json::load(req.body);
        crow::json::wvalue res;

        if (!body || !body.has("username") || !body.has("studentNumber")
            || !body.has("password") || !body.has("confirmPassword")) {
            res["success"] = false;
            res["message"] = "字段不完整喵！";
            return crow::response(400, res);
        }

        const std::string password = body["password"].s();
        try {
            // 插入新用户
            sess.sql("INSERT INTO users (name,studentNumber ,password) VALUES (?, ?, ?)")
                    .bind(static_cast<std::string>(body["username"].s()),
                          static_cast<std::string>(body["studentNumber"].s()),
                          sha256(body["password"].s())).execute();

            res["success"] = true;
            res["message"] = "注册成功";
            return crow::response(200, res);
        } catch (const std::exception &e) {
            res["success"] = false;
            res["message"] = std::string("数据库") + e.what();
            return crow::response(500, res);
        }
    });


    // 获取教室信息请求路由
    CROW_ROUTE(app, "/api/getClassrooms")
    ([&] {
        auto result = sess.sql("select id,buildingNumber,floorNumber,classroomNumber from classrooms").
                execute();
        std::vector<crow::json::wvalue> classrooms;
        for (mysqlx::Row row: result) {
            crow::json::wvalue classroom;
            classroom["id"] = row[0].get<int>();
            classroom["buildingNumber"] = row[1].get<std::string>();
            classroom["floorNumber"] = row[2].get<std::string>();
            classroom["classroomNumber"] = row[3].get<std::string>();
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

    // 添加教室预约信息请求路由
    CROW_ROUTE(app, "/api/reserveClassroom").methods("POST"_method)
    ([&](const crow::request &req) {
        const auto body = crow::json::load(req.body);
        crow::json::wvalue res;
        try {
            auto result = sess.
                    sql(
                        "SELECT COUNT(*) FROM reservations WHERE classroom_id = ? AND user_id = ? AND year = ? AND month = ? AND day = ? AND time_period = ?")
                    .bind(static_cast<int>(body["classroom_id"].i()),
                          static_cast<int>(body["user_id"].i()),
                          static_cast<int>(body["year"].i()),
                          static_cast<int>(body["month"].i()),
                          static_cast<int>(body["day"].i()),
                          static_cast<std::string>(body["time_period"].s())).execute();

            if (result.count() > 0) {
                sess.sql(
                    "INSERT INTO reservations (classroom_id, user_id, year, month, day, time_period) VALUES (?, ?, ?, ?, ?, ?)"
                ).bind(static_cast<int>(body["classroom_id"].i()),
                       static_cast<int>(body["user_id"].i()),
                       static_cast<int>(body["year"].i()),
                       static_cast<int>(body["month"].i()),
                       static_cast<int>(body["day"].i()),
                       static_cast<std::string>(body["time_period"].s())).execute();
                res["success"] = true;
                res["message"] = "预约成功";
                return crow::response(200, res);
            } else {
                res["success"] = false;
                res["message"] = "改教室在这个时间段已被预约";
            }

            std::string timesJson = crow::json::wvalue(body["times"]).dump();
            std::cout << timesJson << std::endl;
            sess.sql("UPDATE classrooms SET reservationStatu = ? WHERE id = ?")
                    .bind(timesJson, static_cast<int>(body["id"].i()))
                    .execute();
        } catch (...) {
            res["success"] = false;
            return crow::response(500, res);
        }
        return crow::response(200, res);
    });

    std::thread clearBuffer([&]() {
        std::string command;
        while (std::cin>>command) {
            if (command == "clear") {
                front.clearBuffer();
            }
        }
    });
    clearBuffer.detach();

    //启动web应用
    app.port(18080).multithreaded().run();

    return 0;
}
