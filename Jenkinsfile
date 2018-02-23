def getRepoURL() {
  sh "git config --get remote.origin.url > .git/remote-url"
  return readFile(".git/remote-url").trim()
}
 
def getCommitSha() {
  sh "git rev-parse HEAD > .git/current-commit"
  return readFile(".git/current-commit").trim()
}
 
def updateGithubCommitStatus(build) {
  // workaround https://issues.jenkins-ci.org/browse/JENKINS-38674
  repoUrl = getRepoURL()
  commitSha = getCommitSha()
 
  step([
    $class: 'GitHubCommitStatusSetter',
    reposSource: [$class: "ManuallyEnteredRepositorySource", url: repoUrl],
    commitShaSource: [$class: "ManuallyEnteredShaSource", sha: commitSha],
    errorHandlers: [[$class: 'ShallowAnyErrorHandler']],
    statusResultSource: [
      $class: 'ConditionalStatusResultSource',
      results: [
        [$class: 'BetterThanOrEqualBuildResult', result: 'SUCCESS', state: 'SUCCESS', message: build.description],
        [$class: 'BetterThanOrEqualBuildResult', result: 'FAILURE', state: 'FAILURE', message: build.description],
        [$class: 'AnyBuildResult', state: 'FAILURE', message: 'Loophole']
      ]
    ]
  ])
}


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
        // sh "`dirname ${ cmake }`/ctest --script ../src/SuperBuild/CTest.cmake"
      }
    }
    stage('Test') {
      dir('build/SuperElastix-build') {
        // sh "`dirname ${ cmake }`/ctest --script ../../src/CTest.cmake"
      }
    }
    stage('Deploy') {
      GIT_BRANCH = sh(returnStdout: true, script: 'git rev-parse --abbrev-ref HEAD').trim()
      when {
        expression { GIT_BRANCH=='SELX-172-Deploy-develop-on-shark'
      }
      steps {
        sh "scp -r src sa_lkeb@shark:~/SuperElastix"
      }
    }
    dir('src') {
      updateGithubCommitStatus(currentBuild)
    }
  }
}
