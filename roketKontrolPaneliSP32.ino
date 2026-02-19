#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"
#include "time.h"

// ========== CONSTANTS ==========
#define COUNTDOWN_START     10
#define MAX_TEMP_C          85
#define MIN_FUEL_PERCENT    95
#define MAX_PRESSURE_BAR    150
#define GRAVITY 9.81
#define LOG_FILE "telemetry.txt"
#define STARTING_FUEL_PERCENT 100
#define ENGINE_ACCELERATION_FORCE 60 // m/s²
#define START_BUTTON 12
#define START_BUTTON_LED 13
#define ABORT_BUTTON 14
#define ABORT_BUTTON_LED 27

typedef enum launch_status {
	STATUS_STANDBY = 0,
	STATUS_COUNTDOWN = 1,
	STATUS_LIFTOFF = 2,
	STATUS_REACHEDSPACE = 3,
  STATUS_ABORT = 4
}LaunchStatus;

LaunchStatus LAUNCH_STATUS = STATUS_STANDBY;

enum SystemCheck {
	CHECK_PASSED = 1,
	CHECK_FAILED = 0
};

typedef union {
	uint8_t all;
	struct {
		uint8_t engine_ready	:1;
		uint8_t fuel_loaded		:1;
		uint8_t nav_online		:1;
		uint8_t comm_active		:1;
		uint8_t weather_clear	:1;
		uint8_t range_safe		:1;
		uint8_t crew_ready		:1;
		uint8_t abort_armed		:1;
	}flags;
}SystemFlags;

typedef struct Telemetry {
	uint8_t time_sec;
	uint8_t engine_temp_c;
	uint8_t fuel_percent;
	uint16_t chamber_pressure;
	float altitude_m;
	SystemFlags systems;
	float velocity;
  uint8_t current_fuel;
}TelemetryPacket;

TelemetryPacket flight_data;

// ========== FUNCTIONS ==========

void ABORT_OPERATION(TelemetryPacket *telemetry) {
	telemetry->systems.all = 0x00;
	Serial.printf("\n [EMERGENCY] ABORT SEQUENCE ACTIVATED! \n");
	Serial.printf("Reason: Critical sensor thresholds exceeded.\n");
	Serial.printf("All engines cut off. Ground safety systems engaged.\n");
}

void init_systems(SystemFlags *sys) {
	sys->all = 0xFF;
}

void update_flight_physics(TelemetryPacket *telemetry, uint8_t flight_time) {
	telemetry->velocity = (ENGINE_ACCELERATION_FORCE - GRAVITY) * flight_time;
	telemetry ->altitude_m =0.5 * (ENGINE_ACCELERATION_FORCE - GRAVITY) * flight_time * flight_time;
}

uint8_t read_sensors() {
	uint8_t temperature_C = random(0, 101);
	return temperature_C;
}

uint16_t read_pressure(TelemetryPacket *telemetry) {
	return telemetry->chamber_pressure =random(0, 151);
}

uint8_t read_fuel(TelemetryPacket *telemetry) {
	if (telemetry->current_fuel > 0) {
		telemetry->current_fuel--;
	}
	return telemetry->fuel_percent = telemetry->current_fuel;
}

uint8_t is_system_safe(TelemetryPacket *telemetry) {
	if(telemetry->systems.all != 0xFF) {
		Serial.printf("\n--------------------\nCan't start launch procedure! All conditions were not met!\n");
		Serial.printf("--------------------\n");
		ABORT_OPERATION(telemetry);
		return 0;
	}

	if (telemetry->chamber_pressure > MAX_PRESSURE_BAR || telemetry->engine_temp_c > MAX_TEMP_C || telemetry->fuel_percent< MIN_FUEL_PERCENT) {
		Serial.printf("\n--------------------\nCritical thresholds exceeded! Operation Abort!\n");
		Serial.printf("--------------------\n");
		ABORT_OPERATION(telemetry);
		return 0;
	}

	return 1;
}

// ========== END OF FUNCTIONS==========

// ========== MAIN BLOCK ==========
void setup() {
  Serial.begin(115200);
	init_systems(&flight_data.systems);
  pinMode(START_BUTTON, INPUT_PULLUP);
  pinMode(START_BUTTON_LED, OUTPUT);
  pinMode(ABORT_BUTTON, INPUT_PULLUP);
  pinMode(ABORT_BUTTON_LED, OUTPUT);

  digitalWrite(START_BUTTON_LED, LOW);
  digitalWrite(ABORT_BUTTON_LED, LOW);

  flight_data.current_fuel = STARTING_FUEL_PERCENT;

  init_systems(&flight_data.systems);

	Serial.printf("== ROCKET LAUNCH CONTROL CENTER ==\n");
  Serial.printf("====================================\n\n");
  
}

