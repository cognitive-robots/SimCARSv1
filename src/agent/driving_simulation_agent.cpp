
#include <ori/simcars/utils/exceptions.hpp>
#include <ori/simcars/agent/basic_constant.hpp>
#include <ori/simcars/agent/simulated_variable.hpp>
#include <ori/simcars/agent/driving_simulation_agent.hpp>

#include <iostream>

namespace ori
{
namespace simcars
{
namespace agent
{

DrivingSimulationAgent::DrivingSimulationAgent() {}

DrivingSimulationAgent::DrivingSimulationAgent(std::shared_ptr<const IDrivingAgent> driving_agent,
                                               std::shared_ptr<const ISimulationScene> simulation_scene,
                                               temporal::Time simulation_start_time, bool allow_late_start) :
    DrivingSimulationAgent(driving_agent, simulation_scene, simulation_start_time,
                           driving_agent->get_max_temporal_limit(), allow_late_start)
{}

DrivingSimulationAgent::DrivingSimulationAgent(std::shared_ptr<const IDrivingAgent> driving_agent,
                                               std::shared_ptr<const ISimulationScene> simulation_scene,
                                               temporal::Time simulation_start_time, temporal::Time simulation_end_time,
                                               bool allow_late_start)
    : driving_agent(driving_agent)
{
    if (simulation_start_time < driving_agent->get_min_temporal_limit())
    {
        if (allow_late_start)
        {
            simulation_start_time = driving_agent->get_min_temporal_limit();
        }
        else
        {
            throw std::invalid_argument("Simulation start time is before earliest event for original agent");
        }
    }

    if (simulation_start_time > driving_agent->get_max_temporal_limit())
    {
        throw std::invalid_argument("Simulation start time is after latest event for original agent");
    }

    if (simulation_start_time > simulation_end_time)
    {
        throw std::invalid_argument("Simulation start time is after simulation end time");
    }

    this->max_temporal_limit = simulation_end_time;


    std::shared_ptr<const IVariable<geometry::Vec>> position_variable = driving_agent->get_position_variable();
    std::shared_ptr<const SimulatedVariable<geometry::Vec>> simulated_position_variable(
                new SimulatedVariable(position_variable, simulation_scene, simulation_start_time, simulation_end_time));
    this->simulated_variable_dict.update(simulated_position_variable->get_full_name(), simulated_position_variable);

    std::shared_ptr<const IVariable<geometry::Vec>> linear_velocity_variable =
            driving_agent->get_linear_velocity_variable();
    std::shared_ptr<const SimulatedVariable<geometry::Vec>> simulated_linear_velocity_variable(
                new SimulatedVariable(linear_velocity_variable, simulation_scene, simulation_start_time,
                                      simulation_end_time));
    this->simulated_variable_dict.update(simulated_linear_velocity_variable->get_full_name(), simulated_linear_velocity_variable);

    std::shared_ptr<const IVariable<FP_DATA_TYPE>> aligned_linear_velocity_variable =
            driving_agent->get_aligned_linear_velocity_variable();
    std::shared_ptr<const SimulatedVariable<FP_DATA_TYPE>> simulated_aligned_linear_velocity_variable(
                new SimulatedVariable(aligned_linear_velocity_variable, simulation_scene, simulation_start_time,
                                      simulation_end_time));
    this->simulated_variable_dict.update(simulated_aligned_linear_velocity_variable->get_full_name(), simulated_aligned_linear_velocity_variable);

    std::shared_ptr<const IVariable<geometry::Vec>> linear_acceleration_variable =
            driving_agent->get_linear_acceleration_variable();
    std::shared_ptr<const SimulatedVariable<geometry::Vec>> simulated_linear_acceleration_variable(
                new SimulatedVariable(linear_acceleration_variable, simulation_scene, simulation_start_time,
                                      simulation_end_time));
    this->simulated_variable_dict.update(simulated_linear_acceleration_variable->get_full_name(), simulated_linear_acceleration_variable);

    std::shared_ptr<const IVariable<FP_DATA_TYPE>> aligned_linear_acceleration_variable =
            driving_agent->get_aligned_linear_acceleration_variable();
    std::shared_ptr<const SimulatedVariable<FP_DATA_TYPE>> simulated_aligned_linear_acceleration_variable(
                new SimulatedVariable(aligned_linear_acceleration_variable, simulation_scene, simulation_start_time,
                                      simulation_end_time));
    this->simulated_variable_dict.update(simulated_aligned_linear_acceleration_variable->get_full_name(), simulated_aligned_linear_acceleration_variable);

    std::shared_ptr<const IVariable<geometry::Vec>> external_linear_acceleration_variable =
            driving_agent->get_external_linear_acceleration_variable();
    std::shared_ptr<const SimulatedVariable<geometry::Vec>> simulated_external_linear_acceleration_variable(
                new SimulatedVariable(external_linear_acceleration_variable, simulation_scene, simulation_start_time,
                                      simulation_end_time));
    this->simulated_variable_dict.update(simulated_external_linear_acceleration_variable->get_full_name(), simulated_external_linear_acceleration_variable);

    std::shared_ptr<const IVariable<FP_DATA_TYPE>> rotation_variable =
            driving_agent->get_rotation_variable();
    std::shared_ptr<const SimulatedVariable<FP_DATA_TYPE>> simulated_rotation_variable(
                new SimulatedVariable(rotation_variable, simulation_scene, simulation_start_time,
                                      simulation_end_time));
    this->simulated_variable_dict.update(simulated_rotation_variable->get_full_name(), simulated_rotation_variable);

    std::shared_ptr<const IVariable<FP_DATA_TYPE>> steer_variable =
            driving_agent->get_steer_variable();
    std::shared_ptr<const SimulatedVariable<FP_DATA_TYPE>> simulated_steer_variable(
                new SimulatedVariable(steer_variable, simulation_scene, simulation_start_time,
                                      simulation_end_time));
    this->simulated_variable_dict.update(simulated_steer_variable->get_full_name(), simulated_steer_variable);

    std::shared_ptr<const IVariable<FP_DATA_TYPE>> angular_velocity_variable =
            driving_agent->get_angular_velocity_variable();
    std::shared_ptr<const SimulatedVariable<FP_DATA_TYPE>> simulated_angular_velocity_variable(
                new SimulatedVariable(angular_velocity_variable, simulation_scene, simulation_start_time,
                                      simulation_end_time));
    this->simulated_variable_dict.update(simulated_angular_velocity_variable->get_full_name(), simulated_angular_velocity_variable);


    std::shared_ptr<const structures::IArray<std::shared_ptr<const IValuelessVariable>>> variables =
            driving_agent->get_variable_parameters();
    for (size_t i = 0; i < variables->count(); ++i)
    {
        if (!this->simulated_variable_dict.contains((*variables)[i]->get_full_name()))
        {
            this->non_simulated_variable_dict.update((*variables)[i]->get_full_name(), (*variables)[i]);
        }
    }
}

std::string DrivingSimulationAgent::get_name() const
{
    return this->driving_agent->get_name();
}

// Not updated by simulation
geometry::Vec DrivingSimulationAgent::get_min_spatial_limits() const
{
    return this->driving_agent->get_min_spatial_limits();
}

// Not updated by simulation
geometry::Vec DrivingSimulationAgent::get_max_spatial_limits() const
{
    return this->driving_agent->get_max_spatial_limits();
}

temporal::Time DrivingSimulationAgent::get_min_temporal_limit() const
{
    return this->driving_agent->get_min_temporal_limit();
}

temporal::Time DrivingSimulationAgent::get_max_temporal_limit() const
{
    return this->max_temporal_limit;
}

std::shared_ptr<structures::IArray<std::shared_ptr<const IValuelessConstant>>> DrivingSimulationAgent::get_constant_parameters() const
{
    return this->driving_agent->get_constant_parameters();
}

std::shared_ptr<const IValuelessConstant> DrivingSimulationAgent::get_constant_parameter(const std::string& constant_name) const
{
    return this->driving_agent->get_constant_parameter(constant_name);
}

std::shared_ptr<structures::IArray<std::shared_ptr<const IValuelessVariable>>> DrivingSimulationAgent::get_variable_parameters() const
{
    std::shared_ptr<structures::stl::STLConcatArray<std::shared_ptr<const IValuelessVariable>>> variables(
                new structures::stl::STLConcatArray<std::shared_ptr<const IValuelessVariable>>(2));

    variables->get_array(0) =
            std::shared_ptr<structures::IArray<std::shared_ptr<const IValuelessVariable>>>(
                new structures::stl::STLStackArray<std::shared_ptr<const IValuelessVariable>>(
                    non_simulated_variable_dict.get_values()));

    std::shared_ptr<structures::IArray<std::shared_ptr<const IValuelessVariable>>> simulated_variables(
                new structures::stl::STLStackArray<std::shared_ptr<const IValuelessVariable>>());
    cast_array<std::shared_ptr<const ISimulatedValuelessVariable>, std::shared_ptr<const IValuelessVariable>>(
                *(simulated_variable_dict.get_values()), *simulated_variables);
    variables->get_array(1) = simulated_variables;

    return variables;
}

std::shared_ptr<const IValuelessVariable> DrivingSimulationAgent::get_variable_parameter(const std::string& variable_name) const
{
    if (non_simulated_variable_dict.contains(variable_name))
    {
        return non_simulated_variable_dict[variable_name];
    }
    else
    {
        return simulated_variable_dict[variable_name];
    }
}

std::shared_ptr<structures::IArray<std::shared_ptr<const IValuelessEvent>>> DrivingSimulationAgent::get_events() const
{
    std::shared_ptr<const structures::IArray<std::string>> non_simulated_variable_names = non_simulated_variable_dict.get_keys();
    std::shared_ptr<const structures::IArray<std::string>> simulated_variable_names = simulated_variable_dict.get_keys();

    std::shared_ptr<structures::stl::STLConcatArray<std::shared_ptr<const IValuelessEvent>>> events(
                new structures::stl::STLConcatArray<std::shared_ptr<const IValuelessEvent>>(
                    non_simulated_variable_names->count() + simulated_variable_names->count()));

    size_t i;

    for(i = 0; i < non_simulated_variable_names->count(); ++i)
    {
        events->get_array(i) = non_simulated_variable_dict[(*non_simulated_variable_names)[i]]->get_valueless_events();
    }

    for(i = 0; i < simulated_variable_names->count(); ++i)
    {
        events->get_array(non_simulated_variable_names->count() + i) = simulated_variable_dict[(*simulated_variable_names)[i]]->get_valueless_events();
    }

    return events;
}

/*
 * NOTE: This should not be called directly by external code, as the simulation scene will not have a pointer to the
 * resulting copy and thus any new simulation data will not be propogated to the copy.
 */
std::shared_ptr<IDrivingAgent> DrivingSimulationAgent::driving_agent_deep_copy() const
{
    std::shared_ptr<DrivingSimulationAgent> driving_agent(new DrivingSimulationAgent());

    driving_agent->driving_agent = this->driving_agent;
    driving_agent->max_temporal_limit = this->max_temporal_limit;

    size_t i;

    std::shared_ptr<const structures::IArray<std::string>> non_simulated_variable_names = non_simulated_variable_dict.get_keys();
    for(i = 0; i < non_simulated_variable_names->count(); ++i)
    {
        driving_agent->non_simulated_variable_dict.update((*non_simulated_variable_names)[i], non_simulated_variable_dict[(*non_simulated_variable_names)[i]]->valueless_deep_copy());
    }

    std::shared_ptr<const structures::IArray<std::string>> simulated_variable_names = simulated_variable_dict.get_keys();
    for(i = 0; i < simulated_variable_names->count(); ++i)
    {
        driving_agent->simulated_variable_dict.update((*simulated_variable_names)[i], simulated_variable_dict[(*simulated_variable_names)[i]]->simulated_valueless_deep_copy());
    }

    return driving_agent;
}

std::shared_ptr<IDrivingAgentState> DrivingSimulationAgent::get_driving_agent_state(temporal::Time time, bool throw_on_out_of_range) const
{
    std::shared_ptr<IDrivingAgentState> driving_agent_state =
            driving_agent->get_driving_agent_state(time, false);

    try
    {
        if (driving_agent_state->get_id_constant()->get_value() == 1310)
        {
            //std::cerr << "DrivingSimulationAgent: " << this->get_position_variable()->get_min_temporal_limit().time_since_epoch().count() << std::endl;
        }
        driving_agent_state->set_position_variable(
                    std::shared_ptr<const IConstant<geometry::Vec>>(
                        new BasicConstant(
                            this->get_name(),
                            "position.base",
                            this->get_position_variable()->get_value(time))));
        driving_agent_state->set_linear_velocity_variable(
                    std::shared_ptr<const IConstant<geometry::Vec>>(
                        new BasicConstant(
                            this->get_name(),
                            "linear_velocity.base",
                            this->get_linear_velocity_variable()->get_value(time))));
        driving_agent_state->set_aligned_linear_velocity_variable(
                    std::shared_ptr<const IConstant<FP_DATA_TYPE>>(
                        new BasicConstant(
                            this->get_name(),
                            "aligned_linear_velocity.base",
                            this->get_aligned_linear_velocity_variable()->get_value(time))));
        driving_agent_state->set_linear_acceleration_variable(
                    std::shared_ptr<const IConstant<geometry::Vec>>(
                        new BasicConstant(
                            this->get_name(),
                            "linear_acceleration.base",
                            this->get_linear_acceleration_variable()->get_value(time))));
        driving_agent_state->set_aligned_linear_acceleration_variable(
                    std::shared_ptr<const IConstant<FP_DATA_TYPE>>(
                        new BasicConstant(
                            this->get_name(),
                            "aligned_linear_acceleration.indirect_actuation",
                            this->get_aligned_linear_acceleration_variable()->get_value(time))));
        driving_agent_state->set_external_linear_acceleration_variable(
                    std::shared_ptr<const IConstant<geometry::Vec>>(
                        new BasicConstant(
                            this->get_name(),
                            "linear_acceleration.external",
                            this->get_external_linear_acceleration_variable()->get_value(time))));
        driving_agent_state->set_rotation_variable(
                    std::shared_ptr<const IConstant<FP_DATA_TYPE>>(
                        new BasicConstant(
                            this->get_name(),
                            "rotation.base",
                            this->get_rotation_variable()->get_value(time))));
        driving_agent_state->set_steer_variable(
                    std::shared_ptr<const IConstant<FP_DATA_TYPE>>(
                        new BasicConstant(
                            this->get_name(),
                            "steer.indirect_actuation",
                            this->get_steer_variable()->get_value(time))));
        driving_agent_state->set_angular_velocity_variable(
                    std::shared_ptr<const IConstant<FP_DATA_TYPE>>(
                        new BasicConstant(
                            this->get_name(),
                            "angular_velocity.base",
                            this->get_angular_velocity_variable()->get_value(time))));
    }
    catch (std::out_of_range e)
    {
        if (throw_on_out_of_range)
        {
            throw e;
        }
    }

    return driving_agent_state;
}

void DrivingSimulationAgent::propogate(temporal::Time time, std::shared_ptr<const IDrivingAgentState> state) const
{
    std::shared_ptr<const structures::IArray<std::shared_ptr<const ISimulatedValuelessVariable>>> simulated_variables =
            simulated_variable_dict.get_values();
    for(size_t i = 0; i < simulated_variables->count(); ++i)
    {
        (*simulated_variables)[i]->simulation_update(time, state);
    }
}

}
}
}
