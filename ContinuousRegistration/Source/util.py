import subprocess, sys, logging, os, random, json
from itertools import islice
import SimpleITK as sitk
import numpy as np

logging.basicConfig(level=logging.DEBUG, datefmt='%d-%m-%Y:%H:%M:%S',
                    format='%(asctime)s,%(msecs)d %(levelname)-8s [%(filename)s:%(lineno)d] %(message)s')

np.random.seed(os.getenv('PYTHON_RANDOM_SEED', 0))

def load_submissions(parameters):
    logging.info('Loading blueprints.')

    submissions = dict()
    if parameters.team_name is not None:
        team_names = parameters.team_name
        logging.info('Only looking for blueprints by %s' % ' '.join(parameters.team_name))
    else:
        team_names = [team_name for team_name in os.listdir(parameters.submissions_directory)
                      if os.path.isdir(os.path.join(parameters.submissions_directory, team_name))]

    for team_name in team_names:
        if parameters.blueprint_file_name is not None:
                blueprint_file_names = [blueprint_file_name
                                        for blueprint_file_name in parameters.blueprint_file_name
                                        if os.path.isfile(os.path.join(parameters.submissions_directory, team_name,  blueprint_file_name))]
        else:
            blueprint_file_names = [blueprint_file_name
                                    for blueprint_file_name in os.listdir(os.path.join(parameters.submissions_directory, team_name))
                                    if (os.path.isfile(os.path.join(parameters.submissions_directory, team_name,  blueprint_file_name)) 
                                        and os.path.splitext(blueprint_file_name)[1].lower() in ['.json', '.xml'])]

        for blueprint_file_name in blueprint_file_names:
            logging.info('Found blueprint %s/%s.' % (team_name, blueprint_file_name))

        blueprint_file_names = [os.path.join(parameters.submissions_directory, team_name,  blueprint_file_name)
                                for blueprint_file_name in blueprint_file_names]

        submissions[team_name] = blueprint_file_names

    return submissions

def load_results_from_json(filename):
    results = json.load(open(filename))
    result_names = dict()

    for team_name, team_results in results.items():
        for blueprint_name, blueprint_results in team_results.items():
            for dataset_name, dataset_results in blueprint_results.items():

                metric_names = []
                disp_field_file_names = []
                metric_results = []

                for dataset_result in dataset_results:
                    if dataset_result is None:
                        results[team_name][blueprint_name][dataset_name] =  {'name': [], 'result': []}
                        continue

                    for disp_field_file_name, disp_field_results in dataset_result.items():
                        disp_field_file_names.append(disp_field_file_name)
                        metric_names.append(list(disp_field_results.keys()))
                        metric_results.append(list(disp_field_results.values()))

                    # Check that the same metrics have been computed for all registrations with this dataset
                    metric_names = [list(b) for b in set(tuple(a) for a in metric_names)]
                    if(len(metric_names) > 1):
                        raise Exception('Metric names not unique: %s' % metric_names)

                # No results for this dataset
                if len(metric_names) == 0:
                    results[team_name][blueprint_name][dataset_name] = {}
                    continue

                # Sort metric names so they are displayed in the HTML table in the right order
                indices = np.argsort(metric_names[0])
                metric_names = [metric_names[0][index] for index in indices]
                metric_results = [[metric_result[index] for index in indices] for metric_result in metric_results]

                # Save metric_names names separately for constructing html table headers
                result_names[dataset_name] = metric_names

                results[team_name][blueprint_name][dataset_name] = { 'name': disp_field_file_names, 'result': metric_results }

    return results, result_names


def results_to_dict(results):
    # Reverse order of dims
    tables = {}
    for team_name, team_results in results.items():
        for blueprint_name, blueprint_results in team_results.items():
            for dataset_name, dataset_results in blueprint_results.items():

                if not dataset_name in tables:
                    tables[dataset_name] = {}

                if not team_name in tables[dataset_name]:
                    tables[dataset_name][team_name] = {}

                tables[dataset_name][team_name][blueprint_name] = results[team_name][blueprint_name][dataset_name]

    return tables


