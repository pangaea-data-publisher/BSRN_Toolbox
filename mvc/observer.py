# coding=utf-8

"""
Observer
"""

import time
from PyQt5.QtCore import *

class Observer(QObject):

    # Signals
    signal_print = pyqtSignal(str, str)

    signal_progress_show = pyqtSignal(bool, bool)
    signal_progress_show_prozent = pyqtSignal(bool)
    signal_progress_hide = pyqtSignal()
    signal_progress_set_wert = pyqtSignal(int)
    signal_progress_set_min_max = pyqtSignal(int, int)
    signal_progress_set_text = pyqtSignal(str)
    signal_statusbar_set_text = pyqtSignal(str)
    signal_workflow_set = pyqtSignal(str, int, int)
    signal_input_combo = pyqtSignal(str, str, list)
    signal_progress_msg = pyqtSignal(object, str) # Info: object instead of int: better cause via int None can not be send
    signal_buffer = pyqtSignal()

    signal_give_qt_time = pyqtSignal()

    # Constructor
    def __init__(self, view):
        QObject.__init__(self)

        # Instance variables
        self.__view = view # View
        self.__status_timer = time.time() # Timer Status

        # Connect the view
        self.__view.connect_to_observer(self)

    # --- Admin UI features

    def print_gui(self, s_text, s_appendix="\n"):
        self.signal_print.emit(s_text, s_appendix)

    def set_status(self, s_text):
        if time.time() - self.__status_timer > 0.25: # Anti spamming - only allow an output every 0.25 seconds
            self.signal_statusbar_set_text.emit(s_text)
            self.__status_timer = time.time()

    def show_progress(self, **kwargs):
        b_abort = True
        b_pulse = False
        if "abort" in kwargs:
            b_abort = kwargs["abort"]
        if "pulse" in kwargs:
            b_pulse = kwargs["pulse"]
        self.signal_progress_show.emit(b_abort, b_pulse)

    def hide_progress(self):
        self.signal_progress_hide.emit()

    def set_progressbar_min_max(self, i_min=0, i_max=0):
        self.signal_progress_set_min_max.emit(i_min, i_max)

    def set_progressbar_val(self, i_wert):
        self.signal_progress_set_wert.emit(i_wert)

    def set_progress_msg(self, i_problem, s_info):
        self.signal_progress_msg.emit(i_problem, s_info)

    def update_buffer(self):
        self.signal_buffer.emit()

    def set_progressbar_proz_show(self, b_perc_show):
        self.signal_progress_show_prozent.emit(b_perc_show)

    def set_workflow(self, s_text, i_step_now, i_step_max):
        self.signal_workflow_set.emit(s_text, i_step_now, i_step_max)

    def set_progressbar_text(self, s_text):
        self.signal_progress_set_text.emit(s_text)

    def input_combo(self, s_titel, s_label, lst_choices):
        self.signal_input_combo.emit(s_titel, s_label, lst_choices)

