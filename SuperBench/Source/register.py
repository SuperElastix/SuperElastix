import argparse
import logging
import os
import subprocess
import json

from Datasets.popi import POPI

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
            for image_file_name_pair, ground_truth_file_name_pair, relative_deformation_field_file_name_pair in dataset.generator():
                try:
                    output_directory = os.path.join(parameters.output_directory,
                                                    team_name, blueprint_name,
                                                    os.path.dirname(relative_deformation_field_file_name_pair[0]))
                    output_file_name_0 = os.path.join(output_directory,
                                                      os.path.basename(relative_deformation_field_file_name_pair[0]))
                    output_file_name_1 = os.path.join(output_directory,
                                                      os.path.basename(relative_deformation_field_file_name_pair[1]))
                    os.makedirs(output_directory, exist_ok=True)

                    logging.info('Running blueprint %s for %s -> %s.',
                                 blueprint_file_name,
                                 image_file_name_pair[0],
                                 image_file_name_pair[1])
                    stdout = subprocess.check_output([parameters.registration_driver,
                                                      blueprint_file_name,
                                                      image_file_name_pair[0],
                                                      image_file_name_pair[1],
                                                      output_file_name_0])
                    logging.info(stdout)

                    logging.info('Running blueprint %s for %s <- %s.',
                                 blueprint_file_name,
                                 image_file_name_pair[1],
                                 image_file_name_pair[0])
                    stdout = subprocess.check_output([parameters.registration_driver,
                                                      blueprint_file_name,
                                                      image_file_name_pair[1],
                                                      image_file_name_pair[0],
                                                      output_file_name_1])
                    logging.info(stdout)
                except subprocess.CalledProcessError as e:
                    stderr = e.output
                    logging.error(stderr)


