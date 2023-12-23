#include <memory>

#include "app_component.hpp"
#include "controller/search_controller.hpp"
#include "controller/static_controller.hpp"
#include "easylogging++.h"
#ifdef USE_CUDA
#include "engine/search_engine_gpu.hpp"
#else
#include "engine/search_engine_cpu.hpp"
#endif
#include "oatpp/core/macro/component.hpp"
#include "oatpp/network/Server.hpp"

INITIALIZE_EASYLOGGINGPP

// cwd in docker container is /app
const char *const path_to_json = "../db/db.jsonl";

void run() {
  const app_component components;
  auto logger = el::Loggers::getLogger("lyric-finder-server");
  // initialize controller
  OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);
  OATPP_COMPONENT(std::shared_ptr<static_files_manager>, file_manager);
  // add controllers
  router->addController(search_controller::createShared());
  router->addController(static_controller::createShared());

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