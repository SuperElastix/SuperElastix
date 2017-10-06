niftyreg_strtof.patch: A patch to build NiftyReg, using Visual Studio 2015 or Visual Studio 2017.

Fixed the following Visual Studio 2015 compilation error:
  1>c:\program files (x86)\microsoft visual studio 14.0\vc\include\xlocnum(77): error C2589: '(': illegal token on right side of '::'
  1>c:\program files (x86)\microsoft visual studio 14.0\vc\include\xlocnum(77): error C2059: syntax error: '::'
  1>c:\program files (x86)\microsoft visual studio 14.0\vc\include\string(307): error C2589: '(': illegal token on right side of '::'
  1>c:\program files (x86)\microsoft visual studio 14.0\vc\include\string(307): error C2059: syntax error: '::'

And the following Visual Studio 2017 compilation error:
  C:\Program Files (x86)\Windows Kits\10\include\10.0.10240.0\ucrt\stdlib.h(483): error C2059: syntax error: '('
  C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\INCLUDE\cstdlib(38): error C2039: 'strtof': is not a member of '`global namespace''
  C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\INCLUDE\cstdlib(38): error C2873: 'strtof': symbol cannot be used in a using-declaration
  C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\INCLUDE\string(307): error C2589: '(': illegal token on right side of '::'
  C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\INCLUDE\string(307): error C2059: syntax error: '::'

which were triggered by having a #define strtof before doing #include <string>, as reproduced at http://rextester.com/UJWL47770

It appears that the #define strtof(_s, _t) in reg-lib/cpu/_reg_maths.h was never actually used, so it could safely be removed.

