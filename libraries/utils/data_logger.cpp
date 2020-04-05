#include "data_logger.hpp"

char* _string_to_char(std::string s){
    char* p = new char[s.length()+1];
    memcpy(p, s.c_str(), s.length()+1);
    return p;
}

bool _dir_exists(std::string path){
    struct stat sb;
    return (stat(_string_to_char(path), &sb) == 0 && S_ISDIR(sb.st_mode));
}

bool _file_exists(const std::string& file) {
    struct stat buffer;   
    return (stat (file.c_str(), &buffer) == 0); 
}

int create_log(std::string dir, std::string file, std::vector<std::string> headers){
    if(_file_exists(file))
        return 0;
    int r = 0;
    std::string command = "";
    // Create directory
    if(dir != "./" || dir == ""){
        command = "mkdir -p " + dir;
        r = system(_string_to_char(command));
    }
    command = "touch " + file;
    r = system(_string_to_char(command));

    write_line(file, headers);
     
    return r; 
}

void write_line(std::string file, std::vector<std::string> values){
    try{
        std::ofstream outfile;
        outfile.open(file, std::ios::app); // append
        
        std::string line = "";
        for(uint i=0; i<values.size(); i++){
            line += values.at(i);
            if(i<values.size()-1)
                line += ",";
        }
        outfile << line << std::endl; 
        outfile.close();
    }catch(const std::exception& e){
        const std::string exception = "\n\033[1;33mFollowing exception occured:\033[0m\n";
        std::cout << exception << e.what() << std::endl;
    }
}
void write_line(std::string file, std::vector<double> values){
    std::vector<std::string> str_values;
    for(uint i=0; i<values.size(); i++){
        str_values.push_back(std::to_string(values[i]));
    }
    write_line(file, str_values);
}

int read_last_rollout(std::string file){
    if(!_file_exists(file))
        return 0;
    
    // Find the last line
    std::ifstream fin;
    fin.open(file);
    if(fin.is_open()){
        uint num_lines = 0;
        // Find the last line
        std::string prev_line = "";
        std::string line = "";
        while(! fin.eof()){
            prev_line = line;
            std::getline(fin, line);
            num_lines++;
        }
        fin.close();
        if(line == "")
            line = prev_line;

        if(num_lines == 2){
            // There is only a header. No rollout logged yet. 
            return 0;
        }

        // Extract the first value
        char del = ',';
        int c = 0;
        for(uint i=0; i<line.length(); i++){
            if(line[i] == del){
                c = i;
                break;
            }
        }
        return std::stoi(line.substr(0,c));
    }
    return 0;
}

int read_last_particle(std::string file){
    if(!_file_exists(file))
        return 0;
    
    // Find the last line
    std::ifstream fin;
    fin.open(file);
    if(fin.is_open()){
        uint num_lines = 0;
        // Find the last line
        std::string prev_line = "";
        std::string line = "";
        while(! fin.eof()){
            prev_line = line;
            std::getline(fin, line);
            num_lines++;
        }
        fin.close();
        if(line == "")
            line = prev_line;

        if(num_lines == 2){
            // There is only a header. No rollout logged yet. 
            return 0;
        }

        // Extract the last evaluated particle
        // Find second and third delimiter locations
        char del = ',';
        uint del_count = 0;
        uint first_del = 0;
        uint sec_del = 0;
        for(uint i=0; i<line.length(); i++){
            if(line[i] == del){
                del_count++;
                if(del_count == 2)
                    first_del = i+1;
                else if (del_count == 3){
                    sec_del = i;
                    break;
                }
                
            }
        }
        return std::stoi(line.substr(first_del,sec_del));
    }
    return 0;
}