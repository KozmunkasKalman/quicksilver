#include <iostream>
#include <fstream>
#include <sstream>

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Error: Incorrect usage.\nCorrect usage:\nqsc <input.qsv>" << std::endl;
    return 1;
  }

  std::fstream input(argv[1], std::ios::in);
  std::stringstream contents_stream;
  contents_stream << input.rdbuf();
  std::string contents = contents_stream.str();
  input.close();
  
  std::cout << contents << std::endl;

  return 0;
}
