#!/bin/bash
# Note: This script will only work on Nodes with TurboVNC installed and configured properly.
# Use 'salloc -p csc549' to get a node with TurboVNC support.

# If asked for password, use `password`

echo "Starting TurboVNC session..."
echo "VNC Password: password"

export PATH=$PATH:/opt/TurboVNC/bin
export LIBGL_DRIVERS_PATH=/usr/lib64/dri
export LC_ALL=en_US.UTF-8

vncserver -depth 24 -geometry 1600x900 :1
vncserver -list