#include "networkIS.h"

int request::networkCheck() {

    //This use Leds as visual indicator for the network status.
    
    //In case of internal shield issue
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
        Serial.println("Ethernet shield was not found.");
        digitalWrite(LINK_STAT_LED, HIGH);
        digitalWrite(ONBOARD_LED, LOW);
        
        return -1;
    }

    //In case of network shutdown
    else if (Ethernet.linkStatus() == LinkOFF) {
        Serial.println("Ethernet cable is not connected.");
        digitalWrite(LINK_STAT_LED, HIGH);
        digitalWrite(ONBOARD_LED, LOW);
        return -2;
    } else {
        digitalWrite(LINK_STAT_LED, LOW);
        digitalWrite(ONBOARD_LED, HIGH);
        return 0;

    }
}

void request::networkSetup() {

    //The CS pin of the Ethernet module is GPIO17
    Ethernet.init(17);
    delay(1000);
    
    delay(3000);//TODO REMOVE AFTER DEBUG

    Serial.println("Using the Default MAC");

    Ethernet.begin(Default_mac, own_ip);
    delay(1000);

    //At this point the IS is connected to the network
    core_debug((char *)"set up\n");
    // IPAddress gateway(192, 168, 10, 1); //Only for local switch network
    // Ethernet.setGatewayIP(gateway);
    Serial.print("My IP address: ");
    Serial.println(Ethernet.localIP());
    Serial.print("My gateway address: ");
    Serial.println(Ethernet.gatewayIP());
    Serial.print("My DNS address: ");
    Serial.println(Ethernet.dnsServerIP());
    

    server.begin();

}

void request::try_connect_to_server() {
    Serial.println(client.connect(server_ip,80));
    if (client.connect(server_ip,80))
    {
        Serial.println("connected to server via port 80");
    }
    else {
        Serial.println("connexion to server via port 80 failed :/");
    }
    
}

void request::sendTime() {

    String Type = "GET";
    String Http_Level = "HTTP/1.1";

    //WARNING il faut une requete avec reponse directe tres courte sinon cela peut entraver la partie Timer
    // et plus de mesure ne peuvent être initiées!!!! LONG BUG
    //   ISensor_alertRequest.sh //original used script WARNING OLD WAY too slow
    String time = (String) millis();
    String URL_Body_sh = "/cgi-bin/get_mydata.sh?time=" + time ;

    String FullReqURL = Type + " " + URL_Body_sh + " " + Http_Level;

    if (client.connect(server_ip, 80)) {
        Serial.println("connected");
        client.println(FullReqURL);
        client.print("Host: ");
        client.println(server_ip);
        client.println("Connection: close");
        client.println();
        client.flush();
        read_data_back();
    } 
    else {
        Serial.println("connection failed");
    }
}


void request::read_data_back() {
    Serial.println("read_data starting ...");

    int count_lines = 0;
    int json_start = 0;
    String date = "";
    String json_str = "";
    int timeout_duree = 0;

    while (!client.available() && timeout_duree < 1000) {
        Serial.print(".");
        timeout_duree++;
        delay(1);
    }
    if (!client.available()) {
        Serial.println("no response from server under 1s , Too slow!!");
    }
    else {
        while (client.available()) {
            
            char c = client.read();

            if (c == '\n')
            {
                count_lines ++;
            }
            
            if (count_lines == DATE_RESPONSE_LINE ){ // la date correspond a ligne 5 pour lighttd et ligne 2 pour apache
                date = date + c;
            }
            
            if (c == '{') {
                json_start ++;
            }

            if (json_start != 0) {
                json_str = json_str + c;
            }

            if (c == '}') {
                json_start --;
            }
        }
        Serial.println(date);
        Serial.println(json_str);
        Serial.println();
    }
    Serial.flush();
    rdisconnect();

}

void request::rdisconnect() {
    if (!client.connected()) {
        // read_data = false;
        Serial.println("disconnecting.");
        client.stop();

  }
}

void request::get_request() {
    handleRoot();
    user_cmd.stop();
}

void request::process_command(DynamicJsonDocument jsonDoc) {
    if (jsonDoc["M"] == 1) //regular measure
    {
        write_to_other_core(jsonDoc["M"]);
        write_to_other_core(jsonDoc["P"]);
        write_to_other_core(jsonDoc["C"]);
        write_to_other_core(jsonDoc["G"]);
        delay(100);
        get_info_from_other_core();
        
    }
    if (jsonDoc["M"] == 9) // unitary measure
    {
        write_to_other_core(jsonDoc["M"]);
        write_to_other_core(jsonDoc["C"]);
        delay(100);
        get_info_from_other_core();
    }
    
    if (jsonDoc["M"] == 11) //clear meausre
    {
        write_to_other_core(jsonDoc["M"]);
        write_to_other_core(jsonDoc["I"]);
        write_to_other_core(jsonDoc["D"]);
        delay(100);
        get_info_from_other_core();
    }
    if (jsonDoc["M"] == 0)
    {
        send_info();
    }
    if (jsonDoc["M"] == 2)
    {
        write_to_other_core(jsonDoc["M"]);
        write_to_other_core(jsonDoc["P"]);
        write_to_other_core(jsonDoc["C"]);
        write_to_other_core(jsonDoc["m"]);
        write_to_other_core(jsonDoc["MM"]);
        write_to_other_core(jsonDoc["G"]);
        delay(100);
        get_info_from_other_core();
    }
    

}

