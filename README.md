Fabric Splice Standalone
===================================
A Fabric Splice C++ Standalone

The Fabric Splice Standalone is an example of how to build a minimalistic C++ standalone utilizing Fabric Splice.

Repository Status
=================

This repository will be maintained and kept up to date by Fabric Software to match the latest Fabric Core / Fabric Splice.

Supported platforms
===================

To date all three major platforms (windows, linux, osx) are supported, if you build the thirdparty dependencies for the corresponding platform.

Building
========

A scons (http://www.scons.org/) build script is provided. Fabric Splice Standalone depends on
* A static build of boost (http://www.boost.org/), version 1.55 or higher.
* A build of Qt 4.8+ (for Windows (http://www.alphapixel.com/content/download-qt-binaries-free), system libs on linux / osx)
* A dynamic build of Fabric Core (matching the latest version).
* The SpliceAPI repository checked out one level above (http://github.com/fabric-engine/SpliceAPI)
* The FTL (Fabric Template Library) repository (it can be pulled here: https://github.com/fabric-engine/FTL).

Fabric Splice Standalone requires a certain folder structure to build properly. You will need to have the SpliceAPI cloned as well on your drive, as such:

    SpliceAPI
    Applications/SpliceStandalone

You can use the bash script below to clone the repositories accordingly:

    git clone git@github.com:fabric-engine/SpliceAPI.git
    mkdir Applications
    cd Applications
    git clone git@github.com:fabric-engine/SpliceStandalone.git
    cd SpliceStandalone
    scons

To inform scons where to find the Fabric Core includes as well as the thirdparty libraries, you need to set the following environment variables:

* FABRIC_BUILD_OS: Should be the type of OS you are building for (Windows, Darwin, Linux)
* FABRIC_BUILD_ARCH: The architecture you are building for (x86, x86_64)
* FABRIC_BUILD_TYPE: The optimization type (Release, Debug)
* FABRIC_SPLICE_VERSION: Refers to the version you want to build. Typically the name of the branch (for example 1.13.0)
* FABRIC_DIR: Should point to Fabric Engine's Core folder.
* BOOST_DIR: Should point to the boost root folder (containing boost/ (includes) and lib/ for the static libraries).
* QT_INCLUDE_DIR: The include folder of the Qt installation.
* QT_LIB_DIR: The lib folder of the Qt installation.
* QT_BIN_DIR: The bin folder of the Qt installation. 

The temporary files will be built into the *.build* folder, while the structured output files will be placed in the *.stage* folder.

To perform a build you can just run

    scons all -j8

To clean the build you can run

    scons clean

License
==========

The license used for the Fabric Splice Standalone can be found in the root folder of this repository.
