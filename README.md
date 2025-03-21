

<h2><strong>Project title</strong></h2>


<p>
Radio Controlled Car Using NRF Protocol
</p>
<h2><strong>Team members</strong></h2>


<p>
Mario Portillo and Sahan Samarakoon
</p>
<h2><strong>Project description</strong></h2>


<p>Our project offers a radio controlled car that includes two Pis that are transmitting data between each other to process and run commands such as “Forward Speed: 54”. Through heavy use of the nRF24L01 we were able to implement communication between two mango Pis in such a way where 32 bytes of data can be transmitted over radio waves in one payload. This payload can then be processed by a receiver and interpreted to be a command which is exactly how we communicate in our motor_control_from_joystick() function. Our project consisted of a lot of low level and mid level programming to fully setup communication between mango pis and external peripherals such as the joystick, NRF, MCP ADC converter, and motors. </p>


<p>
<strong>SPI_comm.h</strong>
</p>
<p>
	Given the fact that we wanted to be able to use multiple SPI protocol devices such as the MCP3008 and the nRF24L01 we needed some way of toggling between the two while still keeping the same low level SPI module functioning. To do so we learned how to properly toggle between chip selects and build up on the SPI module. Using SPI_comm we essentially offer a wrapper for the user when trying to do spi_transfer. Instead of relying on the SPI module to handle chip select we manually control which chip we want to write to by bringing the chip select of that pin we want to select down and lowering the other chip select. This workflow could easily be modified and built upon to offer others the ability to do the same with as many chip select SPI chips needed. In our implementation we only needed two so we made two helper functions to do this.
</p>
<p>
<strong>NRF Communication</strong>
</p>
<p>
	NRF Communication is done using SPI protocol but specifically takes advantage of spi_transfer_device() which builds upon the SPI module provided in code/extra. Using the provided SPI library given by code/extras we were able to use that as a foundation for our communication with the NRF. With the SPI protocol already being implemented we were able to primarily focus on the setting up of the NRF protocols to handle transferring instruction between the mango Pis to the NRF. To do so we needed to fully understand the NRF’s registers to properly map which registers needed what values to run certain commands such as enable_nrf_device() and nrf24_write_byte_register(). With this SPI communication working along with proper register manipulation we were able to begin creating slightly higher level function in the NRF.h module such as nrf24_set_tx_mode(), nrf24_transmit(), nrf24_set_rx_mode(), and nrf24_receive(). These core functions are what allow for higher level functionality to occur when the Pis are transmitting and receiving data over radio. So long as the nrf is properly setup the register manipulation and low level setup is completely abstracted away and proper toggling between receive mode and transmit mode are made easy in code. 
</p>
<p>
<strong>MCP3008 Communication</strong>
</p>
<p>
	Given the fact we wanted to be able to take in joystick input, which is received as analog data, we needed a way of converting this input to a digital format which could be understood by the Mango Pis. This is where the ADC converter MCP3008 comes into play. Given that the MCP3008 also employs SPI transfer it was a matter of simply using our spi_transfer_device() function to handle Pi to MCP communication. The MCP read channel was straightforward from there as in lecture we covered how to communicate between MCP and Pi so by implementing the code we discussed in class we had working communication between MCP and Pi. 
</p>
<p>
<strong>Joystick Functionality</strong>
</p>
<p>
	Using our MCP connection we were then able to process and handle inputs of Joystick positioning which we learned from this resource: <a href="https://lastminuteengineers.com/joystick-interfacing-arduino-processing/">Joystick Reference</a> was done on an x and y axis. Now that we understood what the values were we could process these values and begin building our scan codes to be transmitted to the other NRF device. 
