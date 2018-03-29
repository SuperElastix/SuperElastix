import argparse
import logging
import os
import json

from datasets import CUMC12, DIRLAB, EMPIRE, ISBR18, LPBA40, MGH10, POPI, SPREAD

parser = argparse.ArgumentParser(description='Continuous Registration Challenge command line interface.')

parser.add_argument('--superelastix', '-selx', required=True, help="Path to SuperElastix executable.")
parser.add_argument('--submissions-directory', '-sd', required=True, help='Directory with parameter files.')
parser.add_argument('--output-directory', '-od', required=True, help="Directory where results will be saved.")
parser.add_argument('--make-shell-scripts', '-mss', type=bool, default=True, help="Generate shell scripts (default: True).")
parser.add_argument('--make-batch-scripts', '-mbs', type=bool, default=False, help="Generate shell scripts (default: False).")

parser.add_argument('--cumc12-input-directory', '-cid')
parser.add_argument('--dirlab-input-directory', '-did')
parser.add_argument('--empire-input-directory', '-eid')
parser.add_argument('--isbr18-input-directory', '-iid')
parser.add_argument('--lpba40-input-directory', '-lid')
parser.add_argument('--spread-input-directory', '-sid')
parser.add_argument('--popi-input-directory', '-pid')
parser.add_argument('--mgh10-input-directory', '-mid')

parser.add_argument('--team-name', '-tn', help="If specified, only generated shell scripts for this team.")
parser.add_argument('--blueprint-file-name', '-bfn', help="If specified, only generated shell scripts for this blueprint.")

parser.add_argument('--max-number-of-registrations-per-dataset', '-mnorpd', type=int, default=64)

logging.basicConfig(level=logging.INFO)

def load_submissions(parameters):
    logging.info('Loading blueprints ...')

    submissions = dict()
    team_names = [team_name for team_name in os.listdir(parameters.submissions_directory) if os.path.isdir(os.path.join(parameters.submissions_directory, team_name))]
    for team_name in team_names:
        submissions[team_name] = [os.path.join(parameters.submissions_directory, team_name, blueprint_file_name)
                                  for blueprint_file_name in os.listdir(os.path.join(parameters.submissions_directory, team_name))
                                  if blueprint_file_name.endswith('.json') or blueprint_file_name.endswith('.xml')]

    return submissions


def load_datasets(parameters):
    logging.info('Loading datasets ...')
    datasets = dict()

    if parameters.cumc12_input_directory is not None:
        cumc12 = CUMC12(parameters.cumc12_input_directory,
                        parameters.max_number_of_registrations_per_dataset)
        datasets[cumc12.name] = cumc12
        logging.info('Found ' + cumc12.name + ' ' + cumc12.category + ' dataset.')

    if parameters.dirlab_input_directory is not None:
        dirlab = DIRLAB(parameters.dirlab_input_directory,
                        parameters.max_number_of_registrations_per_dataset)
        datasets[dirlab.name] = dirlab
        logging.info('Found ' + dirlab.name + ' ' + dirlab.category + ' dataset.')

    if parameters.empire_input_directory is not None:
        empire = EMPIRE(parameters.empire_input_directory,
                        parameters.max_number_of_registrations_per_dataset)
        datasets[empire.name] = empire
        logging.info('Found ' + empire.name + ' ' + empire.category + ' dataset.')

    if parameters.isbr18_input_directory is not None:
        isbr18 = ISBR18(parameters.isbr18_input_directory,
                        parameters.output_directory,
                        parameters.max_number_of_registrations_per_dataset)
        datasets[isbr18.name] = isbr18
        logging.info('Found ' + isbr18.name + ' ' + isbr18.category + ' dataset.')

    if parameters.lpba40_input_directory is not None:
        lpba40 = LPBA40(parameters.lpba40_input_directory,
                        parameters.max_number_of_registrations_per_dataset)
        datasets[lpba40.name] = lpba40
        logging.info('Found ' + lpba40.name + ' ' + lpba40.category + ' dataset.')

    if parameters.mgh10_input_directory is not None:
        mgh10 = MGH10(parameters.mgh10_input_directory,
                      parameters.output_directory,
                      parameters.max_number_of_registrations_per_dataset)
        datasets[mgh10.name] = mgh10
        logging.info('Found ' + mgh10.name + ' ' + mgh10.category + ' dataset.')


    if parameters.popi_input_directory is not None:
        popi = POPI(parameters.popi_input_directory,
                    parameters.max_number_of_registrations_per_dataset)
        datasets[popi.name] = popi
        logging.info('Found ' + popi.name + ' ' + popi.category + ' dataset.')

    if parameters.spread_input_directory is not None:
        spread = SPREAD(parameters.spread_input_directory,
                        parameters.output_directory,
                        parameters.max_number_of_registrations_per_dataset)
        datasets[spread.name] = spread
        logging.info('Found ' + spread.name + ' ' + spread.category + ' dataset.')

    return datasets


