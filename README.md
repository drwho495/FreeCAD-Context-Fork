# FreeCAD Context Fork (Old)
This is my small fork to get features into FreeCAD Link Stage 3 and to implement features to make my life easier.

### https://github.com/drwho495/FreeCAD-Context-Bundle/releases/tag/weekly-builds AppImages for the main branch are here now!
There will be a linkstage branch eventually.

This fork has the Ondsel Assembly Workbench built in now.



It also supports a new tool I made called the "assembly context creator". This lets you create assembly contexts in your part. This makes designing considerably easier. A gif will be provided soon.

![](https://github.com/drwho495/FreeCAD-Context-Fork/blob/LinkStable/context-assembly-example2.gif)

<details>
  <summary>Original FreeCAD Link Stage 3 ReadMe. Click to Expand.</summary>


![Logo](https://github.com/realthunder/FreeCADMakeImage/raw/master/conda/branding/asm3/icons/hicolor/64x64/apps/freecad_link.png)

This is the Link branch FreeCAD, with lots of new features and enhancement.
This branch is original created to develop the `App::Link` feature for use
with my [Assembly3](https://github.com/realthunder/FreeCAD_assembly3)
workbench. The feature has since been merged to official FreeCAD, but the
branch stayed and become a forefront of what future FreeCAD might become.

It's function now is to continuously improve and test the [Topological naming
problem](https://wiki.freecad.org/Topological_naming_problem) and it's [fix](https://github.com/realthunder/FreeCAD_assembly3/wiki/Topological-Naming).  
Which I eventually plan to merge back in to the FreeCAD main branch.

You can help test new features by trying one of the pre-built images
released [here](https://github.com/realthunder/FreeCAD_assembly3/releases).
There is also more frequent release in
[snap](https://snapcraft.io/freecad-realthunder) image for Linux folks to try
out.


<details>
  <summary>Original FreeCAD ReadMe. Click to Expand.</summary>

<a href="https://freecad.org"><img src="https://www.freecad.org/svg/icon-freecad.svg" height="100px" width="100px"></a>

### Your own 3D parametric modeler

[Website](https://www.freecad.org) • 
[Documentation](https://wiki.freecad.org) •
[Forum](https://forum.freecad.org/) •
[Bug tracker](https://github.com/FreeCAD/FreeCAD/issues) •
[Git repository](https://github.com/FreeCAD/FreeCAD) •
[Blog](https://blog.freecad.org)


[![Release](https://img.shields.io/github/release/freecad/freecad.svg)](https://github.com/freecad/freecad/releases/latest) [![Master][freecad-master-status]][gitlab-branch-master] [![Crowdin](https://d322cqt584bo4o.cloudfront.net/freecad/localized.svg)](https://crowdin.com/project/freecad) [![Gitter](https://img.shields.io/gitter/room/freecad/freecad.svg)](https://gitter.im/freecad/freecad?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge) [![Liberapay](https://img.shields.io/liberapay/receives/FreeCAD.svg?logo=liberapay)](https://liberapay.com/FreeCAD)

<img src="https://user-images.githubusercontent.com/1828501/174066870-1692005b-f8d7-43fb-a289-6d2f07f73d7f.png" width="800"/>

Overview
--------

* **Freedom to build what you want**  FreeCAD is an open-source parametric 3D 
modeler made primarily to design real-life objects of any size. 
Parametric modeling allows you to easily modify your design by going back into 
your model history to change its parameters. 

* **Create 3D from 2D and back** FreeCAD lets you to sketch geometry constrained
 2D shapes and use them as a base to build other objects. 
 It contains many components to adjust dimensions or extract design details from 
 3D models to create high quality production-ready drawings.

* **Designed for your needs** FreeCAD is designed to fit a wide range of uses
including product design, mechanical engineering and architecture,
whether you are a hobbyist, programmer, experienced CAD user, student or teacher.

* **Cross platform** FreeCAD runs on Windows, macOS and Linux operating systems.

* **Underlying technology**
    * **OpenCASCADE** A powerful geometry kernel, the most important component of FreeCAD
    * **Coin3D library** Open Inventor-compliant 3D scene representation model
    * **Python** FreeCAD offers a broad Python API
    * **Qt** Graphical user interface built with Qt


Installing
----------

Precompiled packages for stable releases are available for Windows, macOS and Linux on the
[Releases page](https://github.com/FreeCAD/FreeCAD/releases).

On most Linux distributions, FreeCAD is also directly installable from the 
software center application.

For development releases check the [weekly-builds page](https://github.com/FreeCAD/FreeCAD-Bundle/releases/tag/weekly-builds).

Other options are described at the [wiki Download page](https://wiki.freecad.org/Download).

Build Status
------------

| Master | 0.20 | Translation |
|:------:|:----:|:-----------:|
|[![Master][freecad-master-status]][gitlab-branch-master]|[![0.20][freecad-0.20-status]][gitlab-branch-0.20]|[![Crowdin](https://d322cqt584bo4o.cloudfront.net/freecad/localized.svg)](https://crowdin.com/project/freecad)|

[freecad-0.20-status]: https://gitlab.com/freecad/FreeCAD-CI/badges/releases/FreeCAD-0-20/pipeline.svg
[freecad-master-status]: https://gitlab.com/freecad/FreeCAD-CI/badges/master/pipeline.svg
[gitlab-branch-0.20]: https://gitlab.com/freecad/FreeCAD-CI/-/commits/releases/FreeCAD-0-20
[gitlab-branch-master]: https://gitlab.com/freecad/FreeCAD-CI/-/commits/master
[travis-builds]: https://travis-ci.org/FreeCAD/FreeCAD/builds

Compiling
---------

Compiling FreeCAD requires installation of several libraries and their 
development files such as OCCT (Open Cascade), Coin and Qt, listed in the 
pages below. Once this is done, FreeCAD can be compiled with 
CMake. On Windows, these libraries are bundled and offered by the 
FreeCAD team in a convenient package. On Linux, they are usually found 
in your distribution's repositories, and on macOS and other platforms, 
you will usually have to compile them yourself.

The pages below contain up-to-date build instructions:

- [Linux](https://wiki.freecad.org/Compile_on_Linux)
- [Windows](https://wiki.freecad.org/Compile_on_Windows)
- [macOS](https://wiki.freecad.org/Compile_on_MacOS)
- [Cygwin](https://wiki.freecad.org/Compile_on_Cygwin)
- [MinGW](https://wiki.freecad.org/Compile_on_MinGW)


Reporting Issues
---------

To report an issue please:

- First post to forum to verify the issue; 
- Link forum thread to bug tracker ticket and vice-a-versa; 
- Use the most updated stable or development versions of FreeCAD; 
- Post version info from eg. `Help > About FreeCAD > Copy to clipboard`; 
- Post a Step-By-Step explanation on how to recreate the issue; 
- Upload an example file to demonstrate problem. 

For more detail see:

- [Bug Tracker](https://github.com/FreeCAD/FreeCAD/issues)
- [Reporting Issues and Requesting Features](https://github.com/FreeCAD/FreeCAD/issues/new/choose)
- [Contributing](https://github.com/FreeCAD/FreeCAD/blob/master/CONTRIBUTING.md)
- [Help Forum](https://forum.freecad.org/viewforum.php?f=3)
- [Developers Handbook](https://freecad.github.io/DevelopersHandbook/)

The [FPA](https://fpa.freecad.org) offers developers the opportunity
to apply for a grant to work on projects of their choosing. Check
[jobs and funding](https://blog.freecad.org/jobs/) to know more.


Usage & Getting help
--------------------

The FreeCAD wiki contains documentation on 
general FreeCAD usage, Python scripting, and development. These 
pages might help you get started:

- [Getting started](https://wiki.freecad.org/Getting_started)
- [Features list](https://wiki.freecad.org/Feature_list)
- [Frequent questions](https://wiki.freecad.org/FAQ/en)
- [Workbenches](https://wiki.freecad.org/Workbenches)
- [Scripting](https://wiki.freecad.org/Power_users_hub)
- [Development](https://wiki.freecad.org/Developer_hub)

The [FreeCAD forum](https://forum.freecad.org) is also a great place
to find help and solve specific problems you might encounter when
learning to use FreeCAD.


<p>This project receives generous infrastructure support from
  <a href="https://www.digitalocean.com/">
    <img src="https://opensource.nyc3.cdn.digitaloceanspaces.com/attribution/assets/SVG/DO_Logo_horizontal_blue.svg" width="91px">
  </a> and <a href="https://www.kipro-pcb.com/">KiCad Services Corp.</a>
</p>

</details>
