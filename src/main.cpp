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
  char* home_env = std::getenv("HOME");
  std::string home_path = home_env ? std::string(home_env) : "/";
  //no path else { std::cout<<"path is not set" << std::endl;}
  //commands set
  void handle_echo(const std::string& raw_line) { 
    std::vector<std::string> tokens;
    
    tokens=in_quotes(raw_line);
    for (size_t i = 0; i < tokens.size(); i++) {
        std::cout << tokens[i];
        if (i + 1 < tokens.size()) {
            std::cout << " ";
        }
    }
    std::cout << std::endl;};
  //exit command
  void handle_exit(const std::string& raw_line) {test=false; };
  //type command
  void handle_type(const std::string& command) {  //check if command is a shell builtin
    if(command_map.find(command) != command_map.end()){ 
      std::cout<<command<<" is a shell builtin" << std::endl;
    }
    //find exe in path
    
   else if (std::string full_path = find_exe(command, path); !full_path.empty()) { 
    std::cout << command << " is " << full_path << std::endl;
}
    else {std::cout<<command<<": not found" << std::endl;}};

  //pwd command
  void handle_pwd(const std::string& raw_line) {std::cout<<fs::current_path().string()<<std::endl; };

  //cd command
  void handle_cd(const std::string& raw_line) {
fs::path new_path;

    if (raw_line == "~") {
        new_path = home_path;
    } 
    else if (raw_line.substr(0, 2) == "~/") {

        new_path = fs::path(home_path) / raw_line.substr(2); 
    } 
    else {
        // Standard absolute or relative path
        new_path = fs::current_path() / raw_line;
    }
try{
new_path = fs::weakly_canonical(new_path);
if(fs::exists(new_path)&&fs::is_directory(new_path)){
  fs::current_path(new_path);
}
else{
  std::cout << "cd: " << raw_line << ": No such file or directory" << std::endl;
}
}catch(const fs::filesystem_error& e){
  std::cout << "cd: " << raw_line << ": No such file or directory" << std::endl;
}

  };
//


  std::string command;
  std::unordered_map<std::string, std::function<void(const std::string&)>> command_map;

  bool test=true;
//toknize the command
  std::vector<std::string> tokenize(const std::string& command,char x) {
    std::vector<std::string> tokens;
    std::stringstream ss(command);
    std::string token;
    while (std::getline(ss, token, x)) {
      tokens.push_back(token);
    }
    return tokens;
  }
//in quotes
std::vector<std::string> in_quotes(const std::string ch) {
    bool in_quotes = false;
    std::vector<std::string> tokens;
    std::string current_token = "";

    for (char c : ch) {
        if (c == '\'') {
            in_quotes = !in_quotes;
        }
        else if (c == ' ' && in_quotes == false) {
            if (!current_token.empty()) {
                tokens.push_back(current_token);
                current_token = "";
            }
        }
        else {
            current_token += c;
        }
    }
    
    if (!current_token.empty()) {
        tokens.push_back(current_token);
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
bool exe_exist(const std::string& command, const std::string& pa) {
    std::vector<std::string> output_vector;
    std::stringstream ss(command);
    std::string word;
    
    getline(ss, word, ' ');
    
    std::string full_path = find_exe(word, pa);
    
    if (full_path != "") {
        output_vector = in_quotes(command.substr(command.find(' ') + 1));
        
        std::string ch = full_path; 
        
        for (int i = 0; i < output_vector.size(); i++) {
            ch += " \"" + output_vector[i] + "\"";
        }
        
        std::system(ch.c_str());
        return true;
    }
    else {
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
        ADD_CMD(cd);
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
    std::string target = command.substr(command.find(' ')+1);

    //cmd in command_map
    if(command_map.find(cmd) != command_map.end()){
      command_map[cmd](target);
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
