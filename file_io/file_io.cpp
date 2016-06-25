#include <iostream> 
#include <fstream>
#include <string>
#include <iomanip>

int
main (int args, char **argv)
{
  if (args != 2) {
    std::cout << "invalid args" << std::endl;
    return 1;
  }
  std::string filename = argv[1];

  std::ifstream reading_file;
  reading_file.open(argv[1], std::ios::in);

  std::string reading_line_buffer;

  std::cout << "reading " << filename << "..." << std::endl;
  
  int i = 0;
  while (!reading_file.eof()) {
    // read by line
    std::getline(reading_file, reading_line_buffer);
    std::cout << std::setw(5) << std::setfill('0') << i << " :" << reading_line_buffer << std::endl;
    std::cout << std::setw(5) << i << " :" << reading_line_buffer << std::endl;
    i++;
  }
  return 0;
}
