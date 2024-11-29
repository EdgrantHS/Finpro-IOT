/*
Modifikasi kode dari contoh painlessMesh untuk leaf node, kode awal dapat dilihat pada link berikut:
https://github.com/gmag11/painlessMesh/blob/master/examples/basic/basic.ino

- Program kedua berperan sebagai root node dimana dia bertugas untuk menerima message dari leaf node. Jika message yang diterima adalah “ON”, maka LED built-in akan menyala. Jika message yang diterima adalah “OFF”, maka LED built-in akan mati.
*/
#include "painlessMesh.h"

#define   MESH_PREFIX     "AmamyaKokoroPrefix"
#define   MESH_PASSWORD   "AmamyaKokoroPass"
#define   MESH_PORT       5555

#define   LED_BUILTIN     2

Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;

// User stub
void sendMessage(String command);

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, []() { sendMessage(""); } );

void sendMessage(String command) {
  String msg = "Hi from leaf node";
    msg += mesh.getNodeId();
    msg += " ";
    msg += command;
  mesh.sendBroadcast( msg );
  taskSendMessage.setInterval( random( TASK_SECOND * 1, TASK_SECOND * 5 ));
}

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
    if (msg == "ON") {
        digitalWrite(LED_BUILTIN, HIGH);
    } else if (msg == "OFF") {
        digitalWrite(LED_BUILTIN, LOW);
    }
}

void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}

void setup() {
  Serial.begin(115200);

//mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();

    pinMode(LED_BUILTIN, OUTPUT);   
}

void loop() {
  // it will run the user scheduler as well
  mesh.update();

}
