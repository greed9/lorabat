#!/bin/bash
echo -n "Starting gpsd..."
sudo gpsd /dev/ttyS0
echo "Done."
