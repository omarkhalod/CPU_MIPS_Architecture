import sys
import json
from PyQt5.QtWidgets import (
    QApplication, QMainWindow, QWidget, QVBoxLayout, QHBoxLayout,
    QLabel, QPushButton, QTableWidget, QTableWidgetItem, QTabWidget,
    QGroupBox, QSpinBox, QFrame, QScrollArea, QToolButton
)
from PyQt5.QtCore import Qt, QTimer, pyqtSignal, QThread
from PyQt5.QtGui import QFont, QColor, QBrush, QIcon
# Removed dependency on PyQt5.QtChart (not used) to avoid extra package requirement
import subprocess
import re

# Dark Mode Stylesheet
DARK_STYLESHEET = """
    QMainWindow {
        background-color: #1e1e1e;
        color: #e0e0e0;
    }
    QWidget {
        background-color: #1e1e1e;
        color: #e0e0e0;
    }
    QGroupBox {
        background-color: #252526;
        color: #e0e0e0;
        border: 1px solid #3e3e42;
        border-radius: 4px;
        margin-top: 8px;
        padding-top: 8px;
    }
    QGroupBox::title {
        subcontrol-origin: margin;
        subcontrol-position: top left;
        padding: 0 3px;
    }
    QPushButton {
        background-color: #007acc;
        color: #ffffff;
        border: none;
        border-radius: 4px;
        padding: 6px 16px;
        font-weight: bold;
    }
    QPushButton:hover {
        background-color: #0098ff;
    }
    QPushButton:pressed {
        background-color: #005a9e;
    }
    QPushButton:disabled {
        background-color: #3e3e42;
        color: #6a6a6a;
    }
    QTableWidget {
        background-color: #252526;
        color: #e0e0e0;
        border: 1px solid #3e3e42;
        gridline-color: #3e3e42;
    }
    QTableWidget::item {
        padding: 5px;
        border: none;
    }
    QTableWidget::item:selected {
        background-color: #094771;
    }
    QHeaderView::section {
        background-color: #2d2d2d;
        color: #e0e0e0;
        padding: 5px;
        border: none;
        border-right: 1px solid #3e3e42;
    }
    QTableCornerButton::section {
        background-color: #2d2d2d;
        border: 1px solid #3e3e42;
    }
    QTabWidget::pane {
        border: 1px solid #3e3e42;
    }
    QTabBar::tab {
        background-color: #2d2d2d;
        color: #e0e0e0;
        padding: 8px 20px;
        border: 1px solid #3e3e42;
        margin-right: 2px;
    }
    QTabBar::tab:selected {
        background-color: #007acc;
        color: #ffffff;
    }
    QTabBar::tab:hover {
        background-color: #3e3e42;
    }
    QSpinBox {
        background-color: #3e3e42;
        color: #e0e0e0;
        border: 1px solid #3e3e42;
        border-radius: 3px;
        padding: 4px;
    }
    QToolButton {
        background-color: #2d2d2d;
        color: #e0e0e0;
        border: 1px solid #3e3e42;
        border-radius: 3px;
        min-width: 18px;
        min-height: 18px;
        padding: 0px;
    }
    QToolButton:hover {
        background-color: #3e3e42;
    }
    QToolButton:pressed {
        background-color: #007acc;
    }
    QLabel {
        color: #e0e0e0;
    }
    QFrame {
        background-color: #252526;
        border: 1px solid #3e3e42;
        border-radius: 4px;
    }
"""


class PipelineVisualizerWidget(QWidget):
    """Visual representation of the pipeline stages"""
    
    def __init__(self):
        super().__init__()
        self.init_ui()
    
    def init_ui(self):
        layout = QHBoxLayout()
        layout.setSpacing(10)
        layout.setContentsMargins(10, 10, 10, 10)
        
        # Pipeline stages
        stages = [
            ("IF", "Fetch"),
            ("ID", "Decode"),
            ("EX", "Execute"),
            ("MEM", "Memory"),
            ("WB", "WriteBack")
        ]
        
        self.stage_frames = {}
        
        for stage_abbr, stage_name in stages:
            frame = QFrame()
            frame.setStyleSheet("""
                QFrame {
                    background-color: #2d2d2d;
                    border: 2px solid #007acc;
                    border-radius: 8px;
                }
            """)
            frame_layout = QVBoxLayout()
            frame_layout.setContentsMargins(10, 10, 10, 10)
            
            # Stage title
            title = QLabel(f"{stage_abbr}")
            title_font = QFont()
            title_font.setPointSize(12)
            title_font.setBold(True)
            title.setFont(title_font)
            title.setAlignment(Qt.AlignCenter)
            frame_layout.addWidget(title)
            
            # Stage subtitle
            subtitle = QLabel(stage_name)
            subtitle_font = QFont()
            subtitle_font.setPointSize(9)
            subtitle.setFont(subtitle_font)
            subtitle.setAlignment(Qt.AlignCenter)
            subtitle.setStyleSheet("color: #888888;")
            frame_layout.addWidget(subtitle)
            
            # Instruction info
            info = QLabel("Empty")
            info.setAlignment(Qt.AlignCenter)
            info.setStyleSheet("color: #00d4ff;")
            info.setWordWrap(True)
            frame_layout.addWidget(info)
            
            frame.setLayout(frame_layout)
            frame.setMinimumHeight(120)
            
            self.stage_frames[stage_abbr] = {
                'frame': frame,
                'info': info
            }
            
            layout.addWidget(frame)
        
        self.setLayout(layout)
    
    def update_stage(self, stage, instruction_info):
        """Update a pipeline stage with instruction info"""
        if stage in self.stage_frames:
            self.stage_frames[stage]['info'].setText(instruction_info)


