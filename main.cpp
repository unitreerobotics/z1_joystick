#include <csignal>
#include <sched.h>
#include "control/armCtrl.h"

bool running = true;

int main(int argc, char** argv){
    std::string controllerName = (argc > 1) ? argv[1] : "B1";
#ifdef COMPILE_WITH_SIMULATION
    //Run joy_node first
    ros::init(argc, argv, "z1_joystick");
#endif

    ArmCtrl arm(controllerName);

    signal(SIGINT, [](int signum){running = false;});
    while (running)
    {
        sleep(1);
    }
    
    return 0;
}