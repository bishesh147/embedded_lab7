import time
import paho.mqtt.client as mqtt
import matplotlib.pyplot as plt
plt.style.use("seaborn-v0_8")

plt.ion()

idx = []
obj_temp = []
start_time = time.time()
# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, reason_code, properties):
    print(f"Connected with result code {reason_code}")
    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("Bishesh-publish")

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    temp = str(msg.payload)
    temp_val = float(temp[8:13])
    print(temp_val)
    idx.append(time.time()-start_time)
    obj_temp.append(temp_val)
    plt.xlim(left=max(0, 0), right=100) #set the x-limits of the current axis 
    #plt.ylim(-1, 2) #set the y-limits of the current axis 
    plt.ylim(0, 50) #set the y-limits of the current axis 
    plt.ylabel('Temperature (C)', fontname='Comic Sans MS', color='blue', fontsize=14) #set the label for the y-axis
    plt.xlabel('time (sec)', fontname='Comic Sans MS', color='blue', fontsize=14) #set the label for the y-axis
    
    plt.grid(True) #turn the grid on 
    plt.plot(idx, obj_temp, color='r', linewidth=2) #plot idx versus obj_temp - red line
    plt.legend(loc='upper right', fontsize=12)

    plt.draw()
    plt.show() #display the figure
    plt.pause(.000001) #pause for interval seconds

mqttc = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
mqttc.on_connect = on_connect
mqttc.on_message = on_message

mqttc.connect("mqtt.eclipseprojects.io", 1883, 60)

# Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.
# Other loop*() functions are available that give a threaded interface and a
# manual interface.
mqttc.loop_forever()