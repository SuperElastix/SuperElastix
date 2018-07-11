pipeline {
  agent {
      label 'local'
  }
  triggers {
      pollSCM('H/5 * * * *')
  }
  stages {
    stage('Checkout') {
      steps {
        sh 'mkdir -p src'
        dir('src') {
          checkout scm
        }
      }
    }
    stage('SuperBuild') {
      steps {
        timeout(time: 120, unit: 'MINUTES') {
          dir('build') {
            sh "ctest -VV --script ../src/SuperBuild/CTest.cmake"
          }
        }
      }

    }
    stage('Test') {
      steps {
        timeout(time: 30, unit: 'MINUTES') {
          dir('build/SuperElastix-build') {
            sh "ctest -VV --script ../../src/CTest.cmake"
          }
        }
      }
    }
    stage('Deploy') {
      when {
         branch 'develop'
      }
      steps {
        timeout(time: 10, unit: 'MINUTES') {
          dir('src') {
            sh '''
              echo "Deploy this source directory of develop on shark cluster"
              rsync -vr --delete ContinuousRegistration sa_lkeb@shark:/exports/lkeb-hpc/sa_lkeb/SuperElastix-source/
              scp -pr . sa_lkeb@shark:/exports/lkeb-hpc/sa_lkeb/SuperElastix-source/

              echo "Deploy this build of develop on shark cluster"
              rsync -vr --delete ContinuousRegistration sa_lkeb@shark:/exports/lkeb-hpc/sa_lkeb/SuperElastix-deployed/
              scp -p ../build/Applications-build/CommandLineInterface/SuperElastix sa_lkeb@shark:/exports/lkeb-hpc/sa_lkeb/SuperElastix-deployed/
              '''
          }
        }
      }
    }
  }
}
