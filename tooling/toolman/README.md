# Toolman
A tool to create, test, run and build stratos applications.

## commands
* `toolman create app_name`<br/>
    The  above command creates a stratos application project.An application project involves,<br/>
    1. `bin` folder for the application binaries these are the binraies compiled and run in debug mode.
    
    2. `src` folder fo the application source code.
    3. `app.conf` file for the application configurations.
    4.  `src/main.st` file with main function that prints out a line  

    An additional optional parameter can be added to specify the project location if none is provided the `pwd`(current working directory) is used

* `toolman run `
    The command runs the application using the configurations provided in the `app.conf`, this is for debugging as there is a stack trace dump,no/little optimizations.<br/>
    The current architecture binary is  generated, dumped into the `bin` folder/

* `toolman get  git_url/local_path`
    The command get packages from the local path or git url and stores them in a `.cache` folder in the compiler path, adds the file in `app.conf.lock` file.<br/>
    Provided without parameters it will loop through the dependencies in the `app.conf` file doing a refetch if  version does not match in `app.conf.lock` file or is a new dependancy.

* `toolman build `
    This command build static binaries for the specified target platforms,applying compiler optimizations.<br/>
    The command creates a `dist` folder, with different subfolders for the different architeres specified in the `app.conf` file.

* `toolman test `
    This command runs test.

* `toolman analysis`
    This command checks code for vulnrable packages, pedantic code(You followed good practises), dead code inference ,