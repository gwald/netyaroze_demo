Hi,

This is a simple sound 2 light program ( knocked up today/yesterdau )
Load up main, put a music CD in the CD player, and type go.

Controls as follows:

Left / Right : Change track
Up / Down : Change CD volume
Cross : Waveform view
Square : Polar view
Circle : Frequency plot
Triangle : Change colours
L1 / L2 : Set sample & hold rate
R1 : Toggle persistence on/off
R2 : Show timing info ( If number changes wildly, alter hold rate until it stabalises )
Start : Toggle text on/off
Select : Terminate....


At the moment I'm only using GsGline's to draw the waveform and frequency data.
I'll add some dynamic TMDs in the next couple of days...

If you want to use ReadCDDA in your own code, just add cdfeed.o to your makefile

ReadCDDA returns a pointer to an array of shorts containing 16 bit audio samples.
[0-255] are the left channel samples, and [512-767] are the right channel samples.
The sample rate is fixed at 44.1KHz


I'll add a general sampling routine when I get the chance, as I reckon it'd be quite cool.


Cheers,

Colin.