</p>
<p>
<strong>Motor</strong> <strong>Functionality</strong>
</p>
<p>
<strong>	</strong>Given the H-bridge component of the motor driver, interfacing with the motor driver was not too challenging seeing that we could employ PWM and basic GPIO write functions to control the motor driver. In the motor.h file we built out basic functions such as driving each motor individually then driving for time given a millisecond input in 4 different directions such as forwarded, left, right, and backward.
</p>
<p>
<strong>PI to PI Communication</strong>
</p>
<p>
Now that we laid the groundwork for communication and controls we put it all together in two massive functions called motor_control_from_joystick() and motorDriveReceive(). These two functions are responsible for the main drive and receive functionality between the two PIs. One Pi acts as the controller and the other PI acts as a receiver which is the heart and soul of the RC car. With just two functions we are able to control total PI to PI communication and drive control of the motors and servo connected. Our Mango PIs communicate using 6 basic commands:
</p>
<p>
Forward
</p>
<p>
Backward
</p>
<p>
Right
</p>
<p>
Left
</p>
<p>
Servo
</p>
<p>
Stop
</p>
<p>
	We found that the simplest and most effective way of communicating was through the use of strings as with a 32 byte payload we are given the ability to transmit up to 31 chars + null terminator. Then by using our comparison functions we can then validate and extract the required information to turn the motors on the receiver side. 
</p>
<h2><strong>Member contribution</strong></h2>


<p>
Throughout this project there was a constant back and forth communication between Sahan and Mario as the nature of the project required both parties to be present in order to properly test Pi to Pi communication which at first might sound like a downside but turned out to be a great experience. This feature allowed for constant parallel coding sessions and constant communication between team members allowing for more teamwork and even spread of contribution from all sides.
</p>
<p>
<strong>Sahan Samarakoon:</strong>
</p>
<p>
nrf.h
</p>
<p>
	This is the module that implements the low-level SPI communication routines to configure the nRF24L01. The nRF has multiple operating states, including Standby, Transmit, and Receive modes. Our library handles switching between these modes efficiently, allowing us to use two (or more) Mango Pis to wirelessly communicate in real-time. 
</p>
<p>
us.h and servo.h
</p>
<p>
	Adapted Julie’s code from lecture to ensure the basic functionality of our Ultrasonic range finder and Servo motor. The servo motor is attached to the range finder, which is placed on the car and can freely rotate for a wider field of view.
</p>
<p>
radar.h 
</p>
<p>
	Uses us.h and servo.h to implement a radar to map the car’s surroundings. The servo sweeps from 0° to 180°, stopping at regular intervals. At each position, the ultrasonic sensor measures the distance to the nearest obstacle. The distance readings are stored in an array, creating a basic map of the surroundings.
</p>
<p>
display.h
</p>
<p>
	This module relies on our graphics modules. It contains functions to display the distance array from radar.h on a screen using ‘ASCII art’ to create a visual map.
</p>
<p>
motor.h
</p>
<p>
	I implemented the low-level functions to drive the 2 DC motors in a specified direction using the Pi’s GPIO. PWM is used to control the speed of the motor.
</p>
<p>
<strong>Mario Portillo:</strong>
</p>
<p>
spi_comm.h 
</p>
<p>
I designed and built the slightly higher level function which allows for chip select to work between multiple SPI devices. 
</p>
<p>
Joystick functionality and transmission
</p>
<p>
	I built out the entire joystick functionality of processing the MCP data and converting that to both speed values and commands. These commands were then transmitted using the NRF functions to be sent by the controller and received by the RC car. 
</p>
<p>
motorDrive Receive()  -> Complete processing and receiving of commands between Pis. 
</p>
<p>
	After successfully completing transmission I built out this function which would be run by the RC Pi continuously to handle the receiving and transmission of data back to the controller PI. Through proper handling of transmission codes and extracting of important information such as Speed it became possible to alter motor speed using PWM. 
</p>
<p>
uart_RCshell.c
</p>
<p>
	The “make shell” command will transport you into a new and improved shell environment that will allow for a new program command called “runNRF.” If you type “help” you will see a list of new commands implemented to help support instant peer to peer communication between two mango Pis. Welcome to your very own Pi chat interface where you can send back and forth messages between Pis. IN this script after transmitting information it will automatically switch over to a receiver to wait for a transmission to occur and print out on the screen. 
</p>
<p>
 
</p>
<p>
motor.h
</p>
<p>
	In the motor.h file I primarily contributed to the higher level functions that dealt with communication between the Pis and use of the lower level code to make the motors actually operate on the receiver function in the RC car. 
