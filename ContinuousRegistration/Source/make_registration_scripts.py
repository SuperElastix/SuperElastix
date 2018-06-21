import os, json, argparse
from ContinuousRegistration.Source.datasets import logging, load_datasets
from ContinuousRegistration.Source.util import load_submissions

parser = argparse.ArgumentParser(description='Continuous Registration Challenge command line interface.')
parser.add_argument('--superelastix', '-selx', required=True,
                    help="Path to SuperElastix executable.")
parser.add_argument('--submissions-directory', '-sd', required=True,
                    help='Directory with parameter files.')
parser.add_argument('--output-directory', '-od', required=True,
                    help="Directory where results will be saved.")
parser.add_argument('--make-shell-scripts', '-mss', type=bool, default=True,
                    help="Generate shell scripts (default: True).")
parser.add_argument('--make-batch-scripts', '-mbs', type=bool, default=False,
                    help="Generate shell scripts (default: False).")
parser.add_argument('--cumc12-input-directory', '-cid')
parser.add_argument('--dirlab-input-directory', '-did')
parser.add_argument('--dirlab-mask-directory', '-dmd', default=None)
parser.add_argument('--empire-input-directory', '-eid')
parser.add_argument('--isbr18-input-directory', '-iid')
parser.add_argument('--lpba40-input-directory', '-lid')
parser.add_argument('--spread-input-directory', '-sid')
parser.add_argument('--popi-input-directory', '-pid')
parser.add_argument('--popi-mask-directory', '-pmd', default=None)
parser.add_argument('--mgh10-input-directory', '-mid')
parser.add_argument('--hbia-input-directory', '-hid')
parser.add_argument('--team-name', '-tn',
                    help="If specified, only generated shell scripts for this team.")
parser.add_argument('--blueprint-file-name', '-bfn',
                    help="If specified, only generated shell scripts for this blueprint.")
parser.add_argument('--max-number-of-registrations-per-dataset', '-mnorpd', type=int, default=64)


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
                if not parameters.blueprint_file_name == os.path.basename(blueprint_file_name):
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
                    logging.info('Blueprint "%s" can also be used for dataset "%s". '
                                 'Supply "%s" input directory to also register "%s" with this blueprint.',
                                  blueprint_file_name, dataset_name, dataset_name, dataset_name)
                    continue

                dataset = datasets[dataset_name]

                logging.info('Generating registration scripts for the %s blueprint and %s dataset.',
                             blueprint_name, dataset_name)
                for file_names in dataset.generator():
                    logging.info('Generating registration script for image pair %s.',
                                 file_names['image_file_names'])
                    dir_name = os.path.dirname(file_names['disp_field_file_names'][0])
                    blueprint_output_directory = os.path.join(parameters.output_directory,
                                                              team_name, blueprint_name,
                                                              dir_name)

                    if not os.path.exists(blueprint_output_directory):
                        os.makedirs(blueprint_output_directory)

                    output_directory = os.path.join(parameters.output_directory,
                                                    team_name, blueprint_name)

                    if parameters.make_shell_scripts:
                        dataset.make_shell_scripts(parameters.superelastix,
                                                   blueprint_file_name, file_names,
                                                   output_directory)

                    if parameters.make_batch_scripts:
                        dataset.make_batch_scripts(parameters.superelastix,
                                                   blueprint_file_name,
                                                   file_names, output_directory)


if __name__ == '__main__':
    parameters = parser.parse_args()

    if not os.path.isfile(parameters.superelastix):
        raise Exception('Could not find SuperElastix '
                        + parameters.superelastix + ".")

    if not os.path.exists(parameters.submissions_directory):
        raise Exception('Could not find submission directory '
                        + parameters.submissions_directory + ".")

    if not os.path.exists(parameters.output_directory):
        os.mkdir(parameters.output_directory)

    run(parameters)

