# Human Activity Recognition with angular speed and angular acceleration sensors.Application in bluetooth headphones.


## Project Overview
This project implements an AI-based activity recognition system on the Nucleo-F429ZI microcontroller. The system uses two sensors, **L3G4200D** (gyroscope) and **ADXL345** (accelerometer), to collect motion data. The embedded AI model processes this data and activates one of six LEDs corresponding to the detected activity:

1. Walking
2. Climbing stairs (up)
3. Climbing stairs (down)
4. Lying
5. Sitting
6. Standing

The goal is to demonstrate the deployment of an AI model on an embedded system for real-time human activity recognition with visual LED output.

---

## Hardware Requirements
- **STM32 Nucleo-F429ZI development board**
- **L3G4200D gyroscope sensor**
- **ADXL345 accelerometer sensor**
- 6 LEDs with corresponding resistors
- Jumper wires for connections
- USB cable for programming and power supply

---

## Software Requirements
- **STM32CubeIDE**: For firmware development
- **Python**: For data preprocessing and AI model training
- **TensorFlow/Keras**: For building and training the AI model
- **STM32CubeMX**: For peripheral configuration

---

## System Architecture
### 1. Data Collection
- **Sensors used**:
  - **L3G4200D**: Measures angular velocity (X, Y, Z axes).
  - **ADXL345**: Measures linear acceleration (X, Y, Z axes).
- Data is collected from these sensors and sent to the STM32 Nucleo board.

### 2. Preprocessing
- Raw sensor data is normalized and scaled.
- A sliding window technique is used to segment data into fixed-size samples (e.g., 2-second windows).

### 3. AI Model
- **Model architecture**: A neural network trained using Python with TensorFlow/Keras.
- **Output**: Classification of six activities.
- Model weights are exported as a `.bin` file for integration into the firmware.

### 4. Deployment
- The trained AI model is integrated into the STM32 project as a `.bin` file.
- The firmware is programmed to activate one of six LEDs based on the AI output.

---

## Circuit Connections
### L3G4200D to Nucleo-F429ZI
- **VCC** -> 3.3V
- **GND** -> GND
- **SDA** -> PB7
- **SCL** -> PB6

### ADXL345 to Nucleo-F429ZI
- **VCC** -> 3.3V
- **GND** -> GND
- **SDA** -> PB9
- **SCL** -> PB8

### LEDs to Nucleo-F429ZI
- Connect each LED anode to the corresponding GPIO pin (e.g., PA0 to PA5).
- Connect each LED cathode to a resistor, then to GND.

---

## How to Run
### 1. Clone the Repository
Clone the project repository to your local machine.
```bash
git clone <repository-link>
```

### 2. Configure the Project
1. Open the project in **STM32CubeIDE**.
2. Verify the I2C configuration for both sensors (L3G4200D and ADXL345).
3. Import the AI model weights (`.bin` file) into the project.

### 3. Flash the Firmware
1. Connect the Nucleo board to your computer via USB.
2. Build the project in STM32CubeIDE.
3. Flash the firmware onto the Nucleo board.

### 4. Test the System
1. Power the Nucleo board.
2. Perform one of the six activities near the sensors.
3. Observe the corresponding LED lighting up based on the detected activity.

---

## AI Model Training
### 1. Data Collection
- Collect labeled motion data for the six activities from the sensors and save it in CSV format.

### 2. Preprocessing
- Normalize raw sensor data.
- Use a sliding window approach to generate input samples for the model.

### 3. Model Development
- Use Python and TensorFlow/Keras to build and train the AI model.
- Export the trained model weights as a `.bin` file.

### 4. Model Deployment
- Integrate the `.bin` file into the STM32 firmware.

---

## Example Output
**System Output (LEDs)**:
- **Walking**: LED 1 lights up
- **Climbing stairs (up)**: LED 2 lights up
- **Climbing stairs (down)**: LED 3 lights up
- **Lying**: LED 4 lights up
- **Sitting**: LED 5 lights up
- **Standing**: LED 6 lights up

---

## Troubleshooting
1. **No sensor data**: Check sensor connections and I2C configuration.
2. **LEDs not lighting up**: Verify GPIO pin configurations and connections.
3. **Incorrect activity classification**: Revalidate model accuracy and ensure correct data preprocessing.

---

## Future Improvements
- Use a more compact AI model for deployment.
- Add support for additional activities.
- Replace LEDs with an LCD display for better visualization.

---

## Credits
- Project by: ルオン・ザ・ギエム
- AI Model Training: TensorFlow/Keras
- Deployment Tools: STM32CubeIDE

---

## License
This project is licensed under the MIT License. See the LICENSE file for details.

