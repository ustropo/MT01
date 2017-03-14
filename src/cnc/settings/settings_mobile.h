/*
 * settings_mobile.h  */

/***********************************************************************/
/**** mobile profile ********************************************/
/***********************************************************************/

// ***> NOTE: The init message must be a single line with no CRs or LFs
#define INIT_MESSAGE "Initializing configs to Shapeoko2 500mm profile"

#define JUNCTION_DEVIATION_MB		0.2// default value, in mm - smaller is faster
#define JUNCTION_ACCELERATION_MB	160000	// 2 million - centripetal acceleration around corners

// *** motor settings ***

// *** motor settings ***
#define M1_MOTOR_MAP_MB				AXIS_Z
#define M1_TRAVEL_PER_REV_RETA_MB	CREM_RETA
#define M1_TRAVEL_PER_REV_HELI_MB	CREM_HELI
#define M1_MICROSTEPS_MB			64
#define M1_POLARITY_MB				0

#define M2_MOTOR_MAP_MB				AXIS_Y  // Y1 - left side of machine
#define M2_TRAVEL_PER_REV_RETA_MB	CREM_RETA
#define M2_TRAVEL_PER_REV_HELI_MB	CREM_HELI
#define M2_MICROSTEPS_MB			64
#define M2_POLARITY_MB				0

#define M3_MOTOR_MAP_MB				AXIS_X  // X2 - right sif of machine
#define M3_TRAVEL_PER_REV_RETA_MB	CREM_RETA
#define M3_TRAVEL_PER_REV_HELI_MB	CREM_HELI
#define M3_MICROSTEPS_MB			64
#define M3_POLARITY_MB				1

#define M4_MOTOR_MAP_MB				AXIS_X
#define M4_TRAVEL_PER_REV_RETA_MB	CREM_RETA
#define M4_TRAVEL_PER_REV_HELI_MB	CREM_HELI
#define M4_MICROSTEPS_MB			64
#define M4_POLARITY_MB				0

#define Z_STEP_PULSE_MB 			(M1_TRAVEL_PER_REV_MB*M1_STEP_ANGLE)/(360*M1_MICROSTEPS_MB)
// *** axis settings ***

// These are relative conservative values for a well-tuned Shapeoko2 or similar XY belt / Z screw machine
#define X_VELOCITY_MAX_MB			5000
#define X_FEEDRATE_MAX_MB			Y_VELOCITY_MAX_MB
#define X_JERK_MAX_MB				250
#define X_JUNCTION_DEVIATION_MB		JUNCTION_DEVIATION_MB

#define Y_VELOCITY_MAX_MB			5000
#define Y_FEEDRATE_MAX_MB			Y_VELOCITY_MAX
#define Y_JERK_MAX_MB				250
#define Y_JUNCTION_DEVIATION_MB		JUNCTION_DEVIATION_MB


#define Z_VELOCITY_MAX_MB			700
#define Z_FEEDRATE_MAX_MB			Z_VELOCITY_MAX_MB
                                                    // value must be large enough to guarantee return to Zmax during homing
#define Z_JERK_MAX_MB				1000					// 50,000,000
#define Z_JUNCTION_DEVIATION_MB		JUNCTION_DEVIATION_MB
