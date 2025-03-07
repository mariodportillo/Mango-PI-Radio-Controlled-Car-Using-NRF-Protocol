
## **Title of your project**

Radio Controlled Car Using NRF Protocol


## **Team members**

Mario Portillo Sahan Samarakoon


## **Project goals**

For our project we intend to implement wireless communication between two mango PIs as our main goal. We want to extend this initial goal by then attaching one PI to a car using DC motors to then be controlled by the mango PI wirelessly. Our main objective for this project is to have two communicating PIs using the NRF protocol which will require us to use chips such as the nRF24L01. The nRF24L01 is a radio transceiver for the world wide 2.4 - 2.5 GHz ISM band. To do so we will need to first understand how to set up the chip using our mango PI GPIOs to properly turn on and configure the chips individually. From there we intend to understand how to send communications and packages between chips to be outputted in some way. We intend to begin with tio printf() statements as a proof of concept of our communication between PIs. Once we have laid the groundwork of communication we will enter the second stage of our project which will be building and programming a wireless car. 

**Main Goals Overview**



1. NRF protocol -> SPI 2 NRF chips required
    1. Using SPI Protocol -> provided by [spi.h](https://github.com/cs107e/cs107e.github.io/blob/master/project/code_extras/spi.h)
    2. Initialize the nRF24L01 by configuring its registers (as described in its product specification) [example github code ](https://github.com/controllerstech/NRF24L01)
    3. Send SPI commands (such as register read, register write, and payload operations)
    4. Handle timing and control signals (like CE and CSN) as required by the nRF24L01 for tasks such as mode switching between TX and RX
2. Communication between 2 PIs
3. Use car kit -> build robot
4. motor.h -> getting the motors to move/initialize -> low level
    5. PWM driven -> built on top of PWM module
5. motordriver.h  -> higher level code/API 
    6. “Drive 10 cm”
    7. “Turn 90 degrees”
    8. “Turn 45 degrees”
6. Joystick
7. Driver Control program -> mango run

**Stretch Goals Extension**



8. Servo.h
9. Ultrasonic.h
10. Design a course that can be completed by robot/driver control


## **Resources, budget**

**Itemize components/materials you will need and plan for acquiring them. We can reimburse the cost of parts up to $20 per person.**


<table>
  <tr>
   <td>4 DC
   </td>
   <td>Motors
   </td>
   <td>Ordered
   </td>
   <td>3/5/2025
   </td>
   <td>3/7/2025
   </td>
   <td>$8.00
   </td>
  </tr>
  <tr>
   <td>4 L298N Motor Drive Controller Board
   </td>
   <td>Chips
   </td>
   <td>Ordered
   </td>
   <td>3/5/2025
   </td>
   <td>3/7/2025
   </td>
   <td>$8.00
   </td>
  </tr>
  <tr>
   <td>2 Servos
   </td>
   <td>Motors
   </td>
   <td>Ordered
   </td>
   <td>3/5/2025
   </td>
   <td>3/7/2025
   </td>
   <td>$8.00
   </td>
  </tr>
  <tr>
   <td>NRF24L01
   </td>
   <td>Chips
   </td>
   <td>Ordered
   </td>
   <td>3/5/2025
   </td>
   <td>3/8/2025
   </td>
   <td>$8.00
   </td>
  </tr>
  <tr>
   <td>3 Joysticks
   </td>
   <td>User Control
   </td>
   <td>Ordered
   </td>
   <td>3/5/2025
   </td>
   <td>3/7/2025
   </td>
   <td>$8.00
   </td>
  </tr>
  <tr>
   <td>Ultrasonic sensors
   </td>
   <td>Sensors
   </td>
   <td>Ordered
   </td>
   <td>3/5/2025
   </td>
   <td>3/7/2025
   </td>
   <td>$6.00
   </td>
  </tr>
</table>


[Link to Shopping List](https://docs.google.com/spreadsheets/d/1hIcRcWLCS1yLfJa3-90HFtc-oYyIPkL5oC7PKs615Zs/edit?usp=sharing)


## **Tasks, member responsibilities**

**Each team member should be responsible for a clearly identifiable part of the project.**

Listed below are the deliverables that each team member is responsible for doing and a category for both which includes the deliverables we share among the team members.

Both



* NRF protocol
* nrf.h 
* motor.h
* Driver Control program
* Ultrasonic Program
* Design a course completable by the Robot -> Extension

Mario Portillo



* motordriver.h
* servo.h
* Assembling of the robot

Sahan Samarakoon



* Ultrasonic.h
* Joystick.h

The NRF Protocol bullet encapsulates both the understanding and outlining of the implementation process of this protocol to our Mango PI. Prior to writing any code we will fully understand the protocol and read the [nRF24L01 datasheet here](https://www.mouser.com/datasheet/2/297/nRF24L01_Product_Specification_v2_0-9199.pdf) to get a better understanding of how we want to handle this protocol in our environment. Given the fact that we have a very bare bones system we may run into many problems with having to go down to the assembly to ensure the correct registers are being accessed and updated in the current order to ensure communication between the Mango PI and the nRF24L01. 

**Schedule, midpoint milestones**

**Sketch a timeline for steady progress across full project period Order tasks so as to resolve unknowns/risks sooner than later What are the one-week goals you will complete by next lab meeting?**

**Week 1: Establishing Communication & Basic Movement**

**Goal: **Achieve reliable wireless communication between Mango Pis via nRF24L01 and implement basic motor control.

**Key Tasks:**

SPI Communication Setup (nRF24L01 Configuration & Testing)



* Understand the registers and settings needed for TX/RX communication.
* Send and receive text messages over SPI, validate with printf.
* Debug potential issues.

Motor Control Implementation (Basic Low-Level Control)



* Implement motor.h using PWM signals for motor control.
* Test forward, backward, stop, and basic turns using direct GPIO control.

Higher-Level Motor API (motordriver.h)



* Implement movement functions like:
    * driveforward(cm)
    * turnleft(degrees), turnright(degrees)
    * stop()

Stretch Goals (if ahead of schedule):



* Implement Servo Control (servo.h) for steering or extra movement.
* Setup Ultrasonic Sensor (ultrasonic.h) for basic distance detection.

 **Week 1 Deliverables (Midpoint Milestone): \
** Two Mango PIs successfully communicate using nRF24L01 via SPI. \
 Motors can be controlled via PWM with a simple API. \
 (Stretch) Initial servo/ultrasonic sensor integration.

**Week 2: Building the Car & Advanced Features**

**Goal**: Integrate wireless joystick control and build the RC car.

 **Key Tasks:**

 Build & Assemble the RC Car



* Attach Mango Pi, nRF module, and motors to the chassis.
* Ensure stable power supply and cable management.

Implement Joystick Control (joystick.h)



* Map joystick movements to car controls.
* Implement joystick-to-SPI command translation.
* Test joystick responsiveness and latency of wireless commands.

Driver Control Program



* Implement a script that allows the joystick to control the car in real time.
* Ensure smooth acceleration, deceleration, and turns.

Stretch Goal: Autonomous Navigation



* Integrate Ultrasonic Sensor to detect obstacles.
* Implement a basic "avoid obstacles" algorithm.
* Test if the car can navigate a simple predefined course.

**Week 2 Deliverables (Final Milestone):** \
 Working RC Car controlled wirelessly via a joystick. \
 Fully integrated and assembled hardware. \
 (Stretch) Autonomous obstacle detection using ultrasonic sensors.


## 


## **Additional resources, issues**

**Are there books/code/tools that you need access to? What are the possible risks to overcome? What are you concerned about? How can we help you succeed?**

Potential challenges/concerns: 



* The nRF24L01 requires careful configuration of registers, power modes, and timing. We have no pre-existing driver for Mango Pi, so we are writing one from scratch.
* Unfamiliarity in working with SPI. 

Reference Materials:



* We have acquired the nRF24L01 Product Specification (official datasheet for register-level understanding)
* SPI communication resources (understanding timing, CSN/CE control, etc.).
* We plan on using Open-source nRF24L01 drivers for other microcontrollers (Arduino, Raspberry Pi) to gain inspiration for our implementation.

How can the course staff help us?



* Frances and Jesse will be our two main resource personnel since they have worked on projects of similar nature. Jesse has already given us a huge boost by pointing us towards the required hardware and datasheet(s).
* Access to debugging tools (logic analyzer: SPI troubleshooting, oscilloscope and multimeter: for hardware troubleshooting).
* Feedback from staff on our planned nRF driver implementation to catch major mistakes early.
