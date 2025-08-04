#!/usr/bin/env python3
"""
VOC Data Collector for WHY2025 Badge
Helps collect labeled training data for cannabis vs cigarette smoke detection
"""

import serial
import csv
import time
import argparse
from datetime import datetime
import os
import sys

class VOCDataCollector:
    def __init__(self, port, baudrate=115200):
        self.port = port
        self.baudrate = baudrate
        self.serial = None
        self.is_collecting = False
        self.current_label = "normal"
        self.data_buffer = []
        
    def connect(self):
        """Connect to the badge via serial"""
        try:
            self.serial = serial.Serial(self.port, self.baudrate, timeout=1)
            print(f"Connected to {self.port} at {self.baudrate} baud")
            return True
        except Exception as e:
            print(f"Failed to connect: {e}")
            return False
    
    def disconnect(self):
        """Disconnect from badge"""
        if self.serial and self.serial.is_open:
            self.serial.close()
            print("Disconnected")
    
    def parse_sensor_data(self, line):
        """Parse sensor data from serial output"""
        # Expected format: "VOC:400,TEMP:25.5,HUM:50.2"
        try:
            if "VOC:" in line:
                parts = line.strip().split(',')
                data = {}
                for part in parts:
                    if ':' in part:
                        key, value = part.split(':')
                        data[key.strip()] = float(value.strip())
                return data
        except Exception as e:
            print(f"Parse error: {e}")
        return None
    
    def collect_data(self, duration_seconds=60, label="normal"):
        """Collect VOC data for specified duration with label"""
        self.current_label = label
        self.is_collecting = True
        start_time = time.time()
        
        print(f"\nCollecting data for '{label}' - Duration: {duration_seconds}s")
        print("Make sure the badge is exposed to the target environment!")
        print("Press Ctrl+C to stop early\n")
        
        try:
            while time.time() - start_time < duration_seconds:
                if self.serial.in_waiting:
                    line = self.serial.readline().decode('utf-8', errors='ignore')
                    data = self.parse_sensor_data(line)
                    
                    if data and 'VOC' in data:
                        timestamp = datetime.now().isoformat()
                        data_point = {
                            'timestamp': timestamp,
                            'voc': data.get('VOC', 0),
                            'temperature': data.get('TEMP', 0),
                            'humidity': data.get('HUM', 0),
                            'label': label
                        }
                        self.data_buffer.append(data_point)
                        
                        # Display current reading
                        print(f"\rVOC: {data_point['voc']:4.0f} | "
                              f"Temp: {data_point['temperature']:4.1f}°C | "
                              f"Humidity: {data_point['humidity']:4.1f}% | "
                              f"Samples: {len(self.data_buffer)}", end='')
                
                time.sleep(0.1)
                
        except KeyboardInterrupt:
            print("\n\nCollection stopped by user")
        
        self.is_collecting = False
        print(f"\n\nCollected {len(self.data_buffer)} samples")
    
    def save_data(self, filename=None):
        """Save collected data to CSV file"""
        if not self.data_buffer:
            print("No data to save")
            return
        
        if not filename:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            filename = f"voc_data_{timestamp}.csv"
        
        with open(filename, 'w', newline='') as csvfile:
            fieldnames = ['timestamp', 'voc', 'temperature', 'humidity', 'label']
            writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
            
            writer.writeheader()
            for row in self.data_buffer:
                writer.writerow(row)
        
        print(f"Data saved to {filename}")
        return filename
    
    def interactive_collection(self):
        """Interactive data collection mode"""
        print("\n=== VOC Data Collection Tool ===")
        print("Labels: normal, cigarette, cannabis, other")
        print("Commands: collect <label> <duration>, save, clear, quit\n")
        
        while True:
            try:
                cmd = input("> ").strip().split()
                
                if not cmd:
                    continue
                
                if cmd[0] == 'quit':
                    break
                    
                elif cmd[0] == 'collect':
                    if len(cmd) < 2:
                        print("Usage: collect <label> [duration]")
                        continue
                    label = cmd[1]
                    duration = int(cmd[2]) if len(cmd) > 2 else 30
                    self.collect_data(duration, label)
                    
                elif cmd[0] == 'save':
                    filename = cmd[1] if len(cmd) > 1 else None
                    self.save_data(filename)
                    
                elif cmd[0] == 'clear':
                    self.data_buffer = []
                    print("Data buffer cleared")
                    
                else:
                    print(f"Unknown command: {cmd[0]}")
                    
            except Exception as e:
                print(f"Error: {e}")

def main():
    parser = argparse.ArgumentParser(description='VOC Data Collector for WHY2025 Badge')
    parser.add_argument('port', help='Serial port (e.g., /dev/ttyUSB0 or COM3)')
    parser.add_argument('--baudrate', type=int, default=115200, help='Baud rate')
    parser.add_argument('--label', default='normal', help='Data label (normal/cigarette/cannabis/other)')
    parser.add_argument('--duration', type=int, default=30, help='Collection duration in seconds')
    parser.add_argument('--output', help='Output CSV filename')
    parser.add_argument('--interactive', action='store_true', help='Interactive mode')
    
    args = parser.parse_args()
    
    collector = VOCDataCollector(args.port, args.baudrate)
    
    if not collector.connect():
        sys.exit(1)
    
    try:
        if args.interactive:
            collector.interactive_collection()
        else:
            collector.collect_data(args.duration, args.label)
            collector.save_data(args.output)
    finally:
        collector.disconnect()

if __name__ == '__main__':
    main()