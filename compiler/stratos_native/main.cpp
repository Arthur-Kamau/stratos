
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include "Lexer.h"
namespace fs = std::filesystem;

void compile_script(std::string basicString);

int main(int argc, char* argv[]) {
    std::cerr << "Booting compiler" << std::endl;

    if(argc==1){
        std::cout << "Console not built";
    }else{


        const char *s = argv[1];
        std::string str(s);

        const std::filesystem::path path(str); // Constructing the path from a string is possible.
        std::error_code ec; // For using the non-throwing overloads of functions below.
        if (fs::is_directory(path, ec))
        {
            // Process a directory.
            std::cerr <<"project directory's not supported yet  " << std::endl;

        }
        if (ec) // Optional handling of possible errors.
        {
            std::cerr << "Error in is_directory: " << ec.message() << std::endl;
        }
        if (fs::is_regular_file(path, ec))
        {
            compile_script(str);
        }
        if (ec) // Optional handling of possible errors. Usage of the same ec object works since fs functions are calling ec.clear() if no errors occur.
        {
            std::cerr << "Error in is_regular_file: " << ec.message() << std::endl;
        }

    }

    return 0;

}

void compile_script(std::string file_path) {
    std::ifstream inFile;
    inFile.open(file_path); //open the input file

    std::stringstream strStream;
    strStream << inFile.rdbuf(); //read the file
    std::string str = strStream.str();


    Lexer l ;
    l.lex_text(str);
}
