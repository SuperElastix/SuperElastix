import logging, os, json
from make_registration_scripts import parser, load_submissions, load_datasets

def write_json(output_file_name, results):
    output_directory = os.path.dirname(output_file_name)
    if not os.path.exists(output_directory):
        os.mkdir(output_directory)

    with open(output_file_name, "w") as f:
        f.write(json.dumps(results, indent=4))

def run(parameters):
    submissions = load_submissions(parameters)
    datasets = load_datasets(parameters)

    for team_name, blueprint_file_name in submissions:
        blueprint_name, blueprint_ext = os.path.splitext(os.path.basename(blueprint_file_name))

        blueprint = json.load(open(blueprint_file_name))

        results = {team_name: {blueprint_name: dict()}}
        for dataset_name in blueprint['Datasets']:
            if not dataset_name in datasets:
                logging.error('Dataset ' + dataset_name + ' requested by ' + blueprint_file_name + ' but no directory provided. See \'--help\' for usage.')
                continue

            dataset = datasets[dataset_name]
            results[team_name][blueprint_name][dataset_name] = dict()
            for file_names in dataset.generator():
                output_directory = os.path.join(parameters.output_directory, team_name, blueprint_name)
                logging.info('Evaluating %s and %s.', file_names['image_file_names'][0], file_names['image_file_names'][1])

                try:
                    file_names['displacement_field_file_names_full_path'] = (
                        os.path.join(output_directory, file_names['displacement_field_file_names'][0]),
                        os.path.join(output_directory, file_names['displacement_field_file_names'][1]))

                    result_0, result_1 = dataset.evaluate(parameters.superelastix, file_names)
                    results[team_name][blueprint_name][dataset.name][file_names['displacement_field_file_names'][0]] = result_0
                    results[team_name][blueprint_name][dataset.name][file_names['displacement_field_file_names'][1]] = result_1
                except Exception as e:
                    results[team_name][blueprint_name][dataset.name][file_names['displacement_field_file_names']][0] = 'Error during evaluation'
                    results[team_name][blueprint_name][dataset.name][file_names['displacement_field_file_names']][1] = 'Error during evaluation'
                    logging.error('Error during evaulation of %s, %s, %s' % (team_name, blueprint_name, dataset.name))

        write_json(os.path.join(parameters.output_directory, "results.json"), results)

if __name__ == '__main__':
    parameters = parser.parse_args()
    run(parameters)
