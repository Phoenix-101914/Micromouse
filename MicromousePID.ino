//Input and output pins
//Motor encoder
const int encoderLeftA = 2;   //interrupt pin
const int encoderLeftB = 12;  //this pin is a normal pin, read upon the interrupt
const int encoderRightA = 3;  //interrupt piin
const int encoderRightB = 11; //this pin is a normal pin, read upon the interrupt
int encoderLeftBValue = 0;  //value of the encoder pin (0 or 1), this pin is read inside the interrupt - used for direction determination
int encoderRightBValue = 0;

const float tolerableErrorStraight = 0.5;
const float tolerableErrorTurn = 0.5;


//PWM motor driver
const int ENA = 5;  
const int ENB = 6;
int speedLeft = 0;
int speedRight = 0;
const int IN1 = 10; 
const int IN2 = 9;  
const int IN3 = 8;
const int IN4 = 7;


//Target values
const float straightTicks = 38;
const float turnTicks = 16;


//Measured values
volatile float motorPositionLeft = 0;  //position based on the encoder
volatile float motorPositionRight = 0;
float motorPosition;


//PID-related
unsigned long previousTime = 0;   //for calculating delta t
float previousError = 0;  //for calculating the derivative (edot)
//float errorIntegral = 0;  integral error not used but can be added
unsigned long currentTime = 0;    //time in the moment of calculation
float deltaTime = 0;      //time difference
float errorValue = 0;     //error
float edot = 0;           //derivative (de/dt)


void setup() {
  Serial.begin(9600);

  //Motor encoder-related
  pinMode(encoderLeftA, INPUT_PULLUP);  //A
  pinMode(encoderLeftB, INPUT_PULLUP);  //B
  attachInterrupt(digitalPinToInterrupt(encoderLeftA), checkEncoderLeft, RISING);

  pinMode(encoderRightA, INPUT_PULLUP);  //A
  pinMode(encoderRightB, INPUT_PULLUP);  //B
  attachInterrupt(digitalPinToInterrupt(encoderRightA), checkEncoderRight, RISING);

  //Motor driver pins

  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
}

void loop() {
  delay(1000);
  moveForward(straightTicks);
  //delay(1000);
  //turn(-turnTicks);
  delay(100);
  turn(turnTicks);
  delay(1000);

}

void checkEncoderLeft() {
  //We need to read the other pin of the encoder which will be either 1 or 0 depending on the direction
  encoderLeftBValue = digitalRead(encoderLeftB);

  if (encoderLeftBValue == 1)  //CW direction
  {
    motorPositionLeft++;
  } else  //CCW direction
  {
    motorPositionLeft--;
  }
}

void checkEncoderRight() {
  //We need to read the other pin of the encoder which will be either 1 or 0 depending on the direction
  encoderRightBValue = digitalRead(encoderRightB);

  if (encoderRightBValue == 1)  //CW direction
  {
    motorPositionRight--;
  } else  //CCW direction
  {
    motorPositionRight++;
  }
}

void moveLeftWheel(int speed, bool direction) {
  if (direction) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
  }
  analogWrite(ENA, speed);
}

void moveRightWheel(int speed, bool direction) {
  if (direction) {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  } else {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  }
  analogWrite(ENB, speed);
}

void stop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 255);
  analogWrite(ENB, 255);
  delay(100);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

