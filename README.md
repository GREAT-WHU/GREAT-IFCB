# GREAT-IFCB: An open-source software for multi-GNSS inter-frequency clock bias estimation

## OVERVIEW

GREAT-IFCB was developed for multi-GNSS inter-frequency clock bias (IFCB) estimation. The open-source GREAT-IFCB software is written in C++ 11 language following the Object-Oriented principle and can compile, run on several popular operating systems, such as Windows, Linux, and Macintosh. It consists of three portable program libraries named libIFCB, libMat and libGnut. The libIFCB library is mainly in charge of estimating IFCB, including the encoding, decoding, and storage of auxiliary data involved in the IFCB estimation, as well as the implementation of IFCB algorithm. As for LibMat and libGnut, they are auxiliary libraries for the software. LibMat is an open-source C++ matrix library named newmat, which offers standard matrix operations. LibGnut library comes from the open-source GNSS software G-Nut, including the decoding and storage of GNSS data as well as basic parameter configuration module. The features of GREAT-IFCB are:

1. It supports multi-GNSS IFCB estimation for: GPS (G), Galileo (E), BeiDou (C) satellites. 

2. It supports IFCB estimation with multi-frequency specified observations.

3. All of the executable binary APs (application programs) for Windows/Linux/Macintosh are included in the package as well as the whole source programs of the libraries and AP.                                                           

4. Adopting the open-source, cross-platform compilation tool CMake, which is convenient for users to customize their own executable APs.


## DIRECTORY STRUCTURE OF PACKAGE
```
GREAT-IFCB_<ver>	
  ./bin	                The executable binary APs for Windows/Linux/Macintosh *
  ./src	                Source programs of GREAT-IFCB software *
    ./app	                Main function of IFCB estimation *
    ./LibIFCB	            Source programs of IFCB estimation library *
    ./LibMat	            Source programs of the newmat library *
    ./LibGnut	            Source programs of the G-Nut library *
  ./sample_data         Sample data for AP *
    ./IFCB_2021003	      Sample data for GREAT-IFCB AP *
  ./util	              Utilities *
    ./PythonScripts	      Python scripts for IFCB estimation *
    ./PreEdit	            Cycle slip detection tool & Sample data *
  ./doc	                Document files *
    IFCB_CONFIG.xml	      Sample XML files for GREAT-IFCB *
  GREAT-IFCB_1.0.pdf	  User manual
```

## INSTALLATION AND USAGE

1. Get CMake via the website (https://cmake.org/download/) and install it. Note that the minimum requirement of CMake version is 3.0.0.

2. Download GREAT-IFCB from https://geodesy.noaa.gov/gps-toolbox
 
3. Extract the program package GREAT-IFCB_<ver>.zip or GREAT-IFCB_<ver>.tgz to appropriate directory <install_dir>. 
```
gzip -d GREAT-IFCB_<ver>.zip or tar xzf GREAT-IFCB_<ver>.tgz
```
                   
4. Run the cmake executable or the cmake-gui to configure the project and then build it with your chosen build tool. 
   
For Linux/Macintosh: 

Creat "build" directory in the directory <install_dir>/GREAT-IFCB_<ver> and then run the following commands to compile the source code. The executable AP can be found in <install_dir>/GREAT-IFCB_<ver>/build/Bin.
   
```
mkdir <install_dir>/GREAT-IFCB_<ver>/build
cd <install_dir>/GREAT-IFCB_<ver>/build
cmake ../
make
```
   
For Windows: 

Drag CMakeList.txt in the directory <install_dir>/GREAT-IFCB_<ver> to the interface of CMake-gui and then perform the following operations.
   * Modify attribute "Where to build the binaries" as <install_dir>/GREAT-IFCB_<ver>/build
   * Click "Configure" button.
   * Choose the Integrated Development Environment (IDE) for the project. (appears only the first time you click "Configure" button)
   * Click "Generate" button.
   * Click "Open Project" button and compile source code.
   

5. Run the program (in GREAT-IFCB_<ver>/build/Bin) with XML config

```
<install_dir>/GREAT-IFCB_<ver>/build/Bin/<pgm> -x IFCB_CONFIG.xml
```


## MANUAL

Refer GREAT-IFCB_<ver>/doc/GREAT-IFCB_<ver>.pdf.


## CONTRIBUTOR

Core developers:

* Xingxing Li (xxli@sgg.whu.edu.cn)
* Hongjie Zheng (hjzheng@whu.edu.cn)
* Xin Li (lixinsgg@whu.edu.cn)
* Guolong Feng (glfeng@whu.edu.cn)

Third party libraries:

* GREAT-IFCB uses G-Nut library (http://www.pecny.cz).
  G-Nut is Copyright (C) 2011-2016 GOP - Geodetic Observatory Pecny, RIGTC.
  
* GREAT-IFCB uses pugixml library (http://pugixml.org).
  pugixml is Copyright (C) 2006-2014 Arseny Kapoulkine.

* GREAT-IFCB uses Newmat library (http://www.robertnz.net/nm_intro.htm)
  Newmat is  Copyright (C) 2008: R B Davies.


## COPYRIGHT AND LICENSE

Copyright (C) 2018-2020, Wuhan University. All rights reserved.
Licensed under the GNU General Public License Version 3.
See LICENSE details.


