byte reconnectCount = 0;

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  display.clearDisplay();
  display.clearDisplay();
  while (WiFi.status() != WL_CONNECTED) {
    reconnectCount++;
    if(reconnectCount > 20){
      ESP.restart();
    }
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

void sendDataToWeb(struct Patient *p){

  for(int x = 0; x < 5; x++){
     digitalWrite(NETWORK_LED, HIGH);
     delay(200);
     digitalWrite(NETWORK_LED, LOW);
     delay(200);
   }
   
 
  if((WiFi.status() == WL_CONNECTED)) {
      
        HTTPClient http;

        USE_SERIAL.print("[HTTP] begin...\n");

        //http://mactechph.com/covidMonitoringv2/resources/data/sensorlog.php?id=P000&sp02=90&pulse=60
        
        String URL = "http://mactechph.com/covidMonitoring/resources/data/sensorlog.php?id=" + String(p->deviceID) + "&temp=" + String(p->temp) + "&sp02=" + String(p->spo2) + "&pulse=" + String(p->bpm);
        Serial.println("URL: " + URL);
        http.begin(URL); 

        USE_SERIAL.print("[HTTP] GET...\n");
        
        int httpCode = http.GET();

        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                USE_SERIAL.println(payload);
            }
        } else {
            USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
    }
}

void checkNetwork(){
  if((WiFi.status() == WL_CONNECTED)) {
    digitalWrite(NETWORK_LED, HIGH);
  }
  else{
    digitalWrite(NETWORK_LED, LOW);
  }
}
