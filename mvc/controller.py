# coding=utf-8

"""
Controller
"""

import sys
from PyQt5.QtWidgets import *
from mvc.model import Model
from mvc.observer import Observer
from mvc.view import View


class Controller():

    # Constructor
    def __init__(self, s_gui_style=None):

        # QT
        app = QApplication(sys.argv)
        if s_gui_style is not None:
            app.setStyle(QStyleFactory.create(s_gui_style))

        # Instanzvariablen
        self.__model = Model() # Model
        self.__view = View(self.__model) # View
        self.__model.set_observer(Observer(self.__view)) # Observer

        # Start
        self.__view.show()
        self.__view.check_if_bsrn_id_system_is_working()

        sys.exit(app.exec_()) # Eventloop
