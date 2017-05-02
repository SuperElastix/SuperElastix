node('lkeb-vm-test') {
	stage('Init') {
		tool 'CMake 3.8.0'
		sh 'mkdir -p build'
	}

	timeout(120) {
		dir('build') {
			stage('Checkout and build') {
				checkout scm
				sh "${ tool 'CMake 3.8.0' } ../SuperBuild"
				sh 'make -j4'
			}
		}
	}

	timeout(45) {
		stage('Test') {
		
		}
	}
}