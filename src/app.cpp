#include <string>

#include "app_component.hpp"
#include "controller/controller.hpp"
#include "easylogging++.h"
#include "engine/search_engine_gpu.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/network/Server.hpp"

INITIALIZE_EASYLOGGINGPP

// cwd in docker container is /app
const char *const path_to_json = "./db/db.jsonl";

void run() {
  const app_component components;
  auto logger = el::Loggers::getLogger("lyric-finder-server");

  // initialize controller
  auto engine = std::make_shared<search_engine_gpu>(path_to_json);
  OATPP_COMPONENT(std::shared_ptr<oatpp::parser::json::mapping::ObjectMapper>,
                  json_mapper);
  OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);
  auto search_controller = std::make_shared<controller>(json_mapper, engine);
  router->addController(search_controller);

  // configure and start server
  OATPP_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>,
                  connectionHandler);
  OATPP_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>,
                  connectionProvider);
  oatpp::network::Server server(connectionProvider, connectionHandler);
  CLOG(INFO, logger->id().c_str())
      << "Server running on port "
      << static_cast<const char *>(
             connectionProvider->getProperty("port").getData());
  server.run();
}

int main(int argc, char *argv[]) {
  oatpp::base::Environment::init();
  run();
  oatpp::base::Environment::destroy();
  return 0;
}