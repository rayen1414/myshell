#include <iostream>
#include <string>

int main() {
  // Flush after every std::cout / std:cerr
  std::string command;
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;
  while(true ) {
    command.clear();
    std::cout<<"$ " << std::flush;
    std::getline(std::cin,command);
    //echo command
    if(command.substr(0,4)=="echo"){
      std::cout<<command.substr(5)<<std::endl;
      
    }
    //exit command
    else if(command=="exit"){
      break;
    }
    else{
      std::cout<<command<<": command not found" << std::endl;
    }
  }

}
