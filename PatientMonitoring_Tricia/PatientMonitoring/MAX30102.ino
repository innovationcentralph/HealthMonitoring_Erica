void initSensor(){
   // Initialize sensor
  particleSensor.begin(Wire, I2C_SPEED_FAST); //Use default I2C port, 400kHz speed
  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
}

void calculateBPM(){
  long delta = millis() - lastBeat;                     //Measure duration between two beats
  lastBeat = millis();

  beatsPerMinute = 60 / (delta / 1000.0);               //Calculating the BPM

  if (beatsPerMinute < 255 && beatsPerMinute > 20)      //To calculate the average we strore some values (20) then do some math to calculate the average
  {
    rates[rateSpot++] = (byte)beatsPerMinute;           //Store this reading in the array
    rateSpot %= RATE_SIZE; //Wrap variable

    //Take average of readings
    patient.bpm = 0;
    for (byte x = 0 ; x < RATE_SIZE ; x++)
      patient.bpm += rates[x];
    patient.bpm /= RATE_SIZE;
    patient.spo2 = particleSensor.getSPO2(patient.bpm, particleSensor.getIR());
  }
}
