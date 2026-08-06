<div align="center">

# S-REHAB

### Wireless Wearable System for Simultaneous sEMG Acquisition and Kinematic Analysis in Upper-Limb Rehabilitation

*Quantitative, objective monitoring of shoulder range of motion and muscular activation for Subacromial Impingement Syndrome (SIS) rehabilitation*

![Status](https://img.shields.io/badge/status-MVP%20%2F%20preliminary%20validation-yellow)
![Platform](https://img.shields.io/badge/platform-ESP32%20%7C%20RP2040-blue)
![License](https://img.shields.io/badge/license-TBD-lightgrey)
![Build](https://img.shields.io/badge/build-TODO-inactive)
![Made at](https://img.shields.io/badge/institution-UPCH-red)

<!-- ![Hero image placeholder](docs/images/hero_banner.png) -->

</div>

---

## Overview

**S-REHAB** is a modular, wireless wearable system for the **quantitative monitoring of upper-limb rehabilitation** in patients with **Subacromial Impingement Syndrome (SIS)**. It combines two independent, body-worn acquisition modules — an **arm module** (inertial + surface electromyography) and a **chest module** (electrocardiography) — that stream data over **Bluetooth Low Energy (BLE)** to a host application built in **Unity 3D**, with a companion **web platform** for deferred/remote review by clinicians.

The system was designed to address a specific clinical gap: during shoulder rehabilitation exercises (flexion/abduction), patients with SIS often develop **compensatory movement patterns** — most notably premature scapular elevation driven by upper-trapezius hyperactivation — which reduce therapeutic effectiveness and can contribute to chronic injury. Conventional clinical follow-up (manual goniometry, visual observation) cannot continuously and simultaneously capture both **kinematics** and **muscle activity**, and instrumented biomechanics/EMG lab systems are typically too costly, non-portable, and cable-constrained for routine clinical use.

S-REHAB targets:

- **Patients** undergoing conservative treatment for SIS.
- **Physiotherapists** supervising outpatient rehabilitation sessions who need objective, continuous indicators of exercise execution.
- **Researchers** developing low-cost, Edge-Computing-based biomedical monitoring architectures.

> **Validation status:** Prototype validation to date has been performed on **healthy volunteers** (18–30 years old, physically active), not on SIS patients. Results reported here describe technical feasibility, not clinical efficacy.

---

## Motivation

- Musculoskeletal disorders (MSDs) affect an estimated **1.71 billion people worldwide** (WHO), and Subacromial Impingement Syndrome is one of the most frequent causes of shoulder pain in adults, with an estimated prevalence of **7%–26%** in the general population, particularly among people performing repetitive overhead activities (swimming, volleyball, manual labor).
- The characteristic "painful arc" (70°–120° of shoulder flexion/abduction) promotes compensatory patterns such as premature scapular elevation caused by upper-trapezius hyperactivation, which lowers therapeutic effectiveness and can favor chronification of the injury.
- **Manual goniometers** — the current standard in most clinical follow-up — cannot continuously track movement kinematics nor simultaneously assess muscle activity during exercise execution.
- **Instrumented biomechanics and clinical EMG systems** provide high-precision objective measurement but are expensive, poorly portable, require multiple wired connections, and can alter the patient's natural biomechanics during evaluation.
- A **low-cost, wireless wearable device** capable of combined biomechanical, physiological, and systemic monitoring — with real-time, localized physical biofeedback that minimizes cognitive load and does not interfere with natural movement — has the potential to become a precise, accessible tool for modern physical therapy.

---

## Key Features

| Feature | Description |
|---|---|
| **Modular wireless architecture** | Two physically independent acquisition modules (arm and chest), each with isolated LiPo power |
| **Edge Computing** | Quaternion-based orientation math and sEMG envelope processing performed locally on the microcontrollers, minimizing wireless payload |
| **BLE communication** | End-to-end Bluetooth Low Energy link streaming packed frames to the host at **10 Hz** |
| **Real-time biofeedback** | Three high-intensity LEDs on the arm module provide immediate color-coded visual guidance (green / yellow / blue) plus an orange idle-state LED |
| **Unity 3D visualization** | Interactive host application driving a virtual anatomical humanoid model in real time |
| **sEMG monitoring** | Upper-trapezius muscle activation via MyoWare 2.0, normalized to %MVIC (Maximum Voluntary Isometric Contraction) |
| **Shoulder ROM estimation** | Quaternion-based orientation tracking (BNO055 9-DoF IMU) avoiding gimbal lock near 90° flexion |
| **ECG monitoring** | 3-lead electrocardiography via HealthyPi 5 (MAX30001), heart-rate estimation |
| **Local SQLite logging** | Session data (angle, %MVIC, heart rate, pain events) stored locally by the Unity host |
| **Web dashboard** | Flask + REST/JSON backend for deferred review of session adherence and progress |
| **Pain-event marker** | Physiotherapist-triggered "pain button" freezes and timestamps angle/%MVIC values in the local database |

---

## System Architecture

S-REHAB is organized into **three functional layers**, chosen to decentralize processing and minimize wireless transfer load:

```
┌──────────────────────────────┐      ┌──────────────────────────────┐
│   ACQUISITION LAYER (Edge)   │      │   ACQUISITION LAYER (Edge)   │
│         Arm Module           │      │        Chest Module          │
│  ESP32 + BNO055 + MyoWare2.0 │      │   HealthyPi 5 (RP2040 +      │
│  (ROM via quaternions,       │      │   MAX30001, 3-lead ECG)      │
│   sEMG %MVIC, LED biofeedback)│      │                              │
└───────────────┬───────────────┘      └───────────────┬───────────────┘
                │ BLE, packed frames @ 10 Hz            │ BLE
                └──────────────────┬─────────────────────┘
                                   ▼
                  ┌─────────────────────────────────┐
                  │   LOCAL INTERFACE LAYER (Host)   │
                  │        Unity 3D application       │
                  │ • Drives virtual anatomical model │
                  │ • Manages alarms / LED logic      │
                  │ • Local SQLite storage             │
                  └──────────────────┬────────────────┘
                                   │ HTTP POST, JSON (REST API)
                                   ▼
                  ┌─────────────────────────────────┐
                  │     CLOUD LAYER (Telemonitoring)  │
                  │  Flask web app + aggregated data  │
                  │ • Max ROM, average variables      │
                  │ • Alarms, detected pain events    │
                  │ • Browser-based adherence review  │
                  └─────────────────────────────────┘
```

**Data flow summary:**
1. Each Edge module filters and pre-processes its own biopotential/physical signal locally.
2. Only processed variables (angle, %MVIC, heart rate) are transmitted via BLE — not raw signal streams.
3. The Unity host renders the anatomical model, evaluates the LED biofeedback logic, and persists every sample/repetition to a local SQLite database.
4. At the end of a session, summarized records are serialized to JSON and pushed via an HTTPS REST endpoint to the web platform for clinician review.

---

## Hardware

| Component | Purpose | Module |
|---|---|---|
| **ESP32 DevKit V1** | Main microcontroller: I²C sensor polling, quaternion math, sEMG ADC sampling, BLE transmission, LED control | Arm |
| **BNO055 (9-DoF IMU)** | Absolute quaternion orientation output for gimbal-lock-free ROM estimation | Arm |
| **MyoWare 2.0** | Surface EMG sensor (rectified/integrated envelope output), placed on upper trapezius | Arm |
| **3× high-intensity LEDs** | Local visual biofeedback (green / yellow / blue) | Arm |
| **1× battery-status LED** | Battery state indication | Arm |
| **HealthyPi 5 platform** | ECG acquisition front-end | Chest |
| **RP2040 microcontroller** | Chest-module processing (part of HealthyPi 5) | Chest |
| **MAX30001** | Clinical-grade analog front-end IC for ECG acquisition | Chest |
| **Ag/AgCl electrodes (3-lead array)** | Electrocardiographic signal pickup | Chest |
| **Independent LiPo cells** | Galvanically isolated, floating power supply per module (IEC 60601-1-oriented design) | Arm & Chest |
| **3D-printed PLA enclosures** | Mechanical housing (Autodesk Fusion 360 design, 3 mm assembly tolerance) | Arm & Chest |

> **Note:** Custom PCBs were designed but **not fabricated** for this prototype due to time/budget constraints; circuits were implemented on perforated protoboards with hand soldering. The MyoWare 2.0 Link Shield accessory was unavailable commercially, so the sEMG sensor was connected to the ESP32 via external wiring. Both items are listed as future work.

---

## Software Stack

| Layer | Language / Framework | Engine / Runtime | Key Libraries / Concepts | Database | Communication Protocol |
|---|---|---|---|---|---|
| Firmware (Arm) | C/C++ (Arduino/ESP-IDF style, TODO: confirm exact toolchain) | ESP32 | Quaternion algebra, I²C, 12-bit ADC sampling, exponential moving-average filter | — | BLE |
| Firmware (Chest) | TODO (HealthyPi 5 / RP2040 firmware stack) | RP2040 | MAX30001 ECG front-end driver | — | BLE |
| Host Application | C# | **Unity 3D** | Real-time anatomical model rendering, frame parsing at 10 Hz, alarm/state logic | **SQLite** (local) | BLE (device ↔ host), REST (host ↔ cloud) |
| Signal Conditioning Tool | Web-based (TODO: confirm stack) | — | Notch filter (50 Hz), high-pass (0.5 Hz), low-pass (40 Hz) for ECG conditioning | — | — |
| Cloud / Telemonitoring | **Python** | **Flask** | REST API endpoint, JSON serialization | **SQLite** (server-side) | REST API over HTTPS |

---

## Signal Processing Pipeline

**1. IMU orientation (Arm module):**
To avoid Gimbal Lock near the 90° flexion boundary (a known limitation of Euler-angle representations), the BNO055's internal processor outputs absolute spatial quaternions directly (`q = w + xi + yj + zk`). At session start, the ESP32 stores a neutral-position offset quaternion (`q_offset`), and on every cycle computes the true relative angle via quaternion multiplication:

```
q_rel = q_offset⁻¹ ⊗ q_actual
```

Angular velocity (ω) is derived from the change in angle over the sampling interval and smoothed with a **first-order exponential moving-average filter** to attenuate drift noise.

**2. sEMG normalization (Arm module):**
The MyoWare 2.0 outputs an already-rectified and integrated analog envelope (its `ENV` output), not raw EMG. Muscle activation amplitude is estimated as the **peak value observed within 100 ms windows** of this envelope (avoiding a digital RMS computation that would require the raw signal). To mitigate inter-subject variability in skin impedance and muscle volume, this amplitude is normalized as a percentage of the peak recorded during an initial **Maximum Voluntary Isometric Contraction (MVIC)** calibration:

```
%MVIC = (Amplitude_current / Amplitude_peak,MVIC) × 100
```

**3. LED biofeedback logic** *(evaluated at 10 Hz against Ludewig & Cook criteria)*:

| LED Color | Meaning | Condition |
|---|---|---|
| Green | Expected execution pattern | ROM ≥ 90° **AND** %MVIC < 25% |
| Yellow | Mild deviation | ROM between 70°–90° **OR** %MVIC between 25%–35% |
| Blue | Critical alert / possible compensation | %MVIC > 35% **OR** (ROM < 70° **AND** angular velocity ≈ 0 sustained >1–2 s) **OR** manual pain-button interruption |
| Orange | System idle/rest state (pre-session) | Deactivated automatically at first detected movement |

**4. ECG acquisition (Chest module):** 3-lead Ag/AgCl electrode array → MAX30001 analog front-end → digital conditioning with a 50 Hz notch filter, 0.5 Hz high-pass filter, and 40 Hz low-pass filter (configurable via a dedicated web tool) → instantaneous heart-rate estimation.

**5. BLE transmission:** Both modules transmit processed (not raw) variables to the Unity host at **10 Hz**, minimizing radio payload and host-side computational load.

**6. Unity 3D host:** Drives the virtual anatomical model, computes instantaneous geometric angle, tracks maximum ROM, counts repetitions (odometer), logs angular velocity, evaluates LED alarm logic, and persists all data locally in **SQLite**.

**7. REST API → Cloud:** At session end, per-repetition detail (max angle, %MVIC) and continuous samples (angle, %MVIC, heart rate) are serialized to JSON and sent via HTTP POST to a REST endpoint. The server automatically computes five summary clinical metrics:

| # | Metric | Definition |
|---|---|---|
| i | **Maximum ROM** | Peak range of motion recorded in the session |
| ii | **Total repetitions** | Repetition odometer count |
| iii | **Efficiency Ratio** | % of repetitions meeting **both** ROM ≥ 90° and %MVIC < 25% simultaneously |
| iv | **ΔHR (max heart-rate delta)** | Change in heart rate across the session |
| v | **Muscular fatigue** | Variation in Median Frequency (MDF) of the sEMG spectrum between session start and end |

---

## Installation

> **TODO:** The paper does not specify exact build/flash instructions, dependency versions, or setup scripts. The following is a scaffold to be completed once the source code is published.

```bash
# Clone the repository
git clone https://github.com/<org>/s-rehab.git
cd s-rehab

# Arm module firmware (ESP32) — TODO: confirm toolchain (Arduino IDE / PlatformIO / ESP-IDF)
# Chest module firmware (RP2040 / HealthyPi 5) — TODO: confirm build system

# Unity host application
# Open the Unity project in Unity Editor (TODO: confirm required Unity version)

# Web platform (Flask backend)
cd web
pip install -r requirements.txt   # TODO: confirm dependencies
python app.py                     # TODO: confirm entry point
```

---

## Usage

Based on the experimental protocol described in the paper, a typical S-REHAB session proceeds as follows:

1. **Skin preparation & sensor placement:** Following SENIAM guidelines, skin is cleaned with isopropyl alcohol; sEMG electrodes are placed on the upper trapezius, and HealthyPi 5 ECG electrodes on the torso.
2. **Baseline & calibration:** With the participant seated (trunk fixed against a backrest), baseline heart rate is recorded for 60 seconds, and a reference quaternion is captured at 0°. A 3-second isometric shoulder-shrug against manual resistance is then performed to obtain the MVIC normalization constant.
3. **Active monitoring & biofeedback:** The user performs shoulder elevation. The firmware analyzes kinematic and muscular variables at 10 Hz and switches the LED state according to the Ludewig & Cook–based rules (see biofeedback table above).
4. **Pain interruption (if needed):** If the participant experiences acute discomfort, the supervising physiotherapist presses a pain button in the app, which halts the exercise flow, generates a timestamped marker, and freezes the current angle and %MVIC value in the local SQLite database.
5. **Session sync:** At the end of training, session tables are packaged into a JSON object and asynchronously sent via REST API to the remote server for later review on the web dashboard.

---

## Validation

Preliminary validation was performed on **healthy volunteers** (not SIS patients), evaluating four aspects:

| Area | Result |
|---|---|
| **Physical / manufacturability** | 3D-printed PLA enclosures (Autodesk Fusion 360, 3 mm tolerance) assembled successfully without significant mechanical interference. PCBs were not fabricated; protoboards were used instead, slightly increasing the arm module's final volume without affecting overall function. |
| **Physiological acquisition (ECG)** | HealthyPi 5 + MAX30001 correctly acquired ECG signals, with clean P waves, QRS complexes, and T waves recorded at rest. Heart rate was stable within a **69–106 bpm** physiological range after a 2-minute stabilization period. |
| **Unity 3D interface & virtual environment** | The virtual anatomical model fluidly replicated upper-limb displacement during controlled simulation testing. Internal logic modules (instantaneous angle computation, max-ROM storage, repetition odometer, angular-velocity logging) operated according to design specifications. End-to-end real-time BLE communication and automated LED alarm switching functioned correctly per the defined logic. |
| **Web platform** | The Flask-based dashboard for deferred session analysis operated as designed for post-session review of adherence and progress. |

**Overall conclusion (as stated by the authors):** results demonstrate preliminary technical feasibility of the hardware/software architecture, stable physiological signal acquisition, and satisfactory real-time visualization/biofeedback operation. The system is positioned as a promising, low-cost tool for **future clinical evaluation** — not yet a validated clinical instrument.

---

## Current Limitations

Explicitly stated in the paper (Section VI-B, "Alcance MVP"):

- The system currently corresponds to a **Minimum Viable Product (MVP)**, not a finished clinical device.
- **Full simultaneous BLE integration** of both physical modules (arm + chest) is still in development.
- **Dynamic %MVIC calculation during the session** (as opposed to post-hoc/calibration-based) is still in development.
- **Automatic synchronization of all physiological variables** within the SQLite database is still in development.
- Validation was performed on **healthy volunteers**, not on the target SIS patient population — results correspond only to this reference cohort.
- Custom **PCBs were not fabricated**; the prototype uses protoboards with manual soldering, increasing module size versus the original design.
- The **MyoWare 2.0 Link Shield** accessory was unavailable, requiring external wiring for the sEMG sensor instead of the intended integrated connection.
- Full **IEC 60601-1** electrical safety compliance has not yet been verified through formal testing — the current floating LiPo power scheme is designed in accordance with its principles but requires certified testing before clinical use.
- The presented validation is **preliminary**; further studies in real clinical settings are required to verify performance during actual rehabilitation.

---

## Research Contributions

- A **three-layer (Edge / Host / Cloud) distributed architecture** for wireless, low-cost biomedical monitoring, validated as a feasible approach for portable biomedical systems by reducing wireless payload and host computational load.
- Application of **quaternion-based orientation estimation** to avoid Gimbal Lock in shoulder ROM tracking near the clinically relevant 90° flexion boundary.
- A **peak-envelope-based %MVIC normalization method** for the MyoWare 2.0's pre-rectified analog output, avoiding the need for raw-signal RMS computation.
- Integration of **localized LED biofeedback** driven by literature-derived clinical rules (Ludewig & Cook compensatory-pattern criteria) to reduce cognitive load during self-monitored exercise execution.
- A combined **kinematic + neuromuscular + cardiovascular** monitoring approach in a single wearable platform, with automatic derivation of five summary clinical metrics (max ROM, repetitions, Efficiency Ratio, ΔHR, sEMG-MDF fatigue).

---

## License

**TODO — no license specified in the paper.**

For a research-driven biomedical wearable project intended for open collaboration, an **MIT** or **Apache 2.0** license is commonly recommended for the software/firmware components (permissive, encourages academic and clinical reuse and citation), while **CERN-OHL-S** or **CC BY 4.0** are common choices for hardware designs (schematics, CAD, PCB files). Final license selection should be made by the authors/institution.

---

## Authors

| Name | Affiliation | Contact |
|---|---|---|
| Daniel Cárdenas | Universidad Peruana Cayetano Heredia (UPCH) | daniel.cardenas.p@upch.pe |
| André Alexis Palomino Mozo | Universidad Peruana Cayetano Heredia (UPCH) | andre.palomino@upch.pe |
| Luis Luque | Universidad Peruana Cayetano Heredia (UPCH) | luis.luque@upch.pe |
| André Vallejo Canchanya | Universidad Peruana Cayetano Heredia (UPCH) | andre.vallejo@upch.pe |
| Anthony Callupe | Universidad Peruana Cayetano Heredia (UPCH) | anthony.callupe@upch.pe |
| Mishelle Llanos | Universidad Peruana Cayetano Heredia (UPCH) | mishelle.llanos@upch.pe |

---

## Acknowledgements

This project was developed at **Universidad Peruana Cayetano Heredia (UPCH)**, Lima, Perú, as part of ongoing biomedical engineering research into low-cost, wireless monitoring solutions for musculoskeletal rehabilitation. The authors acknowledge the volunteers who participated in the preliminary validation sessions, and the open clinical and engineering literature (SENIAM guidelines, Ludewig & Cook compensatory-pattern criteria, IEC 60601-1 principles) that informed the system's design.

---

<div align="center">

**S-REHAB** — an Edge-Computing wearable platform for objective, quantitative shoulder rehabilitation monitoring.

</div>
