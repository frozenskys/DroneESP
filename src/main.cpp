#include <heltec.h>
#include<IoAbstraction.h>

#include <images.h>
#include <MSP.h>

const int spinwheelClickPin = 26;
const int encoderAPin = 14;
const int encoderBPin = 27;
const int maximumEncoderValue = 128;

MSP msp;
int16_t encValue;

void logo(){
	Heltec.display -> clear();
	Heltec.display -> drawXbm(0,0,logo_width,logo_height,(const unsigned char *)logo_bits);
	Heltec.display -> display();
}

void onSpinwheelClicked(pinid_t pin, bool heldDown) {
  Serial.print("Button pressed ");
  Serial.println(heldDown ? "Held" : "Pressed");
}

void onRepeatButtonClicked(pinid_t pin, bool heldDown) {
  Serial.println("Repeat button pressed");
}

void onEncoderChange(int newValue) {
  Serial.print("Encoder change ");
  Serial.println(newValue);
  encValue = newValue;
}

void setup()
{
	Heltec.begin(true /*DisplayEnable Enable*/, false /*LoRa Enable*/, false /*Serial Enable*/);
	Serial.begin(115200);
	Serial.println("Setup Started");

	logo();
	delay(3000);
	
	switches.initialise(ioUsingArduino(), true);
	switches.addSwitch(spinwheelClickPin, onSpinwheelClicked);
	setupRotaryEncoderWithInterrupt(encoderAPin, encoderBPin, onEncoderChange);
	switches.changeEncoderPrecision(maximumEncoderValue, 100);

	Serial.println("Encoder Started...");
	Heltec.display->clear();
	Heltec.display -> drawString(0, 0, "Connecting To FC...");
	Heltec.display -> display();
	Serial2.begin(115200, SERIAL_8N1, 18, 23);
	msp.begin(Serial2);
	delay(1000);
	Serial.println("MSP initialised...");
	Heltec.display -> drawString(0, 9, "Fetching Data...");
	Heltec.display -> display();
	delay(1500);
	Heltec.display->clear();

	Serial.println("Getting Variant...");
	msp_fc_variant_t variant;
	if (msp.request(MSP_FC_VARIANT, &variant, sizeof(variant))) {
		Heltec.display -> drawString(2, 0, variant.flightControlIdentifier);
	}

	Serial.println("Getting Board...");
	msp_board_info_t board;
	if (msp.request(MSP_BOARD_INFO, &board, sizeof(board))) {
		Heltec.display -> drawString(35, 0, board.boardIdentifier);
	}

	Serial.println("Getting Version...");
 	msp_fc_version_t version;
	if (msp.request(MSP_FC_VERSION, &version, sizeof(version))) {
		char data[20];
		sprintf(data, "Version %d.%d.%d", version.versionMajor, version.versionMinor, version.versionPatchLevel);
		Heltec.display -> drawString(2, 18, data);
	}

	Serial.println("Getting Build...");
	msp_build_info_t build;
	if (msp.request(MSP_BUILD_INFO, &build, sizeof(build))) {
		Heltec.display -> drawString(2, 27, build.buildDate);
	}
	
	Heltec.display -> display();
	Serial.println("Setup Finished");
}

void loop()
{
	taskManager.runLoop();
	char data[20];
	sprintf(data, "Value %d", encValue);
	Heltec.display -> setColor(BLACK);
	Heltec.display -> fillRect(0,37,128,9);
	Heltec.display -> setColor(WHITE);
	Heltec.display -> drawString(2, 36, data);
	Heltec.display -> display();
}
