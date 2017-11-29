import argparse
import logging
import os
import subprocess

from test import load_submissions, load_datasets, get_category
from popi import POPI

def run(parameters):
    submissions = load_submissions(parameters)
    datasets = load_datasets(parameters)

    for team_name, blueprint_file_name in submissions:
        blueprint_name, blueprint_ext = os.path.splitext(os.path.basename(blueprint_file_name))
        category = get_category(blueprint_file_name)
        results = dict()
        for dataset in datasets[category]:
            results[dataset.name] = dict()
            for image_file_names, ground_truth_file_names, relative_output_file_name in dataset.generator():
                output_directory = os.path.join(parameters.output_directory, team_name, blueprint_name, os.path.dirname(relative_output_file_name))
                output_file_name = os.path.join(output_directory, relative_output_file_name)
                results[dataset.name][relative_output_file_name] = dataset.evaluate(parameters.superelastix, image_file_names, ground_truth_file_names, output_file_name)

    # TODO: Write results to disk
    print(results)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='SuperBench registration driver.')

    parser.add_argument('--superelastix', '-selx', required=True, help="SuperElastix executable.")
    parser.add_argument('--submissions-directory', '-sd', required=True, help='Directory with parameter files.')
    parser.add_argument('--output-directory', '-od', required=True)
    parser.add_argument('--popi-input-directory', '-pid')

    parameters = parser.parse_args()
    run(parameters)