void moveForward(int target) {
  motorPositionLeft = 0;
  motorPositionRight = 0;
  previousError = 0;
  errorValue = target;
  previousTime = micros();
  float proportional = 1.0;  //k_p
  //float integral = 0;        //k_i = 0
  float derivative = 0;      //k_d
  float controlSignal = 0;
  float Kcorr = 15;
  while ((fabs(errorValue) >= tolerableErrorStraight)) {
    motorPosition = (motorPositionLeft + motorPositionRight) / 2.0;
    //Determining the elapsed time
    currentTime = micros();                                //current time
    deltaTime = (currentTime - previousTime) / 1000000.0;  //time difference in seconds
    previousTime = currentTime;                            //save the current time for the next iteration to get the time difference

    errorValue = target - motorPosition;  //Current position - target position (or setpoint)

    Serial.print(errorValue);
    Serial.print(" ");
    Serial.print(target);
    Serial.print(" ");
    Serial.print(motorPosition);
    Serial.println();

    if (fabs(errorValue) < tolerableErrorStraight) {
      break;
    } else {
      edot = (errorValue - previousError) / deltaTime;  //edot = de/dt - derivative term

      //errorIntegral = errorIntegral + (errorValue * deltaTime);  //integral term - not used

      controlSignal = (proportional * errorValue) + (derivative * edot); //final sum, proportional term also calculated here

      previousError = errorValue;  //save the error for the next iteration to get the difference (for edot)
    }

    float diff = motorPositionLeft - motorPositionRight;
    float speed = controlSignal;  //PWM values cannot be negative and have to be integers
    speedLeft = speed - Kcorr * diff;
    speedRight = speed + Kcorr * diff;


    speedLeft = (int)fabs(speedLeft);
    speedRight = (int)fabs(speedRight);

    if (speedLeft > 255)  
    {
      speedLeft = 255;  
    }
    if (speedRight > 255)  
    {
      speedRight = 255;  
    }

    const int MIN_START = 120;
    const float ERR_MIN = 0.5;  // ticks

    if (fabs(errorValue) > ERR_MIN && speedLeft > 0 && speedLeft < MIN_START) {
      speedLeft = MIN_START;
    }
    if (fabs(errorValue) > ERR_MIN && speedRight > 0 && speedRight < MIN_START) {
      speedRight = MIN_START;
    }

    if (controlSignal < 0) {
      moveLeftWheel(speedLeft, 0);
      moveRightWheel(speedRight, 0);
    }

    else if (controlSignal > 0) {
      moveLeftWheel(speedLeft, 1);
      moveRightWheel(speedRight, 1);
    }

    
  }
  stop();
}

void turn(int target) { //+ve target for right, -ve for left
  motorPositionLeft = 0;
  motorPositionRight = 0;
  errorValue = target;
  previousTime = micros();
  previousError = 0;
  float proportional = 6.0;  //k_p
  //float integral = 0;        //k_i = 0
  float derivative = 1;      //k_d
  float controlSignal = 0;
  float Kdrift = 2;

  while ((fabs(errorValue) >= tolerableErrorTurn)) {
    motorPosition = (motorPositionLeft - motorPositionRight) / 2.0;
    //Determining the elapsed time
    currentTime = micros();                                //current time
    deltaTime = (currentTime - previousTime) / 1000000.0;  //time difference in seconds
    previousTime = currentTime;                            //save the current time for the next iteration to get the time difference

    errorValue = target - motorPosition;  //Current position - target position (or setpoint)
    if (fabs(errorValue) < tolerableErrorTurn) {
      stop();
      break;
    } else {
      edot = (errorValue - previousError) / deltaTime;  //edot = de/dt - derivative term

      //errorIntegral = errorIntegral + (errorValue * deltaTime);  //integral term -not used

      controlSignal = (proportional * errorValue) + (derivative * edot);//final sum, proportional term also calculated here

      previousError = errorValue;  //save the error for the next iteration to get the difference (for edot)
    }

    float diff = motorPositionLeft + motorPositionRight;
    float speed = controlSignal;  //PWM values cannot be negative and have to be integers
    speedLeft = speed - Kdrift * diff;
    speedRight = speed + Kdrift * diff;


    speedLeft = (int)fabs(speedLeft);
    speedRight = (int)fabs(speedRight);

    if (speedLeft > 255) 
    {
      speedLeft = 255; 
    }
    if (speedRight > 255)
    {
      speedRight = 255; 
    }

    const int MIN_START = 160;
    //const float ERR_MIN = 0.5;  // ticks

    if (speedLeft > 0 && speedLeft < MIN_START) {
      speedLeft = MIN_START;
    }
    if (speedRight > 0 && speedRight < MIN_START) {
      speedRight = MIN_START;
    }

    if (controlSignal < 0) {
      moveLeftWheel(speedLeft, 0);
      moveRightWheel(speedRight, 1);
    }

    else if (controlSignal > 0) {
      moveLeftWheel(speedLeft, 1);
      moveRightWheel(speedRight, 0);
    }

    Serial.print(errorValue);
    Serial.print(" ");
    Serial.print(speed);
    Serial.print(" ");
    Serial.print(target);
    Serial.print(" ");
    Serial.print(motorPosition);
    Serial.println();
  }
  stop();
}
