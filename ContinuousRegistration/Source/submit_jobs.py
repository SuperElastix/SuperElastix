import drmaa
import os


def main():
    with drmaa.Session() as s:
        joblist = []
        jt = s.createJobTemplate()

        #jt.joinFiles = True
        #jt.jobEnvironment = os.environ
        #jt.email = ""
        #jt.workingDirectory = os.path.abspath(os.curdir)
        jt.nativeSpecification = "-pe BWA 4 -l h_vmem=5g -cwd -j Y -V"

        for dirpath, dirnames, filenames in os.walk('.'):
            for filename in filenames:
                if (os.path.splitext(filename)[1] == '.sh'):
                    jt.jobName = os.path.splitext(filename)[0]
                    command = os.path.join(dirpath,filename)
                    jt.remoteCommand = command
                    #jt.args = arguments
                    job_id = s.runJob(jt)
                    print("{0} got job_id {1}".format(command, job_id))
                    joblist.append(job_id)
        s.deleteJobTemplate(jt)

        s.synchronize(joblist, drmaa.Session.TIMEOUT_WAIT_FOREVER, False)
        for curjob in joblist:
            print('Collecting job ' + curjob)
            retval = s.wait(curjob, drmaa.Session.TIMEOUT_WAIT_FOREVER)
            print('Job: {0} finished with status {1}'.format(retval.jobId,
                                                         retval.hasExited))


if __name__ == '__main__':
    main()
