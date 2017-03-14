/*
 * settings_compacta.h  */

/***********************************************************************/
/**** compacta profile ********************************************/
/***********************************************************************/

// ***> NOTE: The init message must be a single line with no CRs or LFs
#define INIT_MESSAGE "Initializing configs to Shapeoko2 500mm profile"

#define JUNCTION_DEVIATION_CP		0.2// default value, in mm - smaller is faster
#define JUNCTION_ACCELERATION_CP	160000	// 2 million - centripetal acceleration around corners

// *** motor settings ***
#define M1_MOTOR_MAP_CP				AXIS_Z
#define M1_TRAVEL_PER_REV_RETA_CP	3
#define M1_TRAVEL_PER_REV_HELI_CP	3
#define M1_MICROSTEPS_CP			64
#define M1_POLARITY_CP				0

#define M2_MOTOR_MAP_CP				AXIS_Y  // Y1 - left side of machine
#define M2_TRAVEL_PER_REV_RETA_CP	CREM_RETA
#define M2_TRAVEL_PER_REV_HELI_CP	CREM_HELI
#define M2_MICROSTEPS_CP			64
#define M2_POLARITY_CP				0

#define M3_MOTOR_MAP_CP				AXIS_X  // X2 - right sif of machine
#define M3_TRAVEL_PER_REV_RETA_CP	CREM_RETA
#define M3_TRAVEL_PER_REV_HELI_CP	CREM_HELI
#define M3_MICROSTEPS_CP			64
#define M3_POLARITY_CP				1

#define M4_MOTOR_MAP_CP				AXIS_X
#define M4_TRAVEL_PER_REV_RETA_CP	CREM_RETA
#define M4_TRAVEL_PER_REV_HELI_CP	CREM_HELI
#define M4_MICROSTEPS_CP			64
#define M4_POLARITY_CP				0

#define Z_STEP_PULSE_CP 			(M1_TRAVEL_PER_REV_CP*M1_STEP_ANGLE)/(360*M1_MICROSTEPS_CP)
// *** axis settings ***

// These are relative conservative values for a well-tuned Shapeoko2 or similar XY belt / Z screw machine
#define X_VELOCITY_MAX_CP			10000
#define X_FEEDRATE_MAX_CP			Y_VELOCITY_MAX_CP
#define X_JERK_MAX_CP				1000
#define X_JUNCTION_DEVIATION_CP		JUNCTION_DEVIATION_CP

#define Y_VELOCITY_MAX_CP			10000
#define Y_FEEDRATE_MAX_CP			Y_VELOCITY_MAX
#define Y_JERK_MAX_CP				1000
#define Y_JUNCTION_DEVIATION_CP		JUNCTION_DEVIATION_CP


#define Z_VELOCITY_MAX_CP			900
#define Z_FEEDRATE_MAX_CP			Z_VELOCITY_MAX_CP
                                                    // value must be large enough to guarantee return to Zmax during homing
#define Z_JERK_MAX_CP				6000					// 50,000,000
#define Z_JUNCTION_DEVIATION_CP		JUNCTION_DEVIATION_CP



