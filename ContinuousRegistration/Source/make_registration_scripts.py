import argparse
import logging
import os
import json

from datasets import CUMC12, DIRLAB, EMPIRE, ISBR18, LPBA40, MGH10, POPI, SPREAD

def load_submissions(parameters):
    logging.info('Loading blueprints ...')
    submissions = [(team_name, os.path.join(parameters.submissions_directory, team_name, file_name))
                   for team_name in os.listdir(parameters.submissions_directory) if os.path.isdir(os.path.join(parameters.submissions_directory, team_name))
                   for file_name in os.listdir(os.path.join(parameters.submissions_directory, team_name)) if file_name.endswith('.json') or file_name.endswith('.xml')]

    for team_name, blueprint_file_name in submissions:
        logging.info('Found %s by %s.', blueprint_file_name, team_name)

    return submissions


def load_datasets(parameters):
    logging.info('Loading datasets ...')
    datasets = dict()


    if parameters.cumc12_input_directory is not None:
        cumc12 = CUMC12(parameters.cumc12_input_directory)
        datasets[cumc12.name] = cumc12
        logging.info('Found ' + cumc12.name + ' ' + cumc12.category + ' dataset.')

    if parameters.dirlab_input_directory is not None:
        dirlab = DIRLAB(parameters.dirlab_input_directory)
        datasets[dirlab.name] = dirlab
        logging.info('Found ' + dirlab.name + ' ' + dirlab.category + ' dataset.')

    if parameters.empire_input_directory is not None:
        empire = EMPIRE(parameters.empire_input_directory)
        datasets[empire.name] = empire
        logging.info('Found ' + empire.name + ' ' + empire.category + ' dataset.')

    if parameters.isbr18_input_directory is not None:
        isbr18 = ISBR18(parameters.isbr18_input_directory)
        datasets[isbr18.name] = isbr18
        logging.info('Found ' + isbr18.name + ' ' + isbr18.category + ' dataset.')

    if parameters.lpba40_input_directory is not None:
        lpba40 = LPBA40(parameters.lpba40_input_directory)
        datasets[lpba40.name] = lpba40
        logging.info('Found ' + lpba40.name + ' ' + lpba40.category + ' dataset.')

    if parameters.mgh10_input_directory is not None:
        mgh10 = MGH10(parameters.mgh10_input_directory)
        datasets[MGH10.name] = mgh10
        logging.info('Found ' + mgh10.name + ' ' + mgh10.category + ' dataset.')

    if parameters.popi_input_directory is not None:
        popi = POPI(parameters.popi_input_directory)
        datasets[popi.name] = popi
        logging.info('Found ' + popi.name + ' ' + popi.category + ' dataset.')

    if parameters.spread_input_directory is not None:
        spread = SPREAD(parameters.spread_input_directory)
        datasets[spread.name] = spread
        logging.info('Found ' + spread.name + ' ' + spread.category + ' dataset.')

    return datasets


def run(parameters):

    if not parameters.make_shell_scripts and not parameters.make_batch_scripts:
        logging.error('Neither --make-shell-scripts or --make-batch-scripts were True. Nothing to do.')
        quit()

    submissions = load_submissions(parameters)
    datasets = load_datasets(parameters)

    for team_name, blueprint_file_name in submissions:
        blueprint_name, blueprint_ext = os.path.splitext(os.path.basename(blueprint_file_name))

        if hasattr(parameters, 'team') and not parameters.team is None:
            # User requested to have scripts generated only for this team
            if not parameters.team == team_name:
                continue

        if hasattr(parameters, 'blueprint_file_name') and not parameters.blueprint_file_name is None:
            # User requested to have scripts generated only for this blueprint
            if not parameters.blueprint_file_name == blueprint_file_name:
                continue

        blueprint = json.load(open(blueprint_file_name))

        for dataset_name in blueprint['Datasets']:
            if not dataset_name in datasets:
                logging.error('Dataset ' + dataset_name + ' requested by ' + blueprint_file_name + ' but no directory provided. See \'--help\' for usage.')
                continue

            dataset = datasets[dataset_name]
            for file_names in dataset.generator():
                blueprint_output_directory = os.path.join(parameters.output_directory, team_name, blueprint_name, os.path.dirname(file_names['deformation_field_file_names'][0]))

                if not os.path.exists(blueprint_output_directory):
                    os.makedirs(blueprint_output_directory)

                file_names['output_file_names'] = (os.path.join(blueprint_output_directory, os.path.basename(file_names['deformation_field_file_names'][0])),
                                                   os.path.join(blueprint_output_directory, os.path.basename(file_names['deformation_field_file_names'][1])))

                output_directory = os.path.join(parameters.output_directory, team_name, blueprint_name)

                if parameters.make_shell_scripts:
                    dataset.make_shell_script(parameters.superelastix, blueprint_file_name, file_names, output_directory)

                if parameters.make_batch_scripts:
                    dataset.make_batch_script(parameters.superelastix, blueprint_file_name, file_names, output_directory)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='SuperBench registration driver.')

    parser.add_argument('--superelastix', '-selx', required=True, help="Path to SuperElastix executable.")
    parser.add_argument('--submissions-directory', '-sd', required=True, help='Directory with parameter files.')
    parser.add_argument('--output-directory', '-od', required=True)
    parser.add_argument('--make-shell-scripts', '-mss', type=bool, default=True)
    parser.add_argument('--make-batch-scripts', '-mbs', type=bool, default=False)

    parser.add_argument('--cumc12-input-directory', '-cid')
    parser.add_argument('--dirlab-input-directory', '-did')
    parser.add_argument('--empire-input-directory', '-eid')
    parser.add_argument('--isbr18-input-directory', '-iid')
    parser.add_argument('--lpba40-input-directory', '-lid')
    parser.add_argument('--mgh10-input-directory', '-mid')
    parser.add_argument('--spread-input-directory', '-sid')
    parser.add_argument('--popi-input-directory', '-pid')

    parser.add_argument('--team-name', '-tn')
    parser.add_argument('--blueprint-file-name', '-bfn', help='Generate scripts only for this blueprint file name (including .json). '
                                                              'May produce scripts for multiple blueprints if they share the same name '
                                                              ' and --team-name is not provided.')

    parameters = parser.parse_args()

    if not os.path.isfile(parameters.superelastix):
        raise Exception('Could not find SuperElastix ' + parameters.superelastix + ".")

    if not os.path.exists(parameters.submissions_directory):
        raise Exception('Could not find submission directory ' + parameters.submissions_directory + ".")

    if not os.path.exists(parameters.output_directory):
        os.mkdir(parameters.output_directory)

    run(parameters)

