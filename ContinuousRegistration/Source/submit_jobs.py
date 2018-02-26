# -*- coding: utf-8 -*-
"""
Created on Mon Feb 26 15:15:53 2018

@author: fberendsen
"""

import drmaa
import os

def main():
    """
    Submit a job and wait for it to finish.
    Note, need file called sleeper.sh in home directory.
    """
    with drmaa.Session() as s:
        
        command = "TeamIdentityTransform/IdentityTransform/sh/CUMC12_m2_to_m10.sh"
        arguments = ""
        
        jobid = send_job(s, command, arguments, queue="all.q", walltime=None,
                 job_name=None, memory="4g", ncores=4, joinLogFiles=True,
                 outputLog=None, errorLog=None, hold_job=None, hold=False)

        retval = s.wait(jobid, drmaa.Session.TIMEOUT_WAIT_FOREVER)
        print('Job: {0} finished with status {1}'.format(retval.jobId, retval.hasExited))

        print('Cleaning up')
        s.deleteJobTemplate(jt)

if __name__=='__main__':
    main()
    

# Copyright 2011-2014 Biomedical Imaging Group Rotterdam, Departments of
# Medical Informatics and Radiology, Erasmus MC, Rotterdam, The Netherlands
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

GE_NATIVE_SPEC = {
        'CWD': '-cwd',
        'QUEUE': '-q {queue}',
        'WALLTIME': '-l h_rt={walltime}',
        'MEMORY': '-l h_vmem={memory}',
        'NCORES': '-pe smp {ncores:d}',
        'OUTPUTLOG': '-o {outputlog}',
        'ERRORLOG': '-e {errorlog}',
        'DEPENDS': '-hold_jid {hold_list}',
        'DEPENDS_SEP': ',',
        'HOLD': '-h',
    }



def create_native_spec(queue, walltime, memory, ncores, outputLog,
                       errorLog, hold_job, hold):
        """
        Create the native spec for the DRMAA scheduler. Needs to be implemented
        in the subclasses

        :param str queue: the queue to submit to
        :param str walltime: walltime specified
        :param str memory: memory requested
        :param int ncores: number of cores requested
        :param str outputLog: the location of the stdout log
        :param str errorLog: the location of stderr log
        :param list hold_job: list of jobs to depend on
        :param bool hold: flag if job should be submitted in hold mode
        :return:
        """
        native_spec = []

        native_spec.append(GE_NATIVE_SPEC['CWD'].format(os.path.abspath(os.curdir)))
        native_spec.append(GE_NATIVE_SPEC['QUEUE'].format(queue=queue))

        if walltime is not None:
            native_spec.append(GE_NATIVE_SPEC['WALLTIME'].format(walltime=walltime))

        if memory is not None:
            native_spec.append(GE_NATIVE_SPEC['MEMORY'].format(memory=memory))

        if ncores is not None:
            native_spec.append(GE_NATIVE_SPEC['NCORES'].format(ncores=ncores))

        if outputLog is not None:
            native_spec.append(GE_NATIVE_SPEC['OUTPUTLOG'].format(outputlog=outputLog))

        if errorLog is not None:
            native_spec.append(GE_NATIVE_SPEC['ERRORLOG'].format(errorlog=errorLog))

        if hold_job is not None:
            if isinstance(hold_job, int):
                native_spec.append(GE_NATIVE_SPEC['DEPENDS'].format(hold_list=hold_job))
            elif isinstance(hold_job, list) or isinstance(hold_job, tuple):
                if len(hold_job) > 0:
                    jid_list = GE_NATIVE_SPEC['DEPENDS_SEP'].join([str(x) for x in hold_job])
                    native_spec.append(GE_NATIVE_SPEC.format(hold_list=jid_list))
            else:
                print('Incorrect hold_job type!')

        if hold:
            # Add a user hold to the job
            native_spec.append(self.spec_fields['HOLD'])

        return ' '.join(native_spec)
        
            # FIXME This needs to be more generic! This is for our SGE cluster only!

    def send_job(session, command, arguments, queue=None, walltime=None,
                 job_name=None, memory=None, ncores=None, joinLogFiles=False,
                 outputLog=None, errorLog=None, hold_job=None, hold=False):

        # Create job template
        jt = session.createJobTemplate()
        jt.remoteCommand = command

        jt.args = arguments
        jt.joinFiles = joinLogFiles
        env = os.environ
        # Make sure environment modules do not annoy use with bash warnings
        # after the shellshock bug was fixed
        env.pop('BASH_FUNC_module()', None)
        env['PBS_O_INITDIR'] = os.path.abspath(os.curdir)
        jt.jobEnvironment = env

        if queue is None:
            queue = "all.q"

        # Get native spec from subclass
        native_spec = create_native_spec(
            queue=queue,
            walltime=walltime,
            memory=memory,
            ncores=ncores,
            outputLog=outputLog,
            errorLog=errorLog,
            hold_job=hold_job,
            hold=hold
        )

        print('Setting native spec to: {}'.format(native_spec))
        jt.nativeSpecification = native_spec
        if job_name is None:
            job_name = command
            job_name = job_name.replace(' ', '_')
            job_name = job_name.replace('"', '')
            if len(job_name) > 32:
                job_name = job_name[0:32]

        jt.jobName = job_name

        # Send job to cluster
        job_id = session.runJob(jt)

        # Remove job template
        session.deleteJobTemplate(jt)

        return job_id