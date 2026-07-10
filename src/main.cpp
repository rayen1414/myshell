#include <iostream>
#include <string>
#include <unordered_map>
#include <cstdlib>
#include <filesystem>
#include <functional>
#include <vector>
#include <unistd.h> 
#ifdef _WIN32
#else
    #include <unistd.h>
    #include <sys/wait.h>
    #endif
/*#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
    #include <termios.h>
#endif*/
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
   enum class state{genrale, two, single,backshlash};
   std::string token;
   state s=state::genrale;
   std::vector<std::string> tokens;
   for(char c:ch){
    switch (s){
      case state::genrale:{
        if(c=='\\') s=state::backshlash;
        else if(c=='\'') s=state::single;
        else if(c=='"') s=state::two;
        else if (c == ' ' ){if(!token.empty()){tokens.push_back(token);token="";}}
        else token+=c;
        break;
      }
      case state::backshlash :{token+=c;s=state::genrale;break;}
      case state::single:{
        if(c=='\'') {s=state::genrale;break;}
        token+=c;
        break;

      }
      case state::two:{
        if(c=='\\') {s=state::backshlash;break;}
        if(c=='"') {s=state::genrale;break;}
        token+=c;
        break;
      }
      
    }

   }
if (!token.empty()) {
    tokens.push_back(token);
}
return(tokens);
};
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
    // 1. Extract the name the user actually typed
    std::string word = command.substr(0, command.find(' '));
    
    // 2. Find the full path to check if it exists
    std::string full_path = find_exe(word, pa);
    
    if (full_path != "") {
        std::vector<std::string> output_vector = in_quotes(command.substr(command.find(' ') + 1));
        
        std::vector<char*> args;
        
        // 3. IMPORTANT: Use the ORIGINAL name (word) for Arg #0, not the full_path
        args.push_back(const_cast<char*>(word.c_str())); 
        
        for (auto& token : output_vector) {
            args.push_back(const_cast<char*>(token.c_str()));
        }
        args.push_back(nullptr);

  #ifdef _WIN32

    std::cout << " not supported on Windows " << std::endl;
#else
    
  

    pid_t pid = fork();
    if (pid == 0) {
        execvp(full_path.c_str(), args.data());
        exit(1);
    } else {
        waitpid(pid, NULL, 0);
    }
#endif
    }
    return false;
}

/*std::string get_colored_input(const std::unordered_map<std::string, std::function<void(const std::string&)>>& cmd_map) {
    std::string command = "";
    char c;
    std::cout<<"$ " << std::flush;
    
#ifdef _WIN32
    // Windows Setup
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    DWORD old_mode, current_mode;
    GetConsoleMode(hInput, &old_mode);
    current_mode = old_mode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
    SetConsoleMode(hInput, current_mode);
#else
    // Linux Setup
    struct termios old = {0}, current = {0};
    tcgetattr(0, &old);
    current = old;
    current.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(0, TCSANOW, &current);
#endif

while (true) {
#ifdef _WIN32
        DWORD read_chars;
        ReadConsoleA(hInput, &c, 1, &read_chars, NULL);
        if (read_chars == 0) continue;
#else
        if (read(0, &c, 1) <= 0) break;
#endif
if (c == '\n' || c == '\r') { 
#ifdef _WIN32
           
            DWORD events = 0;
            HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
            GetNumberOfConsoleInputEvents(hInput, &events);
            
            if (events > 0) {
                DWORD read_chars;
                char next_c;
                
                ReadConsoleA(hInput, &next_c, 1, &read_chars, NULL);
               
                if (next_c != '\n' && next_c != '\r') {
                  
                }
            }
#else
           
            char next_c;
            while (read(0, &next_c, 1) > 0 && (next_c == '\n' || next_c == '\r'));
#endif
            break;
        }
        else if (c == 127 || c == 8) { 
            if (!command.empty()) {
                command.pop_back();
                std::cout << "\r\33[2K$ " << command << std::flush;
            }
        } 
        else if (c == ' ' && command.find(' ') == std::string::npos) {
            std::cout << "\r\33[2K$ ";
            if (cmd_map.find(command) != cmd_map.end()) {
                std::cout << "\033[32m" << command << "\033[0m " << std::flush;
            } else {
                std::cout << "\033[31m" << command << "\033[0m " << std::flush;
            }
            command += c;
        } 
        else {
            command += c;
            std::cout << c << std::flush;
        }
    }

#ifdef _WIN32
    SetConsoleMode(hInput, old_mode);
#else
    tcsetattr(0, TCSANOW, &old);
#endif

    std::cout << std::endl; 
    return command;
}
*/


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
    /*command = get_colored_input(command_map);
    if (command.empty()) continue;*/
    std::string cmd = command.substr(0, command.find(' '));
    std::string target = command.substr(command.find(' ') + 1);

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
};
};
int main() {
    shell my_shell;
    return my_shell.run();
}
