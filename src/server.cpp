#include <string>

#include "easylogging++.h"
#include "search_engine_cpu.hpp"

INITIALIZE_EASYLOGGINGPP

const char *path_to_json = "../db/db.jsonl";

int
main (int argc, char *argv[])
{
  const std::string lyric_to_find{ "Young love, young love everything I need I got my young love" };
  search_engine_cpu engine{ path_to_json };
  auto results = engine.query (lyric_to_find);
  print_results (results);
  return 0;
}