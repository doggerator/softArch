
#include <vector>

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/Util/ServerApplication.h"

#include <database/Product.h>
#include <database/Delivery.h>
#include "http_request_factory.h"


class HTTPDeliveryWebServer : public Poco::Util::ServerApplication {
public:
    HTTPDeliveryWebServer() : _helpRequested(false) { }
    ~HTTPDeliveryWebServer() = default;

protected:
    void initialize(Application &self) {
        loadConfiguration();
        ServerApplication::initialize(self);
    }

    void uninitialize() {
        ServerApplication::uninitialize();
    }

    int main([[maybe_unused]] const std::vector<std::string> &args) {
        if (!_helpRequested) {
            database::Product::Initialize();
            database::Delivery::Initialize();

            Poco::Net::ServerSocket svs(Poco::Net::SocketAddress("0.0.0.0", 8081));
            Poco::Net::HTTPServer srv(new HTTPRequestFactory(), svs, new Poco::Net::HTTPServerParams);
            std::cout << "Server listening on http://0.0.0.0:8081" << std::endl;
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
    HTTPDeliveryWebServer app;
    return app.run(argc, argv);
}
