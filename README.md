🔌 Hardware Overview

🔧 Components:
•ESP32 Dev Module
•MAX9814 Analog Microphone
•12V WS2811 RGB Addressable LED Strip (75 LEDs)
•12V DC Power Supply

🔗 Wiring Summary:
•MAX9814
•OUT → GPIO 36 (ADC input)
•VCC → 3.3V
•GND → GND
•WS2811 LED Strip
•DIN (Data In) → GPIO 14
•VCC → 12V power
•GND → shared with ESP32

🧠 Key Code Concepts

🔊 Audio Processing
•ESP32 reads analog mic data (analogRead(MIC_PIN))
•Calculates the amplitude (sound intensity) by removing DC bias and taking the absolute value
•Smooths the amplitude to avoid flickering

🎨 LED Visualization
•Maps loudness to number of LEDs lit (map(audio, 0, MAP_MAX, 0, NUM_LEDS))
•Displays a rainbow-colored volume bar using colorWheel()
•Adds a fall-off effect: if the beat drops, the light bar decays smoothly instead of snapping off

⚙️ Customizable Parameters
•THRESHOLD – filters out background noise
•MAP_MAX – adjusts how loud the music must be to reach full brightness
•BRIGHT_MAX – controls brightness to protect eyes or lower power
•FALL_RATE – controls how quickly LEDs fade after a beat

📈 Outcomes
•Implemented real-time analog audio processing on ESP32
•Learned to control WS2811 RGB strips with NeoPixel library
•Understood color mapping using custom rainbow functions
•Tuned parameters to match environmental sound characteristics
