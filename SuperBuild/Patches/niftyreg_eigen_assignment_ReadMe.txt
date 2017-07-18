niftyreg_eigen_assignment.patch: A patch to build the Eigen library that is included with NiftyReg, using Visual Studio 2015 or Visual Studio 2017.

Fixed the following Visual Studio 2017 compile error:
  \niftyreg\third-party\eigen\src/Core/SolveTriangular.h(189): error C2280: 'Eigen::Block<Eigen::Block<Eigen::Map<Derived,0,Eigen::Stride<0,0>>,-1,-1,false>,-1,-1,false> &Eigen::Block<Eigen::Block<Eigen::Map<Derived,0,Eigen::Stride<0,0>>,-1,-1,false>,-1,-1,false>::operator =(const Eigen::Block<Eigen::Block<Eigen::Map<Derived,0,Eigen::Stride<0,0>>,-1,-1,false>,-1,-1,false> &)': attempting to reference a deleted function

Newer versions of Eigen have a similar test if Visual C++ version < 1900: 
https://bitbucket.org/eigen/eigen/src/1ff051b7a3ae175d9a1dc345402d16fa685f5e9d/Eigen/src/Core/util/Macros.h?at=default&fileviewer=file-view-default#Macros.h-840