void request::handleRoot() {
    user_cmd = server.available();
    if (user_cmd) {
        // Wait for a request to be available
        Serial.println("got it!");
        if (user_cmd.connected()) {
        // Read the GET request
        String request = "";
        while (user_cmd.connected()) {
            if (user_cmd.available()) {
            char c = user_cmd.read();
            request += c;
            if (c == '\n') {
                if (request.startsWith("GET")) {
                
                // Parse the query parameters
                int paramStart = request.indexOf('?');
                
                if (paramStart != -1) {
                    int paramEnd = request.indexOf(' ', paramStart);
                    if (paramEnd != -1) {
                    String query = request.substring(paramStart + 1, paramEnd);
                    
                    // Create a JSON object
                    DynamicJsonDocument jsonDoc(1024);
                    
                    // Split the query parameters
                    int paramCount = 0;
                    while (query.length() > 0) {
                        int separatorIndex = query.indexOf('&');
                        String param;
                        if (separatorIndex != -1) {
                        param = query.substring(0, separatorIndex);
                        query.remove(0, separatorIndex + 1);
                        } else {
                        param = query;
                        query = "";
                        }
                        // Extract parameter name and value
                        int equalsIndex = param.indexOf('=');
                        if (equalsIndex != -1) {
                        String paramName = param.substring(0, equalsIndex);
                        String paramValue = param.substring(equalsIndex + 1);
                        jsonDoc[paramName] = paramValue.toInt();

                        core_print_uint32(jsonDoc[paramName]);
                        paramCount++;
                        }
                    }

                    if (paramCount > 0) {
                        // Convert the JSON object to a string
                        String jsonString;
                        
                        serializeJson(jsonDoc, jsonString);

                        process_command(jsonDoc);
                        
                        // Set the content type of the response to application/json
                        // user_cmd.println("HTTP/1.1 200 OK");
                        // user_cmd.println("Content-Type: application/json");
                        // user_cmd.println();
                        // // Send the JSON response
                        // user_cmd.println(jsonString);
                    }
                    }
                }
                }
                break;
            }
            }
        }
        user_cmd.stop();
        }
    }
}

void request::get_info_from_other_core() {
    uint32_t mode;
    if (read_from_other_core(&mode)) {
        if (mode == 8)
        {
            core_debug((char *)"got mode 8\n");
            read_measure_setup_from_core(mode);
                
        }
        if (mode == 1)
        {
            core_debug((char *)"got mode 1\n");
            read_measure_value_from_core(mode);
        }
        if (mode == 11)
        {
            core_debug((char *) "got mode 11\n");
            read_clear_measure();
        }    
    }  
}
int request::read_clear_measure() {
    uint32_t timer , flag;
    int timeout = 10;
    while (timeout && !read_from_other_core(&timer))
    {
        timeout--;
    }

    if (!timeout)
    {
        Serial.println("timer not recieved from other core");
        return -1;
    }

    while (timeout && !read_from_other_core(&flag))
    {
        timeout--;
    }

    if (!timeout)
    {
        Serial.println("flag not recieved from other core");
        return -1;
    }

    myBehaviors.shutdown_behavior(timer,int mode);
    if (networkCheck() == 0)
    {
        send_response_clear_measure(timer);
    }
    return 1;
}

void request::send_response_clear_measure(int timer) {
    DynamicJsonDocument jsonDoc(1024);
    jsonDoc["status"] = "Timer "+(String)timer+"stopped";
    send_post_response(jsonDoc);
}

int request::read_measure_setup_from_core() {
    uint32_t sensorID , period , timer , flag , push_to_graphana;
    int timeout = 10;
    while (timeout && !read_from_other_core(&sensorID))
    {
        timeout--;
    }

    if (!timeout)
    {
        Serial.println("sensorID not recieved from other core");
        return -1;
    }

    while (timeout && !read_from_other_core(&period))
    {
        timeout--;
    }

    if (!timeout)
    {
        Serial.println("period not recieved from other core");
        return -1;
    }

    while (timeout && !read_from_other_core(&timer))
    {
        timeout--;
    }

    if (!timeout)
    {
        Serial.println("timer not recieved from other core");
        return -1;
    }

    while (timeout && !read_from_other_core(&flag))
    {
        timeout--;
    }

    if (!timeout)
    {
        Serial.println("flag not recieved from other core");
        return -1;
    }

    while (timeout && !read_from_other_core(&push_to_graphana))
    {
        timeout--;
    }

    if (!timeout)
    {
        Serial.println("push_to_graphana not recieved from other core");
        return -1;
    }

    myBehaviors.init_behavior(sensorID,period,timer,flag,push_to_graphana,mode);
    if (networkCheck() == 0)
    {
        send_response_measure_started(timer);
    }
    return 1;
}

