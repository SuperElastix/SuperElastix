import logging, os, json, datetime
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

    results = {}
    for team_name, blueprint_file_names in submissions.items():
        if not team_name in results:
            results[team_name] = {}

        for blueprint_file_name in blueprint_file_names:
            blueprint_name, blueprint_ext = os.path.splitext(os.path.basename(blueprint_file_name))
            blueprint = json.load(open(blueprint_file_name))

            if not blueprint_name in results[team_name]:
                results[team_name][blueprint_name] = {}

            for dataset_name in blueprint['Datasets']:
                if not dataset_name in datasets:
                    logging.error('Dataset ' + dataset_name + ' requested by blueprint ' + blueprint_file_name + ' but no data directory provided. See \'--help\' for usage.')
                    continue

                dataset = datasets[dataset_name]
                results[team_name][blueprint_name][dataset_name] = []

                for file_names in dataset.generator():
                    output_directory = os.path.join(parameters.output_directory, team_name, blueprint_name)

                    try:
                        results[team_name][blueprint_name][dataset.name].append(dataset.evaluate(
                            parameters.superelastix, file_names, output_directory))
                    except Exception as e:
                        logging.error('Error during evaluation of %s\'s blueprint %s on dataset %s: %s'
                                      % (team_name, blueprint_name, dataset.name, str(e)))


    write_json(os.path.join(parameters.output_directory, 'results.json'), results)

if __name__ == '__main__':
    run(parser.parse_args())
