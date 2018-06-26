from util import logging, load_results_from_json
from ContinuousRegistration.Source.datasets import load_datasets
from ContinuousRegistration.Source.make_registration_scripts import parser
from ContinuousRegistration.Source.util import results_to_dict
import glob, os
import seaborn

seaborn.set_style("whitegrid")


def run(parameters):
    figures_dir = os.path.join(parameters.output_directory, 'figures')
    os.makedirs(figures_dir, exist_ok=True)

    result_file_names = glob.glob(os.path.join(parameters.output_directory, 'results*'))

    # Most recent first
    result_file_names.sort(reverse=True)

    if not result_file_names:
        raise Exception('No result JSON files found in %s.' % parameters.output_directory)

    logging.info('Making figures.')
    logging.info('Loading results from %s.' % result_file_names[0])
    latest_results, latest_column_names = load_results_from_json(result_file_names[0], load_datasets(parameters))
    tables = results_to_dict(latest_results)

    for team_name, team_results in latest_results.items():
        for blueprint_name, blueprint_results in team_results.items():
            if hasattr(parameters, 'blueprint_file_name') and not parameters.blueprint_file_name is None:
                if not os.path.basename(blueprint_name) in parameters.blueprint_file_name:
                    continue

            boxplot = []
            boxplot_x_labels = []
            for dataset_name, dataset_results in blueprint_results.items():
                boxplot.append(blueprint_results[dataset_name]['means'])
                boxplot_x_labels.append(blueprint_name)

    # if not os.path.basename(blueprint_file_name) in parameters.blueprint_file_name:
if __name__ == '__main__':
    parameters = parser.parse_args()

    if not os.path.exists(parameters.output_directory):
        raise Exception('Output directory %s not found.' % parameters.output_directory)

    run(parameters)