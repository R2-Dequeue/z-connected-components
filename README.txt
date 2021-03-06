Note: This project requires the GiNaC, CLN, and GMP libraries.

Connected Components
	In the fall of 2010 I took a Computer Algebra course during which we slowly built up a Maple program to calculate the different partitions in the plane that the graphs of arbitrary bivariate polynomials made.  The polynomials where over the rational numbers using infinite precision integers for numerators and denominators.
	I wanted to write a program that solved the same problem, but written in C++ and in true OO fashion, making good use of classes, namespaces, the STL, and template classes/functions.  I used the Boost C++ libraries and some of the new features in the Technical Report 1 (TR1) extensions to the C++ standard.  I used the GCC (G++) compiler (I wanted to use, and have copies of, Visual C++ but several of the libraries I needed would require significant work before they would compile under VC++).  Debugging was done with GDB.
	The development work was done with the Code::Blocks IDE on a WinXP desktop and a Mac OS X laptop. I used subversion for revision control and backup.  I used assebla (assembla.com) as my repository server with TortiseSVN as my Windows client and the command line svn client on Mac.
	I wanted to document my code well and keep the documentation inline with the source code, so I decided on using doxygen to generate the documentation from the source.  The actual comments in the code are QT-style (as opposed to JavaDoc-style comments) because I was considering using QT for a graphical layer and thought there might be value in being able to use doxygen or the QT documentation generator.
	I wanted to use infinite precision arithmetic so I needed extra libraries.  After research I decided on using the GiNaC library for general unlimited-precision computation.  GiNaC itself uses a lower level number library called the Class Library for Numbers (CLN).  CLN itself can use a very low level library that is written mostly in assembly language called the GNU Multi Precision (GMP) library.  GMP is used by many computational software packages like Mathematica and Maple.
	GUI using the native GUI toolkit wxWidgets (formerly wxWindows).




