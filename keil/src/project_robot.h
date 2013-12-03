#include "stm32f4xx.h"
#include <math.h>

#include "project_servo.h"
#include "project_init.h"

#define BOOM_ANGLE_MIN 50
#define BOOM_ANGLE_MAX 135
#define BOOM_ANGLE_STARTING BOOM_ANGLE_MAX
#define CROWD_ANGLE_MIN 80
#define CROWD_ANGLE_MAX 170
#define CROWD_ANGLE_STARTING CROWD_ANGLE_MAX
#define SWING_ANGLE_MIN SERVO_MIN_POSITION
#define SWING_ANGLE_MAX 130
#define SWING_ANGLE_STARTING SWING_ANGLE_MIN

#define POSITION_Y_MIN Y_MIN
#define POSITION_Y_MAX Y_MAX
#define POSITION_Z_MIN Z_MIN
#define POSITION_Z_MAX Z_MAX
#define POSITION_SWING_MIN ANGLE_MIN
#define POSITION_SWING_MAX ANGLE_MAX

#define PARK_Y POSITION_Y_MIN
#define PARK_Z POSITION_Z_MIN
#define PARK_ANGLE POSITION_SWING_MIN

#define BOOM_UP_POSITION BOOM_ANGLE_MAX
#define BOOM_DOWN_POSITION_0 85
#define BOOM_DOWN_POSITION_1 90
#define BOOM_DOWN_POSITION_2 85
#define BOOM_DOWN_POSITION_3 80
#define BOOM_DOWN_POSITION_4 80
#define BOOM_DOWN_POSITION_5 70
#define BOOM_DOWN_POSITION_6 50
#define CROWD_POSITION_0 170
#define CROWD_POSITION_1 150
#define CROWD_POSITION_2 135
#define CROWD_POSITION_3 122
#define CROWD_POSITION_4 110
#define CROWD_POSITION_5 97
#define CROWD_POSITION_6 80
#define SWING_POSITION_0 0
#define SWING_POSITION_1 10
#define SWING_POSITION_2 20
#define SWING_POSITION_3 30
#define SWING_POSITION_4 40
#define SWING_POSITION_5 50
#define SWING_POSITION_6 60
#define SWING_POSITION_7 70
#define SWING_POSITION_8 80
#define SWING_POSITION_9 90
#define SWING_POSITION_10 100
#define SWING_POSITION_11 110
#define SWING_POSITION_12 120
#define SWING_POSITION_13 130

struct Robot {
	struct Servo *boomServo;
	struct Servo *crowdServo;
	struct Servo *swingServo;
	uint32_t positionY;
	uint32_t positionZ;
	uint32_t angle;
	uint32_t boomPosition;
	uint32_t crowdPosition;
	uint32_t swingPosition;
};

struct RobotInit {
	struct Servo *boomServo;
	struct Servo *crowdServo;
	struct Servo *swingServo;
};

void init_robot(struct Robot *robot, struct RobotInit *robotInit);
void moveRobot(struct Robot *robot, uint32_t positionY, uint32_t positionZ, uint32_t angle);
void waitUntilRobotStopped(struct Robot *robot);
void floatRobot(struct Robot *robot);
void parkRobot(struct Robot *robot);
