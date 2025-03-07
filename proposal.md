
<p style="color: red; font-weight: bold">>>>>>  gd2md-html alert:  ERRORs: 0; WARNINGs: 0; ALERTS: 1.</p>
<ul style="color: red; font-weight: bold"><li>See top comment block for details on ERRORs and WARNINGs. <li>In the converted Markdown or HTML, search for inline alerts that start with >>>>>  gd2md-html alert:  for specific instances that need correction.</ul>

<p style="color: red; font-weight: bold">Links to alert messages:</p><a href="#gdcalert1">alert1</a>

<p style="color: red; font-weight: bold">>>>>> PLEASE check and correct alert issues and delete this message and the inline alerts.<hr></p>


<h2><strong>Title of your project</strong></h2>


<p>
Radio Controlled Car Using NRF Protocol
</p>
<h2><strong>Team members</strong></h2>


<p>
Mario Portillo Sahan Samarakoon
</p>
<h2><strong>Project goals</strong></h2>


<p>
For our project we intend to implement wireless communication between two mango PIs as our main goal. We want to extend this initial goal by then attaching one PI to a car using DC motors to then be controlled by the mango PI wirelessly. Our main objective for this project is to have two communicating PIs using the NRF protocol which will require us to use chips such as the nRF24L01. The nRF24L01 is a radio transceiver for the world wide 2.4 - 2.5 GHz ISM band. To do so we will need to first understand how to set up the chip using our mango PI GPIOs to properly turn on and configure the chips individually. From there we intend to understand how to send communications and packages between chips to be outputted in some way. We intend to begin with tio printf() statements as a proof of concept of our communication between PIs. Once we have laid the groundwork of communication we will enter the second stage of our project which will be building and programming a wireless car. 
</p>
<p>
<strong>Main Goals Overview</strong>
</p>
<ol>

<li>NRF protocol -> SPI 2 NRF chips required</li> 
<ol>
 
<li>Using SPI Protocol -> provided by <a href="https://github.com/cs107e/cs107e.github.io/blob/master/project/code_extras/spi.h">spi.h</a></li>
 
<li>Initialize the nRF24L01 by configuring its registers (as described in its product specification) <a href="https://github.com/controllerstech/NRF24L01">example github code </a></li>
 
<li>Send SPI commands (such as register read, register write, and payload operations)</li>
 
<li>Handle timing and control signals (like CE and CSN) as required by the nRF24L01 for tasks such as mode switching between TX and RX</li> 
</ol>

<li>Communication between 2 PIs</li>

<li>Use car kit -> build robot</li>

<li>motor.h -> getting the motors to move/initialize -> low level</li> 
<ol>
 
<li>PWM driven -> built on top of PWM module</li> 
</ol>

<li>motor_driver.h  -> higher level code/API </li> 
<ol>
 
<li>“Drive 10 cm”</li>
 
<li>“Turn 90 degrees”</li>
 
<li>“Turn 45 degrees”</li> 
</ol>

<li>Joystick</li>

<li>Driver Control program -> mango run</li>
</ol>
<p>
<strong>Stretch Goals Extension</strong>
</p>
<ol>

<li>Servo.h</li>

<li>Ultrasonic.h</li>

<li>Design a course that can be completed by robot/driver control</li>
</ol>
<h2><strong>Resources, budget</strong></h2>


<p>
<strong>Itemize components/materials you will need and plan for acquiring them. We can reimburse the cost of parts up to $20 per person.</strong>
</p>
<p>


<p id="gdcalert1" ><span style="color: red; font-weight: bold">>>>>>  gd2md-html alert: inline image link here (to images/image1.png). Store image on your image server and adjust path/filename/extension if necessary. </span><br>(<a href="#">Back to top</a>)(<a href="#gdcalert2">Next alert</a>)<br><span style="color: red; font-weight: bold">>>>>> </span></p>


<img src="images/image1.png" width="" alt="alt_text" title="image_tooltip">

</p>
<p>
<a href="https://docs.google.com/spreadsheets/d/1hIcRcWLCS1yLfJa3-90HFtc-oYyIPkL5oC7PKs615Zs/edit?usp=sharing">Link to Shopping List</a>
</p>
<h2><strong>Tasks, member responsibilities</strong></h2>


<p>
<strong>Each team member should be responsible for a clearly identifiable part of the project.</strong>
</p>
<p>
Listed below are the deliverables that each team member is responsible for doing and a category for both which includes the deliverables we share among the team members.
</p>
<p>
Both
</p>
<ul>

<li>NRF protocol</li>

<li>nrf.h </li>

<li>motor.h</li>

<li>Driver Control program</li>

<li>Ultrasonic Program</li>

<li>Design a course completable by the Robot -> Extension</li>
</ul>
<p>
Mario Portillo
</p>
<ul>

<li>motor_driver.h</li>

<li>servo.h</li>

<li>Assembling of the robot</li>
</ul>
<p>
Sahan Samarakoon
</p>
<ul>

<li>Ultrasonic.h</li>

