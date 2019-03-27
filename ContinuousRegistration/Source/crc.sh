#!/bin/bash -eux
# This file serves as an example on how to run registrations locally on Linux and macOS
# using the Python scripts in this directory. Adapt the parameters to your own needs.

python --version

# Parameters
SUPERELASTIX_SOURCE_DIR=/Users/kasper/Development/SuperElastix
SUPERELASTIX_BUILD_DIR=/Users/kasper/Development/build/SuperElastixRelease
CRC_INPUT_DIR=/Users/kasper/Development/SuperBenchData
CRC_OUTPUT_DIR=/Users/kasper/Data/SuperBench
CRC_MASK_DIR=/Users/kasper/Development/SuperBenchMask
CRC_MAX_NUM_REG_PER_DATASET=8
CRC_DATASETS="--hammers-input-directory $CRC_INPUT_DIR/HAMMERS --popi-input-directory $CRC_INPUT_DIR/POPI/MedPhys11 --popi-mask-directory $CRC_MASK_DIR/POPI --dirlab-input-directory $CRC_INPUT_DIR/DIRLAB --dirlab-mask-directory $CRC_MASK_DIR/DIRLAB --spread-input-directory $CRC_INPUT_DIR/SPREAD  --mgh10-input-directory $CRC_INPUT_DIR/MGH10 --cumc12-input-directory $CRC_INPUT_DIR/CUMC12 --ibsr18-input-directory $CRC_INPUT_DIR/IBSR18 --lpba40-input-directory $CRC_INPUT_DIR/LPBA40"
CRC_BLUEPRINT_FILE_NAME="--blueprint-file-name NiftyRegAffineBSpline.json" # Leave empty to run registrations for all blueprints

pushd $SUPERELASTIX_SOURCE_DIR

# Generate shell scripts
python -m ContinuousRegistration.Source.make_registration_scripts --superelastix $SUPERELASTIX_BUILD_DIR/SuperElastix-build/bin/SuperElastix --submissions-directory $SUPERELASTIX_SOURCE_DIR/ContinuousRegistration/Submissions --output-directory $CRC_OUTPUT_DIR $CRC_DATASETS $CRC_BLUEPRINT_FILE_NAME --max-number-of-registrations-per-dataset $CRC_MAX_NUM_REG_PER_DATASET

# Register images serially
# find $CRC_OUTPUT_DIR -type f -name "*.sh" -exec "{}" \;

# Register images in parallel
find $CRC_OUTPUT_DIR -type f -name "*.sh" | parallel -j6 bash

# Evaluate registrations, generate visualizations, and generate leaderboard
python -m ContinuousRegistration.Source.make_evaluation --superelastix $SUPERELASTIX_BUILD_DIR/SuperElastix-build/bin/SuperElastix --submissions-directory $SUPERELASTIX_SOURCE_DIR/ContinuousRegistration/Submissions --output-directory $CRC_OUTPUT_DIR $CRC_DATASETS $CRC_BLUEPRINT_FILE_NAME --max-number-of-registrations-per-dataset $CRC_MAX_NUM_REG_PER_DATASET --make-image-checkerboards True --make-label-checkerboards True --make-difference-images True
python -m ContinuousRegistration.Source.make_leaderboard --superelastix $SUPERELASTIX_BUILD_DIR/SuperElastix-build/bin/SuperElastix --submissions-directory $SUPERELASTIX_SOURCE_DIR/ContinuousRegistration/Submissions --output-directory $CRC_OUTPUT_DIR $CRC_DATASETS $CRC_BLUEPRINT_FILE_NAME --max-number-of-registrations-per-dataset $CRC_MAX_NUM_REG_PER_DATASET

popd
