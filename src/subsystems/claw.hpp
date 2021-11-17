#include "../components/piston.hpp"
#include "../design_consts.hpp"
#include "../config_consts.hpp"

class Claw
{
private:
    Piston piston = Piston(HARDWARE::CLAW_PORT, HARDWARE::CLAW_REVERSED);

    okapi::MotorGroup mtr = okapi::MotorGroup(HARDWARE::CLAW_ARM_MOTORS);

    int curr = 0;

    std::shared_ptr<okapi::AsyncPositionController<double, double>> controllerl;
    std::shared_ptr<okapi::AsyncPositionController<double, double>> controllerr;

public:
    Claw() {}
    void init()
    {
        mtr.setBrakeMode(okapi::AbstractMotor::brakeMode::hold);
        mtr.setEncoderUnits(okapi::AbstractMotor::encoderUnits::degrees);
        //const okapi::IterativePosPIDController::Gains gains = {0.0015, 0.0007, 0.00004};
        //const okapi::IterativePosPIDController::Gains gains = {0.0072,0.008,0.00007 };
        const okapi::IterativePosPIDController::Gains gains = {0.00002, 0.008, 0.00007};
        controllerl = okapi::AsyncPosControllerBuilder().withMotor(HARDWARE::CLAW_ARM_MOTOR2).withGains(gains).withSensor(HARDWARE::POTL).build();
        controllerr = okapi::AsyncPosControllerBuilder().withMotor(HARDWARE::CLAW_ARM_MOTOR1).withGains(gains).withSensor(HARDWARE::POTR).build();

        ArmSet(3);
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
    bool Toggle()
    {
        printf("CLAW: TOGGLE\n");
        return !this->piston.toggle();
    }

    void ArmSet(double v)
    {
        double val = v;
        double valL = (val * ((HARDWARE::LMAX - HARDWARE::LMIN) / 90)) + HARDWARE::LMIN;
        double valR = (val * ((HARDWARE::RMAX - HARDWARE::RMIN) / 90)) + HARDWARE::RMIN;

        printf("%f %f %f\n", val, valL, valR);

        controllerl->setTarget(valL);
        controllerr->setTarget(valR);
    }

    void ArmSetRelative(double n)
    {
        controllerl->setTarget(controllerl->getTarget() + n);
        controllerr->setTarget(controllerr->getTarget() + n);
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
        while (!(controllerl->isSettled() && controllerr->isSettled()))
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

    void ArmTop(bool await = false)
    {
        ArmSetNum(CLAW_CONF::ARM_POS_LEN - 1);
    };
};
