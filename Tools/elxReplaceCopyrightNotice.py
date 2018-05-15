import sys
import os
import os.path
import fnmatch
import shutil
import glob

#-------------------------------------------------------------------------------
# the main function
# cd tools
# python elxReplaceCopyrightNotice.py
def main() :

    # The path to the source files relative to this script
    srcdir = os.path.join( "..")

    # Get a list of all files
    cpppatterns = ( '*.h', '*.hxx', '*.cxx', '*.cuh', '*.cu', '*.h.in' )
    cppmatches = []
    cmakepatterns = ( 'CMakeLists.txt', '*.cmake')
    cmakematches = []
    skipped = []
    
    for root, dirnames, filenames in os.walk( srcdir ):
        if ".git" in root:
            continue
        print(root)
        for filename in filenames:
            found = False
            for pattern in cpppatterns:
                if fnmatch.fnmatch(filename,pattern):
                    print("[cpp] " + filename)
                    cppmatches.append(os.path.join( root, filename ))
                    found = True
                    break
            if not found:
                for pattern in cmakepatterns:
                    if fnmatch.fnmatch(filename,pattern):
                        print("[cmake] " + filename)
                        cmakematches.append(os.path.join( root, filename ))                  
                        found = True
                        break
            if not found:
                print("[skip] " + filename)
                skipped.append(os.path.join( root, filename ))

    manualcheckfiles=[]
    with  open( "CppCopyrightNotice_Apache.txt" ) as noticefile:
        noticelines = noticefile.readlines()

        for filename in cppmatches:
            matchlines = None
            with open( filename, 'r' ) as matchfile:
                matchlines = matchfile.readlines()
                
            #scan for any other copyright usages
            skip = False
            for line in matchlines:
                if (("COPYRIGHT" in line) or ("Copyright" in line) or ("copyright" in line)):
                    manualcheckfiles.append(filename)
                    skip = True
                    #print("[copyright] " + filename)
            if skip==False:
                with open( filename, 'w') as overwritefile:
                    overwritefile.writelines(noticelines)
                    overwritefile.writelines(matchlines)

    with open( "CmakeCopyrightNotice_Apache.txt" ) as noticefile:
        noticelines = noticefile.readlines()

        for filename in cmakematches:
            matchlines = None
            with open( filename, 'r' ) as matchfile:
                matchlines = matchfile.readlines()
                
            #scan for any other copyright usages
            skip = False
            for line in matchlines:
                if (("COPYRIGHT" in line) or ("Copyright" in line) or ("copyright" in line)):
                    manualcheckfiles.append(filename)
                    skip = True                    
                    #print("[copyright] " + filename)
            if not skip:
                with open( filename, 'w') as overwritefile:
                    overwritefile.writelines(noticelines)
                    overwritefile.writelines(matchlines)
    print("Other Copyright notice found in (please check manually): ")
    print(manualcheckfiles)
      # Exit
    return 0

#-------------------------------------------------------------------------------


if __name__ == '__main__':
    main()
