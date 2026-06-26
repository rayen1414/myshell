#include <iostream>
#include <string>
#include <unordered_map>
#include <cstdlib>
#include <filesystem>
#include <functional>
#include <vector>
namespace fs = std::filesystem;

class shell {
private:
  // Flush after every std::cout / std:cerr
  char*path1 = std::getenv("PATH");
  std::string path = path1 ? std::string(path1) : "";
  //no path else { std::cout<<"path is not set" << std::endl;}
  //commands set
  void handle_echo(const std::string& raw_line) { 
    if (raw_line.size() > 5) {
            std::cout << raw_line.substr(5) << std::endl; 
        } else {
            std::cout << std::endl;
        } };
  //exit command
  void handle_exit(const std::string& raw_line) {test=false; };
  //type command
  void handle_type(const std::string& command) {  //check if command is a shell builtin
    if(command_map.find(command.substr(5)) != command_map.end()){ 
      std::cout<<command.substr(5)<<" is a shell builtin" << std::endl;
    }
    //find exe in path
    
   else if (std::string full_path = find_exe(command.substr(5), path); !full_path.empty()) { 
    std::cout << command.substr(5) << " is " << full_path << std::endl;
}
    else {std::cout<<command.substr(5)<<": not found" << std::endl;}};
  //pwd command
  void handle_pwd(const std::string& raw_line) {std::cout<<fs::current_path()<<std::endl; };
//


  std::string command;
  std::unordered_map<std::string, std::function<void(const std::string&)>> command_map;

  bool test=true;
//toknize the command
  std::vector<std::string> tokenize(const std::string& command) {
    std::vector<std::string> tokens;
    std::stringstream ss(command);
    std::string token;
    while (std::getline(ss, token, ' ')) {
      tokens.push_back(token);
    }
    return tokens;
  }

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
    std::system(command.c_str());
    return true;
    }
    else{
      return false;
    }

}
public:
  shell() {
    #define ADD_CMD(cmd_name) command_map[#cmd_name] = [this](const std::string& line) { handle_##cmd_name(line); }
        ADD_CMD(echo);
        ADD_CMD(exit);
        ADD_CMD(type);
        ADD_CMD(pwd);
        #undef ADD_CMD
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;
  };
  int run() {

  while(test==true) {
    command.clear();
    std::cout<<"$ " << std::flush;
    std::getline(std::cin,command);
    std::string cmd =command.substr(0, command.find(' '));
    //cmd in command_map
    if(command_map.find(cmd) != command_map.end()){
      command_map[cmd](command);
    }
    //command in path 
    else if(exe_exist(command,path)){}

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
