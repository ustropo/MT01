/*
 * settings_maneta.h  */

/* Note: The values in this file are the default settings that are loaded
 * 		 into a virgin EEPROM, and can be changed using the config commands.
 *		 After initial load the EEPROM values (or changed values) are used.
 *
 *		 System and hardware settings that you shouldn't need to change
 *		 are in system.h  Application settings that also shouldn't need
 *		 to be changed are in tinyg.h
 */

/***********************************************************************/
/**** easymak profile ********************************************/
/***********************************************************************/

// ***> NOTE: The init message must be a single line with no CRs or LFs
#define INIT_MESSAGE "Initializing configs to Shapeoko2 500mm profile"

//#define JUNCTION_DEVIATION		0.03// default value, in mm - smaller is faster
//#define JUNCTION_ACCELERATION	1500000	// 2 million - centripetal acceleration around corners

#define JUNCTION_DEVIATION		0.2// default value, in mm - smaller is faster
#define JUNCTION_ACCELERATION	160000	// 2 million - centripetal acceleration around corners

// *** settings.h overrides ***

#undef COMM_MODE
#define COMM_MODE				TEXT_MODE

#undef JSON_VERBOSITY
#define JSON_VERBOSITY 			JV_VERBOSE

#undef SWITCH_TYPE
#define SWITCH_TYPE 			SW_TYPE_NORMALLY_CLOSED	// one of: SW_TYPE_NORMALLY_OPEN, SW_TYPE_NORMALLY_CLOSED

// *** motor settings ***
#define TRAVELX	75.3982236862
#define TRAVELY	75.3982236862/3.222222

#define M1_MOTOR_MAP			AXIS_Z
#define M1_STEP_ANGLE			1.8
#define M1_TRAVEL_PER_REV		4
#define M1_MICROSTEPS			64
#define M1_POLARITY				0
#define M1_POWER_MODE			2

#define M2_MOTOR_MAP			AXIS_Y  // Y1 - left side of machine
#define M2_STEP_ANGLE			1.8
#define M2_TRAVEL_PER_REV		TRAVELY
#define M2_MICROSTEPS			64
#define M2_POLARITY				0
#define M2_POWER_MODE			2

#define M3_MOTOR_MAP			AXIS_X  // X2 - right sif of machine
#define M3_STEP_ANGLE			1.8
#define M3_TRAVEL_PER_REV		TRAVELX
#define M3_MICROSTEPS			64
#define M3_POLARITY				1
#define M3_POWER_MODE			2

#define M4_MOTOR_MAP			AXIS_X
#define M4_STEP_ANGLE			1.8
#define M4_TRAVEL_PER_REV		TRAVELX
#define M4_MICROSTEPS			64
#define M4_POLARITY				0
#define M4_POWER_MODE			2

#define M5_MOTOR_MAP			AXIS_DISABLED
#define M5_STEP_ANGLE			1.8
#define M5_TRAVEL_PER_REV		360		// degrees per motor rev
#define M5_MICROSTEPS			8
#define M5_POLARITY				0
#define M5_POWER_MODE			MOTOR_POWER_MODE

#define M6_MOTOR_MAP			AXIS_DISABLED
#define M6_STEP_ANGLE			1.8
#define M6_TRAVEL_PER_REV		360
#define M6_MICROSTEPS			8
#define M6_POLARITY				0
#define M6_POWER_MODE			MOTOR_POWER_MODE

#define Z_STEP_PULSE (M1_TRAVEL_PER_REV*M1_STEP_ANGLE)/(360*M1_MICROSTEPS)
// *** axis settings ***

// These are relative conservative values for a well-tuned Shapeoko2 or similar XY belt / Z screw machine

