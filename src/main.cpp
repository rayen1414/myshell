#include <iostream>
#include <string>
#include <unordered_set>
#include <cstdlib>
#include <filesystem>
namespace fs = std::filesystem;
//find exe with directory path
std::string find_exe(const std::string&file_name, const std::string&pa){
  if(pa.empty()){
    return "";
  }
  else {
    std::stringstream ss(pa);
    std ::string dir;
    while(std::getline(ss, dir, ':')){
      fs::path full = fs::path(dir) / file_name;
      if(fs::exists(full)){
        return (full.string());
      }
    }
  }
  return"";
}
int main() {

  // Flush after every std::cout / std:cerr
  char*path1 = std::getenv("PATH");
  std::string path(path1);
  
  //no path else { std::cout<<"path is not set" << std::endl;}
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
      //check if command is a shell builtin
    if(commands.find(command.substr(5)) != commands.end()){ 
      std::cout<<command.substr(5)<<" is a shell builtin" << std::endl;
    }
    //find exe in path
    
   else if (std::string full_path = find_exe(command.substr(5), path); !full_path.empty()) { 
    std::cout << command.substr(5) << " is " << full_path << std::endl;
}
    else {std::cout<<command.substr(5)<<": not found" << std::endl;}
    }


    else{
      std::cout<<command<<": command not found" << std::endl;
    }
  } 

}
