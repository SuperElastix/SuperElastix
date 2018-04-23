import json, glob, os, argparse, datetime
import numpy as np

parser = argparse.ArgumentParser(description='Continuous Registration Challenge command line interface.')

parser.add_argument('--output-directory', '-od', required=True, help="Directory where results will be saved.")

parser.add_argument('--cumc12-input-directory', '-cid')
parser.add_argument('--dirlab-input-directory', '-did')
parser.add_argument('--empire-input-directory', '-eid')
parser.add_argument('--isbr18-input-directory', '-iid')
parser.add_argument('--lpba40-input-directory', '-lid')
parser.add_argument('--spread-input-directory', '-sid')
parser.add_argument('--popi-input-directory', '-pid')
parser.add_argument('--mgh10-input-directory', '-mid')

parser.add_argument('--max-number-of-registrations-per-dataset', '-mnorpd', type=int, default=64)


def load_results_from_json(filename):
    results = json.load(open(filename))
    column_names = dict()

    for team_name, team_results in results.items():
        for blueprint_name, blueprint_results in team_results.items():
            for dataset_name, dataset_results in blueprint_results.items():

                # If no registration or evaluations completed
                if all(dataset_result is None for dataset_result in dataset_results):
                    results[team_name][blueprint_name][dataset_name] = None
                    continue

                # TODO: Rewrite to list comprehension
                dataset_metric_names = set()
                dataset_result_array = []
                for dataset_result in dataset_results:
                    for dataset_result_key, dataset_result_values in dataset_result.items():
                        dataset_metric_names.add(tuple(dataset_result_values.keys()))
                        dataset_result_array.append(list(dataset_result_values.values()))

                # All registrations should have been evaluated with the same metrics
                assert(len(dataset_metric_names) == 1)

                # Save column names
                if not dataset_name in column_names:
                    column_names[dataset_name] = dataset_metric_names.pop()

                dataset_result_means = np.mean(dataset_result_array, axis=0)
                dataset_result_stds = np.std(dataset_result_array, axis=0)

                # Save stats in-place
                results[team_name][blueprint_name][dataset_name] = (dataset_result_means,
                                                                    dataset_result_stds)

    return results, column_names


def run(parameters):
    result_file_names = glob.glob(os.path.join(parameters.output_directory, 'results*'))

    # Most recent first
    result_file_names.sort(reverse=True)

    if not result_file_names:
        raise Exception('No result JSON files found in %s.' % parameters.output_directory)

    latest_results, latest_column_names = load_results_from_json(result_file_names[0])

    # Fill tables with team data
    tables = {}
    for team_name, team_results in latest_results.items():
        for blueprint_name, blueprint_results in team_results.items():
            for dataset_name, dataset_results in blueprint_results.items():

                if not dataset_name in tables:
                    tables[dataset_name] = {}

                if not team_name in tables[dataset_name]:
                    tables[dataset_name][team_name] = {}

                tables[dataset_name][team_name][blueprint_name] = latest_results[team_name][blueprint_name][dataset_name]

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

        for column_name in latest_column_names[dataset_name]:
            table += '<th role="columnheader">%s</th>' % column_name

        table += '</tr>'
        table += '</thead>'
        table += '<tbody>'

        for team_name, team_result in dataset_results.items():
            for blueprint_name, blueprint_results in team_results.items():
                if dataset_name in blueprint_results and not blueprint_results[dataset_name] is None:
                    table += '<tr>'
                    table += '<th>%s</th>' % team_name
                    table += '<th>%s</th>' % blueprint_name

                    means, stds = blueprint_results[dataset_name]
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