int request::read_alert_setup_from_core() {
    uint32_t timer,in_alert_mode,min,max;
    int timeout = 10;
    while (timeout && !read_from_other_core(&timer))
    {
        timeout--;
    }

    if (!timeout)
    {
        Serial.println("timer not recieved from other core");
        return -1;
    }

    while (timeout && !read_from_other_core(&in_alert_mode))
    {
        timeout--;
    }

    if (!timeout)
    {
        Serial.println("in_alert_mode not recieved from other core");
        return -1;
    }

    while (timeout && !read_from_other_core(&min))
    {
        timeout--;
    }

    if (!timeout)
    {
        Serial.println("min not recieved from other core");
        return -1;
    }

    while (timeout && !read_from_other_core(&max))
    {
        timeout--;
    }

    if (!timeout)
    {
        Serial.println("max not recieved from other core");
        return -1;
    }

    myBehaviors.init_behavior(sensorID,period,timer,flag,push_to_graphana);
    if (networkCheck() == 0)
    {
        send_response_measure_started(timer);
    }
    return 1;
}


void request::send_response_measure_started(int timer) {
    DynamicJsonDocument jsonDoc(1024);
    
    jsonDoc["status"] = "Timer "+(String)timer+" started";

    send_post_response(jsonDoc);

}

void request::send_push_to_graphana(int timer,float data) {
    DynamicJsonDocument jsonDoc(1024);
    
    jsonDoc["status"] = "Timer "+(String)timer+" started";
    jsonDoc["value"] = (String) data;

    send_post_request(jsonDoc);

}

int request::read_measure_value_from_core() {
    uint32_t timer , data;
    int timeout = 10;
    while (timeout && !read_from_other_core(&timer))
    {
        timeout--;
    }

    if (!timeout)
    {
        Serial.println("timer not recieved from other core");
        return -1;
    }

    while (timeout && !read_from_other_core(&data))
    {
        timeout--;
    }

    if (!timeout)
    {
        Serial.println("data not recieved from other core");
        return -1;
    }

    myBehaviors.update_data(timer,uint32ToFloat(data));

    if (myBehaviors.regular_behaviors[timer].push_to_graphana && networkCheck() == 0)
    {
        send_push_to_graphana(timer,myBehaviors.regular_behaviors[timer].one_value_data);
    }

    return 1;
  
}

void request::send_info() {
    DynamicJsonDocument jsonDoc(1024);
    for (int i = 0; i < myBehaviors.nbre_behaviors; i++)
    {
        if (myBehaviors.regular_behaviors[i].in_use)
        {
            jsonDoc["active_behaviors"][i]["timer"] = myBehaviors.regular_behaviors[i].timer;
            jsonDoc["active_behaviors"][i]["sensorID"] = myBehaviors.regular_behaviors[i].sensorID;
            jsonDoc["active_behaviors"][i]["period"] = myBehaviors.regular_behaviors[i].period;
            jsonDoc["active_behaviors"][i]["flag"] = myBehaviors.regular_behaviors[i].flag;
            jsonDoc["active_behaviors"][i]["maximum"] = myBehaviors.regular_behaviors[i].max;
            jsonDoc["active_behaviors"][i]["minimum"] = myBehaviors.regular_behaviors[i].min;
        }
    }
    send_post_response(jsonDoc);   
}

void request::send_post_request(DynamicJsonDocument jsonDoc) {
    String jsonString;
    serializeJson(jsonDoc, jsonString);
    
    if (client.connect(server_ip, 80)) {
        Serial.println("connected to send data");
        client.println("POST /cgi-bin/get_mydata.sh HTTP/1.1");
        client.print("Host: ");
        client.println(server_ip);
        client.println("Content-Type: application/json");
        client.print("Content-Length: ");
        client.print(jsonString.length());
        client.println();
        client.println();
        client.print(jsonString);
        client.flush();
        read_data_back();
    }
    else {
        Serial.println("connection failed");
    }
}

void request::send_post_response(DynamicJsonDocument jsonDoc) {
    String jsonString;
    serializeJson(jsonDoc, jsonString);
    user_cmd.println("HTTP/1.1 200 OK");
    user_cmd.println("Content-Type: application/json");
    user_cmd.println();

    user_cmd.println(jsonString);
    
    // if (client.connect(server_ip, 80)) {
    //     Serial.println("connected to send data");
    //     client.println("POST /cgi-bin/get_mydata.sh HTTP/1.1");
    //     client.print("Host: ");
    //     client.println(server_ip);
    //     client.println("Content-Type: application/json");
    //     client.print("Content-Length: ");
    //     client.print(jsonString.length());
    //     client.println();
    //     client.println();
    //     client.print(jsonString);
    //     client.flush();
    //     read_data_back();
    // }
    // else {
    //     Serial.println("connection failed");
    // }
}


