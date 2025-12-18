# coding=utf-8

"""
Worker (Parallel Container)
"""

from PyQt5.QtCore import pyqtSignal
from logic.helper import Result
from logic.parallel import Worker
from logic.selection import Selection

# Wrapper for Download, Check/Report and Check Availability features started by UI
class Worker_DownloadCheckReportAvailStations(Worker):

    signalResult = pyqtSignal(Result)

    def __init__(self, model, s_download_path, s_ftp_server, s_ftp_user, s_ftp_pw, b_unzip, b_report, b_check_availability, lst_station, lst_month, lst_year):
        super().__init__()
        self.__model = model
        self.__tm = self.__model.get_task_manager()
        # Download Params
        self.__s_download_path = s_download_path
        self.__s_ftp_server = s_ftp_server
        self.__s_ftp_user = s_ftp_user
        self.__s_ftp_pw = s_ftp_pw
        self.__b_unzip = b_unzip
        self.__b_report = b_report
        self.__b_check_availability = b_check_availability
        self.__lst_station = lst_station
        self.__lst_month = lst_month
        self.__lst_year = lst_year

    def abort(self):
        self.__tm.abort()

    def run(self):

        # FTP
        self.__model.set_ftp_server(self.__s_ftp_server)
        self.__model.set_ftp_user(self.__s_ftp_user)
        self.__model.set_ftp_pw(self.__s_ftp_pw)

        # Selection
        sel = Selection(self.__model)
        sel.load_parts(self.__lst_station, self.__lst_month, self.__lst_year)

        if not self.__b_check_availability:
            # Download station data (check and report)
            res = self.__tm.download_station_data_and_check(sel, self.__s_download_path, unzip=self.__b_unzip, report=self.__b_report)
        else:
            # Check availability of station files
            res = self.__tm.check_availability_on_server(self.__lst_station, self.__s_download_path)

        # Result
        self.send_result(res)

    def send_result(self, result):
        self.signalResult.emit(result)

# Wrapper for Check/Report of locally available files started by UI
class Worker_CheckReport(Worker):

    signalResult = pyqtSignal(Result)

    def __init__(self, model, sel):
        super().__init__()
        self.__model = model
        self.__sel = sel
        self.__tm = self.__model.get_task_manager()

    def abort(self):
        self.__tm.abort()

    def run(self):

        # Check station data (check and report)
        res = self.__tm.process_station_data(self.__sel, report=True)
        res.add_data("title", "Check station data")

        # Result
        self.send_result(res)

    def send_result(self, result):
        self.signalResult.emit(result)

# Wrapper for Export of Station data
class Worker_Export(Worker):

    signalResult = pyqtSignal(Result)

    def __init__(self, model, sel, lst_rec):
        super().__init__()
        self.__model = model
        self.__sel = sel
        self.__tm = self.__model.get_task_manager()
        self.__lst_rec = lst_rec

    def abort(self):
        self.__tm.abort()

    def run(self):

        # Export station data
        res = self.__tm.process_station_data(self.__sel, report=False, export_data=True, export_data_recs=self.__lst_rec)
        res.add_data("title", "Export station (meta) data")

        # Result
        self.send_result(res)

    def send_result(self, result):
        self.signalResult.emit(result)

# Wrapper for Import/Export buffer started by UI
class Worker_Buffer_import_export_del(Worker):

    signalResult = pyqtSignal(Result)

    def __init__(self, model, mode, path_import = None, path_export = None):

        super().__init__()
        self.__model = model
        self.__buffer = self.__model.get_local_working_database()

        # Download Params
        self.__s_mode = mode
        self.s_path_import = path_import
        self.s_path_export = path_export

    def abort(self):
        self.__buffer.abort()

    def run(self):
        if self.__s_mode == "export":
            result = self.__buffer.db_buffer_export_to_zip()
        elif self.__s_mode == "import":
            result = self.__buffer.db_buffer_import_from_zip(self.s_path_import)
        elif self.__s_mode == "delete":
            result = self.__buffer.db_buffer_clean()
        else:
            result = Result(name="Buffer operation", err="internal error")
        self.send_result(result)

    def send_result(self, result):
        self.signalResult.emit(result)