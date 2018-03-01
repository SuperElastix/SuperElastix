import drmaa
import os

with drmaa.Session() as s:
    jt = s.createJobTemplate()
    
    #jt.joinFiles = True
    #jt.jobEnvironment = os.environ
    #jt.email = ""
    #jt.workingDirectory = os.path.abspath(os.curdir)
    jt.nativeSpecification = "-pe BWA 1 -l h_vmem=1g -cwd -j Y -V -m be"

    for dirpath, dirnames, filenames in os.walk('.'):
        for filename in filenames:
            if (os.path.splitext(filename)[1] == '.sh'):
                jt.jobName = os.path.splitext(filename)[0]
                command = os.path.join(dirpath,filename)
                jt.remoteCommand = command
                #jt.args = arguments
                job_id = s.runJob(jt)
                print("{0} got job_id {1}".format(command, job_id))
    s.deleteJobTemplate(jt)