import argparse
import logging
import os
import subprocess
import json

from Datasets.cumc12 import CUMC12
from Datasets.dirlab import DIRLAB
from Datasets.empire import EMPIRE
from Datasets.isbr18 import ISBR18
from Datasets.lbpa40 import LBPA40
from Datasets.mgh10 import MGH10
from Datasets.popi import POPI
from Datasets.spread import SPREAD

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


    if parameters.cumc12_input_directory is not None:
        cumc12 = CUMC12(parameters.mgh10_input_directory)
        datasets[cumc12.category].append(cumc12)
        logging.info('Found MGH10 ' + cumc12.category + ' dataset.')

    if parameters.dirlab_input_directory is not None:
        dirlab = DIRLAB(parameters.dirlab_input_directory)
        datasets[dirlab.category].append(dirlab)
        logging.info('Found EMPIRE ' + dirlab.category + ' dataset.')

    if parameters.empire_input_directory is not None:
        empire = EMPIRE(parameters.mgh10_input_directory)
        datasets[empire.category].append(empire)
        logging.info('Found EMPIRE ' + empire.category + ' dataset.')

    if parameters.isbr18_input_directory is not None:
        isbr18 = ISBR18(parameters.mgh10_input_directory)
        datasets[isbr18.category].append(isbr18)
        logging.info('Found MGH10 ' + isbr18.category + ' dataset.')

    if parameters.lbpa40_input_directory is not None:
        lbpa40 = LBPA40(parameters.mgh10_input_directory)
        datasets[lbpa40.category].append(lbpa40)
        logging.info('Found MGH10 ' + lbpa40.category + ' dataset.')

    if parameters.mgh10_input_directory is not None:
        mgh10 = MGH10(parameters.mgh10_input_directory)
        datasets[MGH10.category].append(mgh10)
        logging.info('Found MGH10 ' + mgh10.category + ' dataset.')

    if parameters.popi_input_directory is not None:
        popi = POPI(parameters.popi_input_directory)
        datasets[popi.category].append(popi)
        logging.info('Found POPI ' + popi.category + ' dataset.')

    if parameters.spread_input_directory is not None:
        spread = DIRLAB(parameters.spread_input_directory)
        datasets[spread.category].append(spread)
        logging.info('Found SPREAD ' + spread.category + ' dataset.')

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


