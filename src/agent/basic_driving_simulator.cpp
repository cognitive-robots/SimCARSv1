
#include <ori/simcars/structures/stl/stl_stack_array.hpp>
#include <ori/simcars/geometry/trig_buff.hpp>
#include <ori/simcars/geometry/o_rect.hpp>
#include <ori/simcars/agent/variable_interface.hpp>
#include <ori/simcars/agent/basic_constant.hpp>
#include <ori/simcars/agent/basic_driving_agent_state.hpp>
#include <ori/simcars/agent/basic_driving_simulator.hpp>
#include <ori/simcars/agent/view_driving_agent_state.hpp>
#include <ori/simcars/agent/driving_simulation_agent.hpp>

#include <iostream>
#include <cassert>

namespace ori
{
namespace simcars
{
namespace agent
{

BasicDrivingSimulator::BasicDrivingSimulator(IDrivingAgentController const *controller) : controller(controller) {}

BasicDrivingSimulator::~BasicDrivingSimulator()
{
    delete controller;
}

void BasicDrivingSimulator::simulate(IReadOnlySceneState const *current_state, ISceneState *next_state, temporal::Duration time_step) const
{
    this->simulate_driving_scene(
                dynamic_cast<IDrivingSceneState const*>(current_state),
                dynamic_cast<IDrivingSceneState*>(next_state),
                time_step);
}

void BasicDrivingSimulator::simulate_driving_scene(
        IReadOnlyDrivingSceneState const *current_state,
        IDrivingSceneState *next_state,
        temporal::Duration time_step) const
{
    structures::IArray<IReadOnlyDrivingAgentState const*> *current_driving_agent_states =
            current_state->get_driving_agent_states();

    structures::IArray<IDrivingAgentState*> *next_driving_agent_states =
            next_state->get_mutable_driving_agent_states();

    size_t i, j;
    for (i = 0; i < current_driving_agent_states->count(); ++i)
    {
        IReadOnlyDrivingAgentState const *current_driving_agent_state = (*current_driving_agent_states)[i];

        for (j = 0; j < next_driving_agent_states->count(); ++j)
        {
            IDrivingAgentState *next_driving_agent_state = (*next_driving_agent_states)[j];

            if (current_driving_agent_state->get_name() == next_driving_agent_state->get_name())
            {
                bool result;
                if (next_driving_agent_state->get_name().find("non_ego") == std::string::npos)
                {
                    result = next_driving_agent_state->is_populated();
                }

                if (next_driving_agent_state->is_populated())
                {
                    break;
                }

                IConstant<geometry::Vec> *external_linear_acceleration_variable_value =
                            new BasicConstant<geometry::Vec>(
                                next_driving_agent_state->get_name(),
                                "linear_acceleration.external",
                                geometry::Vec::Zero());
                next_driving_agent_state->set_external_linear_acceleration_variable(external_linear_acceleration_variable_value);

                controller->modify_driving_agent_state(current_driving_agent_state, next_driving_agent_state);
                simulate_driving_agent(current_driving_agent_state, next_driving_agent_state, time_step);

                break;
            }

        }

        delete current_driving_agent_state;
    }

    delete current_driving_agent_states;

    geometry::TrigBuff const *trig_buff = geometry::TrigBuff::get_instance();

    for (i = 0; i < next_driving_agent_states->count(); ++i)
    {
        IDrivingAgentState *next_driving_agent_state_1 = (*next_driving_agent_states)[i];

        geometry::Vec position_1 = next_driving_agent_state_1->get_position_variable()->get_value();
        geometry::Vec velocity_1 = next_driving_agent_state_1->get_linear_velocity_variable()->get_value();
        FP_DATA_TYPE length_1 = next_driving_agent_state_1->get_bb_length_constant()->get_value();
        FP_DATA_TYPE width_1 = next_driving_agent_state_1->get_bb_width_constant()->get_value();
        FP_DATA_TYPE rotation_1 = next_driving_agent_state_1->get_rotation_variable()->get_value();

        geometry::ORect bounding_box_1(position_1, length_1, width_1, rotation_1);

        for (size_t j = i + 1; j < next_driving_agent_states->count(); ++j)
        {
            IDrivingAgentState *next_driving_agent_state_2 = (*next_driving_agent_states)[j];

            geometry::Vec position_2 = next_driving_agent_state_2->get_position_variable()->get_value();
            geometry::Vec velocity_2 = next_driving_agent_state_2->get_linear_velocity_variable()->get_value();
            FP_DATA_TYPE length_2 = next_driving_agent_state_2->get_bb_length_constant()->get_value();
            FP_DATA_TYPE width_2 = next_driving_agent_state_2->get_bb_width_constant()->get_value();
            FP_DATA_TYPE rotation_2 = next_driving_agent_state_2->get_rotation_variable()->get_value();

            geometry::ORect bounding_box_2(position_2, length_2, width_2, rotation_2);

            if (bounding_box_1.check_collision(bounding_box_2))
            {
                IReadOnlyDrivingAgentState const *current_driving_agent_state_1 =
                        current_state->get_driving_agent_state(next_driving_agent_state_1->get_name());
                IReadOnlyDrivingAgentState const *current_driving_agent_state_2 =
                        current_state->get_driving_agent_state(next_driving_agent_state_2->get_name());

                geometry::Vec direction = (position_2 - position_1).normalized();

                geometry::Vec acceleration_1 = next_driving_agent_state_1->get_linear_acceleration_variable()->get_value();
                FP_DATA_TYPE mass_1 = length_1 * width_1;
                FP_DATA_TYPE collision_velocity_1 = velocity_1.dot(direction);

                geometry::Vec acceleration_2 = next_driving_agent_state_2->get_linear_acceleration_variable()->get_value();
                FP_DATA_TYPE mass_2 = length_2 * width_2;
                FP_DATA_TYPE collision_velocity_2 = velocity_2.dot(direction);

                FP_DATA_TYPE resulting_velocity = ((mass_1 * collision_velocity_1) + (mass_2 * collision_velocity_2))
                        / (mass_1 + mass_2);

                geometry::Vec new_velocity_1 = velocity_1 + (resulting_velocity - collision_velocity_1) * direction;
                geometry::Vec new_velocity_2 = velocity_2 + (resulting_velocity - collision_velocity_2) * direction;

                geometry::Vec previous_velocity_1 = current_driving_agent_state_1->get_linear_velocity_variable()->get_value();
                geometry::Vec previous_velocity_2 = current_driving_agent_state_2->get_linear_velocity_variable()->get_value();

                geometry::Vec mean_acceleration_1 = (new_velocity_1 - previous_velocity_1) / time_step.count();
                geometry::Vec mean_acceleration_2 = (new_velocity_2 - previous_velocity_2) / time_step.count();

                geometry::Vec previous_acceleration_1 =
                        current_driving_agent_state_1->get_linear_acceleration_variable()->get_value();
                geometry::Vec previous_acceleration_2 =
                        current_driving_agent_state_2->get_linear_acceleration_variable()->get_value();

                FP_DATA_TYPE rotation_1 = next_driving_agent_state_1->get_rotation_variable()->get_value();
                FP_DATA_TYPE rotation_2 = next_driving_agent_state_2->get_rotation_variable()->get_value();

                // This is quite messy, would be better not to rely upon casts, this is a temporary solution to see if this
                // approach is viable
                ViewDrivingAgentState *view_driving_agent_state_1 =
                        dynamic_cast<ViewDrivingAgentState*>(next_driving_agent_state_1);
                dynamic_cast<DrivingSimulationAgent const*>(
                            view_driving_agent_state_1->get_agent())->begin_simulation(
                            view_driving_agent_state_1->get_time() - time_step);
                controller->modify_driving_agent_state(current_driving_agent_state_1, view_driving_agent_state_1);
                ViewDrivingAgentState *view_driving_agent_state_2 =
                        dynamic_cast<ViewDrivingAgentState*>(next_driving_agent_state_2);
                dynamic_cast<DrivingSimulationAgent const*>(
                            view_driving_agent_state_2->get_agent())->begin_simulation(
                            view_driving_agent_state_2->get_time() - time_step);
                controller->modify_driving_agent_state(current_driving_agent_state_2, view_driving_agent_state_2);

                FP_DATA_TYPE revised_aligned_acceleration_1 = view_driving_agent_state_1->get_aligned_linear_acceleration_variable()->get_value();
                FP_DATA_TYPE revised_aligned_acceleration_2 = view_driving_agent_state_2->get_aligned_linear_acceleration_variable()->get_value();

                geometry::Vec revised_acceleration_1;
                revised_acceleration_1.x() = revised_aligned_acceleration_1 * trig_buff->get_cos(rotation_1);
                revised_acceleration_1.y() = revised_aligned_acceleration_1 * trig_buff->get_sin(rotation_1);
                geometry::Vec revised_acceleration_2;
                revised_acceleration_2.x() = revised_aligned_acceleration_2 * trig_buff->get_cos(rotation_2);
                revised_acceleration_2.y() = revised_aligned_acceleration_2 * trig_buff->get_sin(rotation_2);

                geometry::Vec new_acceleration_1 = mean_acceleration_1 - 0.5 * (previous_acceleration_1 + revised_acceleration_1);
                geometry::Vec new_acceleration_2 = mean_acceleration_2 - 0.5 * (previous_acceleration_2 + revised_acceleration_2);

                geometry::Vec external_acceleration_1 = new_acceleration_1 - revised_acceleration_1;
                geometry::Vec external_acceleration_2 = new_acceleration_2 - revised_acceleration_2;

                IConstant<geometry::Vec> *external_linear_acceleration_variable_1(
                            new BasicConstant<geometry::Vec>(
                                view_driving_agent_state_1->get_name(),
                                "linear_acceleration.external",
                                external_acceleration_1));
                view_driving_agent_state_1->set_external_linear_acceleration_variable(external_linear_acceleration_variable_1);
                simulate_driving_agent(current_driving_agent_state_1, view_driving_agent_state_1, time_step);

                IConstant<geometry::Vec> *external_linear_acceleration_variable_2(
                            new BasicConstant<geometry::Vec>(
                                next_driving_agent_state_2->get_name(),
                                "linear_acceleration.external",
                                external_acceleration_2));
                next_driving_agent_state_2->set_external_linear_acceleration_variable(external_linear_acceleration_variable_2);
                simulate_driving_agent(current_driving_agent_state_2, next_driving_agent_state_2, time_step);
            }
        }

        IValuelessConstant *ttc_valueless_variable_value =
                next_driving_agent_state_1->get_mutable_parameter_value(
                    next_driving_agent_state_1->get_name() + ".ttc.base");
        if (ttc_valueless_variable_value != nullptr)
        {
            IConstant<temporal::Duration> *ttc_variable_value =
                    dynamic_cast<IConstant<temporal::Duration>*>(
                        ttc_valueless_variable_value);
            temporal::Duration smallest_ttc = temporal::Duration::max();
            for (size_t j = 0; j < next_driving_agent_states->count(); ++j)
            {
                IDrivingAgentState *next_driving_agent_state_2 = (*next_driving_agent_states)[j];

                geometry::Vec position_2 = next_driving_agent_state_2->get_position_variable()->get_value();
                geometry::Vec velocity_2 = next_driving_agent_state_2->get_linear_velocity_variable()->get_value();
                FP_DATA_TYPE length_2 = next_driving_agent_state_2->get_bb_length_constant()->get_value();
                FP_DATA_TYPE width_2 = next_driving_agent_state_2->get_bb_width_constant()->get_value();
                FP_DATA_TYPE rotation_2 = next_driving_agent_state_2->get_rotation_variable()->get_value();

                geometry::ORect bounding_box_2(position_2, length_2, width_2, rotation_2);

                geometry::Vec position_diff = position_2 - position_1;
                FP_DATA_TYPE position_diff_norm = position_diff.norm();
                geometry::Vec velocity_diff = velocity_1 - velocity_2;
                FP_DATA_TYPE velocity_diff_norm = velocity_diff.norm();
                FP_DATA_TYPE combined_span =
                        0.5f * (bounding_box_1.get_span() + bounding_box_2.get_span());
                FP_DATA_TYPE dot_product_limit =
                        1.0f / std::sqrt(std::pow(combined_span / position_diff_norm, 2.0f) + 1.0f);
                FP_DATA_TYPE dot_product =
                        position_diff.dot(velocity_diff) /
                        (position_diff_norm * velocity_diff_norm);

                if (dot_product <= dot_product_limit)
                {
                    temporal::Duration ttc(int64_t(position_diff_norm / (velocity_diff_norm * dot_product)));
                    smallest_ttc = std::min(ttc, smallest_ttc);
                }
            }
            ttc_variable_value->set_value(smallest_ttc);
        }
    }

    for (size_t i = 0; i < next_driving_agent_states->count(); ++i)
    {
        delete (*next_driving_agent_states)[i];
    }

    delete next_driving_agent_states;
}

void BasicDrivingSimulator::simulate_driving_agent(
        IReadOnlyDrivingAgentState const *current_state,
        IDrivingAgentState *next_state,
        temporal::Duration time_step) const
{
    IConstant<FP_DATA_TYPE> const *aligned_linear_acceleration_variable_value =
            current_state->get_aligned_linear_acceleration_variable();
    IConstant<FP_DATA_TYPE> const *new_aligned_linear_acceleration_variable_value =
            next_state->get_aligned_linear_acceleration_variable();
    FP_DATA_TYPE aligned_linear_acceleration = aligned_linear_acceleration_variable_value->get_value();
    FP_DATA_TYPE new_aligned_linear_acceleration = new_aligned_linear_acceleration_variable_value->get_value();

    IConstant<FP_DATA_TYPE> const *steer_variable_value = current_state->get_steer_variable();
    IConstant<FP_DATA_TYPE> const *new_steer_variable_value = next_state->get_steer_variable();
    FP_DATA_TYPE steer = steer_variable_value->get_value();
    FP_DATA_TYPE new_steer = new_steer_variable_value->get_value();

    IConstant<geometry::Vec> const *new_external_linear_acceleration_variable_value =
            next_state->get_external_linear_acceleration_variable();
    geometry::Vec new_external_linear_acceleration = new_external_linear_acceleration_variable_value->get_value();

    IConstant<geometry::Vec> const *linear_acceleration_variable_value =
            current_state->get_linear_acceleration_variable();
    geometry::Vec linear_acceleration = linear_acceleration_variable_value->get_value();

    IConstant<FP_DATA_TYPE> const *aligned_linear_velocity_variable_value =
            current_state->get_aligned_linear_velocity_variable();
    FP_DATA_TYPE aligned_linear_velocity = aligned_linear_velocity_variable_value->get_value();

    IConstant<geometry::Vec> const *linear_velocity_variable_value = current_state->get_linear_velocity_variable();
    geometry::Vec linear_velocity = linear_velocity_variable_value->get_value();

    IConstant<FP_DATA_TYPE> const *angular_velocity_variable_value = current_state->get_angular_velocity_variable();
    FP_DATA_TYPE angular_velocity = angular_velocity_variable_value->get_value();

    IConstant<geometry::Vec> const *position_variable_value = current_state->get_position_variable();
    geometry::Vec position = position_variable_value->get_value();

    IConstant<FP_DATA_TYPE> const *rotation_variable_value = current_state->get_rotation_variable();
    FP_DATA_TYPE rotation = rotation_variable_value->get_value();


    FP_DATA_TYPE mean_aligned_linear_acceleration = (aligned_linear_acceleration + new_aligned_linear_acceleration) / 2.0f;

    FP_DATA_TYPE estimated_new_aligned_linear_velocity = aligned_linear_velocity + mean_aligned_linear_acceleration * time_step.count();

    FP_DATA_TYPE estimated_mean_aligned_linear_velocity = (aligned_linear_velocity + estimated_new_aligned_linear_velocity) / 2.0f;

    FP_DATA_TYPE new_angular_velocity = new_steer * estimated_mean_aligned_linear_velocity;

    IConstant<FP_DATA_TYPE> *new_angular_velocity_variable_value =
                new BasicConstant<FP_DATA_TYPE>(next_state->get_name(), "angular_velocity.base", new_angular_velocity);
    next_state->set_angular_velocity_variable(new_angular_velocity_variable_value);

    FP_DATA_TYPE mean_angular_velocity = (angular_velocity + new_angular_velocity) / 2.0f;

    FP_DATA_TYPE new_rotation = rotation + mean_angular_velocity * time_step.count();
    assert(!std::isnan(new_rotation));

    IConstant<FP_DATA_TYPE> *new_rotation_variable_value =
                new BasicConstant<FP_DATA_TYPE>(next_state->get_name(), "rotation.base", new_rotation);
    next_state->set_rotation_variable(new_rotation_variable_value);


    geometry::TrigBuff const *trig_buff = geometry::TrigBuff::get_instance();

    geometry::Vec new_linear_acceleration;
    new_linear_acceleration.x() = new_aligned_linear_acceleration * trig_buff->get_cos(new_rotation);
    new_linear_acceleration.y() = new_aligned_linear_acceleration * trig_buff->get_sin(new_rotation);
    new_linear_acceleration += new_external_linear_acceleration;

    IConstant<geometry::Vec> *new_linear_acceleration_variable_value =
                new BasicConstant<geometry::Vec>(
                    next_state->get_name(), "linear_acceleration.base", new_linear_acceleration);
    next_state->set_linear_acceleration_variable(new_linear_acceleration_variable_value);

    geometry::Vec mean_linear_acceleration = (linear_acceleration + new_linear_acceleration) / 2.0f;

    geometry::Vec new_linear_velocity = linear_velocity + mean_linear_acceleration * time_step.count();

    IConstant<geometry::Vec> *new_linear_velocity_variable_value =
                new BasicConstant<geometry::Vec>(next_state->get_name(), "linear_velocity.base", new_linear_velocity);
    next_state->set_linear_velocity_variable(new_linear_velocity_variable_value);

    FP_DATA_TYPE new_aligned_linear_velocity = (trig_buff->get_rot_mat(-new_rotation) * new_linear_velocity).x();

    IConstant<FP_DATA_TYPE> *new_aligned_linear_velocity_variable_value =
                new BasicConstant<FP_DATA_TYPE>(
                    next_state->get_name(), "aligned_linear_velocity.base", new_aligned_linear_velocity);
    next_state->set_aligned_linear_velocity_variable(new_aligned_linear_velocity_variable_value);

    geometry::Vec mean_linear_velocity = (linear_velocity + new_linear_velocity) / 2.0f;

    geometry::Vec new_position = position + mean_linear_velocity * time_step.count();
    assert(!std::isnan(new_position.x()));
    assert(!std::isnan(new_position.y()));

    IConstant<geometry::Vec> *new_position_variable_value =
                new BasicConstant<geometry::Vec>(next_state->get_name(), "position.base", new_position);
    next_state->set_position_variable(new_position_variable_value);
}

}
}
}
