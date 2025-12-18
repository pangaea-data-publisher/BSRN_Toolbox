# coding=utf-8

"""
Parallel
"""

from PyQt5.QtCore import QThread, pyqtSignal
from logic.helper import Result

class Stoppable:
    """
    Generic class: Cancelability of an object
    """

    def __init__(self):
        self.__b_abort = False
        self.__lst_childs = []

    def del_abort_childs(self):
        self.__lst_childs = []

    def add_abort_child(self, child):
        self.__lst_childs.append(child)

    def get_abort_childs(self):
        return self.__lst_childs

    def abort(self, b_bool=True):
        self.__b_abort = b_bool
        for child in self.__lst_childs:
            child.abort(b_bool)

    def is_abort(self):
        return self.__b_abort


class Worker(QThread):
    """
    Thread Container
    """

    signalResult = pyqtSignal(Result)

    def __init__(self):
        super().__init__()
        self.__b_abort = False

    def run(self):
        result = Result(info="This is a dummy worker ... Nothing to to ...")  # Dummy Antwort
        self.send_result(result)

    def abort(self, b_abort=True):
        self.__b_abort = b_abort

    def is_abort(self):
        return self.__b_abort

    def connect_finish(self, finish):
        if finish is not None:
            self.signalResult.connect(finish)

    def disconnect_finish(self):
        try:
            self.signalResult.disconnect()
        except Exception:
            pass

    def send_result(self, result):
        self.signalResult.emit(result)


