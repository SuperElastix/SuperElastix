# What is this?
The `Submissions` directory contains all submissions for the [Continuous Registration Challenge](https://continuousregistration.grand-challenge.org/). The `Source` directory contains code for running the submissions on several different data sets.

# How to participate
Open a pull request that creates a directory under `Submissions` which contains your parameter files and a `README.md` file that describes your implementation.

# How to run registrations
`make_registration_scripts.py` in `Source` generate shell scripts or batch scripts for running the registrations. Each script will register one pair of images. The following command will generate shell scripts for all submissions for POPI, DIRLAB, and EMPIRE data sets:

   $ python make_registration_scripts.py
      --superelastix /path/to/superelastix/build/tree/SuperElastix-build/bin/superelastix
      --submissions-directory /path/to/superelastix/source/tree/ContinuousRegistration/Submissions
      --output-directory /tmp
      --popi-input-directory /path/to/POPI
      --dirlab-input-directory /path/to/DIRLAB
      --empire-input-directory /path/to/EMPIRE
      --lpba40-input-directory /path/to/LPBA40
      --isbr18-input-directory /path/to/ISBR18
      --mgh10-input-directory /path/to/MGH10
      --cumc12-input-directory /path/to/CUMC12

This generates shell scripts to running the registrations, one for each registration. For example, on Linux you can navigate to the output directory and run

    $ find . -type f -exec chmod +x "{}" \;
    $ find . -type f -exec "{}" \;

to make the scripts executable and execute them.