void backup_sensor() {
  delete_eeprom(256, 288);
  write_eeprom(sensorIn, 256);
  EEPROM.commit();
}

//void commandbackup(int a) {
//  delete_eeprom(288, 320);
//  write_eeprom(backup_panel, 288+a);
//  EEPROM.commit();
//}

void commandbackup(int data[11], int number, int index)
{
  for (int i = index; i < data[11]; ++i)
  {
    EEPROM.write(number + i, data[11]);
    Serial.print("Wrote: ");
    Serial.print(data[i]);
    Serial.println();
  }
}

//void write_eeprom(String data, int number)
//{
//  for (int i = 0; i < data.length(); ++i)
//  {
//    EEPROM.write(number + i, data[i]);
//    Serial.print("Wrote: ");
//    Serial.print(data[i]);
//    Serial.println();
//  }
//}

void save_array(int address, int numbers[])
{
  int addressIndex = address;
  for (int i = 1; i <= 10; i++)
  {
    EEPROM.write(addressIndex, numbers[i]);
    EEPROM.write(addressIndex + 1, numbers[i]);
    addressIndex += 2;
    EEPROM.commit();
  }
}
