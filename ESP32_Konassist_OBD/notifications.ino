/*
void car_status(){  // TTS
                       car_status_flag = 0;
                        
                       if(!strcmp(language, "hu")) {
                                                     //hu
                                                     if(still_online) txtnotify  = "Kona, úton van, ";
                                                     else txtnotify = "Kona, befejezte az utat,";
                                                     txtnotify = txtnotify + String(SOCpercent);
                                                     txtnotify = txtnotify + " százalékos akkuval.";
                                                     ghn.notify(txtnotify.c_str());
                                                   }
                                           
                       else if (!strcmp(language, "ro")){

                                                         // ro
                                                         if(still_online) txtnotify  = "Kona, se află intr-o călătorie, cu baterie ";
                                                         else txtnotify = "Kona, a terminat călătoria, cu baterie";
                                                         txtnotify = txtnotify + String(SOCpercent);
                                                         txtnotify = txtnotify + " la sută.";
                                                         ghn.notify(txtnotify.c_str());
                                                        }

                        else if (!strcmp(language, "en")){                   
                                                           // en
                                                           if(still_online) txtnotify  = "Your Kona, is on a trip with ";
                                                           else txtnotify = "Your Kona, ended the trip with ";
                                                           txtnotify = txtnotify + String(SOCpercent);
                                                           txtnotify = txtnotify + " percent battery.";
                                                           ghn.notify(txtnotify.c_str());
                                                        }                         
                                                     
                       

                    }


void language_change(){
                        if(!strcmp(language, "hu")) {
                                                      txtnotify  = "Magyar nyelv kiválasztva";
                                                      ghn.notify(txtnotify.c_str());
                                                    }  

                        else if(!strcmp(language, "ro")) {
                                                          txtnotify  = "Limba română setat";
                                                          ghn.notify(txtnotify.c_str());
                                                        }   
                                                     
                        else if(!strcmp(language, "en")) {
                                                          txtnotify  = "English was set";
                                                          ghn.notify(txtnotify.c_str());
                                                        }                                                               
  
                        }   


void car_battery(){
                     car_battery_flag = 0;
                     if(!strcmp(language, "hu")) {
                                                     //hu
                                                     txtnotify  = "Kona, töltési állapota ";
                                                     txtnotify = txtnotify + String(SOCpercent);
                                                     txtnotify = txtnotify + " százalék, és jelenleg, ";
                                                     if(!charging_flag) txtnotify = txtnotify + "nem töltődik.";
                                                     else {
                                                           txtnotify = txtnotify + String(BATTpow,1);  // 1 decimal place
                                                           txtnotify = txtnotify + " kilowattal töltődik. ";
                                                          }
                                                                                                              
                                                     txtnotify = txtnotify + " Az akku hőmérséklete ";
                                                     txtnotify = txtnotify + String(BATTtemp,1);  // 1 decimal place
                                                     txtnotify = txtnotify + " Celsius fok.";
                                                     
                                                     ghn.notify(txtnotify.c_str());
                                               }
                                           
                       else if (!strcmp(language, "ro")){

                                                         // ro
                                                         txtnotify  = "Nivelul de încărcare Kona, ";
                                                         txtnotify = txtnotify + String(SOCpercent);
                                                         txtnotify = txtnotify + " la sută. Momentan ";
                                                         if(!charging_flag) txtnotify = txtnotify + "nu se încarcă. ";
                                                         else {
                                                                 txtnotify = txtnotify + "se încarcă, cu ";
                                                                 txtnotify = txtnotify + String(BATTpow,1);  // 1 decimal place
                                                                 txtnotify = txtnotify + " kilowatts. ";
                                                                 }
                                                         txtnotify = txtnotify + "Temperatura bateriei, ";
                                                         txtnotify = txtnotify + String(BATTtemp,1);  // 1 decimal place
                                                         txtnotify = txtnotify + " grade Celsius.";
                                                         ghn.notify(txtnotify.c_str());
                                                        }

                        else if (!strcmp(language, "en")){                   
                                                           // en
                                                           txtnotify  = "Your Kona battery, is at ";
                                                           txtnotify = txtnotify + String(SOCpercent);
                                                           txtnotify = txtnotify + " percent, and now is ";
                                                           if(!charging_flag) txtnotify = txtnotify + "not charging. ";
                                                           else {
                                                                   txtnotify = txtnotify + "charging, with ";
                                                                   txtnotify = txtnotify + String(BATTpow,1);  // 1 decimal place
                                                                   txtnotify = txtnotify + " kilowatts. ";
                                                                   }
                                                           txtnotify = txtnotify + " Battery temperature, ";
                                                           txtnotify = txtnotify + String(BATTtemp,1);  // 1 decimal place
                                                           txtnotify = txtnotify + " degree Celsius.";
                                                           ghn.notify(txtnotify.c_str());
                                                        }                         
                                                     
                       

                  }

void car_charging(){
                     
                     if(!strcmp(language, "hu")) {
                                                     //hu
                                                     if(charging_flag) txtnotify  = "Töltés megkezdve ";
                                                     else txtnotify  = "Töltés befejezve ";
                                                     txtnotify = txtnotify + String(SOCpercent);
                                                     txtnotify = txtnotify + " százalékon. ";
                                                     if(charging_flag) {
                                                                         txtnotify = txtnotify + "Jelenleg ";
                                                                         txtnotify = txtnotify + String(BATTpow,1);  // 1 decimal place
                                                                         txtnotify = txtnotify + " kilowattal töltődik. ";
                                                                        }
                                                     txtnotify = txtnotify + " Az akku hőmérséklete ";
                                                     txtnotify = txtnotify + String(BATTtemp,1);  // 1 decimal place
                                                     txtnotify = txtnotify + " Celsius fok.";
                                                     
                                                     ghn.notify(txtnotify.c_str());
                                               }
                                           
                       else if (!strcmp(language, "ro")){

                                                         // ro

                                                         if(charging_flag) txtnotify  = "Încărcarea a fost pornit la ";
                                                         else txtnotify  = "Încărcarea a fost oprit la ";
                                                         txtnotify = txtnotify + String(SOCpercent);
                                                         txtnotify = txtnotify + " la sută. ";
                                                         if(charging_flag) {
                                                                             txtnotify = txtnotify + "Momentan, se încarcă cu ";
                                                                             txtnotify = txtnotify + String(BATTpow,1);  // 1 decimal place
                                                                             txtnotify = txtnotify + " kilowatts. ";
                                                                            }
                                                         txtnotify = txtnotify + "Temperatura bateriei, ";
                                                         txtnotify = txtnotify + String(BATTtemp,1);  // 1 decimal place
                                                         txtnotify = txtnotify + " grade Celsius.";
                                                         
                                                         ghn.notify(txtnotify.c_str());
                                                                                                                  
                                                        }

                        else if (!strcmp(language, "en")){                   
                                                           // en

                                                           if(charging_flag) txtnotify  = "Charging started, at ";
                                                           else txtnotify  = "Charging stopped, at ";
                                                           txtnotify = txtnotify + String(SOCpercent);
                                                           txtnotify = txtnotify + " percent. ";
                                                           if(charging_flag) {
                                                                               txtnotify = txtnotify + "With ";
                                                                               txtnotify = txtnotify + String(BATTpow,1);  // 1 decimal place
                                                                               txtnotify = txtnotify + " kilowatts. ";
                                                                              }
                                                           txtnotify = txtnotify + " Battery temperature, ";
                                                           txtnotify = txtnotify + String(BATTtemp,1);  // 1 decimal place
                                                           txtnotify = txtnotify + " degree Celsius.";
                                                                                                     
                                                           ghn.notify(txtnotify.c_str());
                                                        }                         
                                                     
                       

                  }

void cell_voltage(){
                       cell_voltage_flag = 0;
                       
                        
                       if(!strcmp(language, "hu")) {
                                                     //hu
                                                     txtnotify  = "Cella feszültség, minimum ";
                                                     txtnotify = txtnotify + String(MINcellv,2); // 2 decimal place
                                                     txtnotify = txtnotify + " volt, Maximum ";
                                                     txtnotify = txtnotify + String(MAXcellv,2); // 2 decimal place
                                                     txtnotify = txtnotify + " volt. Különbség ";
                                                     txtnotify = txtnotify + String(cell_voltage_difference); 
                                                     txtnotify = txtnotify + " millivolt.";
                                                     ghn.notify(txtnotify.c_str());
                                                   }
                                           
                       else if (!strcmp(language, "ro")){

                                                         // ro
                                                           txtnotify  = "Tensiunea minimă, pe celulă ";
                                                           txtnotify = txtnotify + String(MINcellv,2); // 2 decimal place
                                                           txtnotify = txtnotify + " volți, Maximă ";
                                                           txtnotify = txtnotify + String(MAXcellv,2); // 2 decimal place
                                                           txtnotify = txtnotify + " volți. Diferență ";
                                                           txtnotify = txtnotify + String(cell_voltage_difference); 
                                                           txtnotify = txtnotify + " millivolți.";
                                                           ghn.notify(txtnotify.c_str());
                                                        }

                        else if (!strcmp(language, "en")){                   
                                                           // en
                                                           txtnotify  = "Minimum cell voltage ";
                                                           txtnotify = txtnotify + String(MINcellv,2); // 2 decimal place
                                                           txtnotify = txtnotify + ", Maximum ";
                                                           txtnotify = txtnotify + String(MAXcellv,2); // 2 decimal place
                                                           txtnotify = txtnotify + " volts. Difference ";
                                                           txtnotify = txtnotify + String(cell_voltage_difference); 
                                                           txtnotify = txtnotify + " millivolts.";
                                                           ghn.notify(txtnotify.c_str());
                                                        }                         
                                                     
  }

void home_temphumi(){
                        home_temphumi_flag = 0;
                       
                        
                       if(!strcmp(language, "hu")) {
                                                     //hu
                                                     txtnotify  = "Benti hőmérséklet ";
                                                     txtnotify = txtnotify + String(temp_average,1); // 1 decimal place
                                                     txtnotify = txtnotify + " Celsius fok.";
                                                     txtnotify = txtnotify + " Páratartalom ";
                                                     txtnotify = txtnotify + String(humi_average,1); // 1 decimal place
                                                     txtnotify = txtnotify + " százalék.";
                                                     ghn.notify(txtnotify.c_str());
                                                   } 

                       else if(!strcmp(language, "ro")) {
                                                           //hu
                                                           txtnotify  = "Temperatura în interior ";
                                                           txtnotify = txtnotify + String(temp_average,1); // 1 decimal place
                                                           txtnotify = txtnotify + " grade Celsius.";
                                                           txtnotify = txtnotify + " Umiditate ";
                                                           txtnotify = txtnotify + String(humi_average,1); // 1 decimal place
                                                           txtnotify = txtnotify + " la sută.";
                                                           ghn.notify(txtnotify.c_str());
                                                         } 

                       else if(!strcmp(language, "en")) {
                                                           //hu
                                                           txtnotify  = "Indoor temperature ";
                                                           txtnotify = txtnotify + String(temp_average,1); // 1 decimal place
                                                           txtnotify = txtnotify + " degree Celsius.";
                                                           txtnotify = txtnotify + " Humidity ";
                                                           txtnotify = txtnotify + String(humi_average,1); // 1 decimal place
                                                           txtnotify = txtnotify + " percent.";
                                                           ghn.notify(txtnotify.c_str());
                                                         }                                   
}
*/
