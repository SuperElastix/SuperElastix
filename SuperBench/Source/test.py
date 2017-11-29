import argparse
import logging
import os
import subprocess
import json

from popi import POPI

def load_submissions(parameters):
    logging.info('Loading blueprints ...')
    submissions = [(team_name, os.path.join(parameters.submissions_directory, team_name, file_name))
                            for team_name in os.listdir(parameters.submissions_directory)
                            for file_name in os.listdir(os.path.join(parameters.submissions_directory, team_name))
                            if file_name.endswith('.json') or file_name.endswith('.xml')]

    for team_name, blueprint_file_name in submissions:
        logging.info('Found %s by %s.', blueprint_file_name, team_name)

    return submissions


def load_datasets(parameters):
    logging.info('Loading datasets ...')
    datasets = {
        'Lung': [],
        'Brain': []
    }

    if parameters.popi_input_directory is not None:
        logging.info('Found POPI lung dataset.')
        datasets['Lung'].append(POPI(parameters.popi_input_directory))

    return datasets

def get_category(blueprint_file_name):
    blueprint = json.load(open(blueprint_file_name))
    return blueprint.get('SuperBenchCategory')


def run(parameters):
    submissions = load_submissions(parameters)
    datasets = load_datasets(parameters)

    for team_name, blueprint_file_name in submissions:
        blueprint_name, blueprint_ext = os.path.splitext(os.path.basename(blueprint_file_name))
        category = get_category(blueprint_file_name)
        for dataset in datasets[category]:
            for image_file_names, ground_truth_file_name, relative_output_file_name in dataset.generator():
                try:
                    output_directory = os.path.join(parameters.output_directory, team_name, blueprint_name, os.path.dirname(relative_output_file_name))
                    output_file_name = os.path.join(output_directory, os.path.basename(relative_output_file_name))

                    os.makedirs(output_directory, exist_ok=True)

                    logging.info('Running %s %s registration with blueprint %s (%s -> %s).',
                                 dataset.name,
                                 str.lower(category),
                                 blueprint_file_name,
                                 image_file_names[0],
                                 image_file_names[1])
                    stdout = subprocess.check_output([parameters.registration_driver,
                                                      blueprint_file_name,
                                                      image_file_names[0],
                                                      image_file_names[1],
                                                      output_file_name])

                    logging.info(stdout)
                except subprocess.CalledProcessError as e:
                    stderr = e.output
                    logging.error(stderr)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='SuperBench registration driver.')

    parser.add_argument('--registration-driver', '-rd', required=True, help="Program running registrations.")
    parser.add_argument('--submissions-directory', '-sd', required=True, help='Directory with parameter files.')
    parser.add_argument('--output-directory', '-od', required=True)
    parser.add_argument('--popi-input-directory', '-pid')

    parameters = parser.parse_args()
    run(parameters)

