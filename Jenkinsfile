node('lkeb-vm-test') {
  stage('Init') {
    cmake = tool 'CMake 3.5.1'
    sh 'rm -rf build'
    sh 'mkdir -p build'
  }

  timeout(120) {
    stage('Checkout') {
      sh 'mkdir -p src'
      dir('src') {
        checkout scm
      }
    }
    stage('SuperBuild') {
      dir('build') {
        sh "`dirname ${ cmake }`/bin/ctest -VV --script ../src/SuperBuild/CTest.cmake"
      }
    }
    stage('Test') {
      dir('build/SuperElastix-build') {
        sh "`dirname ${ cmake }`/bin/ctest -VV --script ../../src/CTest.cmake"
      }
    }
    stage('Deploy') {
      dir('src') {
        sh '''
            GIT_BRANCH=$(git name-rev --name-only HEAD)
            echo $GIT_BRANCH
            if [ "$GIT_BRANCH" = "remotes/origin/develop" ]
            then
              echo "Deploy this build of develop on shark cluster"
              rsync -vr --delete ContinuousRegistration sa_lkeb@shark:/exports/lkeb-hpc/sa_lkeb/SuperElastix-deployed/
              scp -p ../build/Applications-build/CommandLineInterface/SuperElastix sa_lkeb@shark:/exports/lkeb-hpc/sa_lkeb/SuperElastix-deployed/
            else
              echo "This is not the develop branch, thus do not deploy"
            fi
        '''
      }
    }
  }
}
