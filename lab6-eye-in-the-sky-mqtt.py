#import python libraries
#import matplot to graph
import matplotlib.pyplot as plt
#import paho to connect with mqtt server
import paho.mqtt.client as mqtt

# Define mqtt broker host
mqtt_broker = "robomqtt.cs.wpi.edu"
# Define mqtt broker port
mqtt_port = 1883
# Define team username
username = "team15"
# Define team password
password = "lolland0248"
# Define empty variable array of tags
tags = []
# Define empty variable array of checked tags
checkTags = []

#Define number of tags
tagNum = 3
#Define topic prior to tag number
tagPre = "theIlluminati/tag"
# Define subtopics
tagSufs = ["/x", "/y"]
# Iterates through number of tags to
# make the tag topics align with the tag topics
for i in range(0, tagNum):
    t = []
    for s in tagSufs:
        tag = tagPre + str(i) + s
        tags.append((tag, 1))
        t.append(tag)
    checkTags.append(t)

# Initialize Matplotlib figure and axis for the grid map
fig, ax = plt.subplots()
ax.set_xlim(0, 160)  # Set the x-axis limits of the grid map
ax.set_ylim(0, 120)  # Set the y-axis limits of the grid map

# Initialize a scatter plot for the robot's position
robot_scatter = ax.scatter([], [], marker='o', color='red', label='Robot')

# Initialize 3 robot (April Tag) points
points = [[0,0], [0,0], [0,0]]

# Update function to be called on MQTT message reception
def on_message(client, userdata, msg):
    # Define global variables points and checkTags for continuous update of position
    global points, checkTags
    #Establish read-in of the topic
    topic = msg.topic
    # Extract x and y coordinates from the received message
    payload = msg.payload.decode("utf-8") #decode the byte to unicode
    for i in range(0, len(checkTags)):
        for j in range(0, len(checkTags[i])):
            if topic == checkTags[i][j]:
                points[i][j] = int(payload)
                

    # Update the position of the robot on the scatter plot
    robot_scatter.set_offsets(points)

    # Redraw the figure
    plt.draw()

# Set up MQTT client
client = mqtt.Client()
client.on_message = on_message
client.username_pw_set(username, password)
client.connect(mqtt_broker, mqtt_port, 60)
client.subscribe(tags)
client.loop_start()


# Show the Matplotlib plot
plt.legend()
plt.title('Robot Position on Grid Map')
plt.xlabel('X-axis')
plt.ylabel('Y-axis')
plt.grid(True)
plt.show()

# Disconnect MQTT client on plot close
client.disconnect()
client.loop_stop()