def take(iterable, n):
    return list(islice(iterable, n))


def sort_file_names(file_names):
    return sorted(file_names, key=lambda dictionary: dictionary['image_file_names'][0])


# Some data sets come without world coordinate information for the label images. This is a problem
# when measuring overlap. Here, we copy information from the image to the label. We write in mha
# format so we can inspect the header with a simple text editor
copy_information_from_images_to_labels_ext = '.mha'
def copy_information_from_images_to_labels(image_file_names, label_file_names,
                                           disp_field_file_names,
                                           output_directory, mhd_pixel_type):
        new_label_file_names = []
        for image_file_name, label_file_name, disp_field_file_name \
                in zip(image_file_names, label_file_names, disp_field_file_names):
            label_file_name_we, label_file_name_ext = splitext(label_file_name)
            dataset_output_directory = os.path.join(output_directory, 'tmp', 'labels_with_world_info',
                                                    os.path.dirname(disp_field_file_name))
            output_file_name = os.path.join(dataset_output_directory,
                                            os.path.basename(label_file_name_we)
                                            + '_label_with_world_info'
                                            + copy_information_from_images_to_labels_ext)

            if not os.path.isdir(dataset_output_directory):
                os.makedirs(dataset_output_directory)

            if not os.path.isfile(output_file_name):

                # File info is read from corresponding image file
                image = sitk.ReadImage(image_file_name)

                # Write raw file with this info
                label = sitk.ReadImage(label_file_name)
                label.CopyInformation(image)

                sitk.WriteImage(sitk.Cast(label, sitk.sitkUInt8), output_file_name)
                print('Created label with world information %s.' % output_file_name)

            new_label_file_names.append(output_file_name)

        return tuple(new_label_file_names)


mask_ext = '.nii.gz'
def create_mask_by_thresholding(label_file_names, disp_field_file_names,
                                output_directory, threshold, dilate, erode):
    mask_file_names = []
    for label_file_name, disp_field_file_name \
            in zip(label_file_names, disp_field_file_names):
        label_file_name_we, label_file_name_ext = splitext(label_file_name)
        dataset_output_directory = os.path.join(output_directory, 'tmp', 'masks',
                                                os.path.dirname(disp_field_file_name))
        output_file_name = os.path.join(dataset_output_directory, os.path.basename(
            label_file_name_we) + '_mask' + mask_ext)

        if not os.path.isdir(dataset_output_directory):
            os.makedirs(dataset_output_directory)

        if not os.path.isfile(output_file_name):
            label = sitk.ReadImage(label_file_name)
            mask = label > threshold
            padding = (erode,)*mask.GetDimension()
            padded_mask = sitk.ConstantPad(mask, padding, padding)
            dilated_mask = sitk.BinaryDilate(padded_mask, dilate, sitk.sitkAnnulus, 0, 1, False) # pixels
            filled_mask = sitk.BinaryErode(dilated_mask, erode, sitk.sitkAnnulus, 0, 1, False)
            cropped_filled_mask = sitk.Crop(filled_mask, padding, padding)
            sitk.WriteImage(cropped_filled_mask, output_file_name)
            print('Created mask %s.' % output_file_name)

        mask_file_names.append(output_file_name)

    return tuple(mask_file_names)


def create_mask_by_size(image_file_name, mask_file_name):
    mask_directory = os.path.dirname(mask_file_name)

    if not mask_file_name.endswith(mask_ext):
        mask_file_name = splitext(mask_file_name)[0] + mask_ext

    if mask_directory is not None:
        os.makedirs(mask_directory, exist_ok=True)

    if not os.path.exists(mask_file_name):
        image = sitk.ReadImage(image_file_name)

        siz = image.GetSize()
        siz = siz[len(siz)-1:] + siz[:len(siz)-1]  # left-shift size
        mask = sitk.GetImageFromArray(np.ones(siz))
        mask.CopyInformation(image)

        os.makedirs(os.path.dirname(mask_file_name), exist_ok=True)
        sitk.WriteImage(sitk.Cast(mask, sitk.sitkUInt8), mask_file_name)

    return mask_file_name


