#include <pgmspace.h>
#define SECRET
#define THINGNAME "cortina_esp32_practica_iot"
const char WIFI_SSID[] = "Wokwi-GUEST";
const char WIFI_PASSWORD[] = "";
const char AWS_IOT_ENDPOINT[] = "a2ahuk7f9b7m4r-ats.iot.us-east-1.amazonaws.com";
const char AWS_IOT_TOPIC_PUB[] = "cortina/pub";
const char AWS_IOT_TOPIC_SUB[] = "cortina/sub";


// Amazon Root CA 1
static const char AWS_CERT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";
 
// Device Certificate                                               //change this
static const char AWS_CERT_CRT[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDWjCCAkKgAwIBAgIVAMhMScAc2Zzqs373DivT0AM4/DUVMA0GCSqGSIb3DQEB
CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t
IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yNDA3MTMwNTM4
MjBaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh
dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQC840kCHY+Ow2m7OHcy
nIkpLdSwd+j0A3myYJ6Z9yYhs1uOJDgG8fM6+APICe0/hn+MxYCNQV1HBqsiF8Fu
s1Noe509g5zJ9VKlRJZx7mrjeX6vwbymzbrVoQtuDEtMPz4UPYFiGHmbuRYQrMdt
hsUiqWzfIrIbdxkHzu5yKleJxv3g9GgPEpttazqQgeQnEVRudTnZDYDju6PBIGWV
A5bRDHc126a0WRpMKGJwFdDBjK0yXzUvseKPYkM//JGVycO3ivWKD67l+mdO9F8V
11/3WVE8gOZLr1LFEfOa7PYz5wuTvsyO4HJyde1fA65pKOnv0H+aNPTOWSKCQkRs
J7SZAgMBAAGjYDBeMB8GA1UdIwQYMBaAFCg6rs65Pbrome1P21tdlfDUy2LCMB0G
A1UdDgQWBBT28LYe4IdgQ51pXo7nsmKMEK/1VTAMBgNVHRMBAf8EAjAAMA4GA1Ud
DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAeeV1vCJf3xfQqd5jivKqpnRT
HM7NuhyAD5WfQcpU2zJgleA2ntazdGyFE977I2OXgKWan5Z3e4j9x0mbRnZ//W6o
a/+N6x6qeg7L4vlXIDrrr0cHOJPCZWx2vlxNP9nwdzZmzwbk2lJANMFrCLD40l5R
kjnar38M0yCngvs33mEOCCyCXcGZ8YRQRE0gK1a6JW2cdyVnquE7t74KGjnrBb35
whQNk9MW51Z4S7ptD1snpD2GNbnmGNa+WKvXz3xatMIejL3QHAs1hNwQvgv7X3Ld
7tyHJ7udAO2MLAPpys2E/ok6p7zsB8hJlPBWp5YkNUxaS27UIKuIB7qOArH1xA==
-----END CERTIFICATE-----
)KEY";
 
// Device Private Key                                               //change this
static const char AWS_CERT_PRIVATE[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEpQIBAAKCAQEAvONJAh2PjsNpuzh3MpyJKS3UsHfo9AN5smCemfcmIbNbjiQ4
BvHzOvgDyAntP4Z/jMWAjUFdRwarIhfBbrNTaHudPYOcyfVSpUSWce5q43l+r8G8
ps261aELbgxLTD8+FD2BYhh5m7kWEKzHbYbFIqls3yKyG3cZB87ucipXicb94PRo
DxKbbWs6kIHkJxFUbnU52Q2A47ujwSBllQOW0Qx3NdumtFkaTChicBXQwYytMl81
L7Hij2JDP/yRlcnDt4r1ig+u5fpnTvRfFddf91lRPIDmS69SxRHzmuz2M+cLk77M
juBycnXtXwOuaSjp79B/mjT0zlkigkJEbCe0mQIDAQABAoIBAQCs0GWjgdguDatW
lzq3G3CCNGQ7b/m6BtP4111EEFLa0nYZfhmkgQjAEtmi0zI0xKE98hTDJ+JncTpx
XrqixX6yQSaDeT6hp8mNW0spwbAZnrbBpscq9V7SxRzn+meJwvLRIdSWIz5fst71
Mp7jHkKOi+QH4n6Bkl1cP375BLRCYDGiJNFu02fmipfyUV1bq2+h2TRbhYcnIpBa
ylT81ryld2095txo6ywWx8tAVz8FpaWqRpuRDZXSA4xsdEX4RHZuDZ8l+7GVjjpv
/A4Y7QKZmj0OYc0VzdQc7CMDff9Sld1opGD5mJP6o5pJz1QESzmVNNGvHnvjVVEj
ywro1rNhAoGBAO2UPseTC4bR89VjSa//lL89QWj9LBu9kYBlJVFIVm8SgMTzitmv
dLVdJaPGcojhimINb05JnYYuq2ZVWrtWyswKULOQk8171/duDOYSJ4yZVd5vESTA
fGi19KUc+yvZAjermmG5sQCfztl9A2Yfh4uMyajp/KtAg3uelCwZDT6fAoGBAMuI
jr3yFaur4oPlavYJdcWI4KdFT/Ju12oKIZu7ZQLfNzd6sJlOxgrSTZJ2zYjaa+yZ
u5WBll0hhQkuzgZIydN+nxB3ir2C3UVcn8DKDV06Kb5AK/m7cvLky0OQSDGBBioF
Tc6/fVS1R7oY840qPn7vjT9k56qD9VRlzAV0MZnHAoGBAIwH6MjxHsYkUbfggbo+
xonBrSAYuTrQjUZEV66FGIz3NvbfJWQRSpTrhhf2C+QqK8qHaKeYNES2lUgH3ya0
jWueWNq9yKBA8pZgQzIIWU9BwZ8Nj+k9mxaMyNnZLyI1El2Fw8T+EhnwF5eRW8OB
qf7SyLnK0KaPq+/dFDC5w1i5AoGBALk2nV6wi5PbKJgKD1mxpiCJvJW691OQFisC
6S6ESxlf8yljVo1MJ9/1HG4p+kONMCTwiMk2UZSSSjPfCdCiMDdrxGgBwJlQGvdh
oGaZ96tcoFvmnZ32PFZYabgj9e5GkUtv5Cw+CVkqVgk9Z3SQeqqcZ5bGxiJfTsLs
l8pmwDRFAoGAJYjdeUS4EN48+ZXxrIo7uco7X4G4zR0HCAq2ezM/QI5sMWV/4VHw
xpui7kvDdhDth36/cGozmeUDjZ6v36hV9BLDsNjqYoHGD/135N6UNq7Apw/eLONT
3OqMR+PsW4Xgf6ax/ZAfwI85QXPT9/1faYf8BnZKN6gEPzTraIRlyvY=
-----END RSA PRIVATE KEY-----
)KEY";