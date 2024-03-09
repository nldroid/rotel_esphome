#include "esphome.h"

class UartReadLineSensor : public Component, public UARTDevice, public TextSensor {
 public:
  TextSensor *volume = new TextSensor();
  TextSensor *input = new TextSensor();
  TextSensor *raw_data = new TextSensor();
  UartReadLineSensor(UARTComponent *parent) : UARTDevice(parent) {}
  

  void setup() override {
    // Nothing here
  }

  int readline(int readch, char *buffer, int len)
  {
    static int pos = 0;
    int rpos;

    if (readch > 0) {
      switch (readch) {
        case '\n': // Ignore new-lines
          break;
        case '$': // Return on $ sign
          rpos = pos;
          pos = 0;  // Reset position index ready for next time
          return rpos;
        default:
          if (pos < len-1) {
            buffer[pos++] = readch;
            buffer[pos] = 0;
          }
      }
    }
    // No end of line has been found, so return -1.
    return -1;
  }

  void loop() override {
    const int max_line_length = 80;
    static char buffer[max_line_length];
    char *property;
    char *value;
    while (available()) {
      if(readline(read(), buffer, max_line_length) > 0) {
        raw_data->publish_state(buffer);
        property = strtok(buffer, "=");
        value = strtok(NULL, "=");
        if (strcmp(property,"volume") == 0) {
          volume->publish_state(value);
        } else if (strcmp(property,"source") == 0) {
          input->publish_state(value);
        }
      }
    }
  }
};