//
// Created by publisher on 14/04/2020.
//

#ifndef STRATOS_COMPILER_PATH_H
#define STRATOS_COMPILER_PATH_H

#ifdef WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

#include <iostream>
#include <stdlib.h>
#include <string>
#include <filesystem>

class path {
    public:
    static std::string homePath(){
        return getenv("HOME");
    }
    static std::string currentExecutablePath(){
//        return std::experimental::filesystem::current_path();
        return  std::filesystem::current_path();
    }
    std::string getFileNameWithExtension(std::string path){
        return path.substr(path.find_last_of("/\\") + 1);
    }
    std::string getFileNameWithoutExtension(std::string path){
        std::string base_filename = path.substr(path.find_last_of("/\\") + 1);
        std::string::size_type const p(base_filename.find_last_of('.'));
        std::string file_without_extension = base_filename.substr(0, p);
        return  file_without_extension;
    }
    static std::string getCurrentDirectory() {
        char buff[FILENAME_MAX]; //create string buffer to hold path
        GetCurrentDir( buff, FILENAME_MAX );
        std::string current_working_dir(buff);
        return current_working_dir;
    }

};


#endif //STRATOS_COMPILER_PATH_H