class CPUSimulatorGUI(QMainWindow):
    
    def __init__(self):
        super().__init__()
        self.initUI()
        self.cycle = 0
        self.states = []
        self.state_index = -1
        self.is_running = False
        
        # Apply dark mode stylesheet
        self.setStyleSheet(DARK_STYLESHEET)
    
    def initUI(self):
        """Initialize the user interface"""
        self.setWindowTitle("MIPS CPU Pipeline Simulator - Dark Mode")
        self.setGeometry(100, 100, 1400, 900)
        
        # Main widget
        main_widget = QWidget()
        self.setCentralWidget(main_widget)
        
        main_layout = QVBoxLayout()
        
        # Top section: Control panel
        control_panel = self.create_control_panel()
        main_layout.addWidget(control_panel)
        
        # Middle section: Pipeline visualization
        pipeline_label = QLabel("Pipeline Stages")
        pipeline_font = QFont()
        pipeline_font.setPointSize(12)
        pipeline_font.setBold(True)
        pipeline_label.setFont(pipeline_font)
        main_layout.addWidget(pipeline_label)
        
        self.pipeline_widget = PipelineVisualizerWidget()
        main_layout.addWidget(self.pipeline_widget)
        
        # Bottom section: Tabs for registers and memory
        tabs = QTabWidget()
        
        # Registers tab
        registers_widget = self.create_registers_table()
        tabs.addTab(registers_widget, "Registers (GPR)")
        
        # Memory tab
        memory_widget = self.create_memory_table()
        tabs.addTab(memory_widget, "Memory View")
        
        # Cycle log tab
        self.log_text = self.create_log_display()
        tabs.addTab(self.log_text, "Execution Log")
        
        main_layout.addWidget(tabs)
        
        main_widget.setLayout(main_layout)
    
    def create_control_panel(self):
        """Create the control panel"""
        panel = QGroupBox("Control Panel")
        layout = QHBoxLayout()
        
        # Cycle counter
        cycle_label = QLabel("Cycle:")
        cycle_font = QFont()
        cycle_font.setPointSize(11)
        cycle_font.setBold(True)
        cycle_label.setFont(cycle_font)
        
        self.cycle_display = QLabel("0")
        self.cycle_display.setFont(cycle_font)
        self.cycle_display.setStyleSheet("color: #00d4ff;")
        self.cycle_display.setMinimumWidth(50)
        
        # Status label
        self.status_label = QLabel("Status: Idle")
        status_font = QFont()
        status_font.setPointSize(10)
        self.status_label.setFont(status_font)
        self.status_label.setStyleSheet("color: #4ec9b0;")
        
        # Buttons
        self.start_btn = QPushButton("Start Simulation")
        self.start_btn.clicked.connect(self.start_simulation)
        
        self.step_btn = QPushButton("Step")
        self.step_btn.clicked.connect(self.step_cycle)
        
        self.pause_btn = QPushButton("Pause")
        self.pause_btn.clicked.connect(self.pause_simulation)
        self.pause_btn.setEnabled(False)
        
        self.reset_btn = QPushButton("Reset")
        self.reset_btn.clicked.connect(self.reset_simulation)
        
        layout.addWidget(cycle_label)
        layout.addWidget(self.cycle_display)
        layout.addSpacing(20)
        layout.addWidget(self.status_label)
        layout.addStretch()
        layout.addWidget(self.start_btn)
        layout.addWidget(self.step_btn)
        layout.addWidget(self.pause_btn)
        layout.addWidget(self.reset_btn)
        
        panel.setLayout(layout)
        return panel
    
    def create_registers_table(self):
        """Create registers display table"""
        container = QWidget()
        layout = QVBoxLayout()
        
        self.registers_table = QTableWidget()
        # Columns: Register | Hex Value | Decimal
        self.registers_table.setColumnCount(3)
        self.registers_table.setHorizontalHeaderLabels(["Register", "Hex Value", "Decimal"])
        self.registers_table.setColumnWidth(0, 100)
        self.registers_table.setColumnWidth(1, 150)
        self.registers_table.setColumnWidth(2, 150)
        
        # Initialize with the tracked registers
        self.registers_table.setRowCount(33)  # 32 GPR + PC
        
        register_names = [f"R{i}" for i in range(32)] + ["PC"]
        for i, name in enumerate(register_names):
            self.registers_table.setItem(i, 0, QTableWidgetItem(name))
            self.registers_table.setItem(i, 1, QTableWidgetItem("0x00000000"))
            self.registers_table.setItem(i, 2, QTableWidgetItem("0"))
        
        self.registers_table.setSelectionBehavior(self.registers_table.SelectRows)
        self.registers_table.setSelectionMode(self.registers_table.SingleSelection)
        
        layout.addWidget(self.registers_table)
        container.setLayout(layout)
        return container
    
    def create_memory_table(self):
        """Create memory display table"""
        container = QWidget()
        layout = QVBoxLayout()
        
        # Memory range selector
        range_layout = QHBoxLayout()
        range_layout.setSpacing(8)

        start_label = QLabel("Start Address:")
        start_label.setFixedHeight(32)
        start_label.setAlignment(Qt.AlignVCenter | Qt.AlignLeft)
        range_layout.addWidget(start_label)

        self.mem_start = QSpinBox()
        self.mem_start.setMaximum(2047)
        self.mem_start.setButtonSymbols(QSpinBox.NoButtons)
        self.mem_start.setFixedHeight(32)
        self.mem_start.valueChanged.connect(self.update_memory_view)
        range_layout.addWidget(self.mem_start)

        start_controls_widget = QWidget()
        start_controls_widget.setFixedHeight(32)

        start_controls = QVBoxLayout(start_controls_widget)
        start_controls.setSpacing(1)
        start_controls.setContentsMargins(0, 0, 0, 0)

        self.mem_start_up = QToolButton()
        self.mem_start_up.setText("▲")
        self.mem_start_up.setFixedHeight(15)
        self.mem_start_up.clicked.connect(lambda: self.mem_start.setValue(min(self.mem_start.maximum(), self.mem_start.value() + 1)))

        self.mem_start_down = QToolButton()
        self.mem_start_down.setText("▼")
        self.mem_start_down.setFixedHeight(15)
        self.mem_start_down.clicked.connect(lambda: self.mem_start.setValue(max(self.mem_start.minimum(), self.mem_start.value() - 1)))

        start_controls.addWidget(self.mem_start_up)
        start_controls.addWidget(self.mem_start_down)
        range_layout.addWidget(start_controls_widget)
        range_layout.addStretch()
        
        layout.addLayout(range_layout)
        
        # Memory table
        self.memory_table = QTableWidget()
        self.memory_table.setColumnCount(5)
        self.memory_table.setHorizontalHeaderLabels(["Address (Hex)", "Address (Dec)", "Hex Value", "Decimal", "Valid"])
        self.memory_table.setColumnWidth(0, 120)
        self.memory_table.setColumnWidth(1, 120)
        self.memory_table.setColumnWidth(2, 130)
        self.memory_table.setColumnWidth(3, 110)
        self.memory_table.setColumnWidth(4, 70)
        
        # Initialize with 20 rows
        self.memory_table.setRowCount(20)
        for i in range(20):
            self.memory_table.setItem(i, 0, QTableWidgetItem(f"0x{i:04X}"))
            self.memory_table.setItem(i, 1, QTableWidgetItem(str(i)))
            self.memory_table.setItem(i, 2, QTableWidgetItem("0x00000000"))
            self.memory_table.setItem(i, 3, QTableWidgetItem("0"))
            self.memory_table.setItem(i, 4, QTableWidgetItem("No"))
        
        layout.addWidget(self.memory_table)
        container.setLayout(layout)
        return container
    
    def create_log_display(self):
        """Create execution log display"""
        container = QWidget()
        layout = QVBoxLayout()
        
        self.log_display = QTableWidget()
        self.log_display.setColumnCount(2)
        self.log_display.setHorizontalHeaderLabels(["Cycle", "Event"])
        self.log_display.setColumnWidth(0, 60)
        self.log_display.setColumnWidth(1, 800)
        self.log_display.setMaximumHeight(200)
        
        layout.addWidget(self.log_display)
        container.setLayout(layout)
        return container
    
    def start_simulation(self):
        """Start the simulation"""
        # Load simulator states produced by the C simulator
        if not self.states:
            loaded = self.load_states()
            if not loaded:
                self.add_log_entry("Error", "Could not load sim_state.jsonl. Run the C simulator first.")
                return
        self.is_running = True
        self.start_btn.setEnabled(False)
        self.step_btn.setEnabled(False)
        self.pause_btn.setEnabled(True)
        self.status_label.setText("Status: Running...")
        self.status_label.setStyleSheet("color: #4ec9b0;")
        
        # Use a timer to step through cycles
        self.timer = QTimer()
        self.timer.timeout.connect(self.auto_step)
        self.timer.start(500)  # Step every 500ms
    
    def auto_step(self):
        """Automatically step through cycles"""
        self.step_cycle()
    
    def step_cycle(self):
        """Advance to next simulator state (from sim_state.jsonl)"""
        if not self.states:
            if not self.load_states():
                self.add_log_entry("Error", "No simulator state available.")
                return

        next_index = self.state_index + 1
        if next_index >= len(self.states):
            if self.states:
                self.state_index = len(self.states) - 1
            self.pause_simulation()
            return

        self.state_index = next_index

        state = self.states[self.state_index]
        self.cycle = state.get('cycle', self.state_index)
        self.cycle_display.setText(str(self.cycle))

        # Update UI from loaded state
        self.update_pipeline_display(state)
        self.update_registers_display(state)
        self.update_memory_display(state)

        self.add_log_entry(str(self.cycle), "Loaded state from sim_state.jsonl")
    
    def pause_simulation(self):
        """Pause the simulation"""
        if hasattr(self, 'timer'):
            self.timer.stop()
        
        self.is_running = False
        self.start_btn.setEnabled(True)
        self.step_btn.setEnabled(True)
        self.pause_btn.setEnabled(False)
        self.status_label.setText("Status: Paused")
        self.status_label.setStyleSheet("color: #d7ba7d;")
    
    def reset_simulation(self):
        """Reset the simulation"""
        if hasattr(self, 'timer'):
            self.timer.stop()
        
        self.cycle = 0
        self.state_index = -1
        self.is_running = False
        self.cycle_display.setText("0")
        self.start_btn.setEnabled(True)
        self.step_btn.setEnabled(True)
        self.pause_btn.setEnabled(False)
        self.status_label.setText("Status: Idle")
        self.status_label.setStyleSheet("color: #4ec9b0;")
        
        # Clear tables
        self.log_display.setRowCount(0)
        self.update_pipeline_display()

    def load_states(self):
        """Load simulator states from sim_state.jsonl (JSON lines)"""
        try:
            with open('sim_state.jsonl', 'r') as f:
                lines = f.read().splitlines()
        except FileNotFoundError:
            return False

        self.states = []
        for ln in lines:
            ln = ln.strip()
            if not ln: continue
            try:
                obj = json.loads(ln)
                self.states.append(obj)
            except Exception as e:
                # skip malformed lines
                continue

        self.state_index = -1
        return len(self.states) > 0
    
    def update_pipeline_display(self, state=None):
        """Update pipeline stage displays. If `state` is None, clear stages."""
        if state is None:
            # clear to default
            self.pipeline_widget.update_stage('IF', "Empty")
            self.pipeline_widget.update_stage('ID', "Empty")
            self.pipeline_widget.update_stage('EX', "Empty")
            self.pipeline_widget.update_stage('MEM', "Empty")
            self.pipeline_widget.update_stage('WB', "Empty")
            return

        if_id = state.get('IF_ID', {})
        id_ex = state.get('ID_EX', {})
        ex_mem = state.get('EX_MEM', {})
        mem_wb = state.get('MEM_WB', {})

        self.pipeline_widget.update_stage('IF', f"Instr: 0x{if_id.get('instruction',0):08X}\n# {if_id.get('instructionNumber','-')}")
        self.pipeline_widget.update_stage('ID', f"op={id_ex.get('op','-')} dest=R{id_ex.get('instructionNumber','-')}\nA={id_ex.get('regA_value','-')} B={id_ex.get('regB_value','-')}")
        self.pipeline_widget.update_stage('EX', f"alu={ex_mem.get('alu_result','-')} dest=R{ex_mem.get('dest_reg','-')}\nmemR={ex_mem.get('mem_read','-')} memW={ex_mem.get('mem_write','-')}")
        self.pipeline_widget.update_stage('MEM', f"data={mem_wb.get('data','-')} dest=R{mem_wb.get('dest_reg','-')}\nreg_write={mem_wb.get('reg_write','-')}")
        self.pipeline_widget.update_stage('WB', f"WB: R{mem_wb.get('dest_reg','-')} <= {mem_wb.get('data','-')}")
    
    def update_registers_display(self, state=None):
        """Update registers display. If `state` is None, clear values."""
        if state is None:
            for i in range(33):
                item_hex = self.registers_table.item(i, 1)
                item_dec = self.registers_table.item(i, 2)
                if item_hex:
                    item_hex.setText("0x00000000")
                if item_dec:
                    item_dec.setText("0")
            return

        regs = state.get('registers', [])
        for i in range(32):
            val = regs[i] if i < len(regs) else 0
            hex_value = f"0x{int(val) & 0xFFFFFFFF:08X}"
            dec_value = str(int(val))
            item_hex = self.registers_table.item(i, 1)
            item_dec = self.registers_table.item(i, 2)
            if item_hex:
                item_hex.setText(hex_value)
            if item_dec:
                item_dec.setText(dec_value)
        # PC
        pc_val = state.get('pc', 0)
        if self.registers_table.item(32, 1):
            self.registers_table.item(32, 1).setText(f"0x{int(pc_val) & 0xFFFFFFFF:08X}")
        if self.registers_table.item(32, 2):
            self.registers_table.item(32, 2).setText(str(int(pc_val)))
    
    def update_memory_display(self, state=None):
        """Update memory display from `state`. If `state` is None, use current state_index snapshot."""
        if state is None:
            # fallback to existing view behavior
            return self.update_memory_view()

        snapshot = state.get('memory_snapshot', [])
        start_addr = self.mem_start.value()
        for i in range(20):
            addr = start_addr + i
            item_addr = self.memory_table.item(i, 0)
            item_addr_dec = self.memory_table.item(i, 1)
            item_hex = self.memory_table.item(i, 2)
            item_dec = self.memory_table.item(i, 3)
            item_valid = self.memory_table.item(i, 4)
            if item_addr: item_addr.setText(f"0x{addr:04X}")
            if item_addr_dec: item_addr_dec.setText(str(addr))
            val = next((entry for entry in snapshot if entry.get('addr') == addr), None)
            if val:
                data = int(val.get('data', 0))
                item_hex.setText(f"0x{data & 0xFFFFFFFF:08X}")
                item_dec.setText(str(data))
                item_valid.setText("Yes" if val.get('valid') else "No")
            else:
                item_hex.setText("0x00000000")
                item_dec.setText("0")
                item_valid.setText("No")
    
    def update_memory_view(self):
        """Update the memory table view from the currently selected state"""
        if not self.states or self.state_index < 0:
            return
        if self.state_index >= len(self.states):
            self.state_index = len(self.states) - 1
        if self.state_index < 0:
            return
        state = self.states[self.state_index]
        snapshot = state.get('memory_snapshot', [])
        start_addr = self.mem_start.value()
        for i in range(20):
            addr = start_addr + i
            item_addr = self.memory_table.item(i, 0)
            item_addr_dec = self.memory_table.item(i, 1)
            item_hex = self.memory_table.item(i, 2)
            item_dec = self.memory_table.item(i, 3)
            item_valid = self.memory_table.item(i, 4)
            if item_addr: item_addr.setText(f"0x{addr:04X}")
            if item_addr_dec: item_addr_dec.setText(str(addr))
            # find snapshot entry with matching addr
            val = next((entry for entry in snapshot if entry.get('addr') == addr), None)
            if val:
                data = int(val.get('data', 0))
                item_hex.setText(f"0x{data & 0xFFFFFFFF:08X}")
                item_dec.setText(str(data))
                item_valid.setText("Yes" if val.get('valid') else "No")
            else:
                item_hex.setText("0x00000000")
                item_dec.setText("0")
                item_valid.setText("No")
    
    def add_log_entry(self, cycle, event):
        """Add an entry to the execution log"""
        row_position = self.log_display.rowCount()
        self.log_display.insertRow(row_position)
        
        self.log_display.setItem(row_position, 0, QTableWidgetItem(str(cycle)))
        self.log_display.setItem(row_position, 1, QTableWidgetItem(event))
        
        # Scroll to bottom
        self.log_display.scrollToBottom()


def main():
    app = QApplication(sys.argv)
    
    # Create and show the main window
    window = CPUSimulatorGUI()
    window.show()
    
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()