void loop() {
  
  if (LAUNCH_STATUS == STATUS_STANDBY) {
        // Is start buton used?
        if (digitalRead(START_BUTTON) == LOW) {
            delay(50);
            if (digitalRead(START_BUTTON) == LOW) {
                LAUNCH_STATUS = STATUS_COUNTDOWN;
                digitalWrite(START_BUTTON_LED, HIGH);
                Serial.printf("\nCOUNTDOWN INITIATED!\n\n");
            }
        }
    }
  
  else if(LAUNCH_STATUS == STATUS_COUNTDOWN) {
    digitalWrite(START_BUTTON_LED, HIGH);

    for(int8_t t = 	COUNTDOWN_START; t>=0; t--) {
      //Update Telemetry
      flight_data.time_sec = t;
      flight_data.engine_temp_c = read_sensors();
      read_pressure(&flight_data);
      flight_data.fuel_percent = read_fuel(&flight_data);

      if (!is_system_safe(&flight_data)) {
			  LAUNCH_STATUS = STATUS_ABORT; //Launch failed!
        break;
		  }

      Serial.printf("T-%2d | Temp: %2d C | Press: %3d Bar | Fuel: %%%2d | Status: GO\n",
				t, flight_data.engine_temp_c, flight_data.chamber_pressure, flight_data.fuel_percent);

      delay(1000);
      //Abort button check
      if (digitalRead(ABORT_BUTTON) == LOW) {
        LAUNCH_STATUS = STATUS_ABORT;
        break;
      }
    }
    if (LAUNCH_STATUS == STATUS_COUNTDOWN) {
      LAUNCH_STATUS = STATUS_LIFTOFF;
    }
  }

  else if (LAUNCH_STATUS == STATUS_LIFTOFF) {
      Serial.printf("\n----- IGNITION! -----\n");
      Serial.printf("||| ROCKET IS LAUNCHED! GAINING ALTITUDE... |||\n");

      uint8_t flight_time = 0;
      while(flight_time < 120) {
        flight_time++;
        
        update_flight_physics(&flight_data, flight_time);

        Serial.printf("FLIGHT T+%d s | Altitude: %.f m | Velocity: %.f m/s\n",
        flight_time, flight_data.altitude_m, flight_data.velocity);

        if (flight_data.altitude_m > 100000) {
        LAUNCH_STATUS = STATUS_REACHEDSPACE;
        break;
        }
        delay(1000);

        if (digitalRead(ABORT_BUTTON) == LOW) {
          Serial.printf("\nOperation Abort!\n");
          LAUNCH_STATUS = STATUS_ABORT;
          break;
        }
      }
  }

  else if (LAUNCH_STATUS == STATUS_REACHEDSPACE) {
    Serial.printf("\n //////SPACE REACHED\\\\\\ \n");
    Serial.printf("Altitude: %.0f m (Kármán Line)\n", flight_data.altitude_m);
    Serial.printf("Velocity: %.0f m/s\n", flight_data.velocity);
    Serial.printf("Mission SUCCESS! 🎉\n\n");

    //HELL YEAH
    for (uint8_t i = 0; i < 5; i++) {
      digitalWrite(START_BUTTON_LED, HIGH);
      delay(200);
      digitalWrite(START_BUTTON_LED, LOW);
      delay(200);
    }

    LAUNCH_STATUS = STATUS_STANDBY;
    Serial.printf("\nSystem ready for next launch.\n");

    //Resetting for next launch
    flight_data.altitude_m = 0;
    flight_data.velocity = 0;
    flight_data.current_fuel = STARTING_FUEL_PERCENT;
    flight_data.fuel_percent = STARTING_FUEL_PERCENT;
  }

  else if (digitalRead(ABORT_BUTTON) == LOW) {
    digitalWrite(ABORT_BUTTON_LED, HIGH);
    LAUNCH_STATUS = STATUS_ABORT;
  }

  else if (LAUNCH_STATUS == STATUS_ABORT) {
    ABORT_OPERATION(&flight_data);
    Serial.printf("\n*** LAUNCH ABORT. CHECK THE LOGS FOR MORE DETAIL. ***\n");

    delay(250);

    //hell nah :(
    delay(300);
    digitalWrite(START_BUTTON_LED, LOW);
    for(uint8_t i = 0; i < 5; i++) {
      digitalWrite(ABORT_BUTTON_LED, HIGH);
      delay(500);
      digitalWrite(ABORT_BUTTON_LED, LOW);
      delay(500);
    }

    digitalWrite(START_BUTTON_LED, LOW);
    digitalWrite(ABORT_BUTTON_LED, LOW);

    init_systems(&flight_data.systems);

    LAUNCH_STATUS = STATUS_STANDBY;
    Serial.printf("\nSystem reset! Ready for next launch...");

    //Resetting for next launch
    flight_data.altitude_m = 0;
    flight_data.velocity = 0;
    flight_data.current_fuel = STARTING_FUEL_PERCENT;
    flight_data.fuel_percent = STARTING_FUEL_PERCENT;
  }

  if (LAUNCH_STATUS == STATUS_STANDBY && digitalRead(ABORT_BUTTON) == LOW) {
    Serial.printf("\nThere is not any countdown already!\n");
  }
}
