niftyreg_eigen_assignment.patch: A patch to build the Eigen library that is included with NiftyReg, using Visual Studio 2015 or Visual Studio 2017.

Fixed the following Visual Studio 2017 compile error:
  \niftyreg\third-party\eigen\src/Core/SolveTriangular.h(189): error C2280: 'Eigen::Block<Eigen::Block<Eigen::Map<Derived,0,Eigen::Stride<0,0>>,-1,-1,false>,-1,-1,false> &Eigen::Block<Eigen::Block<Eigen::Map<Derived,0,Eigen::Stride<0,0>>,-1,-1,false>,-1,-1,false>::operator =(const Eigen::Block<Eigen::Block<Eigen::Map<Derived,0,Eigen::Stride<0,0>>,-1,-1,false>,-1,-1,false> &)': attempting to reference a deleted function
  
which was triggered by having an incompatible #define EIGEN_INHERIT_ASSIGNMENT_EQUAL_OPERATOR(Derived).

Newer versions of Eigen have a similar test if Visual C++ version < 1900 (i.e., before Visual Studio 2015): 
	#if EIGEN_COMP_MSVC_STRICT && (EIGEN_COMP_MSVC < 1900 ||  defined(__CUDACC_VER__)) // for older MSVC versions, as well as 1900 && CUDA 8, using the base operator is sufficient (cf Bugs 1000, 1324)
https://bitbucket.org/eigen/eigen/src/1ff051b7a3ae175d9a1dc345402d16fa685f5e9d/Eigen/src/Core/util/Macros.h?at=default&fileviewer=file-view-default#Macros.h-840

NiftyReg uses (an adaptation of) Eigen version 3.2.2 from https://bitbucket.org/eigen/eigen/commits/1306d75b4a21 (2014-08-04)

The test if Visual C++ version < 1900 is included with Eigen version 3.2.3 (2014-12-16): https://bitbucket.org/eigen/eigen/commits/36fd1ba04c12

Bug 920 - VC++ 2015 compiler error: deleted copy assignment operator for Block (and base classes) 
http://eigen.tuxfamily.org/bz/show_bug.cgi?id=920

Fixed in Eigen/src/Core/util/Macros.h by Gael Guennebaud (2014-12-18)
https://bitbucket.org/eigen/eigen/commits/84873315fb0417bcfcbe664913a7104b5de52cc0