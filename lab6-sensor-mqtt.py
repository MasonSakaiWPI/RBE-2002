# MQTT broker
broker_address = "robomqtt.cs.wpi.edu"
broker_port = 1883

# Topics for the sensor
sonar_topic = "sonar_sensor"
ir_topic = "ir_sensor"
acceleration_topic = "z_axis_acceleration"

# This function defines a callback to receive a message
def on_message(client, userdata, message):
    topic = message.topic #save the topic which reads the sensor information
    payload = message.payload.decode('utf-8') #saves byte sequence as unicode string to decode the message

    # Parse and display messages
    if topic == sonar_topic: #if the topic is reading the sonar
        print(f"Sonar Sensor Distance: {payload} cm") #print out the sonar data in cm
    elif topic == ir_topic: #if the topic is reading the ir
        print(f"IR Sensor Distance: {payload} cm") #print the ir data in cm
    elif topic == acceleration_topic: #if the topic is reading the imu
        print(f"Z-axis Acceleration: {payload} m/s^2") #print the z-axis of the imu in m/s^2

# Create an MQTT client
client = mqtt.Client()

# Set the callback function
client.on_message = on_message

# Connect to the broker
client.connect(broker_address, broker_port, 60)

# Subscribe to the sensor topics where .subscribe(topic, quality of service)
client.subscribe([(sonar_topic, 0), (ir_topic, 0), (acceleration_topic, 0)])

# Loop to listen for messages in an infinite loop
client.loop_forever()
