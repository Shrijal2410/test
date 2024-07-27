# Bionic Arm

Our project aims to provide a realistic, functional prosthetic hand for those who have lost theirs. We're not delving into mastering neurology; instead, we're focusing on using muscle sensor data (EMG) to control the hand's movements. Our design is intended for individuals with sufficient remaining limb functionality to provide us with EMG signals, allowing us to move the fingers and wrist accordingly.

## Objectives

Simple enough to outline, but challenging to achieve:

- Create a functional hand with full control over five fingers and wrist movement.
- Ensure the hand can carry up to 1-2kg and grip objects easily.
- Keep the overall weight under 500g, which is a challenge compared to some existing industrial designs.

## Folder Structure

I’ve kept a minimal yet comprehensive folder structure. Ensure each folder has a `README.md`. No spaces in file names use hyphens '-' or underscores '_'. Keep all file and folder names in lowercase. Stick to this template, but feel free to make necessary adjustments.

```
📁 bionicarm
│
├── 🗃️ code
│   └── 🗂️ esp32
│       ├── 🗂️ build
│       │   ├── 🗂️ multipleMotor
│       │   ├── 🗂️ multipleMotorWithGUI
│       │   └── 🗂️ singleMotor
│       └── 🗂️ test
│           ├── 🗂️ pid
│           └── 🗂️ sensorData
│   └── 🗂️ matlab
│
├── 🗃️ data
│   └── 🗂️ raw
│
├── 🗃️ docs
│   └── 🗂️ research
│
├── 🗃️ electrical
│   ├── 🗂️ assets
│   │   ├── 🗂️ 3dmodel
│   │   ├── 🗂️ footprint
│   │   └── 🗂️ symbol
│   ├── 🗂️ production
│   └── 🗂️ schematic
│
└── 🗃️ mechanical
    └── 🗂️ stl
```

## Commit Message Guidelines

Keep commit messages clean and meaningful. Use the following structure and ensure messages are in lowercase:

```
<type>(<scope>): <short summary>
│       │             │
│       │             └─⫸ Summary in present tense. Not capitalized. No period at the end.
│       │                  - use bullet points for clarity
│       │
│       └─⫸ Commit Scope: mechanical|electrical|firmware|software|integration|documentation
│
└─⫸ Commit Type: build|ci|docs|feat|fix|perf|refactor|test
```

### Example

```
feat(firmware-control): add PID controller for motor speed regulation

- enhances motor speed accuracy
- improves response time
```

### Important Note

Stick to this format to avoid the need for commit message modification via `git rebase`, which could inadvertently list me as a contributor to your commit. This ensures proper credit for your work.

## License

Not my area of expertise. So, I’m leaving this part to someone else.

**Note:** The goal of this README is to provide an overview of the project and guide users on where to go. Keep its purpose in mind while updating.