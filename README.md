# MEDEA
## About MEDEA (Modelling, Experiment DEsign and Analysis)
MEDEA is a tool that can be used to design and analyse performance of Distributed Real-time and Embedded (DRE) systems. The tool is designed to facilitate the creation of a representative model of a distributed system, as well as providing System Execution Modelling (SEM) and performance analysis.

## Minimum software requirements:
* [CMake 3.0](https://cmake.org/)
* [Qt 5.7.0](https://www.qt.io/)
* C++11 Compiler

## Optional software requirements:
* [Qt Installer Framework 2.0](http://doc.qt.io/qtinstallerframework/)
* [Ninja](https://ninja-build.org/)


# Building MEDEA binary from source
* Set environment variable Qt5_DIR to point the installation of QT5.7 (ie /opt/Qt5.7.0/5.7/gcc_64)
* Replace the -G flag with a suitable generator for your operating system, "Unix Makefiles" for Unix
> ```
> git clone https://github.com/cdit-ma/MEDEA --recursive
> cd MEDEA
> mkdir build
> cd build
> cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Release
> cmake --build . --config Release
> ```

# Building MEDEA installer from source
* Set environment variable QTIFWDIR to point the installation of QT Installer Framework
* Follow the build instructions above
> ```
> cpack 
> ```
* The installer will be located in build/_CPack_Packages


# medea_cli
A command line version of MEDEA, provides command line import/export functionality. Returns an error code if failed.

## Command line options
| Flag                                  | Description                           |
|---------------------------------------|---------------------------------------|
| -h, --help                            | Displays this help.                   |
| -v, --version                         | Displays version information.         |
| -o, --open <The graphml file path>    | Open a graphml project.               |
| -i, --import <The graphml file path>  | Import graphml project(s).            |
| -e, --export <The graphml file path>  | Export project as a graphml file.     |
| -f, --functional                      | Functionally export, stripping out    |


## Example Usage:
### Validating that MEDEA can interpret project
Importing and exporting a model will validate that MEDEA can interpret the model correctly. This is useful to test if model functionality has been broken in a newer release. Errors should be printed out as result of the command.
```
./medea_cli -i model.graphml -e model_export.graphml
> Setting up Controller:  [SUCCESS]
> Importing Project: "model.graphml" [SUCCESS]
> Exporting Project: "model_export.graphml" [SUCCESS]
```

### Combining multiple projects into a singular project
You can pass any number of -i flags into medea_cli, which will import and combine all models into one singular project.
```
./medea_cli -i model1.graphml -i model2.graphml -e model_combined.graphml
> Setting up Controller:  [SUCCESS]
> Importing Project: "model1.graphml" [SUCCESS]
> Importing Project: "model2.graphml" [SUCCESS]
> Exporting Project: "model_combined.graphml" [SUCCESS]
```

### Stripping visual data from project
Passing a -f flag to medea_cli will cause export to use a functional export. This strips any data deemed as visual only information. This shrinks the file size, and is useful for comparisons of functional changes to models
```
./medea_cli -i model.graphml -e model_novisual.graphml -f
> Setting up Controller:  [SUCCESS]
> Importing Project: "model.graphml" [SUCCESS]
> Exporting Project: "model_novisual.graphml" [SUCCESS]
```

# idl2graphml
The idl2graphml shell tool will load and interpret a standard corba IDL file into MEDEA's graphml representation. Ommitting an export flag will cause the model to be printed into standard output. All errors and warnings are printed into standard error

## Command line options
| Flag                                  | Description                           |
|---------------------------------------|---------------------------------------|
| -h, --help                            | Displays this help.                   |
| -i, --import <The idl file path>      | Import IDL projects.                   |
| -e, --export <The graphml file path>  | Export as a graphml file.             |

## Example Usage:
### Importing IDL file to .graphml file
To import an IDL file and export it to a a graphml file
```
./idl2graphml -i message.idl -e message.graphml
> IDL Parser: Parsing: 'message.idl'
> Successfully parsed 1 IDL files.
IDL Parser: Exported file 'message.graphml'
```

### Importing multiple IDL files to .graphml file
To import multiple IDL files, multiple -i flags can be added to your command.
```
./idl2graphml -i message.idl -i message2.idl -e message.graphml
> IDL Parser: Parsing: 'message.idl'
> IDL Parser: Parsing: 'message2.idl'
> Successfully parsed 2 IDL files.
IDL Parser: Exported file 'message.graphml'
```

### Importing IDL file
To import an IDL file and pipe its output, simply don't define a -e flag and use standard output piping. Errors/Warnings will still print to console.
```
./idl2graphml -i message.idl > OTHER_TOOL
> IDL Parser: Parsing: 'message.idl'
> Successfully parsed 1 IDL files.
```