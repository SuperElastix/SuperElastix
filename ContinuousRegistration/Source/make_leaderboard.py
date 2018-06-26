import glob, os
from ContinuousRegistration.Source.make_registration_scripts import parser, load_datasets
from ContinuousRegistration.Source.util import logging, load_results_from_json, results_to_dict
from datetime import datetime

def run(parameters):
    result_file_names = glob.glob(os.path.join(parameters.output_directory, 'results*'))
    date = datetime.now().strftime('%d-%m-%Y')

    # Most recent first
    result_file_names.sort(reverse=True)

    if not result_file_names:
        raise Exception('No result JSON files found in %s.' % parameters.output_directory)

    logging.info('Loading results from %s.' % result_file_names[0])
    latest_results, latest_column_names = load_results_from_json(result_file_names[0], load_datasets(parameters))
    tables = results_to_dict(latest_results)
    for dataset_name, dataset_results in tables.items():
        if not dataset_name in latest_column_names:
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
        table += '<th role="columnheader">Number of Registrations</th>'

        for column_name in latest_column_names[dataset_name]:
            table += '<th role="columnheader">%s</th>' % column_name

        table += '</tr>'
        table += '</thead>'
        table += '<tbody>'

        for team_name, team_results in dataset_results.items():
            for blueprint_name, blueprint_results in team_results.items():
                if parameters.blueprint_file_name is not None and not blueprint_name \
                        in [os.path.splitext(blueprint_file_name)[0] for blueprint_file_name in parameters.blueprint_file_name]:
                    continue

                table += '<tr>'
                table += '<th>%s</th>' % team_name
                table += '<th>%s</th>' % blueprint_name
                table += '<th>%s</th>' % date
                table += '<th>%s</th>' % blueprint_results['blueprint_commit']
                table += '<th>%s</th>' % blueprint_results['repo_commit']
                table += '<td>%s</th>' % blueprint_results['number_of_registrations']

                means, stds = blueprint_results['means'], blueprint_results['stds']
                for mean, std in zip(means, stds):
                    table+= '<td>%.2f \pm %.2f</td>' % (mean, std)



                table += '</tr>'

        table += '</tbody>'
        table += '</table>'

        table += '<script>new Tablesort(document.getElementById("leaderboard-%s"));</script>' % dataset_name

        table += '</body>'
        table += '</html>'

        # + '-{:%Y-%m-%d-%H-%M-%S-%f}'.format(datetime.datetime.now())
        table_file = open(os.path.join(parameters.output_directory,
                                       'leaderboard-' + dataset_name + '.html'), "w")

        table_file.write(table)
        table_file.close()


if __name__ == '__main__':
    parameters = parser.parse_args()

    if not os.path.exists(parameters.output_directory):
        raise Exception('Output directory %s not found.' % parameters.output_directory)

    run(parameters)