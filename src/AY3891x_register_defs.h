/* -----------------------------------------------------------------
   AY3891x Library
   https://github.com/Andy4495/AY3891x

   This file contains definitions for the various register fields
   in the AY-3-891x chips.

   1/1/21 - A.T. - Original

*/

#ifndef AY3891X_REGISTER_DEFS
#define AY3891X_REGISTER_DEFS

// Tone Generator Control, Course and Fine
// Register 0x0 - AY3891x::ChA_Tone_Period_Fine_Reg
// Register 0x1 - AY3891x::ChA_Tone_Period_Coarse_Reg
// Register 0x2 - AY3891x::ChB_Tone_Period_Fine_Reg
// Register 0x3 - AY3891x::ChB_Tone_Period_Coarse_Reg
// Register 0x4 - AY3891x::ChC_Tone_Period_Fine_Reg
// Register 0x5 - AY3891x::ChC_Tone_Period_Coarse_Reg
#define TONE_GENERATOR_COARSE      0x0F
#define TONE_GENERATOR_FINE        0xFF
// Noise Generator Control
// Register 0x6 - AY3891x::Noise_Period_Reg
#define NOISE_GENERATOR_PERIOD     0x1F
// Mixer Control - I/O Enable
// Register 0x7 - AY3891x::Enable_Reg
// Note that the MIXER enable bits are active LOW
#define MIXER_INPUT_B_DISABLE      0x80
#define MIXER_INPUT_A_DISABLE      0x40
#define MIXER_NOISE_C_DISABLE      0x20
#define MIXER_NOISE_B_DISABLE      0x10
#define MIXER_NOISE_A_DISABLE      0x08
#define MIXER_TONE_C_DISABLE       0x04
#define MIXER_TONE_B_DISABLE       0x02
#define MIXER_TONE_A_DISABLE       0x01
#define MIXER_INPUTS_DISABLE       (MIXER_INPUT_B_DISABLE | MIXER_INPUT_A_DISABLE)
#define MIXER_NOISES_DISABLE       (MIXER_NOISE_C_DISABLE | MIXER_NOISE_B_DISABLE | MIXER_NOISE_A_DISABLE)
#define MIXER_TONES_DISABLE        (MIXER_TONE_C_DISABLE | MIXER_TONE_B_DISABLE | MIXER_TONE_A_DISABLE)
#define MIXER_ALL_DISABLED         (MIXER_INPUTS_DISABLE | MIXER_NOISES_DISABLE | MIXER_TONES_DISABLE)
// Amplitude Control
// Registers 0x8 - AY3891x::ChA_Amplitude
// Registers 0x9 - AY3891x::ChB_Amplitude
// Registers 0xA - AY3891x::ChC_Amplitude
#define AMPLITUDE_CONTROL_MODE      0x10
#define AMPLITUDE_CONTROL_LEVEL     0x0F
// Envelope Period Control
// Register 0xB - AY3891x::Env_Period_Fine_Reg
// Register 0xC - AY3891x::Env_Period_Coarse_Reg
// Envelop3 Shape/Cycle Control
// Register 0xD - AY3891x::Env_Shape_Cycle
#define ENVELOPE_CONTROL_CONTINUE     0x08
#define ENVELOPE_CONTROL_ATTACK       0x04
#define ENVELOPE_CONTROL_ALTERNATE    0x02
#define ENVELOPE_CONTROL_HOLD         0x01
// I/O Port Data Store
// Register 0xE - AY3891x::IO_Port_A_Reg
// Register 0xF - AY3891x::IO_Port_B_Reg
#endif
