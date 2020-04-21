#include <iostream>
#include "file/file.h"
#include "path/path.h"

int main() {
    std::cout << "Stratos Compiler !" << std::endl;
   std::cout << path::currentExecutablePath();
//   file stratosFile ;
//   stratosFile.readFile();
    return 0;
}
