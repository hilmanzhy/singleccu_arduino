
void event(uint8_t * payload, int value)
{
  String text = ((const char *)&payload[value]);
  if (text.substring(2, 9) == "command") {
    int pos = text.indexOf('{');
    String json = text.substring(pos, text.length() - 1);
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, json);
    JsonObject root = doc.as<JsonObject>();
    String mode = root["mode"];
    String pin = root["pin"];
    String switc = root["switch"];
    saklar = switc;
    int value = switc.toInt();
    int pins = pin.toInt();

    Serial1.print(mode);
    Serial1.print(",");
    Serial1.print(relay[pins]);
    Serial1.print(",");
    Serial1.println(value);
  }
}

void commandpanel(int pin, int trigger, String modes) {
  time_t utc = now();
  time_t local = myTZ.toLocal(utc, &tcr);
  Serial.println();
  printDateTime(local);
  DynamicJsonDocument doc(1024);
  JsonArray array = doc.to<JsonArray>();
  array.add("commandpanel");
  JsonObject object1 = array.createNestedObject();
  object1["device_id"] = edevice;
  object1["user_id"] = euser;
  object1["mode"] = String(modes);
  object1["mini"] = boolean(true);
  object1["switch"] = String(trigger);
  object1["date"] = String(date);
  object1["pin"] = String(pin);

  String body;
  serializeJsonPretty(doc, body);
  socketIO.sendEVENT(body);
  Serial.println(body);
  backup_panel = trigger;
}

void handshake()
{
  DynamicJsonDocument doc(1024);
  JsonArray array = doc.to<JsonArray>();
  array.add("handshake");
  JsonObject object1 = array.createNestedObject();
  object1["device_id"] = edevice;
  object1["user_id"] = euser;
  object1["device_ip"] = WiFi.localIP().toString();
  object1["device_name"] = ename;
  object1["device_type"] = "1";
  object1["pin"] = "10";
  object1["mac_address"] = WiFi.macAddress();
  object1["firmware_version"] = e_new;
  object1["flag_update"] = e_flag;

  String body;
  serializeJsonPretty(doc, body);
  socketIO.sendEVENT(body);
  serial.println(body);
  flag_update = "0";
  flag = flag_update;
  delete_eeprom(250, 253);
  write_eeprom(flag, 250);
  EEPROM.commit();
}

void sensordata(int pin, int trigger, float amps, float watt)
{
  if (trigger == 1 && watt > 3.0) {
    sensor_status = 1;
  }
  else {
    sensor_status = 0;
  }

  time_t utc = now();
  time_t local = myTZ.toLocal(utc, &tcr);
  Serial.println();
  printDateTime(local);
  DynamicJsonDocument doc(1024);
  JsonArray array = doc.to<JsonArray>();
  array.add("sensordata");
  JsonObject object1 = array.createNestedObject();
  object1["device_id"] = edevice;
  object1["user_id"] = euser;
  object1["device_ip"] = WiFi.localIP().toString();
  object1["device_type"] = "0";
  object1["pin"] = String(pin);
  object1["ampere"] = String(amps);
  object1["wattage"] = String(watt);
  object1["switch"] = String(trigger);
  object1["sensor_status"] = String(sensor_status);
  object1["date"] = String(date);

  String body;
  serializeJsonPretty(doc, body);
  socketIO.sendEVENT(body);
  serial.println(body);
}

void upgrade_firmware(uint8_t * payload, int value)
{
  String text = ((const char *)&payload[value]);
  if (text.substring(2, 18) == "upgrade_firmware") {
    int pos = text.indexOf('{');
    String json = text.substring(pos, text.length() - 1);

    DynamicJsonDocument doc(1024);
    deserializeJson(doc, json);
    JsonObject object = doc.as<JsonObject>();
    firm_url = object["firmware_url"];
    new_version = object["firmware_version"];
    flag_update = "1";
    true_updates = "1";
    Serial.print("Url: ");
    Serial.println(firm_url);
    Serial.print("version: ");
    Serial.println(new_version);
    url = firm_url;
    new_versi = new_version;
    flag = flag_update;
    updates = true_updates;
    delete_eeprom(128, 256);
    Serial.println("Writing EEPROM firm_url:");
    write_eeprom(url, 128);
    Serial.println("Writing EEPROM new_version:");
    write_eeprom(new_versi, 224);
    Serial.println("Writing EEPROM flag_update:");
    write_eeprom(flag, 250);
    Serial.println("Writing EEPROM updates:");
    write_eeprom(updates, 253);
    EEPROM.commit();
    ESP.restart();
  }
}

void save_url()
{
  delete_eeprom(253, 256);
  updates = "0";
  Serial.println("Writing EEPROM updates Upgrade....:");
  write_eeprom(updates, 253);
  EEPROM.commit();

  WiFi.begin(esid, epass);
  while (1) {
    if (WiFi.status() != WL_CONNECTED) {
      delay(300);
      Serial.print(".");
    }
    if (WiFi.status() == WL_CONNECTED) {
      BearSSL::WiFiClientSecure client;
      client.setInsecure();
      t_httpUpdate_return ret = ESPhttpUpdate.update(client, e_url.c_str());
      switch (ret) {
        case HTTP_UPDATE_FAILED:
          Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
          break;
        case HTTP_UPDATE_NO_UPDATES:
          Serial.println("HTTP_UPDATE_NO_UPDATES");
          break;
        case HTTP_UPDATE_OK:
          Serial.println("HTTP_UPDATE_OK");
          break;
      }
    }
  }
}

void resetdevice(uint8_t * payload, int value)
{
  String text = ((const char *)&payload[value]);
  if (text.substring(2, 7) == "reset") {
    int pos = text.indexOf('{');
    String json = text.substring(pos, text.length() - 1);
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, json);
    JsonObject root = doc.as<JsonObject>();
    String Reset = root[String("reset")];
    Serial.print("Reset:  ");
    serial.println(Reset);
    if (Reset == "1")
    {
      reset(256);
    }
  }
}
