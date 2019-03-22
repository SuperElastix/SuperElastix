import os, json, argparse, platform
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
parser.add_argument('--brain2d-input-directory', '-b2d')
parser.add_argument('--lung2d-input-directory', '-l2d')
parser.add_argument('--cumc12-input-directory', '-cid')
parser.add_argument('--dirlab-input-directory', '-did')
parser.add_argument('--dirlab-mask-directory', '-dmd', default=None)
parser.add_argument('--empire-input-directory', '-eid')
parser.add_argument('--hammers-input-directory', '-hid')
parser.add_argument('--ibsr18-input-directory', '-iid')
parser.add_argument('--lpba40-input-directory', '-lid')
parser.add_argument('--spread-input-directory', '-sid')
parser.add_argument('--popi-input-directory', '-pid')
parser.add_argument('--popi-mask-directory', '-pmd', default=None)
parser.add_argument('--mgh10-input-directory', '-mid')
parser.add_argument('--hbia-input-directory', '-hbiaid')
parser.add_argument('--team-name', '-tn', action='append',
                    help="If specified, only generated shell scripts for these teams.")
parser.add_argument('--blueprint-file-name', '-bfn', action='append',
                    help="If specified, only generated shell scripts for this blueprint.")
parser.add_argument('--max-number-of-registrations-per-dataset', '-mnorpd', type=int, default=8)
parser.add_argument('--source-directory', '-srcd', default='.')


def run(parameters):

    if not parameters.make_shell_scripts:
        logging.error('--make-shell-scripts was False. Nothing to do.')
        quit()

    submissions = load_submissions(parameters)
    datasets = load_datasets(parameters)

    for team_name, blueprint_file_names in submissions.items():
        for blueprint_file_name in blueprint_file_names:
            logging.info('Loading blueprint %s.' % blueprint_file_name)
            blueprint = json.load(open(blueprint_file_name))

            if not 'Datasets' in blueprint:
                logging.error('Missing key \'Datasets\' in blueprint %s. '
                              'Blueprint must specify on which datasets it should be evaluated. '
                              'Example: { Datasets: [\"SPREAD\", \"POPI\", \"LPBA40\"] }. '
                              'Skipping blueprint.' % blueprint_file_name)
                continue

            for dataset_name in blueprint['Datasets']:
                if not dataset_name in datasets:
                    continue

                dataset = datasets[dataset_name]

                blueprint_name, blueprint_ext = os.path.splitext(os.path.basename(blueprint_file_name))
                for file_names in dataset.generator():
                    logging.info('Generating registration scripts for blueprint "%s" and images %s.',
                                 blueprint_name, file_names['image_file_names'])
                    dir_name = os.path.dirname(file_names['disp_field_file_names'][0])
                    blueprint_output_directory = os.path.join(parameters.output_directory,
                                                              team_name, blueprint_name,
                                                              dir_name)

                    if not os.path.exists(blueprint_output_directory):
                        os.makedirs(blueprint_output_directory)

                    output_directory = os.path.join(parameters.output_directory,
                                                    team_name, blueprint_name)

                    if parameters.make_shell_scripts:
                        ext = '.bat' if platform.system() == 'Windows' else '.sh'
                        dataset.make_shell_scripts(parameters.superelastix,
                                                   blueprint_file_name,
                                                   file_names, output_directory, ext)

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

