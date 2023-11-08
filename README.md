# Net Yaroze Demo
Net Yaroze SCEE source code demos and tutorials.

Source code demos and tutorials from:
https://archive.org/details/webyaroze.7z

Documentation from:
https://archive.org/details/net-yaroze-documents/

# How to

First, your [Net Yaroze toolchain](http://netyaroze.com/Development/Toolchain) should be set up and working. You should have a basic understanding of:
1) Makefiles
2) GNU C compiler and linker
3) ANSI C programming language
4) Operating System command line interface

Start by reading the [User Guide doc](https://github.com/gwald/netyaroze_demo/blob/main/documents/020-1997-userguide.pdf), it explains PlayStation hardware and Net Yaroze game development in general terms.


Follow the [tutorials](https://github.com/gwald/netyaroze_demo/tree/main/tutor), start with chapter 1, check the [Reference](https://github.com/gwald/netyaroze_demo/blob/main/documents/030-1997-libraryref.pdf) doc for function and structure description.
The tuts add new features to the previous one. It builds and builds, so you have to understand the previous examples before continuing.
Make sure you can build each tut, run them and change things ect.


To create a PS1 executable (psx.exe), run [yarexe](https://github.com/gwald/yarexe) with the Siocons script text file (it contains a series of load commands, and commonly called auto or batch).

After going through the User Guide, Tutorials and API Reference document, you should be familiar enough with the API that you can create your own programs. The [demos](https://github.com/gwald/netyaroze_demo/tree/main/demos ) folder contains more examples.
