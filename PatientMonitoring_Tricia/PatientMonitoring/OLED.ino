void displayReadings(struct Patient * patient, bool isBeatDetected){
    
    display.clearDisplay();  
                           
    if(!isBeatDetected)          
      display.drawBitmap(5, 5, logo2_bmp, 24, 21, WHITE);   
    else
      display.drawBitmap(0, 0, logo3_bmp, 32, 32, WHITE); 
          
    display.setTextSize(1);                                   
    display.setTextColor(WHITE); 
    display.setCursor(50,0);                
    display.println("BPM");       
    display.setCursor(90,0);                
    display.println("SP0");     
    display.setTextSize(2);       
    display.setCursor(47,12);   
     
    if(patient->bpm < 40)
      display.println("--");
    else            
      display.println(patient->bpm); 
      
    display.setCursor(87,12);   

    if(patient->spo2 < 90)
      display.println("--");
    else
      display.println(patient->spo2); 
      
    display.display();
}

void displayNoneDetection(){
   display.clearDisplay();
   display.setTextSize(1);                    
   display.setTextColor(WHITE);             
   display.setCursor(30,10);                
   display.println("Please Place "); 
   display.setCursor(30,20);
   display.println("your finger ");  
   display.display();
}

void displaySending(){
  display.clearDisplay();
   display.setTextSize(2);                    
   display.setTextColor(WHITE);             
   display.setCursor(10,10);                
   display.println("SENDING...");  
   display.display();
}
