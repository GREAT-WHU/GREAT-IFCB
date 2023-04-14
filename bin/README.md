# GREAT-IFCB: An open-source software for multi-GNSS inter-frequency clock bias estimation

## COMPILER ENVIRONMENT
  
* Windows: 
The executable CUI AP (application program) included in the package requires Microsoft Windows environment. The executable binary AP in the package was built by VS (Microsoft Visual Studio) 2017 on Windows 10 (64bit). 
      
* Linux:
The CUI AP and shared libraries for Linux were built and tested on CentOS Linux release 7.9.2009 and x64 CPU.


* Macintosh:
The CUI AP and dynamic libraries for Macintosh were built and tested by AppleClang 12.0.5 on MacOS Big Sur 11.6, in which kernel version is Darwin 20.5.0

## USAGE

1. To run GREAT-IFCB, please type the command line 
```
<install_dir>/GREAT-IFCB_<ver>/bin/<platform>/GREAT-IFCB -x IFCB_CONFIG.xml 
```

2. For more command line information, please type 
```
<install_dir>/GREAT-IFCB_<ver>/bin/<platform>/GREAT-IFCB --help 
```
 
3. NOTE: For the  Unix-like (Linux and Macintosh) operating system, before you run the executable AP, please enter the following command to load the relavant shared libraries in the current terminal
```
export LD_LIBRARY_PATH=<install_dir>/GREAT-IFCB_<ver>/bin/<platform>"
```
