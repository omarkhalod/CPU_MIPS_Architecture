# MIPS CPU Pipeline Simulator - GUI

A dark-mode PyQt5 GUI for visualizing and simulating the MIPS 5-stage pipeline architecture.

## Features

- **Dark Mode Interface**: Professional dark theme optimized for long coding sessions
- **Pipeline Visualization**: Real-time display of all 5 pipeline stages (IF, ID, EX, MEM, WB)
- **Register Display**: 32 General Purpose Registers + PC counter
- **Memory Viewer**: Browse and inspect memory contents
- **Execution Control**: Start, pause, step, and reset simulation
- **Execution Log**: Track all events during simulation

## Installation

### Prerequisites
- Python 3.7 or higher
- pip package manager

### Setup

1. Install dependencies:
```bash
pip install -r requirements.txt
```

2. Run the GUI:
```bash
python gui.py
```

## Interface Overview

### Control Panel
- **Cycle Counter**: Shows the current simulation cycle
- **Status Display**: Shows current simulation state (Idle, Running, Paused)
- **Start Button**: Begin simulation
- **Step Button**: Execute one cycle manually
- **Pause Button**: Pause running simulation
- **Reset Button**: Reset to initial state

### Pipeline Stages
Visual representation of the 5-stage pipeline:
- **IF**: Instruction Fetch
- **ID**: Instruction Decode
- **EX**: Execute
- **MEM**: Memory Access
- **WB**: Write Back

### Tabs

#### Registers (GPR)
Displays all 32 General Purpose Registers with their hexadecimal values, plus PC and HI/LO counters.

#### Memory View
- Browse memory with adjustable start address
- View memory in hexadecimal and decimal formats
- See validity flags for each memory location

#### Execution Log
Chronological log of all pipeline events and operations.

## Usage

1. **Start Simulation**: Click "Start Simulation" to begin automatic pipeline execution
2. **Step Through**: Use "Step" button to manually advance one cycle at a time
3. **Pause**: Click "Pause" to stop the running simulation
4. **Reset**: Click "Reset" to return to initial state
5. **View Registers**: Switch to "Registers (GPR)" tab to see register values
6. **Browse Memory**: Use the Memory View tab to explore memory contents
7. **Check Logs**: View execution history in the Execution Log tab

## Color Scheme (Dark Mode)

- **Background**: Dark gray (#1e1e1e)
- **Primary Accent**: Blue (#007acc)
- **Text**: Light gray (#e0e0e0)
- **Highlights**: Cyan (#00d4ff)
- **Borders**: Darker gray (#3e3e42)

## Architecture

The GUI is built with PyQt5 and includes:
- `CPUSimulatorGUI`: Main application window
- `PipelineVisualizerWidget`: Visual representation of pipeline stages
- Dark theme stylesheet for consistent styling
- Interactive tables for registers and memory

## Future Enhancements

- Integration with C backend for real simulation
- Breakpoint support
- Performance metrics and statistics
- Export execution logs
- Customizable cycle timing
