#include <Arduino.h>
#include <Servo.h>

// Note: with STEP 1 grad and PERIOD 100 mseconds, speed is 10 grad/second

/*
 *  Definitions in platformio.ini
 *
 *  VRX_PIN
 *  VRY_PIN
 *  SERVO_X_PIN
 *  SERVO_Y_PIN
 *  STEP
 *  PERIOD
 *  GUARD
 *  IDLE_X
 *  IDLE_Y
 *  SERIAL_BAUD
 */

Servo servoX;
Servo servoY;

static int joy_idle_X, joy_idle_Y;      //  Untouched position of joystick

/*
 *  process both joystick position
 *
 *  Each process function return -1, 0 or +1
 */

static int
process_X(void)
{
   int delta;

    delta = analogRead(VRX_PIN) - joy_idle_X;
    if( abs(delta) < GUARD )
        return 0;
    return delta < 0 ? -1 : 1;
}

static int
process_Y(void)
{
    int delta;

    delta = analogRead(VRY_PIN) - joy_idle_Y;
    if( abs(delta) < GUARD )
        return 0;
    return delta < 0 ? -1 : 1;
}

/*
 *  calculate_move:
 *      Receives:
 *          present position of servo
 *          delta movement: -1, 0 or +1
 *      Calculates and return next position
 */

static int
calculate_move( int present, int delta )
{
    if( delta > 0 )
    {
        if( ( present += STEP ) > 179 )
            present = 179;
    } else if( delta < 0 )
    {
        if( ( present -= STEP ) < 0 )
            present = 0;
    }
    return present;
}


void
setup()
{
    // initialize Serial
    Serial.begin(SERIAL_BAUD);
    // attach servo to pin
    servoX.attach(SERVO_X_PIN);
    servoY.attach(SERVO_Y_PIN);
    delay(1000);
    // send servos to idle positions
    servoX.write(IDLE_X);
    servoY.write(IDLE_Y);
    // take readings of joystick when not actuated
    joy_idle_X = analogRead(VRX_PIN);
    joy_idle_Y = analogRead(VRY_PIN);
}


void
loop()
{
    int present, result;

    present = servoX.read();                        //  present position
    if( ( result = process_X() ) != 0 )             //  delta movement -1, 0 or +1
    {
        present = calculate_move(present,result);   //  next angle position now in present
        servoX.write(present);                      //  delta != 0, then go for it !
    }

    present = servoY.read();                        //  present position
    if( ( result = process_Y() ) != 0 )             //  delta movement -1, 0 or +1
    {
        present = calculate_move(present,result);   //  next angle position now in present
        servoY.write(present);                      //  delta != 0, then go for it !
    }

    delay(PERIOD);
}
