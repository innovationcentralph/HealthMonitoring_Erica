void tone(byte pin, int freq) {
  ledcSetup(0, 2000, 8); // setup beeper
  ledcAttachPin(pin, 0); // attach beeper
  ledcWriteTone(0, freq); // play tone
}

void sendingTone(){
  for(int x = 0; x < 5; x++){
    tone(19,3000);                                        
    delay(80);
    tone(19, 0);
    delay(50);
  }
}