#define X_AXIS_MODE				AXIS_STANDARD		// xam		see canonical_machine.h cmAxisMode for valid values
#define X_VELOCITY_MAX			10000 				// xvm		G0 max velocity in mm/min
#define X_FEEDRATE_MAX			X_VELOCITY_MAX		// xfr 		G1 max feed rate in mm/min
#define X_TRAVEL_MIN			0					// xtn		minimum travel
#define X_TRAVEL_MAX			3000					// xtm		maximum travel (travel between switches or crashes)
#define X_JERK_MAX				400				// xjm		yes, that's "5 billion" mm/(min^3)
#define X_JERK_HOMING			400				// xjh
#define X_JUNCTION_DEVIATION	JUNCTION_DEVIATION	// xjd
#define X_SWITCH_MODE_MIN		SW_MODE_HOMING		// xsn		SW_MODE_DISABLED, SW_MODE_HOMING, SW_MODE_LIMIT, SW_MODE_HOMING_LIMIT
#define X_SWITCH_MODE_MAX 		SW_MODE_DISABLED	// xsx		SW_MODE_DISABLED, SW_MODE_HOMING, SW_MODE_LIMIT, SW_MODE_HOMING_LIMIT
#define X_SEARCH_VELOCITY		3000				// xsv		minus means move to minimum switch
#define X_LATCH_VELOCITY		100					// xlv		mm/min
#define X_LATCH_BACKOFF			10					// xlb		mm
#define X_ZERO_BACKOFF			2					// xzb		mm

#define Y_AXIS_MODE				AXIS_STANDARD
#define Y_VELOCITY_MAX			10000
#define Y_FEEDRATE_MAX			Y_VELOCITY_MAX
#define Y_TRAVEL_MIN			0
#define Y_TRAVEL_MAX			1500
#define Y_JERK_MAX				400
#define Y_JERK_HOMING			400				// xjh
#define Y_JUNCTION_DEVIATION	JUNCTION_DEVIATION
#define Y_SWITCH_MODE_MIN		SW_MODE_HOMING
#define Y_SWITCH_MODE_MAX		SW_MODE_DISABLED
#define Y_SEARCH_VELOCITY		3000
#define Y_LATCH_VELOCITY		100
#define Y_LATCH_BACKOFF			10
#define Y_ZERO_BACKOFF			2

#define Z_AXIS_MODE				AXIS_STANDARD
#define Z_VELOCITY_MAX			1200
#define Z_FEEDRATE_MAX			Z_VELOCITY_MAX
#define Z_TRAVEL_MAX			0
#define Z_TRAVEL_MIN			-120                // this is approximate as Z depth depends on tooling
                                                    // value must be large enough to guarantee return to Zmax during homing
#define Z_JERK_MAX				6000					// 50,000,000
#define Z_JERK_HOMING			6000
#define Z_JUNCTION_DEVIATION	JUNCTION_DEVIATION
#define Z_SWITCH_MODE_MIN		SW_MODE_DISABLED
#define Z_SWITCH_MODE_MAX		SW_MODE_HOMING
#define Z_SEARCH_VELOCITY		Z_VELOCITY_MAX
#define Z_LATCH_VELOCITY		100
#define Z_LATCH_BACKOFF			10
#define Z_ZERO_BACKOFF			3

/***************************************************************************************
 * A Axis rotary values are chosen to make the motor react the same as X for testing
 *
 * To calculate the speeds here, in Wolfram Alpha-speak:
 *
 *   c=2*pi*r, r=0.609, d=c/360, s=((S*60)/d), S=40 for s
 *   c=2*pi*r, r=5.30516, d=c/360, s=((S*60)/d), S=40 for s
 *
 * Change r to A_RADIUS, and S to the desired speed, in mm/s or mm/s/s/s.
 *
 * It will return s= as the value you want to enter.
 *
 * If the value is over 1 million, the code will divide it by 1 million,
 * so you have to pre-multiply it by 1000000.0. (The value is in millions, btw.)
 *
 * Note that you need these to be floating point values, so always have a .0 at the end!
 *
 ***************************************************************************************/

#define A_AXIS_MODE 			AXIS_RADIUS
#define A_RADIUS 				5.30516             //
#define A_VELOCITY_MAX          25920.0             // ~40 mm/s, 2,400 mm/min
#define A_FEEDRATE_MAX 			25920.0/2.0         // ~20 mm/s, 1,200 mm/min
#define A_TRAVEL_MIN 			-1                  // identical mean no homing will occur
#define A_TRAVEL_MAX 			-1
#define A_JERK_MAX 				324000              // 1,000 million mm/min^3
                                                    // * a million IF it's over a million
                                                    // c=2*pi*r, r=5.30516476972984, d=c/360, s=((1000*60)/d)
