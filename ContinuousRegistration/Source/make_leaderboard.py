import glob, os
from ContinuousRegistration.Source.make_registration_scripts import parser, load_datasets, load_submissions
from ContinuousRegistration.Source.util import logging, load_results_from_json, get_script_path
from datetime import datetime
import numpy as np
import json
import subprocess

date = datetime.now().strftime('%d-%m-%Y')

def run(parameters):
    result_file_names = glob.glob(os.path.join(parameters.output_directory, 'results*'))

    # Most recent first
    result_file_names.sort(reverse=True)

    if not result_file_names:
        raise Exception('No result JSON files found in %s.' % parameters.output_directory)

    logging.info('Loading results from %s.' % result_file_names[0])
    results, result_names = load_results_from_json(result_file_names[0])
    datasets = load_datasets(parameters)

    for dataset_name, dataset in datasets.items():
        if not dataset_name in result_names:
            # This dataset was not evaluated
            continue

        make_dataset_results(dataset_name, result_names, results)

def make_dataset_results(dataset_name, result_names, results):
    table = '<!DOCTYPE html>'
    table += '<html>'

    table += '<head>'
    table += '<link href="https://rawgit.com/tristen/tablesort/gh-pages/tablesort.css" rel="stylesheet">'
    table += '<link href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-BVYiiSIFeK1dGmJRAkycuHAHRg32OmUcww7on3RYdg4Va+PmSTsz/K68vbdEjh4u" crossorigin="anonymous">'
    table += '<script src="https://rawgit.com/tristen/tablesort/gh-pages/dist/tablesort.min.js"></script>'
    table += '<style>'
    table += 'table { width: 100%; } th, td { padding: 5px; text-align: left; }'
    table += 'font-size: small'
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

    # Try to get blueprint commit hash
    try:
        repo_commit = subprocess.check_output(['git', 'describe', '--always'],
                                              cwd=parameters.source_directory)
    except subprocess.CalledProcessError as e:
        repo_commit = None
        logging.error('Error (exit code {0}): {1}'.format(e.returncode, e.output))
    except Exception as e:
        repo_commit = None
        logging.error('Error reading commit hash from source directory "%s".' % parameters.source_directory)

    if repo_commit:
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
                raise NotImplementedError('Cannot read xml blueprints yet.')
            else:
                raise Exception('Could not load blueprint.')

            if dataset_name not in blueprint['Datasets']:
                continue

            make_blueprint_results(dataset_name, team_name, blueprint_name, blueprint_results, result_names)

            table += '<tr>'
            table += '<td>%s</td>' % team_name
            table += '<td><a href="%s">%s</a></td>' % ('leaderboard-' + dataset_name + '-' + team_name + '-' + blueprint_name + '.html', blueprint_name)
            table += '<td>%s</td>' % date

            if repo_commit:
                table += '<td>%s</td>' % repo_commit

                # Try to get blueprint commit hash
                try:
                    table += '<td>%s</td>' % subprocess.check_output(['git', 'describe', '--always'],
                                                                     cwd=parameters.source_directory)
                except subprocess.CalledProcessError as e:
                    table += '<td></td>'
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

def make_blueprint_results(dataset_name, team_name, blueprint_name, blueprint_results, result_names):
    blueprint_table = '<!DOCTYPE html>'
    blueprint_table += '<html>'

    blueprint_table += '<head>'
    blueprint_table += '<link href="https://rawgit.com/tristen/tablesort/gh-pages/tablesort.css" rel="stylesheet">'
    blueprint_table += '<link href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-BVYiiSIFeK1dGmJRAkycuHAHRg32OmUcww7on3RYdg4Va+PmSTsz/K68vbdEjh4u" crossorigin="anonymous">'
    blueprint_table += '<script src="https://rawgit.com/tristen/tablesort/gh-pages/dist/tablesort.min.js"></script>'
    blueprint_table += '<style>'
    blueprint_table += 'table { width: 100%; } th, td { padding: 5px; text-align: left; }'
    blueprint_table += '</style>'
    blueprint_table += '</head>'

    blueprint_table += '<body>'
    blueprint_table += '<a href="leaderboard-' + dataset_name + '.html">Back</a>'
    blueprint_table += '<table id="leaderboard-%s" class="sort">' % dataset_name

    # Add table header
    blueprint_table += '<thead>'
    blueprint_table += '<tr>'
    blueprint_table += '<th role="columnheader">Team</th>'
    blueprint_table += '<th role="columnheader">Blueprint</th>'
    blueprint_table += '<th role="columnheader">DisplacementField</th>'
    blueprint_table += '<th role="columnheader">Date</th>'

    for result_name in result_names[dataset_name]:
        blueprint_table += '<th role="columnheader">%s</th>' % result_name
    blueprint_table += '</tr>'

    if dataset_name in blueprint_results \
            and 'result' in blueprint_results[dataset_name]:

        for result_name, results in zip(blueprint_results[dataset_name]['name'], blueprint_results[dataset_name]['result']):
            blueprint_table += '<tr>'
            blueprint_table += '<td>%s</td>' % team_name
            blueprint_table += '<td>%s</td>' % blueprint_name
            blueprint_table += '<td>%s</td>' % result_name
            blueprint_table += '<td>%s</td>' % date

            for result in results:
                blueprint_table += '<td>%.2f</td>' % result
            blueprint_table += '</tr>'

    # Add table footer
    blueprint_table += '</tbody>'
    blueprint_table += '</table>'
    blueprint_table += '<a href="leaderboard-' + dataset_name + '.html">Back</a>'

    blueprint_table += '<script>new Tablesort(document.getElementById("leaderboard-%s"));</script>' % dataset_name

    blueprint_table += '</body>'
    blueprint_table += '</html>'

    f = open(os.path.join(parameters.output_directory,
                          'leaderboard-' + dataset_name + '-' + team_name + '-' + blueprint_name + '.html'), "w")

    f.write(blueprint_table)
    f.close()

if __name__ == '__main__':
    parameters = parser.parse_args()

    if not os.path.exists(parameters.output_directory):
        raise Exception('Output directory %s not found.' % parameters.output_directory)

    run(parameters)
