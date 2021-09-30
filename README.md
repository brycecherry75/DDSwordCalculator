#DDSwordCalculator Library#

DDSwordCalculator is a library for high precision calcuation of a word for a Direct Digital Synthesizer (DDS).

Revisions

1.0.0	First release

USAGE:

ArrayToSingle(DDSresolution, *DDSwordArray) - converts *DDSwordArray to a uint64_t

convert(*DDSreference, *DDSfrequency, DDSresolution, uint8_t *DDSword) - generate *DDSword based on *DDSreference and *DDSfrequency char strings and DDSresolution (8-48) - some DDS devices such as the Analog Devices AD9854 use a frequency word longer than 32 bits (48 bits is the largest known DDS tuning word length) and an 8 bit array output takes into account issues with compiler uint64_t support

uint64_t support including bitshifts is required by the compiler; otherwise, only the least significant 32 bits will be handled by ArrayToSingle.