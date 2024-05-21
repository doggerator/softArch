#include <csignal>
#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/Util/ServerApplication.h"

#include "http_request_factory.h"
#include <database/User.h>
#include <database/Product.h>
#include <database/Delivery.h>

class HTTPWebServer : public Poco::Util::ServerApplication
{
public:
    HTTPWebServer() : _helpRequested(false) { }
    ~HTTPWebServer() { }

protected:
    void initialize(Poco::Util::Application& self) {
        loadConfiguration();
        Poco::Util::ServerApplication::initialize(self);
    }

    void uninitialize() {
        Poco::Util::ServerApplication::uninitialize();
    }

    int main([[maybe_unused]] const std::vector<std::string>& args) {
        if (!_helpRequested) {
            database::User::init();
            Poco::Net::ServerSocket svs(Poco::Net::SocketAddress("0.0.0.0", 8080));
            Poco::Net::HTTPServer srv(new HTTPRequestFactory(), svs, new Poco::Net::HTTPServerParams);
            std::cout << "Server listening on http://0.0.0.0:8080" << std::endl;
            srv.start();
            waitForTerminationRequest();
            srv.stop();
        }
        return Application::EXIT_OK;
    }

private:
    bool _helpRequested;
};

int main(int argc, char*argv[]) {
    std::signal(SIGTERM, [](int) { exit(0); });
    HTTPWebServer app;
    return app.run(argc, argv);
    return 0;
}
