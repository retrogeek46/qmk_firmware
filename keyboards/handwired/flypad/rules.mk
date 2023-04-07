# MCU info
MCU_FAMILY = KINETIS

# KL2x for Teensy LC
# K20x for Teensy 3.x
MCU_SERIES = K20x

# MKL26Z64 for Teensy LC
# MK20DX128 for Teensy 3.0
# MK20DX256 for Teensy 3.1 and 3.2
MCU_LDSCRIPT = MK20DX128

# kl2x for Teensy LC
# k20x5 for Teensy 3.0
# k20x7 for Teensy 3.1 and 3.2
MCU_STARTUP = k20x5

# PJRC_TEENSY_LC for Teensy LC
# PJRC_TEENSY_3 for Teensy 3.0
# PJRC_TEENSY_3_1 for Teensy 3.1 or 3.2
BOARD = PJRC_TEENSY_3

# cortex-m0 for Teensy LC
# cortex-m4 for Teensy 3.x
MCU = cortex-m4

# 6 for Teensy LC
# 7 for Teensy 3.x
ARMV = 7

# Bootloader
BOOTLOADER = halfkay

# Build Options
#   change yes to no to disable
#
BOOTMAGIC_ENABLE = yes       # Virtual DIP switch configuration
MOUSEKEY_ENABLE = yes       # Mouse keys
EXTRAKEY_ENABLE = yes       # Audio control and System control
CONSOLE_ENABLE = no        # Console for debug
COMMAND_ENABLE = yes        # Commands for debug and configuration
# Do not enable SLEEP_LED_ENABLE. it uses the same timer as BACKLIGHT_ENABLE
SLEEP_LED_ENABLE = no       # Breathing sleep LED during USB suspend
# if this doesn't work, see here: https://github.com/tmk/tmk_keyboard/wiki/FAQ#nkro-doesnt-work
NKRO_ENABLE = yes            # USB Nkey Rollover
BACKLIGHT_ENABLE = no       # Enable keyboard backlight functionality
RGBLIGHT_ENABLE = no        # Enable keyboard RGB underglow
MIDI_ENABLE = no            # MIDI support
BLUETOOTH_ENABLE = no       # Enable Bluetooth with the Adafruit EZ-Key HID
AUDIO_ENABLE = no           # Audio output on port C6
FAUXCLICKY_ENABLE = no      # Use buzzer to emulate clicky switches
HD44780_ENABLE = no         # Enable support for HD44780 based LCDs
