#!/usr/bin/env bash
#
# Copyright 2026
# Carnegie Robotics, LLC
# 4501 Hatfield Street, Pittsburgh, PA 15201
# https://www.carnegierobotics.com
#
# This source code is licensed under the Apache License, Version 2.0
# found in the LICENSE file in the root directory of this source tree.
#
# Installs the Vephor Web Gateway as a systemd service on Ubuntu.

set -e

SERVICE_NAME="vephor-gateway"
SERVICE_FILE="/etc/systemd/system/${SERVICE_NAME}.service"

# Resolve absolute paths
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
GATEWAY_SCRIPT="${SCRIPT_DIR}/gateway.py"

# Find system Python 3 (prefer global over pyenv shims for system services if possible,
# but we will fall back to resolving the current python3 path)
PYTHON_EXEC="/usr/bin/python3"

echo "Installing Vephor Web Gateway as a systemd service..."
echo "Gateway Script: ${GATEWAY_SCRIPT}"
echo "Python Executable: ${PYTHON_EXEC}"

# Must be run as root
if [ "$EUID" -ne 0 ]; then
    echo "Error: Please run this script with sudo."
    exit 1
fi

echo "Installing required system dependencies..."
apt-get update
apt-get install -y python3-pip
/usr/bin/python3 -m pip install --upgrade websockets

echo "Generating ${SERVICE_FILE}..."

cat <<EOF > "${SERVICE_FILE}"
[Unit]
Description=Vephor Web Gateway Service
After=network.target
StartLimitIntervalSec=0

[Service]
Type=simple
Restart=always
RestartSec=3
User=root
WorkingDirectory=${SCRIPT_DIR}
ExecStart=${PYTHON_EXEC} ${GATEWAY_SCRIPT}

[Install]
WantedBy=multi-user.target
EOF

echo "Setting permissions..."
chmod 644 "${SERVICE_FILE}"

echo "Reloading systemd daemon..."
systemctl daemon-reload

echo "Enabling service to start on boot..."
systemctl enable "${SERVICE_NAME}.service"

echo "Starting service..."
systemctl restart "${SERVICE_NAME}.service"

echo ""
echo "Installation complete!"
echo "You can check the status with: sudo systemctl status ${SERVICE_NAME}.service"
echo "You can view logs with: sudo journalctl -u ${SERVICE_NAME}.service -f"
