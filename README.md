# Simulation-based Causal Analysis and Reasoning System (SimCARSv1)
Provides a framework for carrying out causal discovery by utilising simulation to derive counterfactual data on the outcomes associated with different sets of autonomous agent decisions. As the name suggests, this framework was primarily designed with driving agents in mind, but is by no means limited to this. Corresponds to v1 of the framework, the v2 SCM-based rewrite of the framework can be found at: https://github.com/cognitive-robots/SimCARSv2.

## Paper
This framework was developed as part of research into counterfactual causal development. Currently the first paper associated with this work is under review. The other resources associated with this paper can be found at the following link: https://github.com/cognitive-robots/counterfactual-cd-paper-resources. This section will be updated with links/information in the future.

## Compilation
While the system can be compiled and run on a variety of systems with a variety of libraries, for the purposes of reproduction, the framework was last compiled using the following set of software/libraries:

* GCC 10.3.0
* Eigen 3.3.7
* RapidJSON 1.1.0
* SFML 2.5.1
* Qt 5.12.8
* Qt Creator 4.11.0

From there, the project can be imported as a CMake project and compiled using the Qt Creator interface.

Alternatively assuming Qt and GCC are installed, the framework can be built using the following shell commands (starting in the project root directory):
```
mkdir build
cd build
cmake ..
make -j8
```
It may be necessary to use a tool like ccmake or cmake-gui in order to properly configure cmake for your system. Note: ```-j8``` can be omitted, it just specifies the maximum number of jobs to run at once.

## Built Executables

### Trigonometry Buffer Test
Tests that the trigonometry buffer compiles and runs without segmentation fault, also outputs execution times for the trigonometry buffer vs standard trigonometry functions.

```
usage: trig_buff_test
```

