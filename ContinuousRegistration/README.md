# What is this?
The `Submissions` directory contains all submissions for the [Continuous Registration Challenge](https://continuousregistration.grand-challenge.org/). The `Source` directory contains code for running the submissions on several different data sets.

# How to participate
Open a pull request that creates a directory under `Submissions` which contains your parameter files and a `README.md` file that describes your implementation.

# How to run registrations
`make_registration_scripts.py` in `Source` generate shell scripts or batch scripts for running the registrations. Each script will register one pair of images. The following command will generate shell scripts for all submissions for POPI, DIRLAB, and EMPIRE data sets:
```bash
python make_registration_scripts.py \
      --superelastix /path/to/superelastix/build/tree/SuperElastix-build/bin/superelastix \
      --submissions-directory /path/to/superelastix/source/tree/ContinuousRegistration/Submissions \
      --output-directory /tmp \
      --popi-input-directory /path/to/POPI \
      --dirlab-input-directory /path/to/DIRLAB \
      --empire-input-directory /path/to/EMPIRE \
      --lpba40-input-directory /path/to/LPBA40 \
      --ibsr18-input-directory /path/to/IBSR18 \
      --cumc12-input-directory /path/to/CUMC12 \
      --hbia-input-directory /path/to/HistoBIA
```

This generates shell scripts to running the registrations, one for each registration. For example, on Linux you can navigate to the output directory and run
```bash
find . -name "*.sh" -exec chmod +x "{}" \;
find . -name "*.sh" -exec "{}" \;
find . -name "*.sh" -exec echo "nohup {} &" \; > parallel.sh;
```

to recursively find scripts, make them executable, and run them.
