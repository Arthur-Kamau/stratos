//
// Created by publisher on 14/04/2020.
//

#include "file.h"


void file::readFile(const std::string& filePath){
   
    std::ifstream appFile(filePath);
    std::string line;
    if(appFile.is_open()){
        while (!appFile.eof()){
            std::getline(appFile,line);
            std::cout << line << "\n";
        }
        appFile.close();
    }else{
        std::cout << "unable to read file";
    }

}