def run(parameters):

    if not parameters.make_shell_scripts and not parameters.make_batch_scripts:
        logging.error('Neither --make-shell-scripts or --make-batch-scripts were True. Nothing to do.')
        quit()

    submissions = load_submissions(parameters)
    datasets = load_datasets(parameters)

    for team_name, blueprint_file_names in submissions.items():

        if hasattr(parameters, 'team') and not parameters.team is None:
            # User requested to have scripts generated only for this team
            if not parameters.team == team_name:
                continue

        for blueprint_file_name in blueprint_file_names:
            blueprint_name, blueprint_ext = os.path.splitext(os.path.basename(blueprint_file_name))

            if hasattr(parameters, 'blueprint_file_name') and not parameters.blueprint_file_name is None:
                # User requested to have scripts generated only for this blueprint
                if not parameters.blueprint_file_name == blueprint_file_name:
                    continue

            blueprint = json.load(open(blueprint_file_name))

            if not 'Datasets' in blueprint:
                logging.error('Missing key \'Datasets\' in blueprint %s. '
                              'Blueprint must specify on which datasets it should be evaluated. '
                              'Example: { Datasets: [\"SPREAD\", \"POPI\", \"LPBA40\"] }. '
                              'Skipping blueprint.' % blueprint_file_name)
                continue

            for dataset_name in blueprint['Datasets']:
                if not dataset_name in datasets:
                    logging.error('Dataset ' + dataset_name + ' requested by ' + blueprint_file_name + ' but no directory provided. Skipping dataset for this blueprint. See \'--help\' for usage.')
                    continue

                dataset = datasets[dataset_name]
                for file_names in dataset.generator():
                    blueprint_output_directory = os.path.join(parameters.output_directory, team_name, blueprint_name, os.path.dirname(file_names['displacement_field_file_names'][0]))

                    if not os.path.exists(blueprint_output_directory):
                        os.makedirs(blueprint_output_directory)

                    output_directory = os.path.join(parameters.output_directory, team_name, blueprint_name)

                    if parameters.make_shell_scripts:
                        dataset.make_shell_scripts(parameters.superelastix, blueprint_file_name, file_names, output_directory)

                    if parameters.make_batch_scripts:
                        dataset.make_batch_scripts(parameters.superelastix, blueprint_file_name, file_names, output_directory)

            logging.info('Generated registration scripts for %s.' % blueprint_file_name)

if __name__ == '__main__':
    parameters = parser.parse_args()

    if not os.path.isfile(parameters.superelastix):
        raise Exception('Could not find SuperElastix ' + parameters.superelastix + ".")

    if not os.path.exists(parameters.submissions_directory):
        raise Exception('Could not find submission directory ' + parameters.submissions_directory + ".")

    if not os.path.exists(parameters.output_directory):
        os.mkdir(parameters.output_directory)

    run(parameters)

