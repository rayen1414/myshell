#include <iostream>
#include <string>

int main() {
  // Flush after every std::cout / std:cerr
  std::string command;
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;
  while(true) {
    std::cout<<"$ " << std::flush;
    std::getline(std::cin,command);
    if(command.substr(0,4)=="echo"){
      std::cout<<command.substr(5)<<std::endl;
      break;
    }
    if(command=="exit"){
      break;
    }
    std::cout<<command<<": command not found" << std::endl;
  }

}
