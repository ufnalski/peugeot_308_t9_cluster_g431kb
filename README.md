# Peugeot 308 T9 (2014+) instrument panel cluster hacking (STM32G431KB)
A similar story as for the [Mustang](https://github.com/ufnalski/ford_mustang_cluster_h503rb). Let me therefore not to retell the whole story here.

![Peugeot 308 T9 IPC in action](/Assets/Images/another_peugeot_308_t9_ipc_in_action.jpg)

Check [/Assets/Images/](/Assets/Images/) for the pinout and the CAN bus settings.

# Missing files?
Don't worry :slightly_smiling_face: Just hit Alt-K to generate /Drivers/CMCIS/ and /Drivers/STM32G4xx_HAL_Driver/ based on the .ioc file. After a couple of seconds your project will be ready for building.

# Termination (pull-together) resistors in ISO 11898-2, also called high-speed CAN (HS-CAN)[^1]
## Let's start with why...
* [Why Are Termination Networks in CAN Transceivers So Important?](https://www.ti.com/lit/ta/ssztb40/ssztb40.pdf) (Texas Instruments)
* [How to test if your CAN termination works correctly](https://kvaser.com/developer-blog/how-to-test-your-can-termination-works-correctly/) (Kvaser)
* [Using termination to ensure recessive bit transmission](https://kvaser.com/using-termination-ensure-recessive-bit-transmission/) (Kvaser)
* [CAN Physical Layer and Termination Guide](https://www.ni.com/en/support/documentation/supplemental/09/can-physical-layer-and-termination-guide.html) (NI)

The terminating resistors are there essentially to take care of two things: preventing reflections at the ends of the transmission line[^2] and enabling the recessive state of the bus to occur.

[^1]: [CAN bus](https://en.wikipedia.org/wiki/CAN_bus) (Wikipedia)
[^2]: [Reflections of signals on conducting lines](https://en.wikipedia.org/wiki/Reflections_of_signals_on_conducting_lines) (Wikipedia)

## ... and continue with some calculations to understand why the standard specifies the 120 Ohms terminators on both ends

* [Dielectric Constants of Insulations](https://www.omnicable.com/technical-resources/dielectric-constants-of-insulations)
* [Twisted-Pair Impedance Calculator](https://uk.farnell.com/twisted-pair-impedance) (Farnell)
* [AN5878 How to design a robust automotive CAN system](https://www.st.com/resource/en/application_note/an5878-how-to-design-a-robust-automotive-can-system-stmicroelectronics.pdf) (ST)

![Twisted-Pair Impedance Calculator (Farnell)](/Assets/Images/twisted_pair_impedance_calculator.JPG)

The parasitic capacitance of the bus then limits the transition speed from the dominant state to the recessive one. That is why the standard specifies cable max length for a given bitrate.

In the case of low speed, e.g. 125 kbps, and a short line (mine is less than 1 meter long), you can make it operational on a lab/home workbench even with a non-twisted pair and no termination resistor on the IPC side - the 120 Ohms termination on the [transceiver evaluation board](https://www.waveshare.com/wiki/SN65HVD230_CAN_Board) is sufficient. However, if you want to make the bus up to the specs, use a dedicated twisted pair and add the second termination.

![HS-CAN termination](/Assets/Images/hs_can_termination.png)

# Pull-up and pull-down resistors in ISO 11898-3, also called low-speed (LS) or fault-tolerant CAN (up to 125 kbit/s)
* [CAN-Bus: Introduction and History](https://resources.altium.com/p/Controller-Area-Network-Bus-Introduction-and-History) (Altium)
* [CAN Physical Layer and Termination Guide](https://www.ni.com/en/support/documentation/supplemental/09/can-physical-layer-and-termination-guide.html) (NI)
* [CAN Low Speed + interfejs High Speed = Sukces!](https://www.youtube.com/watch?v=JCZyb7mBZMo) (Systemy Wbudowane)
* [Can High Speed vs. Can Low Speed - różnice między protokołami](https://systemywbudowane.pl/can-high-speed-vs-can-low-speed-roznice-miedzy-protokolami/) (Systemy Wbudowane)
* [CAN Bus Explained - A Simple Intro [2024]](https://www.csselectronics.com/pages/can-bus-simple-intro-tutorial) (CSS Electronics)
* [Determining the Necessary Termination Resistance for the Board](https://www.ni.com/docs/en-US/bundle/ni-xnet/page/determining-the-necessary-termination-resistance-for-the-board.html) (NI)

![LS-CAN termination](/Assets/Images/ls_can_termination.png)

# Is my device ISO 11898-2 or ISO 11898-3?
Easy to figure out if the relevant resistors are already the part of the device. Simply measure CAN_L to CAN_H, CAN_H to GND and CAN_L to V_DD resistances. If the pull-together resistor is present it is ISO 11898-2. If CAN_H is pulled down to GND and CAN_L is pulled up to V_DD it is ISO 11898-3. None of the two? You have to add the relevant resistors. Most evaluation boards popular among DIYers implement pull-together resistor or no resistors at all as they use a HS-CAN transceiver:
* [SN65HVD230 CAN Board](https://www.waveshare.com/wiki/SN65HVD230_CAN_Board),
* [Moduły CAN](https://sklep.msalamon.pl/kategoria/moduly/komunikacyjne/can/).

To use them with this particular IPC, i.e. the LS-CAN device, you have to remove the pull-together resistor and add the two resistors compliant with the fault-tolerant CAN. For some inspiration please visit [/Assets/Images/](/Assets/Images/). Obviously, the communication between the devices can go into the single-wire mode only if their transceivers are compatible with this mode. However, as long as both CAN_L and CAN_H lines are fully-functional, you don't need a fault-tolerant transceiver to successfully talk with the fault-tolerant one - the "regular" one will do the job equally well.

![HS-CAN and LS-CAN transceivers talking with each other](/Assets/Images/fault_tolerant_can_physical_layer.png)

# What's inside Peugeot 308 T9 IPC?
* [TJA1055 Enhanced fault-tolerant CAN transceiver](https://www.nxp.com/docs/en/data-sheet/TJA1055.pdf) (NXP)

> Bus failure management:
> - Supports single-wire transmission modes with ground offset voltages up to 1.5 V;
> - Automatic switching to single-wire mode in the event of bus failures, even when the CAN_H bus wire is short-circuited to V_CC;
> - Automatic reset to differential mode if bus failure is removed;
> - Full wake-up capability during failure modes.

# What if
What if a transceiver with the pull-together resistor is used? You will still be able to send messages to the IPC. However, you will be unable to read the ACK bit send by the cluster or any other messages send by the cluster. BTW, the exemplary code reads the brightness button on the left-hand side of the cluster and sets the brightness accordingly. The inability to read messages from the IPC would come from automatic switching to single-wire (single-ended, non-differential) mode in the event of bus failures. And yes, connecting the pull-together resistor to that IPC is correctly interpreted as a partial bus failure. Nevertheless, these are just my ramblings in case of you wondering what if. There is no point in mixing both approaches on one bus. Simply do it right and arrange the resistors on your side to reflect the physical layer on the IPC side.

![Waveshare LS-CAN termination](/Assets/Images/waveshare_ls_can_termination.jpg)

# Call for action
Create your own [home laboratory/workshop/garage](http://ufnalski.edu.pl/control_engineering_for_hobbyists/2024_dzien_otwarty_we/Dzien_Otwarty_WE_2024_Control_Engineering_for_Hobbyists.pdf)! Get inspired by [ControllersTech](https://www.youtube.com/@ControllersTech), [DroneBot Workshop](https://www.youtube.com/@Dronebotworkshop), [Andreas Spiess](https://www.youtube.com/@AndreasSpiess), [GreatScott!](https://www.youtube.com/@greatscottlab), [ElectroBOOM](https://www.youtube.com/@ElectroBOOM), [Phil's Lab](https://www.youtube.com/@PhilsLab), [atomic14](https://www.youtube.com/@atomic14), [That Project](https://www.youtube.com/@ThatProject), [Paul McWhorter](https://www.youtube.com/@paulmcwhorter), [Max Imagination](https://www.youtube.com/@MaxImagination), and many other professional hobbyists sharing their awesome projects and tutorials! Shout-out/kudos to all of them!

> [!WARNING]
> Automotive hacking - do try this at home :sunglasses:

190+ challenges to start from: [Control Engineering for Hobbyists at the Warsaw University of Technology](http://ufnalski.edu.pl/control_engineering_for_hobbyists/Control_Engineering_for_Hobbyists_list_of_challenges.pdf).

Stay tuned!
