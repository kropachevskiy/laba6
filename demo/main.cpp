#include <Hash_calculator.hpp>

int main(int argc, char* argv[]) {
  signal(SIGINT, Hash_calculator::Signal_catch);
  Hash_calculator hash(argc, argv);
  hash.Start(true);
}