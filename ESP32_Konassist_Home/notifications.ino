void welcome_message(){
                         welcome_flag = 1;
                         if(!strcmp(language, "hu")) {
                                                      txtnotify  = "Hello! Konaszisztens csatlakozva!";
                                                      ghn.notify(txtnotify.c_str());
                                                    }  

                        else if(!strcmp(language, "ro")) {
                                                          txtnotify  = "Salut! Konassist conectat!";
                                                          ghn.notify(txtnotify.c_str());
                                                        }   
                                                     
                        else if(!strcmp(language, "en")) {
                                                          txtnotify  = "Hi! Konassist connected!";
                                                          ghn.notify(txtnotify.c_str());
                                                        }                                              
  
  }

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
                        
                        language_flag = 0;
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
                                                     if(charging_flag == 1) txtnotify  = "AC töltés megkezdve ";
                                                     else if(charging_flag == 2) txtnotify  = "DC töltés megkezdve ";
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

                                                         if(charging_flag == 1) txtnotify  = "Încărcarea AC a fost pornit la ";
                                                         else if(charging_flag == 2) txtnotify  = "Încărcarea DC a fost pornit la ";
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

                                                           if(charging_flag == 1) txtnotify  = "AC charging started, at ";
                                                           else if(charging_flag == 2) txtnotify  = "DC charging started, at ";
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

void AUXSOCpercent_Warning_notify(){

                                       if(!strcmp(language, "hu")) {
                                                                           //hu
                                                                           txtnotify  = "Figyelem! Külső akku, töltési szintje, ";
                                                                           txtnotify = txtnotify + String(AUXBATTsoc); 
                                                                           txtnotify = txtnotify + " százalék alatt!";
                                                                           ghn.notify(txtnotify.c_str());
                                                                         } 
      
                                       else if(!strcmp(language, "ro")) {
                                                                           //ro
                                                                           txtnotify  = "Atenție! Încărcarea bateriei externe, sub ";
                                                                           txtnotify = txtnotify + String(AUXBATTsoc);
                                                                           txtnotify = txtnotify + " la sută!";
                                                                           ghn.notify(txtnotify.c_str());
                                                                         } 
      
                                       else if(!strcmp(language, "en")) {
                                                                           //en
                                                                           txtnotify  = "Warning! Auxiliary battery charge level, less than ";
                                                                           txtnotify = txtnotify + String(AUXBATTsoc);
                                                                           txtnotify = txtnotify + " percent!";
                                                                           ghn.notify(txtnotify.c_str());
                                                                         }                                   
  
  }

void BATTtemp_Warning_notify(){
                                       if(!strcmp(language, "hu")) {
                                                                           //hu
                                                                           txtnotify  = "Figyelem! Akku, hőmérséklet, ";
                                                                           txtnotify = txtnotify + String(BATTtemp); 
                                                                           txtnotify = txtnotify + " Celsius fok!";
                                                                           ghn.notify(txtnotify.c_str());
                                                                         } 
      
                                       else if(!strcmp(language, "ro")) {
                                                                           //ro
                                                                           txtnotify  = "Atenție! Temperatura bateriei, peste ";
                                                                           txtnotify = txtnotify + String(BATTtemp);
                                                                           txtnotify = txtnotify + " grade Celsius!";
                                                                           ghn.notify(txtnotify.c_str());
                                                                         } 
      
                                       else if(!strcmp(language, "en")) {
                                                                           //en
                                                                           txtnotify  = "Warning! Main battery temperature, above ";
                                                                           txtnotify = txtnotify + String(BATTtemp);
                                                                           txtnotify = txtnotify + " degree Celsius!";
                                                                           ghn.notify(txtnotify.c_str());
                                                                         }                                   
  
  }  

