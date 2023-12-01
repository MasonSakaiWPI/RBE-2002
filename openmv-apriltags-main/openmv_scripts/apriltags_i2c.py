# AprilTags Pixy I2C Emulation Script
#
# This script allows your OpenMV Cam to transmit AprilTag detection data like
# a Pixy (CMUcam5) tracking colors in I2C mode. This script allows you to
# easily replace a Pixy (CMUcam5) color tracking sensor with an OpenMV Cam
# AprilTag tracking sensor. Note that this only runs on the OpenMV Cam M7.
#
# P4 = SCL
# P5 = SDA
#
# Note: The tag family is TAG36H11. Additionally, in order to for the
#       signature value of a tag detection to be compatible with pixy
#       interface libraries all tag ids have 8 added to them in order
#       to move them in the color code signature range. Finally, tags
#       are all reported as color code blocks...

# Comm Parameters ############################################################

max_blocks = 255
max_blocks_per_id = 255

i2c_address = 0x12

##############################################################################

###
# Changed to using rpc.put_bytes, which has a little more control
# Plan is to make it report back the number
###

import image, math, pyb, sensor, rpc, struct, time
import pyb

red_led = pyb.LED(1)
lastBlinkTime = 0

# Camera Setup

sensor.reset()
sensor.set_pixformat(sensor.GRAYSCALE)
sensor.set_framesize(sensor.QQVGA)
sensor.skip_frames(time = 2000)

# Link Setup

#bus = pyb.I2C(2, pyb.I2C.SLAVE, addr = i2c_address)
interface = rpc.rpc_i2c_slave(slave_addr=0x12)

# Helper Stuff

def checksum(data):
    checksum = 0
    for i in range(0, len(data), 2):
        checksum += ((data[i+1] & 0xFF) << 8) | ((data[i+0] & 0xFF) << 0)
    return checksum & 0xFFFF

def to_object_block_format(tag):
    angle = int((tag.rotation() * 180) / math.pi)
    temp = struct.pack("<hhhhhh", tag.id(), tag.cx(), tag.cy(), tag.w(), tag.h(), angle)
    return struct.pack("<hh12s", 0xAA56, checksum(temp), temp)

# Main Loop

clock = time.clock()
while(True):
    clock.tick()
    img = sensor.snapshot()
    tags = img.find_apriltags(families=image.TAG25H9) # default TAG16H5 family

    followTags = []
    for tag in tags:
        #if(tag.id() == 4):
        followTags.append(tag)

    if(len(followTags) > 0):
        red_led.off()
    elif(time.time_ns() - lastBlinkTime >= 500000000):
        red_led.toggle()
        lastBlinkTime = time.time_ns()


    # Transmit Tags #

    dat_buf = struct.pack("<b", len(followTags))
    interface.put_bytes(dat_buf, timeout_ms = 200)

    if followTags and (max_blocks > 0) and (max_blocks_per_id > 0): # new frame
        # dat_buf = struct.pack("<h", 0xAA55)

        # sort biggest to smallest
        for tag in sorted(followTags, key = lambda x: x.h() * x.w(), reverse = True)[0:max_blocks]:

            dat_buf = to_object_block_format(tag)
            img.draw_rectangle(tag.rect())
            img.draw_cross(tag.cx(), tag.cy())

            # dat_buf += struct.pack("<h", 0x0000)
            # write(dat_buf) # write all data in one packet...
            interface.put_bytes(dat_buf, timeout_ms = 200)

    num_tags = min(len(tags), max_blocks)
    print("%d tags(s) found - FPS %f" % (num_tags, clock.fps()))