### Lyft
Executables specific to the Lyft Level-5 Prediction Dataset (https://self-driving.lyft.com/level5/prediction/).

#### Map Test
Tests that the Lyft map compiles and runs without segmentation fault.

```
usage: lyft_map_test map_file_path
```

Parameters:
* map_file_path: Specifies the file path of the map to load. Input format expected to be LZ4 compressed JSON map (See https://github.com/cognitive-robots/lyft_prediction_dataset_tools).

#### Scene Test
Tests that the Lyft scene compiles and runs without segmentation fault.

```
usage: lyft_scene_test scene_file_path
```

Parameters:
* scene_file_path: Specifies the file path of the scene to load. Input format expected to be LZ4 compressed JSON scene (See https://github.com/cognitive-robots/lyft_prediction_dataset_tools).

#### Scene Action Extraction
Converts a Lyft map and scene to a CSV file containing time series data for each variable.

```
usage: lyft_scene_action_extraction input_map_file_path input_scene_file_path output_scene_file_path
```

Parameters:
* input_map_file_path: Specifies the file path of the map to load. Input format expected to be LZ4 compressed JSON map (See https://github.com/cognitive-robots/lyft_prediction_dataset_tools).
* input_scene_file_path: Specifies the file path of the scene to load. Input format expected to be LZ4 compressed JSON scene (See https://github.com/cognitive-robots/lyft_prediction_dataset_tools).
* output_scene_file_path: Specifies the file path to output the CSV file to.

#### QSceneWidget Test
Tests that the QSceneWidget compiles, runs without segmentation fault and visualises correctly. QSceneWidget is designed to visualise the movement of agents during a scene.

```
usage: lyft_qscene_widget_test scene_file_path
```

Parameters:
* scene_file_path: Specifies the file path of the scene to load. Input format expected to be LZ4 compressed JSON scene (See https://github.com/cognitive-robots/lyft_prediction_dataset_tools).

#### QMapSceneWidget Test
Tests that the QMapSceneWidget compiles, runs without segmentation fault and visualises correctly. QMapSceneWidget is designed to visualise the same as QSceneWidget with the addition of a map.

```
usage: lyft_qmap_scene_widget_test map_file_path scene_file_path
```

Parameters:
* map_file_path: Specifies the file path of the map to load. Input format expected to be LZ4 compressed JSON map (See https://github.com/cognitive-robots/lyft_prediction_dataset_tools).
* scene_file_path: Specifies the file path of the scene to load. Input format expected to be LZ4 compressed JSON scene (See https://github.com/cognitive-robots/lyft_prediction_dataset_tools).

#### SimCARS Demo
Visualises two scenes, on the left the original scene, and on the right the original scene until the half way point and a simulation of the scene from that point onwards. Intended to allow for the comparison of the simulated scene against the original scene.

```
usage: lyft_simcars_demo map_file_path scene_file_path
```

Parameters:
* map_file_path: Specifies the file path of the map to load. Input format expected to be LZ4 compressed JSON map (See https://github.com/cognitive-robots/lyft_prediction_dataset_tools).
* scene_file_path: Specifies the file path of the scene to load. Input format expected to be LZ4 compressed JSON scene (See https://github.com/cognitive-robots/lyft_prediction_dataset_tools).

### High-D
Executables specific to the High-D Dataset (https://www.highd-dataset.com/).

#### Map Test
Tests that the High-D map compiles and runs without segmentation fault.

```
usage: highd_map_test recording_meta_file_path
```

Parameters:
* recording_meta_file_path: Specifies the file path of the scene recording meta file to load. The recording meta file contains map data despite being scene specific. This is one of the base formats used by High-D and it stores data as a CSV file.

#### Scene Test
Tests that the High-D scene compiles and runs without segmentation fault.

```
usage: highd_scene_test tracks_meta_file_path tracks_file_path
```

Parameters:
* tracks_meta_file_path: Specifies the file path of the scene tracks meta file to load. The tracks meta file contains meta information for individual agents. This is one of the base formats used by High-D and it stores data as a CSV file.
* tracks_file_path: Specifies the file path of the scene tracks file to load. The tracks file contains time series data for individual agents. This is one of the base formats used by High-D and it stores data as a CSV file.

#### Simulation Test
Tests the simulation functionality of the framework with High-D data.

```
usage: highd_simulation_test recording_meta_file_path tracks_meta_file_path tracks_file_path
```

Parameters:
* recording_meta_file_path: Specifies the file path of the scene recording meta file to load. The recording meta file contains meta information for the entire scene recording. This is one of the base formats used by High-D and it stores data as a CSV file.
* tracks_meta_file_path: Specifies the file path of the scene tracks meta file to load. The tracks meta file contains meta information for individual agents. This is one of the base formats used by High-D and it stores data as a CSV file.
* tracks_file_path: Specifies the file path of the scene tracks file to load. The tracks file contains time series data for individual agents. This is one of the base formats used by High-D and it stores data as a CSV file.

#### Bulk Simulation Test
Tests the bulk simulation functionality of the framework with High-D data. Currently configured to run 100 simulations.

```
usage: highd_bulk_simulation_test recording_meta_file_path tracks_meta_file_path tracks_file_path
```

Parameters:
* recording_meta_file_path: Specifies the file path of the scene recording meta file to load. The recording meta file contains meta information for the entire scene recording. This is one of the base formats used by High-D and it stores data as a CSV file.
* tracks_meta_file_path: Specifies the file path of the scene tracks meta file to load. The tracks meta file contains meta information for individual agents. This is one of the base formats used by High-D and it stores data as a CSV file.
* tracks_file_path: Specifies the file path of the scene tracks file to load. The tracks file contains time series data for individual agents. This is one of the base formats used by High-D and it stores data as a CSV file.

#### JSON Meta Simulation Test
Tests the simulation functionality of the framework with High-D data. JSON meta file contains data for a specific causal scene within the base High-D scene.

```
usage: highd_json_meta_simulation_test json_meta_file_path trimmed_data_directory_path
```

Parameters:
* json_meta_file_path: Specifies the file path of a JSON file describing meta information for a given causal scene, namely the base High-D scene id, and the agent ids of the lead convoy agent, tail convoy agent and independent agent.
* trimmed_data_directory_path: Specifies path to a directory containing trimmed versions of the base High-D scene files. In this case trimmed just means the files are cut to just the section where the relevant agents for a given causal scene are. This preprocessing step drastically speeds up load times.

#### JSON Meta Bulk Simulation Test
Tests the bulk simulation functionality of the framework with High-D data. Currently configured to run 10 simulations. JSON meta file contains data for a specific causal scene within the base High-D scene.

```
usage: highd_json_meta_bulk_simulation_test json_meta_file_path trimmed_data_directory_path
```

Parameters:
* json_meta_file_path: Specifies the file path of a JSON file describing meta information for a given causal scene, namely the base High-D scene id, and the agent ids of the lead convoy agent, tail convoy agent and independent agent.
* trimmed_data_directory_path: Specifies path to a directory containing trimmed versions of the base High-D scene files. In this case trimmed just means the files are cut to just the section where the relevant agents for a given causal scene are. This preprocessing step drastically speeds up load times.

#### JSON Meta Link Test
Tests the causal link testing functionality of the framework with High-D data. JSON meta file contains data for a specific causal scene within the base High-D scene.

```
usage: highd_json_meta_link_test json_meta_file_path trimmed_data_directory_path
```

Parameters:
* json_meta_file_path: Specifies the file path of a JSON file describing meta information for a given causal scene, namely the base High-D scene id, and the agent ids of the lead convoy agent, tail convoy agent and independent agent.
* trimmed_data_directory_path: Specifies path to a directory containing trimmed versions of the base High-D scene files. In this case trimmed just means the files are cut to just the section where the relevant agents for a given causal scene are. This preprocessing step drastically speeds up load times.

#### JSON Meta Causal Discovery
Carries out causal discovery on the specified causal scene comprised of High-D data. JSON meta file contains data for a specific causal scene within the base High-D scene.

```
usage: highd_json_meta_causal_discovery reward_diff_threshold input_json_meta_file_path trimmed_data_directory_path [output_json_meta_file_path]
```

Parameters:
* reward_diff_threshold: Specifies the threshold to apply to the reward-based metric for the purposes of the reward-based and hybrid variants for causal link testing.
* input_json_meta_file_path: Specifies the file path of a JSON file describing meta information for a given causal scene, namely the base High-D scene id, and the agent ids of the lead convoy agent, tail convoy agent and independent agent.
* trimmed_data_directory_path: Specifies path to a directory containing trimmed versions of the base High-D scene files. In this case trimmed just means the files are cut to just the section where the relevant agents for a given causal scene are. This preprocessing step drastically speeds up load times.
* output_json_meta_file_path: Specifies a file path to output a JSON file describing meta information for a given causal scene, including any causal links that have been discovered. If this is omitted the causal discoveries will not be written to file.

#### SimCARS Demo
Visualises two scenes, on the left the original scene, and on the right the original scene until the half way point and a simulation of the scene from that point onwards. Intended to allow for the comparison of the simulated scene against the original scene.

```
usage: highd_simcars_demo recording_meta_file_path tracks_meta_file_path tracks_file_path
```

Parameters:
* recording_meta_file_path: Specifies the file path of the scene recording meta file to load. The recording meta file contains meta information for the entire scene recording. This is one of the base formats used by High-D and it stores data as a CSV file.
* tracks_meta_file_path: Specifies the file path of the scene tracks meta file to load. The tracks meta file contains meta information for individual agents. This is one of the base formats used by High-D and it stores data as a CSV file.
* tracks_file_path: Specifies the file path of the scene tracks file to load. The tracks file contains time series data for individual agents. This is one of the base formats used by High-D and it stores data as a CSV file.

#### JSON Meta SimCARS Intervention Demo
Visualises two scenes, on the left the original scene, and on the right a scene that has had agent decisions are intervened upon, with simulation beginning at the point of behavioural divergence. Intended to allow for the comparison of the simulated intervened scene against the original scene.

```
usage: highd_json_meta_simcars_intervention_demo json_meta_file_path trimmed_data_directory_path

```

Parameters:
* json_meta_file_path: Specifies the file path of a JSON file describing meta information for a given causal scene, namely the base High-D scene id, and the agent ids of the lead convoy agent, tail convoy agent and independent agent.
* trimmed_data_directory_path: Specifies path to a directory containing trimmed versions of the base High-D scene files. In this case trimmed just means the files are cut to just the section where the relevant agents for a given causal scene are. This preprocessing step drastically speeds up load times.

#### JSON Meta SimCARS Causal Discovery Demo
Visualises four scenes reflecting the original scene, scene minus potential effect decision, scene minus potential cause decision, and scene minus both potential cause and effect decisions. This helps to visualise the causal discovery process undertaken by the framework.

```
usage: highd_json_meta_simcars_causal_discovery_demo json_meta_file_path trimmed_data_directory_path [potential_causal_link_index]
```

Parameters:
* json_meta_file_path: Specifies the file path of a JSON file describing meta information for a given causal scene, namely the base High-D scene id, and the agent ids of the lead convoy agent, tail convoy agent and independent agent.
* trimmed_data_directory_path: Specifies path to a directory containing trimmed versions of the base High-D scene files. In this case trimmed just means the files are cut to just the section where the relevant agents for a given causal scene are. This preprocessing step drastically speeds up load times.
* potential_causal_link_index: Specifies the potential causal link to focus upon with the visualisation. Omitting this will cause the exectuable to print a list of the potential causal links with associated indexes instead of producing a visualisation.
