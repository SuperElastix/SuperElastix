import argparse
import subprocess
import os
import SimpleITK as sitk
import numpy as np

from popi import POPI

def run(parameters):

    datasets = []

    if parameters.popi_input_directory is not None:
        datasets.append(POPI(parameters.popi_input_directory))

    blueprint_file_names = [os.path.join(parameters.submissions_directory, submission_name, file_name)
                            for submission_name in os.listdir(parameters.submissions_directory)
                            for file_name in os.listdir(os.path.join(parameters.submissions_directory, submission_name))
                            if file_name.endswith('.json') or file_name.endswith('.xml')]

    for blueprint_file_name in blueprint_file_names:
        blueprint_file_base_name = os.path.basename(blueprint_file_name)
        for dataset in datasets:
            for image_file_names, ground_truth_file_name, relative_output_file_name in dataset.data():
                try:
                    os.makedirs(os.path.join(parameters.output_directory, blueprint_file_base_name, os.path.dirname(relative_output_file_name)), exist_ok=True)
                    subprocess.call([parameters.registration_driver,
                                     blueprint_file_name,
                                     image_file_names[0],
                                     image_file_names[1],
                                     os.path.join(parameters.output_directory, blueprint_file_base_name, relative_output_file_name)])

                except Exception as e:
                    print(e)

    for bluerint_file_name in blueprint_file_names:
        blueprint_file_base_name = os.path.basename(bluerint_file_name)
        for dataset in datasets:
            for image_file_names, ground_truth_file_names, relative_output_file_name in dataset.data():
                try:
                    dataset.evaluate(image_file_names, ground_truth_file_names, relative_output_file_name)

                except Exception as e:
                    print(e)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='SuperBench registration driver.')

    parser.add_argument('--superelastix-executable', '-selx', required=True, help="Path to SuperElastix executable.")
    parser.add_argument('--registration-driver', '-rd', required=True, help="Program running actual registrations.")
    parser.add_argument('--submissions-directory', '-sd', required=True, help='Directory with parameter files.')
    parser.add_argument('--output-directory', '-od', required=True)
    parser.add_argument('--popi-input-directory', '-pid', help='Directory with POPI data.')

    parameters = parser.parse_args()
    run(parameters)

