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

    parameter_file_names = [os.path.join(parameters.parameter_file_directory, parameter_file_name)
                            for parameter_file_name
                            in os.listdir(parameters.parameter_file_directory)
                            if parameter_file_name.endswith('.json')
                            or parameter_file_name.endswith('.xml')]

    for parameter_file_name in parameter_file_names:
        parameter_file_base_name = os.path.basename(parameter_file_name)
        if not parameter_file_base_name.startswith('elastix'): continue  # DBG
        for dataset in datasets:
            for image_file_names, point_set_file_names, relative_output_file_name in dataset.data():
                try:
                    os.makedirs(os.path.join(parameters.output_directory, parameter_file_base_name, os.path.dirname(relative_output_file_name)), exist_ok=True)
                    subprocess.call([parameters.registration_driver,
                                     parameter_file_name,
                                     image_file_names[0],
                                     image_file_names[1],
                                     os.path.join(parameters.output_directory, parameter_file_base_name, relative_output_file_name)])
                except Exception as e:
                    # TODO: Log to user
                    print(e)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='SuperBench registration driver.')

    # Data sets
    parser.add_argument('--registration-driver', '--rd', required=True, help="Program running actual registrations.")
    parser.add_argument('--parameter-file-directory', '-pfd', required=True, help='Directory with parameter files.')
    parser.add_argument('--output-directory', '-od', required=True)
    parser.add_argument('--popi-input-directory', '-pid', help='Directory with POPI data.')

    parameters = parser.parse_args()
    run(parameters)

