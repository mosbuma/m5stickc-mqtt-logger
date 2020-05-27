const long nWifiAttemptsPerSSID = 2;  // try to connect to each SSID n times
const long nWifiConnectWaitSecs = 5;  // wait X seconds before ending connection attempt

const int nSSIDs = 2;
const char* ssids[] = {"ssid1", "ssid2"};
const char* passwords[] = {"passwd1", "passwd2"};

const char* mqttServer = "broker.hivemq.com"; 
const int mqttPort = 1883;

const char* sensorId = "bumos-1";

// this channel is used to send commands to the sensor
// chxx -> set channel to xx
// qryyyxxxxxxxxx -> show QR code with content xxxxxxxxx for yyy seconds
const char* listenToTopic = "bumos/%s"; // %s -> replaced with sensorId

// temperature measurements are written to this topic
const char* publishToTopic = "bumos/temp/%s"; // %s -> replaced with channel name
// const char* publishToTopic = "smartys-%s/temp" 

// Button B is used to switch between channels
// for each channel, define a name, background color and text color
const int nchannels = 6;
const char* channels[] = {"yellow", "red", "white", "orange", "blue", "green"}; 
const long backgroundcolors[] = {TFT_YELLOW, TFT_RED, TFT_WHITE, TFT_ORANGE, TFT_BLUE, TFT_GREEN};
const long textcolors[] = {TFT_BLACK, TFT_BLACK, TFT_BLACK, TFT_BLACK, TFT_WHITE, TFT_BLACK};
