#include <main.h>

tp::Front front;

int main() {
    front.setHtmlRootPath("front/templates/");
    front.setCssRootPath("front/static/css/");
    front.setJsRootPath("front/static/js/");
    crow::SimpleApp app;

    CROW_ROUTE(app, "/")([]()-> std::string {
        return front.getHtml("index.html");
    });
    CROW_ROUTE(app, "/index.js")([]()-> std::string {
        std::cout<<front.getJs("index.js")<<std::endl;
        return front.getJs("index.js");
    });

    app.port(18080).multithreaded().run();

    return 0;
}