</p>
<p>
	
</p>
<h2><strong>References</strong></h2>


<p>
<a href="https://www.mouser.com/datasheet/2/297/nRF24L01_Product_Specification_v2_0-9199.pdf">nRF24L01 datasheet here</a>
</p>
<p>
<a href="https://cdn-shop.adafruit.com/datasheets/MCP3008.pdf">MCP3008 datasheet here</a>
</p>
<p>
<a href="https://lastminuteengineers.com/l298n-dc-stepper-driver-arduino-tutorial/">L298N references</a>
</p>
<p>
<a href="https://www.youtube.com/watch?v=MCi7dCBhVpQ&ab_channel=BenEater">SPI Reference </a>
</p>
<p>
<a href="https://lastminuteengineers.com/joystick-interfacing-arduino-processing/">Joystick Reference</a>
</p>
<p>
Our MangoPi NRF library was inspired by <a href="https://github.com/controllerstech/NRF24L01/blob/master/NRF24L01.c">this</a> amazing library by ControllersTech for an STM32 microcontroller.
</p>
<h2><strong>Self-evaluation</strong></h2>


<p>
Given our outline and project goals in our proposal we feel that we greatly surpassed even some of our extension goals we had in mind with this project. Once we were able to crack radio communication using the NRF we went to fully relying on our module to transmit the correct messages and correct bits.  We were both simply shocked when we got the first “Hello world” outputted on the screen. From then on we knew that we had the capability of completely controlling another Pi. This fundamentally sound base allowed us to build upward and to focus on much higher level issues such as motor control and user interface with a joystick. The fact that we were able to crack NRF communication in the matter of a couple days gave us the freedom to dive deep into our design for building upon th SPI module, having a sound Joystick interface, and even a data collection module that collects servo data and transmits it back to the controller to be displayed on the screen. Given all these added features along with the motor control we felt as though we had a pretty cool project that almost emulates a data collection rover. 
</p>
<p>
Throughout this project we believe one of the key things we developed was a true understanding of working with peripheral devices to lead to data collection, data transmission, and data display. We felt these were all key parts of our project which we were able to go ahead and complete throughout our time working on this project. We thoroughly enjoyed simply building upon the foundation we had in the form of lower level code to then abstract away the low level functionality to just focus on output and data manipulation. At many points throughout the project we just kept saying “Wow I cannot believe how good the radio communication is.” This was a direct result of two of our key learning objectives which we came to master. These included debugging peripheral devices and understanding how to read data sheets to properly interface with these device peripherals. 
</p>
<p>
Another very important learning objective we accomplished was learning how to work with our tools in order to troubleshoot and build out. Our makefile was greatly adapted to showcase multiple program executables that could be run on our Pis. Since we were needing two Pis firing at once we realized a main priority from the start was to have a solid makefile that linked together all the different files we would pull from and also build the correct modules. When it came to the hardware we had to become even more accustomed to using logic analyzers to understand SPI and how chip select works.  During our second lab meeting we were having significant issues with chip select as we were trying to implement MCP so to combat this issue we used the logic analyzer to diagnose our issue. 
</p>
<p>
Overall we had a very good experience building out and debugging both the hardware and software issues we came across. In the end we were able to complete communication between two mango PIs and allow for back and forth transmission of commands such that data was able to be processed and handled. We were really happy with our end product and felt that both the code and hardware really came together nicely in the end. 
</p>
<h2><strong>Photos</strong></h2>


<p>
<a href="https://github.com/cs107e/win25-project-Sahan-Samarakoon-mariodportillo/blob/master/Demo_Day.HEIC">Demo_Day.HEIC</a>
</p>
<p>
<a href="https://github.com/cs107e/win25-project-Sahan-Samarakoon-mariodportillo/blob/master/Final_Robot.HEIC">Final_Robot.HEIC</a>
</p>
<p>
<a href="https://github.com/cs107e/win25-project-Sahan-Samarakoon-mariodportillo/blob/master/simple-motor.jpeg">simple-motor.jpeg</a>
</p>