def create_identity_world_information(file_names, dataset_name, input_directory, output_directory):
    new_file_names = []
    for file_name in file_names:
        output_file_name = os.path.join(output_directory, 'tmp', 'identity_world_information', dataset_name, file_name)

        if not os.path.isfile(output_file_name):
            file_output_directory = os.path.dirname(output_file_name)
            if not os.path.isdir(file_output_directory):
                os.makedirs(file_output_directory)

            image = sitk.ReadImage(os.path.join(input_directory, file_name))
            direction = np.identity(image.GetDimension()).flatten()
            image.SetDirection(direction)
            image.SetSpacing((1,) * image.GetDimension())
            image.SetOrigin((0,) * image.GetDimension())
            sitk.WriteImage(image, output_file_name)
            print('Created image %s with identity world information.' % output_file_name)

        new_file_names.append(output_file_name)

    return tuple(new_file_names)


def create_disp_field_names(image_file_names, dataset_name):
    name_0, name_1 = image_file_names
    name_0 = os.path.basename(name_0)
    name_1 = os.path.basename(name_1)
    name_we_0, image_extension_we_0 = splitext(name_0)
    name_we_1, image_extension_we_1 = splitext(name_1)

    name_pair_0 = "moving" + name_we_1 + "_to_fixed" + name_we_0 + ".mha"
    name_pair_1 = "moving" + name_we_0 + "_to_fixed" + name_we_1 + ".mha"

    return (os.path.join(dataset_name, name_pair_0), os.path.join(dataset_name, name_pair_1))


def merge_dicts(*dicts):
    return { key: value for dict in dicts for key, value in dict.items() }


def write_vtk(point_set, point_set_file_name):

    try:
        with open(point_set_file_name, 'w+') as f:
            f.write("# vtk DataFile Version 2.0\n")
            f.write("Point set for Continuous Registration Challenge\n")
            f.write("ASCII\n")
            f.write("DATASET POLYDATA\n")
            f.write("POINTS %i float\n" % point_set.shape[0])

            for point in point_set:
                for p in point:
                    f.write("%f " % p)

                f.write("\n")
    except Exception as e:
        raise Exception('Error during txt2vtk: %s' % str(e))


def get_script_path():
    return os.path.dirname(os.path.realpath(sys.argv[0]))


def read_vtk(file_name):
    return np.loadtxt(file_name, skiprows=5)


def read_pts(file_name, skiprows=0):
    return np.loadtxt(file_name, skiprows=skiprows)

def write_pts(point_set, point_set_file_name):
    np.savetxt(point_set_file_name, point_set)

def read_csv(path_file):
    """ loading points from a CSV file as ndarray of floats

    :param str path_file:
    :return ndarray:

    >>> content = " ,X,Y\\n1,226.4,173.5\\n2,278,182\\n3,256.7,171.2"
    >>> _= open('sample_points.csv', 'w').write(content)
    >>> load_csv('sample_points.csv')
    array([[ 226.4,  173.5],
           [ 278. ,  182. ],
           [ 256.7,  171.2]])
    >>> os.remove('sample_points.csv')
    """
    with open(path_file, 'r') as fp:
        lines = fp.readlines()
    points = [list(map(float, l.rstrip().split(',')[1:])) for l in lines[1:]]
    return np.array(points)


