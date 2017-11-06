/*
 * settings_mobile.h  */

/***********************************************************************/
/**** mobile profile ********************************************/
/***********************************************************************/

// ***> NOTE: The init message must be a single line with no CRs or LFs
#define INIT_MESSAGE "Initializing configs to Shapeoko2 500mm profile"

#define JUNCTION_DEVIATION_UN		0.2// default value, in mm - smaller is faster
#define JUNCTION_ACCELERATION_UN	160000	// 2 million - centripetal acceleration around corners

// *** motor settings ***

// *** motor settings ***
#define M1_MOTOR_MAP_UN				AXIS_Z
#define M1_TRAVEL_PER_REV_RETA_UN	CREM_RETA
#define M1_TRAVEL_PER_REV_HELI_UN	CREM_HELI
#define M1_MICROSTEPS_UN			64
#define M1_POLARITY_UN				0

#define M2_MOTOR_MAP_UN				AXIS_Y  // Y1 - left side of machine
#define M2_TRAVEL_PER_REV_RETA_UN	CREM_RETA
#define M2_TRAVEL_PER_REV_HELI_UN	CREM_HELI
#define M2_MICROSTEPS_UN			64
#define M2_POLARITY_UN				0

#define M3_MOTOR_MAP_UN				AXIS_X  // X2 - right sif of machine
#define M3_TRAVEL_PER_REV_RETA_UN	CREM_RETA
#define M3_TRAVEL_PER_REV_HELI_UN	CREM_HELI
#define M3_MICROSTEPS_UN			64
#define M3_POLARITY_UN				1

#define M4_MOTOR_MAP_UN				AXIS_X
#define M4_TRAVEL_PER_REV_RETA_UN	CREM_RETA
#define M4_TRAVEL_PER_REV_HELI_UN	CREM_HELI
#define M4_MICROSTEPS_UN			64
#define M4_POLARITY_UN				0

#define Z_STEP_PULSE_UN 			(M1_TRAVEL_PER_REV_UN*M1_STEP_ANGLE)/(360*M1_MICROSTEPS_UN)
// *** axis settings ***

// These are relative conservative values for a well-tuned Shapeoko2 or similar XY belt / Z screw machine
#define X_VELOCITY_MAX_UN			7000
#define X_FEEDRATE_MAX_UN			Y_VELOCITY_MAX_UN
#define X_JERK_MAX_UN				400
#define X_JUNCTION_DEVIATION_UN		JUNCTION_DEVIATION_UN

#define Y_VELOCITY_MAX_UN			7000
#define Y_FEEDRATE_MAX_UN			Y_VELOCITY_MAX
#define Y_JERK_MAX_UN				400
#define Y_JUNCTION_DEVIATION_UN		JUNCTION_DEVIATION_UN


#define Z_VELOCITY_MAX_UN			700
#define Z_FEEDRATE_MAX_UN			Z_VELOCITY_MAX_UN
                                                    // value must be large enough to guarantee return to Zmax during homing
#define Z_JERK_MAX_UN				1000					// 50,000,000
#define Z_JUNCTION_DEVIATION_UN		JUNCTION_DEVIATION_UN
