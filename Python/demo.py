#!/bin/python3
import ServoProjectModules.Communication as ServoComModule
from ServoProjectModules.Communication import pi

import numpy as np
import threading
import time
import math
import random
import matplotlib.pyplot as plt

dt = 0.018

def createRobot(port = '/dev/ttyACM0'):
    def createServoFunction(robot):
        simCom = ServoComModule.SimulateCommunication()
        if port != '':
            com = ServoComModule.SerialCommunication(port)
        else:
            com = simCom

        servoArray = []

        newServo = ServoComModule.DCServoCommunicator(1, com)
        newServo.setOffsetAndScaling(360.0 / 4096.0, -111.0, 0)
        newServo.setControlSpeed(32)
        newServo.setBacklashControlSpeed(6, 180.0, 0.00)
        servoArray.append(newServo)

        newServo = ServoComModule.DCServoCommunicator(2, com)
        newServo.setOffsetAndScaling(180.0 / 1900.0, 0.0, 0.0)
        servoArray.append(newServo)

        robot.servoArray = servoArray

    robot = ServoComModule.Robot(cycleTime=dt, initFunction=createServoFunction)

    return robot

def playTrajectory(robot, trajectory):
    doneRunning = False
    index = 0
    def sendCommandHandlerFunction(dt, robot):
        nonlocal index
        nonlocal doneRunning

        if index == 0:
            pos = trajectory[0]
            vel = [0.0] * len(pos)
        elif index < len(trajectory) - 1:
            pos = trajectory[index]
            vel = (np.array(trajectory[index +1]) - np.array(trajectory[index - 1])) / 2.0 / dt
        else:
            pos = trajectory[-1]
            vel = [0.0] * len(pos)

        for i, servo in enumerate(robot.servoArray):
            servo.setReference(pos[i], vel[i], 0.0)

        index += 1
        if index == len(trajectory):
            robot.removeHandlerFunctions()
            doneRunning = True
        return

    def readResultHandlerFunction(dt, robot):
        pos = robot.getPosition()
        return

    robot.setHandlerFunctions(sendCommandHandlerFunction, readResultHandlerFunction)

    while not doneRunning:
        if not robot.isAlive():
            break
        time.sleep(0.1)

def addLinearMove(trajectory, endPos, duration):
    global dt
    startPos = np.array(trajectory[-1])
    endPos = np.array(endPos)

    steps = int(round(duration / dt))
    for i in range(1, steps + 1):
        t = i / steps
        trajectory.append(endPos * t + startPos * (1 - t))

    return trajectory

def addSmoothMove(trajectory, endPos, duration):
    global dt
    startPos = np.array(trajectory[-1])
    endPos = np.array(endPos)

    steps = int(round(duration / dt))
    for i in range(1, steps + 1):
        t = i / steps
        t = (1.0 - math.cos(pi * t)) / 2.0
        trajectory.append(endPos * t + startPos * (1 - t))

    return trajectory

def addWait(trajectory, duration):
    global dt
    startPos = np.array(trajectory[-1])

    steps = int(round(duration / dt))
    for i in range(1, steps + 1):
        trajectory.append(startPos)

    return trajectory

def main():
    with createRobot('/dev/ttyACM0') as robot:
        trajectory = []
        trajectory.append(robot.getPosition())

        trajectory = addSmoothMove(trajectory, [0.0, 0.0], 1.0)
        trajectory = addWait(trajectory, 10.0)

        for i in range(0, 3):
            #trajectory = addSmoothMove(trajectory, [2.0, 2.0], 0.2)
            trajectory = addSmoothMove(trajectory, [-10.0, -10.0], 1.0)
            trajectory = addSmoothMove(trajectory, [0.0, 0.0], 3.0)
            trajectory = addWait(trajectory, 3.0)
            #trajectory = addSmoothMove(trajectory, [-2.0, -2.0], 0.2)
            trajectory = addSmoothMove(trajectory, [10.0, 10.0], 1.0)
            trajectory = addSmoothMove(trajectory, [0.0, 0.0], 3.0)
            trajectory = addWait(trajectory, 3.0)

        trajectory = addWait(trajectory, 7.0)

        playTrajectory(robot, trajectory)

if __name__ == '__main__':
    main()
