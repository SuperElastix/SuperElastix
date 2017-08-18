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
		tool 'CMake 3.5.1'
		//tool 'CTest 3.5.1' does not exist, TODO find proper way
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
		stage('Build') {
			dir('build') {
				sh "${ tool 'CMake 3.5.1' } ../src/SuperBuild"
				sh 'make clean'
				sh 'make -j4'
			}
		}
		dir('src') {
			updateGithubCommitStatus(currentBuild)
		}
	}

	timeout(45) {
		stage('Test') {
			dir('build/SuperElastix-build') {
				sh '../../../../tools/cmake/bin/ctest' //tool 'CTest 3.5.1' does not exist, TODO find proper way
			}
		}
	}
}
