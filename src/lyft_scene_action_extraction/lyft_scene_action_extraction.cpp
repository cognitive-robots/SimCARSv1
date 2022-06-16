
#include <ori/simcars/geometry/trig_buff.hpp>
#include <ori/simcars/map/lyft/lyft_map.hpp>
#include <ori/simcars/agent/driving_agent_scene.hpp>
#include <ori/simcars/agent/lyft/lyft_scene.hpp>
#include <ori/simcars/agent/csv/csv_scene.hpp>

#include <iostream>
#include <exception>

using namespace ori::simcars;

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        std::cerr << "Usage: ./lyft_scene_action_extraction input_map_file_path input_scene_file_path output_scene_file_path" << std::endl;
        return -1;
    }

    std::shared_ptr<const geometry::TrigBuff> trig_buff = geometry::TrigBuff::init_instance(360000, geometry::AngleType::RADIANS);

    std::cout << "Beginning map load" << std::endl;

    std::shared_ptr<const map::IMap<std::string>> map;

    try
    {
        map = map::lyft::LyftMap::load(argv[1]);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception occured during map load:" << std::endl << e.what() << std::endl;
        return -1;
    }

    std::cout << "Finished map load" << std::endl;

    std::cout << "Beginning scene load" << std::endl;

    std::shared_ptr<const agent::IFileBasedScene> scene;

    try
    {
        scene = agent::lyft::LyftScene::load(argv[2]);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception occured during scene load:" << std::endl << e.what() << std::endl;
        return -1;
    }

    std::cout << "Finished scene load" << std::endl;

    temporal::Duration scene_duration = scene->get_max_temporal_limit() - scene->get_min_temporal_limit();
    std::cout << "Scene is " << std::to_string(scene_duration.count() / 1000.0) << " s in length" << std::endl;

    std::cout << "Beginning action extraction" << std::endl;

    std::shared_ptr<const agent::IScene> scene_with_actions;

    try
    {
        scene_with_actions = agent::DrivingAgentScene::construct_from(scene, map);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception occured during action extraction:" << std::endl << e.what() << std::endl;
        return -1;
    }

    std::cout << "Finished action extraction" << std::endl;


    std::cout << "Beginning scene save" << std::endl;

    std::shared_ptr<const agent::IFileBasedScene> csv_scene_with_actions;

    try
    {
        csv_scene_with_actions = agent::csv::CSVScene::construct_from(scene_with_actions);
        csv_scene_with_actions->save(argv[3]);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception occured during scene save:" << std::endl << e.what() << std::endl;
        return -1;
    }

    std::cout << "Finished scene save" << std::endl;
}