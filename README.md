# Z1_Joystick (B2 Release)

This package is used to control z1 on the Unitree B2 robot.
The joystick key command is the same as B1.

## Setup

Copy the `release` directory to PC4(192.168.123.164) and execute the following steps.

1. add library

```bash
sudo mv release/lib/* /usr/local/lib
sudo ldconfig
```

2. Move the z1 program to home directory.

```bash
mv release/z1 /home/unitree/
sudo chmod +x /home/unitree/z1/bin/*
```

3. Setup autostart

```bash
sudo mv release/scripts/* /etc/systemd/system
sudo systemctl daemon-reload
sudo systemctl enable z1_ctrl
sudo systemctl enable z1_joystick
```

Then you can control z1 by B2 joystick.