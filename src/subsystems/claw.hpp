#include "../components/piston.hpp"
#include "../design_consts.hpp"
#include "../config_consts.hpp"

//static okapi::ADIButton **buttonsFromStr(const char *a)
//{
//    static okapi::ADIButton *ret[HARDWARE::CLAW_NUM_MOTORS];
//
//    int count = 0;
//    while (a[count++] != '\0')
//        *ret[count - 1] = okapi::ADIButton(*a);
//    return ret;
//}
//
//static okapi::Motor **motorsFromInitList(std::initializer_list<okapi::Motor> a)
//{
//
//    const int s = a.size();
//    static okapi::Motor *ret[s];
//    int count = 0;
//    for (auto i : a)
//        *ret[count++] = i;
//    return ret;
//}

class Claw
{
private:
    Piston piston = Piston(HARDWARE::CLAW_PORT, HARDWARE::CLAW_REVERSED);

    okapi::MotorGroup mtr = okapi::MotorGroup(HARDWARE::CLAW_ARM_MOTORS);

    bool currentlyLTOperating = false;
    int curr = 1;

public:
    Claw() {}
    void init()
    {
        mtr.setBrakeMode(okapi::AbstractMotor::brakeMode::hold);
        mtr.setEncoderUnits(okapi::AbstractMotor::encoderUnits::degrees);
    };
    void Clasp()
    {
        printf("CLAW: CLASPING\n");
        return this->piston.extend();
    };
    void Leave()
    {

        printf("CLAW: LEAVING\n");
        this->piston.retract();
    };
    void Toggle()
    {
        printf("CLAW: TOGGLE\n");
        this->piston.toggle();
    }

    void ArmMove(double v)
    {
        printf("armmove: %f\n", v * CLAW_CONF::arm_top_velocity.convert(1_rpm) / HARDWARE::claw_arm_gear_ratio);
        v = v > 1 ? 1 : v;
        int ans = mtr.moveVelocity(v * CLAW_CONF::arm_top_velocity.convert(1_rpm) / HARDWARE::claw_arm_gear_ratio);
    };

    void ArmSet(double v)
    {
        int code = mtr.moveAbsolute(v / HARDWARE::claw_arm_gear_ratio, 0.5 * CLAW_CONF::arm_top_velocity.convert(1_rpm) / HARDWARE::claw_arm_gear_ratio);
        if (code != 1)
        {
            printf("AAAAAAAAAAAAAAAAAAA %d\n", code);
        }
    }

    void ArmUp()
    {
        ;
        if (curr < CLAW_CONF::ARM_POS_LEN - 1)
        {
            printf("ArmUp\n");
            ArmSet(CLAW_CONF::armPos[++curr]);
        }
    }
    void ArmDown()
    {
        if (curr > 0)
        {
            printf("ArmDown\n");
            ArmSet(CLAW_CONF::armPos[--curr]);
        }
    }

    void WaitUntilSettled()
    {
        const auto adjustedvel = CLAW_CONF::min_zeroed_velocity.convert(1_rpm) / HARDWARE::claw_arm_gear_ratio;
        printf("%f\n", adjustedvel);
        while (mtr.getActualVelocity() < -adjustedvel || mtr.getActualVelocity() > adjustedvel)
        {
            pros::delay(10);
        }
    }

    void ArmSetNum(const int n)
    {
        if (n >= 0 && n < CLAW_CONF::ARM_POS_LEN)
        {
            curr = n;
            ArmSet(CLAW_CONF::armPos[n]);
        }
    }

    void ArmSoftStop()
    {
        printf("ArmSoft Stop: %d\n", CLAW_CONF::armPos[curr]);
        //if (!currentlyLTOperating)
        //maybe not needed now that things r only called on change???
        //ArmMove(0);
    }

    void ArmTop(bool await = false)
    {
        ArmSet(CLAW_CONF::armPos[CLAW_CONF::ARM_POS_LEN - 1]);
        //lets just try relying on protect to take care of this for once
        //        mtr.moveAbsolute(HARDWARE::claw_max_angle.convert(1_deg), CLAW_CONF::arm_top_velocity.convert(1_rpm));
        //if (await)
    };

    void ArmBottom()
    {
        ArmMove(-.5);
    };

    static void Protect()
    {
        static int count = 0;
        count++;

        const double max_ang = (HARDWARE::claw_max_angle / HARDWARE::claw_arm_gear_ratio).convert(1_deg);

        for (auto const &x : HARDWARE::LIMIT_SWITCHES)
        {

            int port_num = x.second;

            double vel = (port_num / abs(port_num)) * pros::c::motor_get_actual_velocity(abs(port_num)) * HARDWARE::claw_arm_gear_ratio;

            double pos = pros::c::motor_get_position(abs(port_num));

            if (!(count % 200))
            {
                printf("%c %d %f %f %d %d\n", x.first, port_num, vel, pos, count, pros::c::adi_digital_read(x.first));
            }

            //if switch is pressed AND is moving down
            if (vel < -0.01 && pros::c::adi_digital_read(x.first))
            {
                printf("stopping %d\n", port_num);
                pros::c::motor_move_voltage(abs(port_num), 0);
                pros::c::motor_tare_position(abs(port_num));
            }

            if (vel > 0.01 && pos >= max_ang)
            {
                pros::c::motor_move_voltage(abs(port_num), 0);
            }
        }
    }
};
