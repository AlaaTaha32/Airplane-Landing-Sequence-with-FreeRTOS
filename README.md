# Airplane Landing Sequence with FreeRTOS
A system simulating airplane landing sequence (landing - take-off - stable) on ARM Tiva C TM4C123GH6PM microcontroller.
The system also includes an 7 segment display that provides real-time information about the airplane's status during the landing sequence, such as task number 
The display is updated in real-time using FreeRTOS, which ensures that critical tasks running.

The tasks are ordered based on the priority as follows:

1- Landing Task

2- Take-off Task

3- Stable Task

Landing task, when switch 2 on tiva is pressed this task will run and Red led is on and display number 1 on 7 segment. 

Take off when switch 1 on tiva is pressed this task will run and green led is on and number 2 on 7 segment

Stable task with low priority and periodic task will run after take off or landing finished and display on 7 segment number 3.

When system will be on idle state the yellow, pink and sky blue leds will run using round robin scheduling technique.



