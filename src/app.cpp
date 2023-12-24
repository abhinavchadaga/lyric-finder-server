#include <boost/program_options.hpp>
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

void run(bool use_gpu, size_t nthreads) {
  const app_component components{use_gpu, nthreads};
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
  namespace po = boost::program_options;
  bool use_gpu;
  size_t nthreads = 0;
  po::options_description desc("Allowed options");
  desc.add_options()("help", "produce help message")(
      "gpu,g", po::bool_switch(&use_gpu), "use gpu")(
      "threads,t", po::value<size_t>(&nthreads), "number of threads to use (cpu only)");
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);
#ifndef USE_CUDA
  if (use_gpu) {
    std::cerr << "GPU support not compiled in" << std::endl;
    return 1;
  }
  use_gpu = false;
#endif
#ifdef USE_CUDA
  if (use_gpu && nthreads > 0) {
    LOG(WARNING) << "GPU support enabled, ignoring number of threads";
  }
#endif
  oatpp::base::Environment::init();
  run(use_gpu, nthreads);
  oatpp::base::Environment::destroy();
  return 0;
}