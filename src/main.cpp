#include <iostream>
#include <string>
#include <unordered_set>
#include <cstdlib>
#include <filesystem>
#include <vector>
namespace fs = std::filesystem;
class shell {
private:
  // Flush after every std::cout / std:cerr
  char*path1 = std::getenv("PATH");
  std::string path = path1 ? std::string(path1) : "";
  //no path else { std::cout<<"path is not set" << std::endl;}
  std::unordered_set<std::string> commands ;
  std::string command;



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
        fs::perms p=fs::status(full).permissions();
        if((p & fs::perms::owner_exec) != fs::perms::none || (p & fs::perms::group_exec) != fs::perms::none || (p & fs::perms::others_exec) != fs::perms::none){
        return (full.string());
      }}
    }
  }
  return"";
}
//execute the exe
bool exe_exist(const std::string&command, const std::string&pa){
  std::vector<std::string> output_vector;
    std::stringstream ss(command);
    std ::string word;
    while(std::getline(ss,word,' ')){
      output_vector.push_back(word);
    }
    if(find_exe(output_vector[0],pa) !=""){
      std::cout<<"Program was passed "<<output_vector.size()<<" arguments"<<std::endl;
      for(int i=0;i<output_vector.size();++i){
        std::cout<<"arg #"<<i<<":"<<output_vector[i]<<std::endl;
      }
    std::system(command.c_str());
    return true;
    }
    else{
      return false;
    }

}
public:
  shell() {
    commands = {"echo", "exit", "type"};
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;
  };
  int run() {

  while(true ) {
    command.clear();
    std::cout<<"$ " << std::flush;
    std::getline(std::cin,command);
    //echo command
    if(command.substr(0,4)=="echo "){
      std::cout<<command.substr(5)<<std::endl;
      
    }
    //exit command
    else if(command=="exit"){
      break;
    }

    //type command 
    else if(command.substr(0,5)=="type "){
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

    //command in path 
    else if(exe_exist(command,path)){
    
    }

    else{
      std::cout<<command<<": command not found" << std::endl;
    }
  } 
return 0;
}
};
int main() {
    shell my_shell;
    return my_shell.run();
}
