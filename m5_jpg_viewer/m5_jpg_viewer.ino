#include <M5Stack.h>

#define MAX_JPG_FILES 30
char* jpg_path[MAX_JPG_FILES];
uint8_t jpg_files = 0;
uint8_t current_jpg_file = 0;

void searchDirectory(File dir) {
  while (true) {
    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    if (entry.isDirectory()) {
      searchDirectory(entry);
    } else {
      String file_name = entry.name();
      if (file_name.indexOf(".jpg") > -1) {
        jpg_path[jpg_files] = strdup(entry.name());
        M5.Lcd.printf("found %s \n", jpg_path[jpg_files]);
        if (jpg_files >= MAX_JPG_FILES) {
          // max files
          break;
        }
        jpg_files++;
      }
    }
    entry.close();
  }
}

void initJpgList() {
  if (!SD.begin(4)) {
    M5.Lcd.printf("SD initialization failed!\n");
    M5.Lcd.printf("please insert sd card and reboot\n");
    while (1);
  }
  M5.Lcd.printf("SD initialization done.\n");
  File root = SD.open("/");
  searchDirectory(root);
  M5.Lcd.printf("Searced jpgs : %d\n", jpg_files);  
}

enum updateJpgState {
  UPDATE_TO_TOP,
  UPDATE_TO_PREV,
  UPDATE_TO_NEXT,
  MAX = UPDATE_TO_NEXT
};

void updateJpg(uint8_t state) {
  switch(state) {
    case UPDATE_TO_NEXT:
      current_jpg_file = (current_jpg_file + 1) % jpg_files;
      break;
    case UPDATE_TO_PREV:
      current_jpg_file =
        current_jpg_file == 0 ?
          jpg_files - 1 :
          (current_jpg_file - 1) % jpg_files;
      break;
    case UPDATE_TO_TOP:
    default:
      current_jpg_file = 0;    
  }
  M5.Lcd.drawJpgFile(SD, jpg_path[current_jpg_file]);  
}

void setup(void) {
  M5.begin();
  Wire.begin();
  M5.Lcd.setBrightness(100);

  initJpgList();

}

void loop() {
  M5.update();
  if(M5.BtnA.wasPressed()) {
    updateJpg(UPDATE_TO_PREV);
  } else if(M5.BtnB.wasPressed()) {
    updateJpg(UPDATE_TO_TOP);
  } else if(M5.BtnC.wasPressed()){
    updateJpg(UPDATE_TO_NEXT);
  }
  vTaskDelay(100 / portTICK_PERIOD_MS);
}
