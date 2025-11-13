# GruvBok

A minimal MIDI controller for Teensy 4.1 that plays a four-on-the-floor drum beat over USB MIDI.

## Hardware

- Teensy 4.1 microcontroller
- USB connection for MIDI

## Features

- USB MIDI output
- 120 BPM tempo
- Four-on-the-floor kick drum pattern (GM Standard Bass Drum on MIDI channel 10)

## Building

This project uses PlatformIO. To build and upload:

```bash
pio run -t upload
```

## MIDI Output

- Channel: 10 (drums)
- Note: 36 (Bass Drum 1)
- Velocity: 100
- Tempo: 120 BPM (500ms per beat)
