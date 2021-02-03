#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "Lexer.h"

void readFile(std::string basicString);

int main(int argc, char* argv[]) {
    std::cout << "Booting compiler" << std::endl;

    if(argc==1){
        std::cout << "Console not built";
    }else{

        const char *s = argv[1];
        std::string str(s);
                readFile(str);
    }

    return 0;

}

void readFile(std::string file_path) {

    std::vector<std::string> lines ;
    std::string line;
    std::ifstream my_file (file_path);
    if (my_file.is_open())
    {
        while ( getline (my_file,line) )
        {
            std:: cout << line << '\n';
            lines.push_back(line);
        }
        my_file.close();


    }

    else std::cout << "Unable to open file";
}