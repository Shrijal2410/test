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
├── 🗃️ bom
│   └── 🗂️ invoice
│   └── 📋 bom.csv
│
├── 🗃️ code
│   └── 🗂️ arduino
│   └── 🗂️ esp32
│   └── 🗂️ raspi
│   └── 🗂️ matlab
│
└── 🗃️ docs
│   └── 🗂️ research
└── 🗃️ electrical
└── 🗃️ mechanical
```

## Commit Message
Keep it clean and meaningful. Use the following structure, and ensure the commit messages are in lowercase:

```
function(dir or subdir/file name): meaningful message

- additional info
- try to use bullet points

e.g., update(electrical): change pi pins for easier wiring

- to minimize the use of vias
```

Stick to this format. If not, I might use git rebase to change your commit message, which could list me as a contributor to that commit. I don’t want to take credit for your work.

## License
Not my area of expertise. So, I’m leaving this part to someone else.

**Note:** The goal of this README is to provide an overview of the project and guide users on where to go. Keep its purpose in mind while updating.