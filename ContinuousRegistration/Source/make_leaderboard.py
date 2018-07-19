import glob, os
from ContinuousRegistration.Source.make_registration_scripts import parser, load_datasets, load_submissions
from ContinuousRegistration.Source.util import logging, load_results_from_json, get_script_path
from datetime import datetime
import numpy as np
import json
import subprocess

def run(parameters):
    result_file_names = glob.glob(os.path.join(parameters.output_directory, 'results*'))
    date = datetime.now().strftime('%d-%m-%Y')

    # Most recent first
    result_file_names.sort(reverse=True)

    if not result_file_names:
        raise Exception('No result JSON files found in %s.' % parameters.output_directory)

    logging.info('Loading results from %s.' % result_file_names[0])
    results, result_names = load_results_from_json(result_file_names[0])
    datasets = load_datasets(parameters)
    submissions = load_submissions(parameters)

    for dataset_name, dataset in datasets.items():
        if not dataset_name in result_names:
            # This dataset was not evaluated
            continue

        table = '<!DOCTYPE html>'
        table += '<html>'

        table += '<head>'
        table += '<link href="https://rawgit.com/tristen/tablesort/gh-pages/tablesort.css" rel="stylesheet">'
        table += '<link href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-BVYiiSIFeK1dGmJRAkycuHAHRg32OmUcww7on3RYdg4Va+PmSTsz/K68vbdEjh4u" crossorigin="anonymous">'
        table += '<script src="https://rawgit.com/tristen/tablesort/gh-pages/dist/tablesort.min.js"></script>'
        table += '<style>'
        table += 'table { width: 100%; } th, td { padding: 5px; text-align: left; }'
        table += '</style>'
        table += '</head>'

        table += '<body>'
        table += '<table id="leaderboard-%s" class="sort">' % dataset_name

        # Add table header
        table += '<thead>'
        table += '<tr>'
        table += '<th role="columnheader">Team</th>'
        table += '<th role="columnheader">Blueprint</th>'
        table += '<th role="columnheader">Date</th>'
        table += '<th role="columnheader">Blueprint Commit</th>'
        table += '<th role="columnheader">Repo Commit</th>'
        table += '<th role="columnheader">Completed</th>'

        for result_name in result_names[dataset_name]:
            table += '<th role="columnheader">%s</th>' % result_name

        table += '</tr>'
        table += '</thead>'
        table += '<tbody>'

        for team_name, team_results in results.items():
            for blueprint_name, blueprint_results in team_results.items():
                if parameters.blueprint_file_name is not None and not blueprint_name \
                        in [os.path.splitext(blueprint_file_name)[0] for blueprint_file_name in parameters.blueprint_file_name]:
                    # User requested specific blueprints and this blueprint is not one of them
                    continue

                blueprint_file_name_json = os.path.join(parameters.submissions_directory, team_name, blueprint_name + '.json')
                blueprint_file_name_xml = os.path.join(parameters.submissions_directory, team_name,blueprint_name + '.xml')
                if os.path.isfile(blueprint_file_name_json):
                    blueprint = json.load(open(blueprint_file_name_json))
                elif os.path.isfile(blueprint_file_name_xml):
                    pass
                else:
                    raise Exception('Could not load blueprint.')

                if dataset_name not in blueprint['Datasets']:
                    continue

                table += '<tr>'
                table += '<td>%s</td>' % team_name
                table += '<td>%s</td>' % blueprint_name
                table += '<td>%s</td>' % date

                # Get blueprint commit hash
                try:
                    table += '<td>%s</td>' % subprocess.check_output(['git', 'log', '-n', '1', '--pretty=format:%h',
                                                                      '--', '%s/../Submissions/%s/%s.json' %
                                                                      (get_script_path(), team_name, blueprint_name)],
                                                                     cwd=parameters.source_directory)
                except subprocess.CalledProcessError as e:
                    logging.error('Error (exit code {0}): {1}'.format(e.returncode, e.output))

                # Get repo commit hash
                try:
                    table += '<td>%s</td>' % subprocess.check_output(['git', 'describe', '--always'],
                                                                     cwd=parameters.source_directory)
                except subprocess.CalledProcessError as e:
                    logging.error('Error (exit code {0}): {1}'.format(e.returncode, e.output))

                if dataset_name in blueprint_results \
                    and 'result' in blueprint_results[dataset_name] \
                    and not np.isnan(blueprint_results[dataset_name]['result']).all():
                        result = blueprint_results[dataset_name]['result']
                        table += '<td>%s/%s</td>' % (len(~np.isnan(result)), len(result))
                        means = np.nanmean(result, axis=0)
                        stds = np.nanstd(result, axis=0)
                        for mean, std in zip(means, stds):
                            table += '<td>%.2f \pm %.2f</td>' % (mean, std)
                else:
                    table += '<td>0</td>'
                    for result_name in result_names[dataset_name]:
                        table += '<td>N/A</td>'

                table += '</tr>'

        table += '</tbody>'
        table += '</table>'

        table += '<script>new Tablesort(document.getElementById("leaderboard-%s"));</script>' % dataset_name

        table += '</body>'
        table += '</html>'

        table_file = open(os.path.join(parameters.output_directory,
                                       'leaderboard-' + dataset_name + '.html'), "w")

        table_file.write(table)
        table_file.close()


if __name__ == '__main__':
    parameters = parser.parse_args()

    if not os.path.exists(parameters.output_directory):
        raise Exception('Output directory %s not found.' % parameters.output_directory)

    run(parameters)
