// TODO: Rename this file to config.h, before compilation. This file only describes the parameters.

#define WIFI_SSID "YOUR_WIFI_NAME"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#define AWS_IOT_ENDPOINT "xxxxxxxxxxxxx-ats.iot.eu-north-1.amazonaws.com"
#define THINGNAME "incubator-test-001"
#define AWS_IOT_PUBLISH_TOPIC   "incubator/data"
#define AWS_IOT_SUBSCRIBE_TOPIC "incubator/" THINGNAME "/cmd"
#define AWS_IOT_STATUS_TOPIC    "incubator/" THINGNAME "/status"

static const char AWS_CERT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
...
-----END CERTIFICATE-----
)EOF";

static const char AWS_CERT_CRT[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
...
-----END CERTIFICATE-----
)KEY";

static const char AWS_CERT_PRIVATE[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
...
-----END RSA PRIVATE KEY-----
)KEY";