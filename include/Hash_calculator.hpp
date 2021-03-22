// Copyright 2020 Your Name <your_email>

#ifndef INCLUDE_HASH_CALCULATOR_HPP_
#define INCLUDE_HASH_CALCULATOR_HPP_
#include <unistd.h>

#include <boost/log/attributes/named_scope.hpp>
#include <boost/log/attributes/timer.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/smart_ptr/make_shared_object.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <chrono>
#include <csignal>
#include <fstream>
#include <iostream>
#include <mutex>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "../third-party/PicoSHA2/picosha2.h"

using time_point = std::chrono::system_clock::time_point;
using high_clock = std::chrono::high_resolution_clock;
using boost::shared_ptr;

namespace logging = boost::log;
namespace sources = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;
namespace attributes = boost::log::attributes;
namespace expressions = boost::log::expressions;

static const char End[] = "0000";
const int Mb {10 * 1024 * 1024};

class Hash_calculator {
 private:
  std::mutex Mutex;
  unsigned int Number_of_threads;
  int64_t Duration;
  std::string Random_string;
  std::string Hash_string;
  std::vector<std::thread> Threads;
  static std::vector<nlohmann::json> Right_hashs;
  sources::severity_logger<boost::log::trivial::severity_level> level;

 public:
  Hash_calculator(int argc, char* argv[]);

  ~Hash_calculator();

  static void Signal_catch(int signal) {
    Write_to_json_file("map.json");
    sleep(1);
    std::cout << "\nProgram aborted with code " << --signal << "\n";
    exit(signal);
  }

  static void Write_to_json_file(const std::string& file_name) {
    std::ofstream output_file;
    std::ifstream input_file;
    output_file.open(file_name);
    input_file.open(file_name);
    nlohmann::json out_json;
    if (input_file.peek() != EOF) input_file >> out_json;
    for (auto& right_hash : Right_hashs) {
      out_json["values"].push_back(right_hash);
    }
    output_file << out_json.dump(4);
    output_file.close();
  }

  void Start(const bool& key);

  void Sort_hash(std::string& hash);

  void Do_hashing(const bool& key);
};

#endif  // INCLUDE_HASH_CALCULATOR_HPP_