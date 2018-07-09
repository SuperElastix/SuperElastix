from ContinuousRegistration.Source.datasets import load_datasets
from ContinuousRegistration.Source.make_registration_scripts import parser
from ContinuousRegistration.Source.util import results_to_dict, logging, load_results_from_json
import glob, os
import seaborn
import numpy as np

seaborn.set_style("whitegrid")
target_metrics = ['DSC', 'TRE']


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
    latest_results, latest_column_names = load_results_from_json(result_file_names[0], load_datasets(parameters), make_means=False)
    results = results_to_dict(latest_results)

    for dataset_name, dataset_results in results.items():
        boxplot_xlabel = {}
        boxplot_data = {}
        for team_name, team_results in dataset_results.items():
            for blueprint_name, blueprint_results in team_results.items():
                for metric_name, metric_results in zip(latest_column_names[dataset_name], blueprint_results['means']):

                    if metric_results is np.NaN:
                        continue

                    if not metric_name in boxplot_data:
                        boxplot_data[metric_name] = {}
                        boxplot_xlabel[metric_name] = {}

                    if not blueprint_name in boxplot_data[metric_name]:
                        boxplot_data[metric_name][blueprint_name] = []
                        boxplot_xlabel[metric_name][blueprint_name] = []

                    boxplot_data[metric_name][blueprint_name].append(metric_results)
                    boxplot_xlabel[metric_name][blueprint_name].append('%s/%s' % (team_name, blueprint_name))

        for metric_name, metric_results in boxplot_data[blueprint_name].items():
            for blueprint_name, blueprint_results in boxplot_data.items():
                    ax = seaborn.boxplot(data=boxplot_data[metric_name][metric_name], palette=seaborn.color_palette("GnBu_d"))
                    ax.set(xlabel=boxplot_xlabel[blueprint_name][metric_name], ylabel=metric_name)
                    fig = ax.get_figure()
                    fig.savefig(os.path.join(figures_dir, dataset_name + metric_name + '_boxplot.png'))


    # for team_name, team_results in latest_results.items():
    #     for blueprint_name, blueprint_results in team_results.items():
    #         if hasattr(parameters, 'blueprint_file_name') and not parameters.blueprint_file_name is None:
    #             if not os.path.basename(blueprint_name) in parameters.blueprint_file_name:
    #                 continue
    #
    #         boxplot = []
    #         boxplot_x_labels = []
    #         for dataset_name, dataset_results in blueprint_results.items():
    #             boxplot.append(blueprint_results[dataset_name]['means'])
    #             boxplot_x_labels.append(blueprint_name)

    # if not os.path.basename(blueprint_file_name) in parameters.blueprint_file_name:
if __name__ == '__main__':
    parameters = parser.parse_args()

    if not os.path.exists(parameters.output_directory):
        raise Exception('Output directory %s not found.' % parameters.output_directory)

    run(parameters)