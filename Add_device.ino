///////////////////////////////// Add Device /////////////////////////////////////
void setupAP()
{
  WiFi.disconnect();
  Serial.println("AP Mode...");
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP(ssidAP, passAP);
  WiFi.softAPIP();
  server.setRSACert(new BearSSL::X509List(serverCert), new BearSSL::PrivateKey(serverKey));
  server.on("/ssiddevice", ssiddevice);
  server.on("/adddevice", adddevice);
  server.begin();
  for (int a = 0; a < 10; a++) {
    indicator();
  }
}

///////////////////////////////// Change AP //////////////////////////////////////
void AP()
{
  WiFi.disconnect();
  Serial.println("Change AP......");
  WiFi.softAP(ssidAP, passAP);
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAPIP();
  server.setRSACert(new BearSSL::X509List(serverCert), new BearSSL::PrivateKey(serverKey));
  server.on("/ssiddevice", ssiddevice);
  server.on("/adddevice", modeAP);
  server.begin();
  for (int a = 0; a < 10; a++) {
    indicator();
  }
}

void adddevice()
{
  Serial.print(ssid);
  Serial.print("  ");
  Serial.println(pass);
  StaticJsonDocument<200> json;
  deserializeJson(json, server.arg("plain"));
  user_id = json["user_id"];
  device_id = json["device_id"];
  device_name = json["device_name"];
  ssid = json["ssid"];
  pass = json["pass"];

  Serial.print("user_id:  ");
  Serial.println(user_id);
  Serial.print("device_id:  ");
  Serial.println(device_id);
  Serial.print("device_name:  ");
  Serial.println(device_name);
  Serial.print("ssid:  ");
  Serial.println(ssid);
  Serial.print("pass:  ");
  Serial.println(pass);
  server.send(200, "json/plain", "Success");
  delay(500);
  changeAP(ssid, pass);
}

///////////////// Mode Reset /////////////////////
char changeAP(const char* ssid, const char* pass) {
  Serial.println("Reset Device...");
  WiFi.begin(ssid, pass);
  int a;
  while (a < 30)
  {
    if (WiFi.status() == WL_CONNECTED) {
      delay(500);
      Serial.println("Connected to Wifi.....");
      Serial.println(WiFi.localIP());
      Serial.println();
      return true;
    }
    delay(500);
    Serial.print(WiFi.status());
    a++;
  }
  Serial.print("IP address:  ");
  Serial.println(WiFi.localIP());  //Print the local IP
  ssidnew = ssid;
  passnew = pass;
  namenew = device_name;
  usernew = user_id;
  devicenew = device_id;
  url = "https://api.vasdev.co.id:8066/sitamoto/cdn/firmware/hedeg-v0.0.0.bin";
  new_versi = "hedeg-v0.0.0";
  flag = "0";
  updates = "0";
  if (ssidnew.length() > 0 && passnew.length() > 0) {
    Serial.println("Clearing EEPROM.....");
    delete_eeprom(0, 512);
    Serial.print(ssidnew);
    Serial.print(passnew);
    Serial.print(".....................");
    Serial.println("Writing EEPROM SSID:");
    write_eeprom(ssidnew, 0);
    Serial.println("Writing EEPROM Password:");
    write_eeprom(passnew, 32);
    Serial.println("Writing EEPROM Device Name:");
    write_eeprom(namenew, 64);
    Serial.println("Writing EEPROM User ID:");
    write_eeprom(usernew, 90);
    Serial.println("Writing EEPROM Device ID:");
    write_eeprom(devicenew, 96);
    Serial.println("Writing EEPROM firm_url:");
    write_eeprom(url, 128);
    Serial.println("Writing EEPROM new_version:");
    write_eeprom(new_versi, 224);
    Serial.println("Writing EEPROM flag_update:");
    write_eeprom(flag, 250);
    Serial.println("Writing EEPROM updates:");
    write_eeprom(updates, 253);
    EEPROM.commit();
  }
  Serial.println(ssid);
  Serial.println("  ");
  Serial.println(pass);
  Serial.println("  ");
  Serial.println(device_id);
  Serial.println("  ");
  Serial.println(user_id);
  Serial.println("  ");
  Serial.println(device_name);
  Serial.println("  ");
  Serial.println(url);
  Serial.println("  ");
  Serial.println(new_versi);
  Serial.println("  ");
  Serial.println(flag);
  Serial.println("  ");
  Serial.println(updates);
  Serial.println("EEPROM Saved......");
  ESP.restart();
}

/////////////////////// Change AP Mode ////////////////////////
void modeAP()
{
  Serial.print(ssid);
  Serial.print("  ");
  Serial.println(pass);
  StaticJsonDocument<200> json;
  deserializeJson(json, server.arg("plain"));
  device_name = json["device_name"];
  ssid = json["ssid"];
  pass = json["pass"];

  Serial.print("device_name:  ");
  Serial.println(device_name);
  Serial.print("ssid:  ");
  Serial.println(ssid);
  Serial.print("pass:  ");
  Serial.println(pass);
  server.send(200, "json/plain", "Success");
  delay(500);
  gantiAP(ssid, pass);
}

char gantiAP(const char* ssid, const char* pass) {
  Serial.println("Change AP");
  WiFi.begin(ssid, pass);
  int a;
  while (a < 30)
  {
    if (WiFi.status() == WL_CONNECTED) {
      delay(500);
      Serial.println("Connected to Wifi.....");
      Serial.println(WiFi.localIP());
      Serial.println();
      return true;
    }
    delay(500);
    Serial.print(WiFi.status());
    a++;
  }
  Serial.print("IP address:  ");
  Serial.println(WiFi.localIP());  //Print the local IP
  ssidnew = ssid;
  passnew = pass;
  namenew = device_name;
  if (ssidnew.length() > 0 && passnew.length() > 0) {
    Serial.println("Clearing EEPROM.....");
    delete_eeprom(0, 90);
    Serial.print(ssidnew);
    Serial.print(passnew);
    Serial.print(".....................");
    Serial.println("Writing EEPROM SSID:");
    write_eeprom(ssidnew, 0);
    Serial.println("Writing EEPROM Password:");
    write_eeprom(passnew, 32);
    Serial.println("Writing EEPROM Device Name:");
    write_eeprom(namenew, 64);
    EEPROM.commit();
  }
  Serial.println(ssid);
  Serial.println("  ");
  Serial.println(pass);
  Serial.println("  ");
  Serial.println(device_name);
  Serial.println("EEPROM Saved......");
  ESP.restart();
}

void ssiddevice()
{
  StaticJsonDocument<200> json;
  deserializeJson(json, server.arg("plain"));
  json["ssid"] = ssidAP;

  Serial.print("ssidAP:  ");
  Serial.println(ssidAP);
  server.send(200, "json/plain", ssidAP);
}
