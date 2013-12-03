/*!
	@file project_robot.c
	Source file that provides robot control functionality.
*/
#include "stm32f4xx.h"
#include <math.h>

#include "project_robot.h"
#include "project_init.h"

/**  
  * @brief  Function that updates position of robot
  * @param  *robot: pointer to Robot structure.  
  * @retval None  
  */
static void updateRobotPosition(struct Robot *robot) {
	if (robot->positionY < POSITION_Y_MIN) {
		robot->positionY = POSITION_Y_MIN;
	} else if (robot->positionY > POSITION_Y_MAX) {
		robot->positionY = POSITION_Y_MAX;
	}
	if (robot->positionZ < POSITION_Z_MIN) {
		robot->positionZ = POSITION_Z_MIN;
	} else if (robot->positionZ > POSITION_Z_MAX) {
		robot->positionZ = POSITION_Z_MAX;
	}
	if (robot->angle < POSITION_SWING_MIN) {
		robot->angle = POSITION_SWING_MIN;
	} else if (robot->angle > POSITION_SWING_MAX) {
		robot->angle = POSITION_SWING_MAX;
	}
	if (robot->positionZ) {
		robot->boomPosition = BOOM_UP_POSITION;
		switch (robot->positionY) {
			case 0:
				robot->crowdPosition = CROWD_POSITION_0;
				break;
			case 1:
				robot->crowdPosition = CROWD_POSITION_1;
				break;
			case 2:
				robot->crowdPosition = CROWD_POSITION_2;
				break;
			case 3:
				robot->crowdPosition = CROWD_POSITION_3;
				break;
			case 4:
				robot->crowdPosition = CROWD_POSITION_4;
				break;
			case 5:
				robot->crowdPosition = CROWD_POSITION_5;
				break;
			default:
				robot->crowdPosition = CROWD_POSITION_6;
		}
	} else {
		switch (robot->positionY) {
			case 0:
				robot->boomPosition = BOOM_DOWN_POSITION_0;
				robot->crowdPosition = CROWD_POSITION_0;
				break;
			case 1:
				robot->boomPosition = BOOM_DOWN_POSITION_1;
				robot->crowdPosition = CROWD_POSITION_1;
				break;
			case 2:
				robot->boomPosition = BOOM_DOWN_POSITION_2;
				robot->crowdPosition = CROWD_POSITION_2;
				break;
			case 3:
				robot->boomPosition = BOOM_DOWN_POSITION_3;
				robot->crowdPosition = CROWD_POSITION_3;
				break;
			case 4:
				robot->boomPosition = BOOM_DOWN_POSITION_4;
				robot->crowdPosition = CROWD_POSITION_4;
				break;
			case 5:
				robot->boomPosition = BOOM_DOWN_POSITION_5;
				robot->crowdPosition = CROWD_POSITION_5;
				break;
			default:
				robot->boomPosition = BOOM_DOWN_POSITION_6;
				robot->crowdPosition = CROWD_POSITION_6;
		}
	}
	switch (robot->angle) {
		case 0:
			robot->swingPosition = SWING_POSITION_0;
			break;
		case 1:
			robot->swingPosition = SWING_POSITION_1;
			break;
		case 2:
			robot->swingPosition = SWING_POSITION_2;
			break;
		case 3:
			robot->swingPosition = SWING_POSITION_3;
			break;
		case 4:
			robot->swingPosition = SWING_POSITION_4;
			break;
		case 5:
			robot->swingPosition = SWING_POSITION_5;
			break;
		case 6:
			robot->swingPosition = SWING_POSITION_6;
			break;
		case 7:
			robot->swingPosition = SWING_POSITION_7;
			break;
		case 8:
			robot->swingPosition = SWING_POSITION_8;
			break;
		case 9:
			robot->swingPosition = SWING_POSITION_9;
			break;
		case 10:
			robot->swingPosition = SWING_POSITION_10;
			break;
		case 11:
			robot->swingPosition = SWING_POSITION_11;
			break;
		case 12:
			robot->swingPosition = SWING_POSITION_12;
			break;
		default:
			robot->swingPosition = SWING_POSITION_13;
	}
}

/**  
  * @brief  Function that waits until mechanical parts of robot stop moving.
  * @param  *robot: pointer to Robot structure.  
  * @retval None  
  */
void waitUntilRobotStopped(struct Robot *robot) {
	waitUntilServoStopped(robot->boomServo);
	waitUntilServoStopped(robot->crowdServo);
	waitUntilServoStopped(robot->swingServo);
}


/**  
  * @brief  Function that initializes robot.
  * @param  *robot: pointer to Robot structure.
  * @param  *robotInit: pointer to RobotInit structure.  
  * @retval None  
  */
void init_robot(struct Robot *robot, struct RobotInit *robotInit) {
	robot->boomServo = robotInit->boomServo;
	robot->crowdServo = robotInit->crowdServo;
	robot->swingServo = robotInit->swingServo;
	
	robot->boomPosition = robot->boomServo->minPosition;
	robot->crowdPosition = robot->crowdServo->minPosition;
	robot->swingPosition = robot->swingServo->minPosition;
	
	robot->positionY = 0;
	robot->positionZ = 0;
	robot->angle = 0;
}

/**  
  * @brief  Function that moves robot arm to desired position.
  * @param  *robot: pointer to Robot structure.
  * @param  positionY: Forward/backward position of arm
  * @param  positionZ: Up/Down position (binary value).
  * @param  angle: angle of robot arm 
  * @retval None  
  */
void moveRobot(struct Robot *robot, uint32_t positionY, uint32_t positionZ, uint32_t angle) {
	if (robot->positionY != positionY || robot->positionZ != positionZ || robot->angle != angle) {
		robot->positionY = positionY;
		robot->positionZ = positionZ;
		robot->angle = angle;
		
		updateRobotPosition(robot);
		
		moveServo(robot->boomServo, robot->boomServo->maxPosition);
		waitUntilServoStopped(robot->boomServo);
		
		moveServo(robot->crowdServo, robot->crowdPosition);
		moveServo(robot->swingServo, robot->swingPosition);
		waitUntilRobotStopped(robot);
		
		moveServo(robot->boomServo, robot->boomPosition);
		waitUntilServoStopped(robot->boomServo);
	}
}

/**  
  * @brief  Hold motors of robot arm at current position.
  * @param  *robot: pointer to Robot structure.
  * @retval None  
  */
void floatRobot(struct Robot *robot) {
	floatServo(robot->boomServo);
	floatServo(robot->crowdServo);
	floatServo(robot->swingServo);
}


/**  
  * @brief  Move robot arm to "Parked" position.
  * @param  *robot: pointer to Robot structure.
  * @retval None  
  */
void parkRobot(struct Robot *robot) {
	if (robot->positionY == PARK_Y && robot->positionZ == PARK_Z && robot->angle == PARK_ANGLE) {
		robot->positionY = 1;
		robot->positionZ = 1;
		robot->angle = 1;
	}
	moveRobot(robot, PARK_Y, PARK_Z, PARK_ANGLE);
}
