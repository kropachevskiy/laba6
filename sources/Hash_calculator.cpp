// Copyright 2020 Your Name <your_email>

#include <Hash_calculator.hpp>

Hash_calculator::Hash_calculator(int argc, char** argv) {
  if (argc == 1) {
    Number_of_threads = std::thread::hardware_concurrency();
  } else {
    Number_of_threads = static_cast<unsigned int>(std::stoi(argv[1]));
  }
  Duration = 0;
}

void Hash_calculator::Start(const bool& key) {
  logging::add_common_attributes();
  logging::add_console_log(
      std::clog, keywords::format = "[%Severity%] %TimeStamp%: %Message%");

  logging::add_file_log(
      keywords::file_name = "sample_%N.log",
      keywords::rotation_size = Mb,
      keywords::time_based_rotation =
          sinks::file::rotation_at_time_point(0, 0, 0),
      keywords::format = "[%Severity%][%TimeStamp%]: %Message%");
  for (unsigned int i = 0; i < Number_of_threads; ++i) {
    Threads.emplace_back(std::thread([&]() { Do_hashing(key); }));
  }
}

void Hash_calculator::Do_hashing(const bool& key) {
  time_point start = high_clock::now();
  while (key) {
    Mutex.lock();
    Random_string = std::to_string(std::rand());
    Hash_string = picosha2::hash256_hex_string(Random_string);
    time_point end = high_clock::now();
    Duration += static_cast<int>(std::chrono::nanoseconds(end - start).count());
    start = end;
    Sort_hash(Hash_string);
    Mutex.unlock();
    std::this_thread::sleep_for(std::chrono::microseconds(2));
  }
}

void Hash_calculator::Sort_hash(std::string& hash) {
  if (Hash_string.substr(60, 4) == End) {
    BOOST_LOG_SEV(level, boost::log::trivial::severity_level::info)
        << std::endl
        << "sourse: " << std::hex << std::stol(Random_string) << std::dec
        << " hash: " << hash << " duration: " << Duration
        << " thread: " << std::this_thread::get_id() << std::endl;
    nlohmann::json j = {{"sourse", Random_string},
                        {"hash_hex", Hash_string},
                        {"duration", Duration}};
    Right_hashs.push_back(j);
  } else {
    BOOST_LOG_SEV(level, boost::log::trivial::severity_level::trace)
        << std::endl
        << "sourse: " << std::hex << std::stol(Random_string) << std::dec
        << " hash: " << Hash_string
        << " thread: " << std::this_thread::get_id() << std::endl;
  }
}

Hash_calculator::~Hash_calculator() {
  for (unsigned int i = 0; i < Number_of_threads; ++i) {
    Threads[i].join();
  }
}

std::vector<nlohmann::json> Hash_calculator::Right_hashs;
