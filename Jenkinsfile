node('lkeb-vm-test') {
	stage('Init') {
		tool 'CMake 3.8.0'
	}

	timeout(45) {
		stage('Checkout and build') {
			checkout scm
			sh 'mkdir -p build'
			sh 'cd build'
			sh 'cmake ../SuperBuild'
			sh 'make'
		}
	}

	timeout(45) {
		stage('Test') {
		
		}
	}
}