def warp_point_set(superelastix, point_set, disp_field_file_name):

    input_point_set_file_name = os.path.splitext(disp_field_file_name)[0] + '_input_point_set.vtk'
    output_point_set_file_name = os.path.splitext(disp_field_file_name)[0] + '_output_point_set.vtk'

    write_vtk(point_set, input_point_set_file_name)

    try:
        subprocess.check_output([
            superelastix,
            '--conf', os.path.join(os.path.dirname(os.path.realpath(__file__)), 'warp_point_set.json'),
            '--in', 'InputPointSet=%s' % input_point_set_file_name,
            'DisplacementField=%s' % disp_field_file_name,
            '--out', 'OutputPointSet=%s' % output_point_set_file_name,
            '--loglevel', 'trace',
            '--logfile', os.path.splitext(output_point_set_file_name)[0] + '.log'])
    except Exception as e:
        logging.error('\nFailed to warp %s: %s\n See %s.' %
                        (input_point_set_file_name, e, os.path.splitext(output_point_set_file_name)[0] + '.log'))
        raise Exception

    return read_vtk(output_point_set_file_name)


def warp_image(superelastix, input_image_file_name, disp_field_file_name, type_name):
    output_image_base_name, output_image_ext = disp_field_file_name.split(os.extsep, 1)
    output_image_file_name = output_image_base_name + '_' + type_name + "." + output_image_ext

    try:
        stdout = subprocess.check_output([superelastix,
                                          '--conf', os.path.join(os.path.dirname(os.path.realpath(__file__)), 'warp_label_image.json'),
                                          '--in', 'LabelImage=%s' % input_image_file_name,
                                          'DisplacementField=%s' % disp_field_file_name,
                                          '--out', 'WarpedLabelImage=%s' % output_image_file_name,
                                          '--loglevel', 'trace',
                                          '--logfile', os.path.splitext(output_image_file_name)[0] + '.log'])
    except:
        logging.error('Failed to warp %s.' % (input_image_file_name))


    return output_image_file_name


def compose_displacement_fields(superelastix, disp_field_file_name_0, disp_field_file_name_1):
    output_image_base_name, output_image_ext = disp_field_file_name_0.split(os.extsep, 1)
    output_image_file_name = output_image_base_name + '_inverse_consistency' + "." + output_image_ext

    try:
        cmd = [superelastix,
               '--conf', os.path.join(os.path.dirname(os.path.realpath(__file__)), 'compose_displacement_fields.json'),
               '--in', 'WarpingDisplacementField=%s' % disp_field_file_name_0,
               'DisplacementField=%s' % disp_field_file_name_1,
               '--out', 'DisplacementFieldSink=%s' % output_image_file_name,
               '--loglevel', 'trace',
               '--logfile', os.path.splitext(output_image_file_name)[0] + '.log']

        logging.debug(' '.join(cmd))
        stdout = subprocess.check_output(cmd)
    except Exception as e:
        logging.error('Failed to compose blueprints %s and %s' % (disp_field_file_name_0, disp_field_file_name_1))
        raise e


    return output_image_file_name


def index2point(image, index_set):
    dim = image.GetDimension()
    direction = np.array(image.GetDirection()).reshape((dim, dim))
    spacing = np.array(image.GetSpacing()).reshape((dim, 1))
    origin = np.array(image.GetOrigin()).reshape((dim, 1))

    # Point should be d-by-n, but are typically stored on disk in n-by-d. At least this function assumes so.
    index_set = np.array(index_set).transpose()

    point_set = np.dot(direction, (index_set - origin)) * spacing

    return point_set.transpose()

def splitext(filename):
    name, ext = os.path.splitext(filename)

    # Handle .nii.gz
    if name.endswith('.nii'):
        name, gz = os.path.splitext(name)
        return name, ext + gz
    else :
        return name, ext


def write_json(output_file_name, results):
    output_directory = os.path.dirname(output_file_name)
    if not os.path.exists(output_directory):
        os.mkdir(output_directory)

    with open(output_file_name, "w") as f:
        f.write(json.dumps(results, indent=4))
