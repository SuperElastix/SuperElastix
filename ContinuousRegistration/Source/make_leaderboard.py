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
                if not dataset_results:
                    column_names[dataset_name] = dict()
                    continue

                dataset_result_names = [[result_name
                                         for result_name, result_value in registration_results.items()]
                                         for registration_name, registration_results in dataset_results.items()]

                # All registrations should have been evaluated with the same metrics
                dataset_result_names = set(tuple(x) for x in dataset_result_names)
                assert(len(dataset_result_names) == 1)

                # Save column names
                if not column_names.has_key(dataset_name):
                    column_names[dataset_name] = dataset_result_names.pop()


                # Average stats
                dataset_result_array = np.array([[result_value
                                                  for result_name, result_value in registration_results.items()]
                                                  for registration_name, registration_results in dataset_results.items()])

                dataset_result_means = np.mean(dataset_result_array, axis=0)
                dataset_result_stds = np.std(dataset_result_array, axis=0)

                # Save stats in-place
                results[team_name][blueprint_name][dataset_name] = (dataset_result_means,
                                                                    dataset_result_stds)

    return results, column_names


def run(parameters):
    result_file_names = glob.glob(os.path.join(parameters.output_directory, 'results*'))
    result_file_names.sort(reverse=True)

    if not result_file_names:
        raise Exception('No result JSON files found in %s.' % parameters.output_directory)

    latest_results, latest_column_names = load_results_from_json(result_file_names[0])

    tables = dict()
    for team_name, team_results in latest_results.items():
        for blueprint_name, blueprint_results in team_results.items():
            for dataset_name, dataset_results in blueprint_results.items():

                if not tables.has_key(dataset_name):
                    tables[dataset_name] = dict()

                if not tables[dataset_name].has_key(team_name):
                    tables[dataset_name][team_name] = dict()

                tables[dataset_name][team_name][blueprint_name] = latest_results[team_name][blueprint_name][dataset_name]


    # TODO: Assert that all teams have the same number of results (columns)
    # http: // tristen.ca / tablesort / demo /

    for dataset_name, dataset_results in tables.items():

        table = '<table id="sort" class="sort">'

        # Add table header
        table += '<thead>'
        table += '<tr>'
        table += '<th role="columnheader">Team ::after</th>'
        table += '<th role="columnheader">Blueprint ::after</th>'

        for column_name in latest_column_names[dataset_name]:
            table += '<th role="columnheader">%s ::after</th>' % column_name

        table += '</tr>'
        table += '</thead>'

        for team_name, team_result in tables[dataset_name].items():
            for blueprint_name, blueprint_results in team_results.items():
                if blueprint_results.has_key(dataset_name) and bool(blueprint_results[dataset_name]):
                    table += '<tr>'
                    table += '<th>%s</th>' % team_name
                    table += '<th>%s</th>' % blueprint_name

                    means, stds = blueprint_results[dataset_name]
                    for mean, std in zip(means, stds):
                        table+= '<td>%f \pm %f</td>' % (mean, std)

                    table += '</tr>'

        table += '<tbody>'
        table += '</tbody>'
        table += '</table>'

        table_file = open(os.path.join(parameters.output_directory,
                                       'leaderboard-' + dataset_name + '-{:%Y-%m-%d-%H-%M-%S-%f}'.format(datetime.datetime.now()) + '.html'), "w")
        table_file.write(table)
        table_file.close()



if __name__ == '__main__':
    parameters = parser.parse_args()

    if not os.path.exists(parameters.output_directory):
        raise Exception('Output directory %s not found.' % parameters.output_directory)

    run(parameters)