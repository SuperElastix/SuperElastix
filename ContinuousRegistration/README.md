# What is this?
The `Submissions` directory contains all submissions for the [Continuous Registration Challenge](https://continuousregistration.grand-challenge.org/). The `Source` directory contains code for running the submissions on several different data sets.

# How to participate
Open a pull request that creates a directory under `Submissions` which contains your parameter files and a `README.md` file that describes your implementation.

# How to run registrations
`make_registration_scripts.py` in `Source` generate shell scripts or batch scripts for running the registrations. Each script will register one pair of images. The following command will generate shell scripts for all submissions for POPI, DIRLAB, and EMPIRE data sets:

   make_registration_scripts.py --superelastix /path/to/superelastix/build/tree/bin/superelastix
                                --submissions-directory /path/to/superelastix/source/tree/ContinuousRegistration/Submissions
                                --output-directory /tmp
                                --popi-input-directory /Users/kasper/Development/SuperBenchData/POPI
                                --dirlab-input-directory /Users/kasper/Development/SuperBenchData/DIR-LAB
                                --empire-input-directory /Users/kasper/Development/SuperBenchData/EMPIRE

On Linux, navigate to the output directory and run `find . -type f -exec chmod +x "{}" \;` to make the scripts executable, and `find . -type f -exec "{}" \;` to run all registrations.