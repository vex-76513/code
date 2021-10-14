#include "design_consts.hpp"
#pragma once

namespace CONFIG_DRIVE
{
    using namespace okapi;
    enum MODE
    {
        DRIVE_MODE_TANK,
        DRIVE_MODE_ARCADE,
        COUNT_OR_INVALID
    };

    //don't allow changing the mode from default
    const bool FIXED_MODE = false;

    // default mode that the robot starts in
    const MODE DEFAULT_MODE = DRIVE_MODE_TANK;

    const QTime delay = 10_ms;

}

namespace ButtonMapping
{

    using namespace okapi;

    const QTime delay = 20_ms;

    const ControllerId drive_controller = ControllerId::master;

    const ControllerAnalog tank_drive[] = {
        ControllerAnalog::leftY,
        ControllerAnalog::rightY,
    };

    const ControllerAnalog arcade_drive[] = {
        ControllerAnalog::leftY,
        ControllerAnalog::rightX,
    };

    const ControllerDigital drive_mode_switch = ControllerDigital::A;

    //handles both the claw and the arm
    const ControllerId claw_controller = ControllerId::master;

    const ControllerDigital claw_hook_button = ControllerDigital::B;

    const ControllerDigital claw_arm_up = ControllerDigital::R1;
    const ControllerDigital claw_arm_down = ControllerDigital::R2;

}

namespace CLAW_CONF
{
    using namespace okapi;
    const QAngularSpeed arm_top_velocity = 90_deg / 2_s;

    const int armPos[] = {-10000, 15, 59, 69};
    const int ARM_POS_LEN = 4;
}