void SOCpercent_Warning_notify(){
                                    if(!strcmp(language, "hu")) {
                                                                           //hu
                                                                           txtnotify  = "Figyelem! Az akku, töltési szintje, ";
                                                                           txtnotify = txtnotify + String(SOCpercent); 
                                                                           txtnotify = txtnotify + " százalék alatt!";
                                                                           ghn.notify(txtnotify.c_str());
                                                                         } 
      
                                       else if(!strcmp(language, "ro")) {
                                                                           //ro
                                                                           txtnotify  = "Atenție! Încărcarea bateriei, sub ";
                                                                           txtnotify = txtnotify + String(SOCpercent);
                                                                           txtnotify = txtnotify + " la sută!";
                                                                           ghn.notify(txtnotify.c_str());
                                                                         } 
      
                                       else if(!strcmp(language, "en")) {
                                                                           //en
                                                                           txtnotify  = "Warning! Main battery charge level, less than ";
                                                                           txtnotify = txtnotify + String(SOCpercent);
                                                                           txtnotify = txtnotify + " percent!";
                                                                           ghn.notify(txtnotify.c_str());
                                                                         }                                   
  }  

 void trip_info(){
                      home_tripinfo_flag = 0;
                       
                        
                       if(!strcmp(language, "hu")) {
                                                     //hu

                                                     txtnotify  = "Megtett távolság ";
                                                     txtnotify = txtnotify + String(TRIP1odo); // 1 decimal place
                                                     txtnotify = txtnotify + " kilométer. ";
                                                     txtnotify = txtnotify + "Felhasznált energia ";
                                                     txtnotify = txtnotify + String(TRIP1used,1); // 1 decimal place
                                                     txtnotify = txtnotify + " kilowattóra. ";
                                                     txtnotify = txtnotify + "Átlagfogyasztás ";
                                                     txtnotify = txtnotify + String(TRIP1avg, 1); // 1 decimal place
                                                     txtnotify = txtnotify + " kilowattóra per száz kilométer. ";
                                                     
                                                     /*
                                                     txtnotify  = "Visszatöltött energia ";
                                                     txtnotify = txtnotify + String(TRIP1ch,1); // 1 decimal place
                                                     txtnotify = txtnotify + " kilowattóra. ";
                                                     txtnotify = txtnotify + "Felhasznált energia ";
                                                     txtnotify = txtnotify + String(TRIP1dch,1); // 1 decimal place
                                                     txtnotify = txtnotify + " kilowattóra.";
                                                     */
                                                     ghn.notify(txtnotify.c_str());
                                                   } 

                       else if(!strcmp(language, "ro")) {
                                                           //ro

                                                           txtnotify  = "Distanța parcursă ";
                                                           txtnotify = txtnotify + String(TRIP1odo); // 1 decimal place
                                                           txtnotify = txtnotify + " kilometri. ";
                                                           txtnotify = txtnotify + "Energia consumată ";
                                                           txtnotify = txtnotify + String(TRIP1used,1); // 1 decimal place
                                                           txtnotify = txtnotify + " kilowattoră. ";
                                                           txtnotify = txtnotify + "Consum mediu ";
                                                           txtnotify = txtnotify + String(TRIP1avg, 1); // 1 decimal place
                                                           txtnotify = txtnotify + " kilowattoră per o sută de kilometri. "; 
                                                           /*
                                                           txtnotify  = "Energia recuperată ";
                                                           txtnotify = txtnotify + String(TRIP1ch,1); // 1 decimal place
                                                           txtnotify = txtnotify + " kilowattoră. ";
                                                           txtnotify = txtnotify + "Energia consumată ";
                                                           txtnotify = txtnotify + String(TRIP1dch,1); // 1 decimal place
                                                           txtnotify = txtnotify + " kilowattoră.";
                                                           */
                                                           ghn.notify(txtnotify.c_str());
                                                         } 

                       else if(!strcmp(language, "en")) {
                                                           //en
                                                           txtnotify  = "Trip distance ";
                                                           txtnotify = txtnotify + String(TRIP1odo); // 1 decimal place
                                                           txtnotify = txtnotify + " kilometers. ";
                                                           txtnotify = txtnotify + "Energy used ";
                                                           txtnotify = txtnotify + String(TRIP1used,1); // 1 decimal place
                                                           txtnotify = txtnotify + " kilowatthours. ";
                                                           txtnotify = txtnotify + "Average consumption ";
                                                           txtnotify = txtnotify + String(TRIP1avg, 1); // 1 decimal place
                                                           txtnotify = txtnotify + " kilowatthours per one hundred kilometers. "; 
                                                           
                                                           /*
                                                           txtnotify  = "Charged energy ";
                                                           txtnotify = txtnotify + String(TRIP1ch,1); // 1 decimal place
                                                           txtnotify = txtnotify + " kilowatthours. ";
                                                           txtnotify = txtnotify + "Discharged energy ";
                                                           txtnotify = txtnotify + String(TRIP1dch,1); // 1 decimal place
                                                           txtnotify = txtnotify + " kilowatthours.";
                                                           */
                                                           ghn.notify(txtnotify.c_str());
                                                         }
  
  }

void boiler_info(){
                       boiler_info_flag = 0;
                       
                        
                       if(!strcmp(language, "hu")) {
                                                     //hu
                                                     txtnotify  = "Vizhőmérséklet ";
                                                     txtnotify = txtnotify + String(boiler_temp,1); // 1 decimal place
                                                     txtnotify = txtnotify + " Celsius fok!";
                                                     
                                                     ghn.notify(txtnotify.c_str());
                                                   } 

                       else if(!strcmp(language, "ro")) {
                                                           //ro
                                                           txtnotify  = "Temperatura apei ";
                                                           txtnotify = txtnotify + String(boiler_temp,1); // 1 decimal place
                                                           txtnotify = txtnotify +  " grade Celsius!";
                                                           
                                                           ghn.notify(txtnotify.c_str());
                                                         } 

                       else if(!strcmp(language, "en")) {
                                                           //en
                                                           txtnotify  = "Water temperature ";
                                                           txtnotify = txtnotify + String(boiler_temp,1); // 1 decimal place
                                                           txtnotify = txtnotify + " degree Celsius!";
                                                           
                                                           ghn.notify(txtnotify.c_str());
                                                         }
  }   
