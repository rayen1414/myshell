#include <iostream>
#include <string>
#include <unordered_set>
int main() {
  // Flush after every std::cout / std:cerr
  std::unordered_set<std::string> commands = {"echo", "exit", "type"};
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
    //type command 
    else if(command.substr(0,4)=="type"){
      if(commands.find(command.substr(5)) != commands.end()){ 
      std::cout<<command.substr(5)<<" is a shell builtin" << std::endl;
    }
    else {std::cout<<command.substr(5)<<": command not found" << std::endl;}
    }
    else{
      std::cout<<command<<": command not found" << std::endl;
    }
  }

}