#define A_JERK_HOMING			A_JERK_MAX
#define A_JUNCTION_DEVIATION	0.1
#define A_SWITCH_MODE_MIN		SW_MODE_HOMING
#define A_SWITCH_MODE_MAX		SW_MODE_DISABLED
#define A_SEARCH_VELOCITY 		2000
#define A_LATCH_VELOCITY 		2000
#define A_LATCH_BACKOFF 		5
#define A_ZERO_BACKOFF 			2

/*
#define A_AXIS_MODE				AXIS_STANDARD
#define A_VELOCITY_MAX			60000
#define A_FEEDRATE_MAX			48000
#define A_JERK_MAX				24000				// yes, 24 billion
#define A_JERK_HOMING			A_JERK_MAX
#define A_RADIUS				1.0
#define A_SWITCH_MODE_MIN		SW_MODE_HOMING
#define A_SWITCH_MODE_MAX		SW_MODE_DISABLED
#define A_SEARCH_VELOCITY		6000
#define A_LATCH_VELOCITY		1000
#define A_LATCH_BACKOFF			5
#define A_ZERO_BACKOFF			2
*/

#define B_AXIS_MODE				AXIS_DISABLED
#define B_VELOCITY_MAX			3600
#define B_FEEDRATE_MAX			B_VELOCITY_MAX
#define B_TRAVEL_MAX			-1
#define B_TRAVEL_MIN			-1
#define B_JERK_MAX				20
#define B_JERK_HOMING			B_JERK_MAX
#define B_JUNCTION_DEVIATION	JUNCTION_DEVIATION
#define B_RADIUS				1
#define B_SWITCH_MODE_MIN		SW_MODE_HOMING
#define B_SWITCH_MODE_MAX		SW_MODE_DISABLED
#define B_SEARCH_VELOCITY		6000
#define B_LATCH_VELOCITY		1000
#define B_LATCH_BACKOFF			5
#define B_ZERO_BACKOFF			2

#define C_AXIS_MODE				AXIS_DISABLED
#define C_VELOCITY_MAX			3600
#define C_FEEDRATE_MAX			C_VELOCITY_MAX
#define C_TRAVEL_MAX			-1
#define C_TRAVEL_MIN			-1
#define C_JERK_MAX				20
#define C_JERK_HOMING			C_JERK_MAX
#define C_JUNCTION_DEVIATION	JUNCTION_DEVIATION
#define C_RADIUS				1
#define C_SWITCH_MODE_MIN		SW_MODE_HOMING
#define C_SWITCH_MODE_MAX		SW_MODE_DISABLED
#define C_SEARCH_VELOCITY		6000
#define C_LATCH_VELOCITY		1000
#define C_LATCH_BACKOFF			5
#define C_ZERO_BACKOFF			2

// *** DEFAULT COORDINATE SYSTEM OFFSETS ***
// Our convention is:
//	- leave G54 in machine coordinates to act as a persistent absolute coordinate system
//	- set G55 to be a zero in the middle of the table
//	- no action for the others

#define G54_X_OFFSET 0			// G54 is traditionally set to all zeros
#define G54_Y_OFFSET 0
#define G54_Z_OFFSET 0
#define G54_A_OFFSET 0
#define G54_B_OFFSET 0
#define G54_C_OFFSET 0

#define G55_X_OFFSET X_TRAVEL_MAX/2	// set g55 to middle of table
#define G55_Y_OFFSET Y_TRAVEL_MAX/2
#define G55_Z_OFFSET 0
#define G55_A_OFFSET 0
#define G55_B_OFFSET 0
#define G55_C_OFFSET 0

#define G56_X_OFFSET 0
#define G56_Y_OFFSET 0
#define G56_Z_OFFSET 0
#define G56_A_OFFSET 0
#define G56_B_OFFSET 0
#define G56_C_OFFSET 0

#define G57_X_OFFSET 0
#define G57_Y_OFFSET 0
#define G57_Z_OFFSET 0
#define G57_A_OFFSET 0
#define G57_B_OFFSET 0
#define G57_C_OFFSET 0

#define G58_X_OFFSET 0
#define G58_Y_OFFSET 0
#define G58_Z_OFFSET 0
#define G58_A_OFFSET 0
#define G58_B_OFFSET 0
#define G58_C_OFFSET 0

#define G59_X_OFFSET 0
#define G59_Y_OFFSET 0
#define G59_Z_OFFSET 0
#define G59_A_OFFSET 0
#define G59_B_OFFSET 0
#define G59_C_OFFSET 0