<li>Joystick.h</li>
</ul>
<p>
The NRF Protocol bullet encapsulates both the understanding and outlining of the implementation process of this protocol to our Mango PI. Prior to writing any code we will fully understand the protocol and read the <a href="https://www.mouser.com/datasheet/2/297/nRF24L01_Product_Specification_v2_0-9199.pdf">nRF24L01 datasheet here</a> to get a better understanding of how we want to handle this protocol in our environment. Given the fact that we have a very bare bones system we may run into many problems with having to go down to the assembly to ensure the correct registers are being accessed and updated in the current order to ensure communication between the Mango PI and the nRF24L01. 
</p>
<p>
<strong>Schedule, midpoint milestones</strong>
</p>
<p>
<strong>Sketch a timeline for steady progress across full project period Order tasks so as to resolve unknowns/risks sooner than later What are the one-week goals you will complete by next lab meeting?</strong>
</p>
<p>
<strong>Week 1: Establishing Communication & Basic Movement</strong>
</p>
<p>
<strong>Goal: </strong>Achieve reliable wireless communication between Mango Pis via nRF24L01 and implement basic motor control.
</p>
<p>
<strong>Key Tasks:</strong>
</p>
<p>
SPI Communication Setup (nRF24L01 Configuration & Testing)
</p>
<ul>

<li>Understand the registers and settings needed for TX/RX communication.</li>

<li>Send and receive text messages over SPI, validate with printf.</li>

<li>Debug potential issues.</li>
</ul>
<p>
Motor Control Implementation (Basic Low-Level Control)
</p>
<ul>

<li>Implement motor.h using PWM signals for motor control.</li>

<li>Test forward, backward, stop, and basic turns using direct GPIO control.</li>
</ul>
<p>
Higher-Level Motor API (motordriver.h)
</p>
<ul>

<li>Implement movement functions like:</li> 
<ul>
 
<li>driveforward(cm)</li>
 
<li>turnleft(degrees), turnright(degrees)</li>
 
<li>stop()</li> 
</ul></li> 
</ul>
<p>
Stretch Goals (if ahead of schedule):
</p>
<ul>

<li>Implement Servo Control (servo.h) for steering or extra movement.</li>

<li>Setup Ultrasonic Sensor (ultrasonic.h) for basic distance detection.</li>
</ul>
<p>
 <strong>Week 1 Deliverables (Midpoint Milestone):<br></strong> Two Mango PIs successfully communicate using nRF24L01 via SPI.<br> Motors can be controlled via PWM with a simple API.<br> (Stretch) Initial servo/ultrasonic sensor integration.
</p>
<p>
<strong>Week 2: Building the Car & Advanced Features</strong>
</p>
<p>
<strong>Goal</strong>: Integrate wireless joystick control and build the RC car.
</p>
<p>
 <strong>Key Tasks:</strong>
</p>
<p>
 Build & Assemble the RC Car
</p>
<ul>

<li>Attach Mango Pi, nRF module, and motors to the chassis.</li>

<li>Ensure stable power supply and cable management.</li>
</ul>
<p>
Implement Joystick Control (joystick.h)
</p>
<ul>

<li>Map joystick movements to car controls.</li>

<li>Implement joystick-to-SPI command translation.</li>

<li>Test joystick responsiveness and latency of wireless commands.</li>
</ul>
<p>
Driver Control Program
</p>
<ul>

<li>Implement a script that allows the joystick to control the car in real time.</li>

<li>Ensure smooth acceleration, deceleration, and turns.</li>
</ul>
<p>
Stretch Goal: Autonomous Navigation
</p>
<ul>

<li>Integrate Ultrasonic Sensor to detect obstacles.</li>

<li>Implement a basic "avoid obstacles" algorithm.</li>

<li>Test if the car can navigate a simple predefined course.</li>
</ul>
<p>
<strong>Week 2 Deliverables (Final Milestone):</strong><br> Working RC Car controlled wirelessly via a joystick.<br> Fully integrated and assembled hardware.<br> (Stretch) Autonomous obstacle detection using ultrasonic sensors.
</p>
<h2></h2>


<h2><strong>Additional resources, issues</strong></h2>


<p>
<strong>Are there books/code/tools that you need access to? What are the possible risks to overcome? What are you concerned about? How can we help you succeed?</strong>
</p>
<p>
Potential challenges/concerns: 
</p>
<ul>

<li>The nRF24L01 requires careful configuration of registers, power modes, and timing. We have no pre-existing driver for Mango Pi, so we are writing one from scratch.</li>

<li>Unfamiliarity in working with SPI. </li>
</ul>
<p>
Reference Materials:
</p>
<ul>

<li>We have acquired the nRF24L01 Product Specification (official datasheet for register-level understanding)</li>

<li>SPI communication resources (understanding timing, CSN/CE control, etc.).</li>

<li>We plan on using Open-source nRF24L01 drivers for other microcontrollers (Arduino, Raspberry Pi) to gain inspiration for our implementation.</li>
</ul>
<p>
How can the course staff help us?
</p>
<ul>

<li>Frances and Jesse will be our two main resource personnel since they have worked on projects of similar nature. Jesse has already given us a huge boost by pointing us towards the required hardware and datasheet(s).</li>

<li>Access to debugging tools (logic analyzer: SPI troubleshooting, oscilloscope and multimeter: for hardware troubleshooting).</li>

<li>Feedback from staff on our planned nRF driver implementation to catch major mistakes early.
