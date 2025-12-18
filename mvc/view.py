# coding=utf-8

"""
View
"""

import ctypes, os, random, subprocess, sys, webbrowser
from collections import OrderedDict
from functools import partial
from pathlib import Path
from PyQt5 import uic
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from PyQt5.QtWidgets import *
from logic.helper import PrettyText, Result
from logic.selection import Selection
from logic.worker import Worker_DownloadCheckReportAvailStations, Worker_Buffer_import_export_del, Worker_CheckReport, \
    Worker_Export

# -------------------------------------
# View
# -------------------------------------

# Main View
file_main_win = os.path.dirname(os.path.realpath(__file__)) + "/data/mainwindow.ui"
class View(QMainWindow, uic.loadUiType(file_main_win)[0]):

    # --- Helper Classes

    # Drag and drop file list view
    class FileListView(QListView):

        def __init__(self, parent=None):
            super().__init__(parent)

            # Drag & Drop
            self.setDragEnabled(True)
            self.setAcceptDrops(True)
            self.setDropIndicatorShown(True)
            self.setDragDropMode(QListView.InternalMove)

            # Scrollbars
            self.setVerticalScrollBarPolicy(Qt.ScrollBarAsNeeded)
            self.setHorizontalScrollBarPolicy(Qt.ScrollBarAsNeeded)

            # contextmenu
            self.setContextMenuPolicy(Qt.CustomContextMenu)
            self.customContextMenuRequested.connect(self.show_context_menu)

            # Multi-Selection
            self.setSelectionMode(QListView.ExtendedSelection)

            # Listview Model
            self.lv_model = QStandardItemModel()
            self.setModel(self.lv_model)

            # Parent UI elements used
            self.parent = parent # View
            self.files_count = parent.files_count
            self.print_console = parent.print_gui

            # Font
            font = QFont()
            font.setFamily("Courier New")
            font.setPointSize(9)
            self.setFont(font)

        def model_block_signals(self, b_bool):
            self.lv_model.blockSignals(b_bool)
            if b_bool is False:
                self.reset()

        def get_selection(self):
            file_list = [self.lv_model.item(row).text() for row in range(self.lv_model.rowCount())]
            return file_list

        def update_files_count(self):
            i_count = self.lv_model.rowCount()
            self.files_count.setText(f"{i_count}" if i_count > 0 else "-")

        def all_list_sel(self):
            self.selectAll()

        def none_list_sel(self):
            self.clearSelection()

        def inv_list_sel(self):
            i_progress_starts = 100 # At how many items should the UI start a progress bar?
            progress = self.parent.get_smartprinter().progress
            workflow = self.parent.get_smartprinter().workflow
            selection_model = self.selectionModel()
            i_max = self.lv_model.rowCount()
            if i_max > i_progress_starts: # UI
                progress("pulse", abort=False)
                workflow("Inverting selection")
            for i_idx, row in enumerate(range(self.lv_model.rowCount())):
                if i_max > i_progress_starts: # UI
                    progress(i_idx, self.lv_model.rowCount(), 100, proz=True, abs=True, cli=False, problem="",info="")
                index = self.lv_model.index(row, 0)
                if selection_model.isSelected(index):
                    selection_model.select(index, selection_model.Deselect)
                else:
                    selection_model.select(index, selection_model.Select)
                if i_max > i_progress_starts:
                    if (i_idx % 20) == 0:
                        QApplication.processEvents()  # Give UI time to update
            if i_max > i_progress_starts: # UI
                workflow()
                progress("off")

        def clear_list_all(self):
            self.lv_model.clear()
            self.update_files_count()

        def clear_list_sel(self):
            i_progress_starts = 100 # At how many items should the UI start a progress bar?
            progress = self.parent.get_smartprinter().progress
            workflow = self.parent.get_smartprinter().workflow
            selected_indexes = self.selectedIndexes()
            i_idx = 0
            i_max = len(selected_indexes)
            self.model_block_signals(True)
            if i_max > i_progress_starts: # UI
                progress("pulse", abort=False)
                workflow("Remove selection")
            for index in sorted(selected_indexes, key=lambda x: x.row(), reverse=True):
                self.lv_model.removeRow(index.row())
                if i_max > i_progress_starts:
                    progress(i_idx, i_max, 100, proz=True, abs=True, cli=False, problem="", info="")
                    if (i_idx % 20) == 0:
                        QApplication.processEvents()  # Give UI time to update
            self.model_block_signals(False)
            if i_max > i_progress_starts:
                workflow()
                progress("off")
            self.update_files_count()

        def show_context_menu(self, pos: QPoint):
            menu = QMenu()
            delete_action = menu.addAction("Delete Selected")
            action = menu.exec_(self.mapToGlobal(pos))
            if action == delete_action:
                self.delete_selected_items()

        def delete_selected_items(self):
            selected_indexes = self.selectedIndexes()
            for index in sorted(selected_indexes, key=lambda x: x.row(), reverse=True):
                self.lv_model.removeRow(index.row())
            self.update_files_count()

        def dragEnterEvent(self, event):
            if event.mimeData().hasUrls():
                event.acceptProposedAction()
            else:
                event.ignore()

        def dropEvent(self, event):
            if event.mimeData().hasUrls():
                # Data
                lst_urls = event.mimeData().urls()
                i_added_files = 0
                i_progress_start_item_num = 100
                b_use_progress = False
                if len(lst_urls) > i_progress_start_item_num:
                    b_use_progress = True

                # UI
                self.model_block_signals(True)
                progress = self.parent.get_smartprinter().progress
                workflow = self.parent.get_smartprinter().workflow
                if b_use_progress:
                    progress("pulse", abort=False)
                    workflow("Loading files")

                # Loop urls
                for i_url, url in enumerate(lst_urls):
                    s_url = url.toLocalFile()
                    if s_url:
                        if Path(s_url).is_dir():
                            # url is dir -> get all files (recursively)
                            lst_files = list(Path(s_url).rglob("*.dat")) + list(Path(s_url).rglob("*.dat.gz"))
                            if len(lst_files) > i_progress_start_item_num:
                                b_use_progress = True
                            if b_use_progress:
                                progress("pulse", abort=False)
                                workflow("Loading files")
                            for i_file, s_file in enumerate(lst_files):
                                if b_use_progress:
                                    progress(i_file, len(lst_files), 100, proz=True, abs=True, cli=False, text=f"{s_file}",problem="", info="")
                                b_ok = self.add_file_to_list(s_file)
                                if b_ok:
                                    i_added_files += 1
                                if (i_added_files % 20) == 0:
                                    QApplication.processEvents() # Give UI time to update
                        else:
                            # url single file
                            if b_use_progress:
                                progress(i_url, len(lst_urls), 100, proz=True, abs=True, cli=False, text=f"{s_url}",problem="", info="")
                            b_ok = self.add_file_to_list(s_url)
                            if b_ok:
                                i_added_files += 1
                            if (i_added_files % 20) == 0:
                                QApplication.processEvents()  # Give UI time to update

                # UI
                self.add_file_to_list_info(i_added_files)
                self.model_block_signals(False)
                workflow()
                progress("off")

                # Accept drop of files
                event.acceptProposedAction()

        def add_file_to_list(self, s_file):
            s_file = str(s_file)  # Tweak -> all has to be string
            # Check ending
            suffixes = Path(s_file).suffixes
            if suffixes != ['.dat'] and suffixes != ['.dat', '.gz']:
                return False
            # Check existing
            for row in range(self.lv_model.rowCount()):
                if self.lv_model.item(row).text() == s_file:
                    return False
            # Add file
            item = QStandardItem(s_file)
            item.setFlags(
                item.flags() | Qt.ItemIsDragEnabled | Qt.ItemIsDropEnabled | Qt.ItemIsSelectable | Qt.ItemIsEnabled)
            self.lv_model.appendRow(item)
            return True

        def add_file_to_list_info(self, i_added_files):
            if i_added_files > 0:
                self.print_console(f"Added files to file list: {i_added_files}")
            else:
                self.print_console(
                    f"No files addded; either all were already in the list, or the suffix wasn’t supported: .dat or .dat.gz")
            self.update_files_count()


    def update_listview_to_dragdrop(self):

        index = self.splitter.indexOf(self.listView)

        # remove old listview
        self.listView.setParent(None)
        self.listView.deleteLater()

        # new drag & drop listview
        self.listView = self.FileListView(self)

        # put to old position
        self.splitter.insertWidget(index, self.listView)

    # ---

    # Constructor
    def __init__(self, model):

        # Super
        QMainWindow.__init__(self)

        # Instance
        self.__model = model
        self.__printer = self.__model.get_smart_printer()
        self.__s_working_dir = os.path.dirname(os.path.realpath(__file__)) # Working Dir
        self.__statusbar = QStatusBar()

        # Initialization
        self.setupUi(self)

        # Update Listview to drag and drop
        self.update_listview_to_dragdrop()

        # Progress
        self.hide_progress()

        # Windows
        dic_info = self.__model.get_prg_infos()
        self.setWindowTitle(dic_info["name"]+ " v." + dic_info["version"])
        self.setWindowIcon(QIcon(self.__s_working_dir + "/data/logo.png"))
        self.setStatusBar(self.__statusbar)
        try:
            if os.name == "nt":
                s_myappid = u'awi.functional-ecology.tinytools.0'
                ctypes.windll.shell32.SetCurrentProcessExplicitAppUserModelID(s_myappid)
        except:
            pass

        # Menubar
        # File
        action = QAction("Open file", self)
        action.triggered.connect(self.action_file_chooseFile)
        self.menuFile.addAction(action)
        action = QAction("Select folder", self)
        action.triggered.connect(self.action_file_chooseFolder)
        self.menuFile.addAction(action)
        self.menuFile.addSeparator()
        action = QAction("Quit", self)
        action.triggered.connect(self.action_file_quit)
        self.menuFile.addAction(action)
        # Station-to-archive
        action = QAction("Download station data files", self)
        action.triggered.connect(self.action_download_report_avail_station_files)
        self.menuStationToArchive.addAction(action)
        action = QAction("Check local station data files", self)
        action.triggered.connect(self.action_check_station_files)
        self.menuStationToArchive.addAction(action)
        # Meta data
        action = QAction("File ID, LR0001", self)
        action.triggered.connect(partial(self.action_export_data, ["0001"]))
        self.menuMetadata.addAction(action)
        action = QAction("Scientist description, LR0002", self)
        action.triggered.connect(partial(self.action_export_data, ["0002"]))
        self.menuMetadata.addAction(action)
        action = QAction("Messages, LR0003", self)
        action.triggered.connect(partial(self.action_export_data, ["0003"]))
        self.menuMetadata.addAction(action)
        action = QAction("Station description, LR0004", self)
        action.triggered.connect(partial(self.action_export_data, ["0004"]))
        self.menuMetadata.addAction(action)
        action = QAction("Radiosonde equipment, LR0005", self)
        action.triggered.connect(partial(self.action_export_data, ["0005"]))
        self.menuMetadata.addAction(action)
        action = QAction("Ozone equipment, LR0006", self)
        action.triggered.connect(partial(self.action_export_data, ["0006"]))
        self.menuMetadata.addAction(action)
        action = QAction("Station history, LR0007", self)
        action.triggered.connect(partial(self.action_export_data, ["0007"]))
        self.menuMetadata.addAction(action)
        action = QAction("Radiation instruments, LR0008", self)
        action.triggered.connect(partial(self.action_export_data, ["0008"]))
        self.menuMetadata.addAction(action)
        action = QAction("Assignment of radiation quantities, LR0009", self)
        action.triggered.connect(partial(self.action_export_data, ["0009"]))
        self.menuMetadata.addAction(action)
        self.menuMetadata.addSeparator()
        action = QAction("Create reference import file", self)
        action.setEnabled(False)
        self.menuMetadata.addAction(action)
        self.menuMetadata.addSeparator()
        action = QAction("Create all metadata files", self)
        action.triggered.connect(partial(self.action_export_data, self.__model.get_metadata_recs_str()))
        self.menuMetadata.addAction(action)
        self.menuMetadata.addSeparator()
        action = QAction("Refresh BSRN IDs database", self)
        self.menuMetadata.addAction(action)
        action.triggered.connect(self.action_bsrn_refresh)
        action = QAction("Refresh BSRN reference IDs database", self)
        action.setEnabled(False)
        self.menuMetadata.addAction(action)
        # Data
        action = QAction("Basic and other measurements, LR0100 + LR0300", self)
        action.triggered.connect(partial(self.action_export_data, ["0100", "0300"]))
        self.menuData.addAction(action)
        action = QAction("Other measurements in minutes intervals, LR0300", self)
        action.triggered.connect(partial(self.action_export_data, ["0300"]))
        self.menuData.addAction(action)
        action = QAction("Ultra-violet measurements, LR0500", self)
        action.triggered.connect(partial(self.action_export_data, ["0500"]))
        self.menuData.addAction(action)
        action = QAction("SYNOP, LR1000", self)
        action.triggered.connect(partial(self.action_export_data, ["1000"]))
        self.menuData.addAction(action)
        action = QAction("Radiosonde measurements, LR1100", self)
        action.triggered.connect(partial(self.action_export_data, ["1100"]))
        self.menuData.addAction(action)
        action = QAction("Ozone measurements, LR1200", self)
        action.triggered.connect(partial(self.action_export_data, ["1200"]))
        self.menuData.addAction(action)
        action = QAction("Expanded measurements in hours intervals, LR1300", self)
        action.triggered.connect(partial(self.action_export_data, ["1300"]))
        self.menuData.addAction(action)
        action = QAction("Other measurements at 10 m, LR3010", self)
        action.triggered.connect(partial(self.action_export_data, ["3010"]))
        self.menuData.addAction(action)
        action = QAction("Other measurements at 30 m, LR3030", self)
        action.triggered.connect(partial(self.action_export_data, ["3030"]))
        self.menuData.addAction(action)
        action = QAction("Other measurements at 300 m, LR3300", self)
        action.triggered.connect(partial(self.action_export_data, ["3300"]))
        self.menuData.addAction(action)
        self.menuData.addSeparator()
        action = QAction("Create all data files", self)
        action.triggered.connect(partial(self.action_export_data, self.__model.get_data_recs_str()))
        self.menuData.addAction(action)
        # Import
        action = QAction("Overwrite dataset", self)
        action.setEnabled(False)
        action.triggered.connect(self.action_import_overwriteDataset)
        self.menuImport.addAction(action)
        self.menuImport.addSeparator()
        action = QAction("Basic and other measurements, LR0100 + LR0300", self)
        action.setEnabled(False)
        action.triggered.connect(self.action_import_basicAndOtherMeasurements)
        self.menuImport.addAction(action)
        action = QAction("Ultra-violet measurements, LR0500", self)
        action.setEnabled(False)
        action.triggered.connect(self.action_import_ultraVioletMeasurements)
        self.menuImport.addAction(action)
        action = QAction("SYNOP, LR1000", self)
        action.setEnabled(False)
        action.triggered.connect(self.action_import_synop)
        self.menuImport.addAction(action)
        action = QAction("Radiosonde measurements, LR1100", self)
        action.setEnabled(False)
        action.triggered.connect(self.action_import_radiosondeMeasurements)
        self.menuImport.addAction(action)
        action = QAction("Ozone measurements, LR1200", self)
        action.setEnabled(False)
        action.triggered.connect(self.action_import_ozoneMeasurements)
        self.menuImport.addAction(action)
        action = QAction("Expanded measurements in hours intervals, Part I, LR1300", self)
        action.setEnabled(False)
        action.triggered.connect(self.action_import_expandedMeasurementsInHoursIntervalsPart1)
        self.menuImport.addAction(action)
        action = QAction("Other measurements at 10 m, LR3010", self)
        action.setEnabled(False)
        action.triggered.connect(self.action_import_otherMeasurementsAt10m)
        self.menuImport.addAction(action)
        action = QAction("Other measurements at 30 m, LR3030", self)
        action.setEnabled(False)
        action.triggered.connect(self.action_import_otherMeasurementsAt30m)
        self.menuImport.addAction(action)
        action = QAction("Other measurements at 300 m, LR3300", self)
        action.setEnabled(False)
        action.triggered.connect(self.action_import_otherMeasurementsAt300m)
        self.menuImport.addAction(action)
        self.menuImport.addSeparator()
        action = QAction("Create all import files", self)
        action.setEnabled(False)
        action.triggered.connect(self.action_import_createAllImportFiles)
        self.menuImport.addAction(action)
        # Tools
        action = QAction("Concatenate files...", self)
        action.setEnabled(False)
        action.triggered.connect(self.action_tools_concatenateFiles)
        self.menuTools.addAction(action)
        self.menuTools.addSeparator()
        action = QAction("Convert Windows End-of-Line to UNIX", self)
        action.setEnabled(False)
        action.triggered.connect(self.action_tools_convertWindowsEndOfLineToUnix)
        self.menuTools.addAction(action)
        action = QAction("Convert macOS End-of-Line to UNIX", self)
        action.setEnabled(False)
        action.triggered.connect(self.action_tools_convertMacOS9endOfLineToUnix)
        self.menuTools.addAction(action)
        self.menuTools.addSeparator()
        action = QAction("Decompress files", self)
        action.setEnabled(False)
        action.triggered.connect(self.action_tools_decompressFiles)
        self.menuTools.addAction(action)
        action = QAction("Compress files with gzip", self)
        action.setEnabled(False)
        action.triggered.connect(self.action_tools_compressFilesWithGzip)
        self.menuTools.addAction(action)
        self.menuTools.addSeparator()
        action = QAction("Convert tab files to unformatted format...", self)
        action.setEnabled(False)
        action.triggered.connect(self.action_tools_convertTabFilesToUnformattedFormat)
        self.menuTools.addAction(action)
        action = QAction("Convert tab files to formatted format...", self)
        action.setEnabled(False)
        action.triggered.connect(self.action_tools_convertTabFilesToFormattedFormat)
        self.menuTools.addAction(action)
        # Quality Check
        action = QAction("BSRN Recommended V2.0", self)
        action.setEnabled(False)
        action.triggered.connect(self.action_qc_bsrnRecommendedV20)
        self.menuQualityCheck.addAction(action)
        # Help
        action = QAction("About BSRN Tools", self)
        action.triggered.connect(self.action_help_about)
        self.menuHelp.addAction(action)
        self.menuHelp.addSeparator()
        action = QAction("Manual", self)
        action.triggered.connect(self.action_help_manual)
        self.menuHelp.addAction(action)
        self.menuHelp.addSeparator()
        action = QAction("BSRN Homepage", self)
        action.triggered.connect(self.action_help_bsrnHomepage)
        self.menuHelp.addAction(action)
        action = QAction("How to get the BSRN account", self)
        action.triggered.connect(self.action_help_howToGetTheBsrnAccount)
        self.menuHelp.addAction(action)
        action = QAction("BSRN Status", self)
        action.triggered.connect(self.action_help_bsrnStatus)
        self.menuHelp.addAction(action)
        action = QAction("BSRN snapshot 2015-09", self)
        action.triggered.connect(self.action_help_bsrnSnapshot2015_09)
        self.menuHelp.addAction(action)
        action = QAction("Station-to-archiv file format description", self)
        action.triggered.connect(self.action_help_stationToArchiveFileFormatDescription)
        self.menuHelp.addAction(action)
        self.menuHelp.addSeparator()
        action = QAction("Excel macro for BSRN .dat hourly averages", self)
        action.triggered.connect(self.action_help_macroEnabledExcelSpreadsheetToCalculateHourlyAveragesFromBsrnDatFiles)
        self.menuHelp.addAction(action)

        # Toolbar
        self.toolBar.setVisible(False) # DEV
        self.actionBeenden.setIcon(QIcon(os.path.dirname(os.path.realpath(__file__)) + "/data/icon_exit.png"))
        action = QAction("Test: Download manager dialog", self)
        action.triggered.connect(self.dev_test1)
        action.setIcon(QIcon(os.path.dirname(os.path.realpath(__file__)) + "/data/icon_1.png"))
        self.toolBar.addAction(action)
        action = QAction("Test: Parameter selection dialog", self)
        action.triggered.connect(self.dev_test2)
        action.setIcon(QIcon(os.path.dirname(os.path.realpath(__file__)) + "/data/icon_2.png"))
        self.toolBar.addAction(action)
        action = QAction("Test: Working ...", self)
        action.triggered.connect(self.dev_test3)
        action.setIcon(QIcon(os.path.dirname(os.path.realpath(__file__)) + "/data/icon_3.png"))
        self.toolBar.addAction(action)
        self.actionInfo.triggered.connect(self.info_prg)
        self.actionBeenden.setIcon(QIcon(os.path.dirname(os.path.realpath(__file__)) + "/data/icon_exit.png"))
        self.actionBeenden.triggered.connect(self.quit_prg)
        self.toolBar.addAction(self.actionBeenden)

        # Info
        self.actionInfo.triggered.connect(self.info_prg)

        # Buttons,etc verbinden
        self.progressAbort.clicked.connect(self.stop_worker)
        self.buttonConsoleClear.clicked.connect(self.console_clear)
        self.buttonConsoleSave.clicked.connect(self.console_save)
        self.buffer_switch.clicked.connect(self.action_buffer_switch)
        self.buffer_refresh.clicked.connect(self.action_buffer_refresh)
        self.buffer_del.clicked.connect(self.action_buffer_del)
        self.buffer_show.clicked.connect(self.action_buffer_show)
        self.files_clear.clicked.connect(self.action_files_clear_sel)
        self.files_inv.clicked.connect(self.action_files_inv_sel)
        self.files_all.clicked.connect(self.action_files_all_sel)
        self.files_none.clicked.connect(self.action_files_none_sel)
        self.files_clear_all.clicked.connect(self.action_files_clear_all)
        self.buffer_export.clicked.connect(self.action_buffer_export)
        self.buffer_import.clicked.connect(self.action_buffer_import)

        # UI elements setup
        self.update_buffer_switch_status()
        self.update_buffer_size()
        self.comboVerbosity.setCurrentIndex(0)
        if self.__model.is_verbose():
            self.comboVerbosity.setCurrentIndex(1)
        if self.__model.is_debug():
            self.comboVerbosity.setCurrentIndex(2)

        # Connect Verbosity Change
        self.comboVerbosity.currentIndexChanged.connect(self.change_verbosity)

    # --- Admin

    # Check if Lookup ios initialized
    def check_if_bsrn_id_system_is_working(self):
        lup = self.__model.get_bsrn_id_system()
        if not lup.is_working():
            s_err = PrettyText.lst2strR(lup.get_init_error())
            s_title_window = "Download of BSRN Ids"
            s_title = "Problems with BSRN Ids"
            s_text = f"Something went wrong during evaluating the BSRN Ids. Please refer to the console output below for more detailed information."
            s_folder = lup.get_bsrn_id_dir()
            self.__popup_generic(title=None, header=s_title, text=s_text, text_extra=s_err, folder=s_folder)
            return False
        else:
            return True

    # Connect to Observer
    def connect_to_observer(self, observer):
        # Print to console
        observer.signal_print.connect(self.print_gui)
        # Progress
        observer.signal_progress_hide.connect(self.hide_progress)
        observer.signal_progress_show.connect(self.show_progress)
        observer.signal_progress_set_min_max.connect(self.set_progressbar_min_max)
        observer.signal_progress_set_wert.connect(self.set_progressbar_val)
        observer.signal_progress_set_text.connect(self.set_progressbar_text)
        observer.signal_progress_show_prozent.connect(self.set_progressbar_percent_show)
        observer.signal_statusbar_set_text.connect(self.set_statusbar_text)
        observer.signal_workflow_set.connect(self.set_workflow)
        observer.signal_input_combo.connect(self.user_input_combo)
        observer.signal_progress_msg.connect(self.set_progressbar_msg)
        observer.signal_buffer.connect(self.set_buffer_status)

    def isWorkerRunning(self, **kwargs):
        if self.__model.get_worker() is None:
            return False
        else:
            if self.__model.get_worker().isRunning():
                sMsg1 = "Another process is runnning ..."
                sMsg2 = "Please abort it or wait until it has finished."
                self.__popup_generic(header=sMsg1, text=sMsg2, icon="info")
                return True
            else:
                return False

    # Get Smartprinter
    def get_smartprinter(self):
        return self.__printer

    # Print to Console
    def print_gui(self, s_text, s_appendix="\n"):
        self.textBox.insertPlainText(s_text + s_appendix)
        self.textBox.moveCursor(QTextCursor.End)

    # Print Exception
    def print_exception(self, ex):
        self.print_gui("* An error occurred")
        self.print_gui(str(ex))

    # Progressbar
    def show_progress(self, b_abort = True, b_pulse = False):
        self.files_label.hide()
        self.files_label_selection.hide()
        self.files_count.hide()
        self.files_clear.hide()
        self.files_clear_all.hide()
        self.files_inv.hide()
        self.files_none.hide()
        self.files_all.hide()
        if b_abort:
            self.progressAbort.show()
        else:
            self.progressAbort.hide()
        #self.set_workflow("", 0, 0) # why?
        self.progressBar.reset()
        if b_pulse:
            self.progressBar.setMinimum(0)
            self.progressBar.setMaximum(0)
        self.progress_msg_problems.show()
        self.progress_msg_info.show()
        self.progressBar.show()

    def hide_progress(self, b_reset = True):
        self.files_label.show()
        self.files_label_selection.show()
        self.files_count.show()
        self.files_clear.show()
        self.files_clear_all.show()
        self.files_inv.show()
        self.files_none.show()
        self.files_all.show()
        self.progress_msg_problems.hide()
        self.progress_msg_info.hide()
        self.progressBar.hide()
        self.labelExpanderWorkflowStep.setText("")
        self.labelExpanderWorkflowInfo.setText("")
        self.progressBar.reset()
        self.progressAbort.hide()

    def set_progressbar_min_max(self, i_min=0, i_max=0):
        self.progressBar.setMinimum(i_min)
        self.progressBar.setMaximum(i_max)

    def set_progressbar_val(self, i_val):
        self.progressBar.setValue(i_val)

    def set_progressbar_percent_show(self, b_perc_show):
        self.progressBar.setTextVisible(b_perc_show)

    def set_progressbar_msg(self, i_problems, s_info):
        lst = [
            (i_problems, self.progress_msg_problems),
            (s_info, self.progress_msg_info)
        ]
        for val, widget in lst:
            if val in ["None", "", 0, "0"]:
                val = None
            if val is not None:
                widget.show()
                widget.setText(str(val))
                widget.adjustSize()
            else:
                widget.hide()

    def set_progressbar_text(self, s_text):
        if s_text != "":
            self.progressBar.setFormat(s_text)

    def set_progressbar_msg(self, i_problems, s_info):
        lst = [
            (i_problems, self.progress_msg_problems),
            (s_info, self.progress_msg_info)
        ]
        for val, widget in lst:
            if val in ["None", "", 0, "0"]:
                val = None
            if val is not None:
                widget.show()
                widget.setText(str(val))
                widget.adjustSize()
            else:
                widget.hide()

    # Update Buffer size
    def set_buffer_status(self):
        self.update_buffer_size()

    # Workflow
    def set_workflow(self, s_text, i_step_now, i_step_max):
        if s_text == "":
            self.labelExpanderWorkflowStep.setText("Working ...")
            self.labelExpanderWorkflowInfo.setText("")
        elif i_step_now != 0 and i_step_max != 0:
            self.labelExpanderWorkflowStep.setText("Working [" + str(i_step_now) + "/" + str(i_step_max) + "]")
            self.labelExpanderWorkflowInfo.setText(s_text)
        else:
            self.labelExpanderWorkflowStep.setText("Working")
            self.labelExpanderWorkflowInfo.setText(s_text)

    # Statusbar
    def set_statusbar_text(self, s_text=""):
        if s_text == "":
            self.__statusbar.clearMessage()
        else:
            self.__statusbar.showMessage("  " + s_text, 2000)

    # Switch Buffer
    def update_buffer_switch_status(self):
        if self.__model.get_local_working_database().db_buffer_is_active():
            self.buffer_switch.setIcon(QIcon(os.path.dirname(os.path.realpath(__file__)) + "/data/icon_switch_on.png"))
            self.buffer_switch.setToolTip("on")
            self.buffer_refresh.setEnabled(True)
        else:
            self.buffer_switch.setIcon(QIcon(os.path.dirname(os.path.realpath(__file__)) + "/data/icon_switch_off.png"))
            self.buffer_switch.setToolTip("off")
            self.buffer_refresh.setEnabled(False)
        if self.__model.get_local_working_database().db_buffer_is_refresh():
            self.buffer_refresh.setChecked(True)
        else:
            self.buffer_refresh.setChecked(False)


    def update_buffer_size(self):
        lwdb = self.__model.get_local_working_database()
        i_size_total_mb = lwdb.db_get_size_all_mb()
        s_size_total_mb = lwdb.db_get_size_all_as_string()
        if i_size_total_mb > 0:
            self.buffer_size_mb.setText(s_size_total_mb)
        else:
            self.buffer_size_mb.setText("-")

    # Give QT-Loop time to update the widget
    def __giveQtTimeToUpdate(self):
        QApplication.processEvents()

    # Center child window to view (or other window)
    def center_widget(self, childWidget, parentWidget=None):
        if childWidget is None:
            return
        if parentWidget is None:
            parentWidget = self
        childWidget.move(parentWidget.frameGeometry().center().x() - int(childWidget.frameGeometry().width() / 2),
                         parentWidget.frameGeometry().center().y() - int(childWidget.frameGeometry().height() / 2))

    # Generic creation of a dialog window
    def __popup_generic(self, **kwargs):
        """
        Shows a compact, HTML-enabled information dialog with an optional link.
        Kwargs:
            title (str): Window title
            header (str): Header (displayed in bold)
            text (str): text
            text_extra (str): Optional extra text
            icon (str): "info", "warn", "error"
            folder (str): Directory to open
        """

        s_icon = kwargs.get("icon", "info")
        s_titel = kwargs.get("title", "")
        s_header = kwargs.get("header", "")
        s_text = kwargs.get("text", "")
        s_text_extra = kwargs.get("text_extra", "")
        s_folder = kwargs.get("folder", None)

        # Icons aus dem Standard-Stil
        icon_map = {
            "info": QApplication.style().standardIcon(QApplication.style().SP_MessageBoxInformation),
            "warn": QApplication.style().standardIcon(QApplication.style().SP_MessageBoxWarning),
            "error": QApplication.style().standardIcon(QApplication.style().SP_MessageBoxCritical)
        }
        icon_pixmap = icon_map.get(s_icon, icon_map["info"]).pixmap(48, 48)

        # Dialog erstellen
        dlg = QDialog()
        dlg.setWindowTitle(s_titel)
        dlg.setWindowModality(Qt.ApplicationModal)

        # Main layout
        layout = QVBoxLayout()
        layout.setSpacing(4)

        # Icon + fettgedruckter Haupttext
        icon_label = QLabel()
        icon_label.setPixmap(icon_pixmap)

        # Header
        header_label = QLabel()
        header_label.setTextFormat(Qt.RichText)
        header_label.setTextInteractionFlags(Qt.TextBrowserInteraction)
        header_label.setOpenExternalLinks(True)
        header_label.setWordWrap(True)
        header_label.setText(f"<b>{s_header}</b>")
        hlayout = QHBoxLayout()
        hlayout.addWidget(icon_label)
        hlayout.addWidget(header_label, stretch=1)
        layout.addLayout(hlayout)

        # Text
        if s_text:
            text_label = QLabel()
            text_label.setTextFormat(Qt.RichText)
            text_label.setTextInteractionFlags(Qt.TextBrowserInteraction)
            text_label.setOpenExternalLinks(True)
            text_label.setWordWrap(True)
            text_label.setText(s_text)
            layout.addWidget(text_label)

        # Optionaler Detailbereich
        if s_text_extra:
            extra_box = QTextBrowser()
            extra_box.setText(s_text_extra)
            extra_box.setMinimumHeight(100)
            layout.addWidget(extra_box)

        # Buttons
        layout_buttons = QHBoxLayout()
        # Files Button
        if s_folder:
            btn_files = QPushButton("Files")
            btn_files.clicked.connect(lambda: dlg.done(10))
            layout_buttons.addWidget(btn_files)
        # OK-Button
        btn_ok = QPushButton("OK")
        btn_ok.clicked.connect(dlg.accept)
        layout_buttons.addWidget(btn_ok)
        layout.addLayout(layout_buttons)

        # Layout
        dlg.setLayout(layout)
        for i in range(layout.count()): # All widgets size policy to minimum
            widget = layout.itemAt(i).widget()
            if widget is not None:
                widget.setSizePolicy(QSizePolicy.Minimum, QSizePolicy.Minimum)
        dlg.setFixedWidth(300)
        dlg.adjustSize()
        dlg.setFixedSize(dlg.size())

        # Start
        ant = dlg.exec_()
        if ant == 10:
            # Open Folder
            if s_folder is not None:
                s_folder = 'file://' + str(s_folder)
                if sys.platform == "win32":
                    os.startfile(s_folder)
                elif sys.platform == "darwin":  # macOS
                    subprocess.Popen(["open", s_folder])
                else:  # Linux
                    subprocess.Popen(["xdg-open", s_folder])

    def __popup_text(self, **kwargs):
        """
        Create a pop-up window with information
        Kwargs:
            titel (str): Title text
            text (str): Window text
            file (bool): Text file
            html (bool): Content is HTML
            browser (bool): Browser instead of text widget
            save (bool): Content can be saved
            title_capitalize (bool): Capitalize title
        """

        b_save = kwargs["save"] if ("save" in kwargs and kwargs["save"] in [True, False]) else False
        b_html = kwargs["html"] if ("html" in kwargs and kwargs["html"] in [True, False]) else False
        b_browser = kwargs["browser"] if ("browser" in kwargs and kwargs["browser"] in [True, False]) else False
        b_capitalize_title = kwargs["title_capitalize"] if (
                    "title_capitalize" in kwargs and kwargs["title_capitalize"] in [True, False]) else False
        s_title = kwargs["title"] if "title" in kwargs else None
        s_file = kwargs["file"] if "file" in kwargs else None
        s_text = kwargs["text"] if "text" in kwargs else None

        # Tests
        if b_capitalize_title and s_title is not None:
            s_title = s_title.title()
        if s_file not in [None, ""] and s_text not in [None, ""]:
            self.print_gui("Internal error: File and Text given")
        if s_file in [None, ""] and s_text in [None, ""]:
            self.print_gui("Internal error: No File or Text given")

        # Dialog
        # *** Textwidget
        dia = QDialog()
        dia.setWindowTitle(s_title)
        dia.setWindowModality(Qt.ApplicationModal)
        dia.setWindowFlags(Qt.Dialog | Qt.CustomizeWindowHint | Qt.WindowTitleHint | Qt.WindowCloseButtonHint)
        vbox = QVBoxLayout(dia)
        vbox.setContentsMargins(5, 5, 5, 5)
        vbox.setSpacing(6)
        if s_file not in [None, ""]:
            try:
                file = open(s_file, "r", encoding="utf-8")
                s_text = file.read()
                file.close()
            except Exception as ex:
                s_text = "Internal error: " + str(ex)
        text = QTextEdit()

        if b_html:
            text.setHtml(s_text)
        else:
            text.setPlainText(s_text)

        font = QFont()
        font.setFamily("Courier New")
        font.setPointSize(9)
        text.setFont(font)
        text.setLineWrapMode(QTextEdit.NoWrap)
        text.setReadOnly(True)
        vbox.addWidget(text)
        buttonBox = QDialogButtonBox()
        if b_save:
            btnSave = QPushButton()
            btnSave.setText("Save")
            btnSave.setToolTip("Saving to file")
            btnSave.clicked.connect(lambda: QDialog.done(dia, 2))
            buttonBox.addButton(btnSave, QDialogButtonBox.NoRole)
        # OK
        btn_ok = QPushButton()
        btn_ok.setText("OK")
        btn_ok.clicked.connect(lambda: QDialog.done(dia, 0))
        buttonBox.accepted.connect(dia.accept)
        buttonBox.addButton(btn_ok, QDialogButtonBox.NoRole)
        # font = QFont()
        # font.setFamily("Arial")
        # font.setPointSize(10)
        # buttonBox.setFont(font)
        vbox.addWidget(buttonBox)
        dia.resize(800, 600)

        # *** Dialog open
        ant = dia.exec_()
        if b_browser and ant == 3:
            # External Webbrowser
            webbrowser.open("file://" + s_file)
        if b_save and ant == 2:
            # Save
            if s_title is not None:
                s_filename_default = PrettyText.clean_2filename(s_title)
            else:
                s_filename_default = "text"
            sEnd = ".html" if b_html else ".txt"
            s_filename = QFileDialog.getSaveFileName(self, "Save to file", s_filename_default + sEnd)[0]
            if s_filename != "":
                try:
                    with open(s_filename, "w", encoding="utf-8", newline="\r\n") as file:
                        file.write(s_text)
                    self.popup_generic(title="Save to file", head="Data Successfully saved to file",
                                       info="File: " + s_filename)
                except Exception as ex:
                    self.ui_printConsoleNormal(("Error saving to file (" + s_filename + "): " + str(ex)))

    def user_input_combo(self, s_titel, s_label, lst_choices):

        answerInput = self.__model.get_smart_printer().set_input_answer
        pn = self.print_gui

        idx = None
        while True:
            s_item, b_ok = QInputDialog.getItem(self, s_titel, s_label, lst_choices, 0, False)
            i_idx = lst_choices.index(s_item)
            if b_ok:
                break
        answerInput(i_idx)

    # --- Actions
    def action_file_chooseFile(self):
        filter = "dat files (*.dat *.dat.gz)"
        s_file = QFileDialog.getOpenFileName(self, "Choose file", self.__model.get_working_dir(), filter)[0]
        if s_file not in ["", None]:
            b_ok = self.listView.add_file_to_list(s_file)
            i_added_files = 0
            if b_ok:
                i_added_files += 1
            self.listView.add_file_to_list_info(i_added_files)
            self.listView.update_files_count()

    def action_file_chooseFolder(self):
        pn = self.__printer.normal
        workflow = self.__printer.workflow
        progress = self.__printer.progress

        # UI
        s_dir = QFileDialog.getExistingDirectory(self, "Choose folder", self.__model.get_working_dir())
        workflow("Loading files")
        progress("pulse", abort=False)
        if s_dir not in ["", None]:
            self.listView.model_block_signals(True)
            lst_files = list(Path(s_dir).rglob("*.dat")) + list(Path(s_dir).rglob("*.dat.gz"))
            i_added_files = 0
            for i_idx, s_file in enumerate(lst_files):
                progress(i_idx, len(lst_files), 100, proz=True, abs=True, cli=False, text=f"{s_file}", problem="", info="")
                b_ok = self.listView.add_file_to_list(s_file)
                if b_ok:
                    i_added_files += 1
                if (i_idx % 20) == 0:
                    self.__giveQtTimeToUpdate()
            self.listView.model_block_signals(False)
            self.listView.add_file_to_list_info(i_added_files)
            self.listView.update_files_count()
        # UI
        workflow()
        progress("off")

    def action_file_quit(self):
        self.quit_prg()

    def action_download_report_avail_station_files(self):
        pn = self.__printer.normal
        if self.isWorkerRunning():
            return

        # Dialog
        dia = DialogDownload(self.__model)
        ant = dia.exec_()
        if ant == 0:
            return
        dic_conf = dia.get_config()

        # Get data from dia
        s_station_code = dic_conf.get("stations_code", "")
        s_month_code = dic_conf.get("months_code", "")
        s_year_code = dic_conf.get("years_code", "")
        s_download_path = dic_conf.get("dl_dir", "")
        s_ftp_server = dic_conf.get("ftp_server", "")
        s_ftp_user = dic_conf.get("ftp_user", "")
        s_ftp_pw = dic_conf.get("ftp_pw", "")
        b_unzip = dic_conf.get("opt_zip", False)
        b_report = dic_conf.get("opt_check", False)
        b_check_availability = dic_conf.get("opt_avail", False)
        lst_station = dic_conf.get("stations", list())
        lst_month = dic_conf.get("months", list())
        lst_year = dic_conf.get("years", list())
        # Save cfg in model (for saveing on file later)
        self.__model.set_stations_code(s_station_code)
        self.__model.set_months_code(s_month_code)
        self.__model.set_years_code(s_year_code)
        self.__model.set_working_dir(s_download_path)
        self.__model.set_opt_unzip(b_unzip)
        self.__model.set_opt_report(b_report)
        self.__model.set_opt_avail(b_check_availability)
        self.__model.set_ftp_server(s_ftp_server)
        self.__model.set_ftp_user(s_ftp_user)
        self.__model.set_ftp_pw(s_ftp_pw)

        # Config save
        self.__model.cfg_save()

        # Start parallel container
        w = Worker_DownloadCheckReportAvailStations(self.__model, s_download_path, s_ftp_server, s_ftp_user, s_ftp_pw, b_unzip, b_report, b_check_availability, lst_station, lst_month, lst_year)
        self.start_worker(w, self.worker_finished_generic)

    def action_check_station_files(self):

        # Selection
        lst_files = self.listView.get_selection()
        sel = Selection(self.__model)
        sel.load_filenames(lst_files)

        # Start parallel container
        w = Worker_CheckReport(self.__model, sel)
        self.start_worker(w, self.worker_finished_generic)

    def action_bsrn_refresh(self):
        bsrn = self.__model.get_bsrn_id_system()
        bsrn.initialize()
        if bsrn.is_working():
            s_title_window = "BSRN Ids"
            s_title = "BSRN Id database"
            s_text = f"The BSRN Id database was refreshed successfully."
            self.__popup_generic(title=None, header=s_title, text=s_text)
            return False
        else:
            s_err = bsrn.get_init_error()
            s_title_window = "BSRN Ids"
            s_title = "BSRN Id database"
            s_text = f"Something went wrong during refreshing the BSRN Ids. Please refer to the console output below for more detailed information."
            s_folder = bsrn.get_bsrn_id_dir()
            self.__popup_generic(title=None, header=s_title, text=s_text, text_extra=s_err, folder=s_folder)
            return False

    def action_export_data(self, lst_rec):

        # Selection
        lst_files = self.listView.get_selection()
        if len(lst_files) == 0:
            return
        sel = Selection(self.__model)
        sel.load_filenames(lst_files)

        # Process data (Export)
        # Selection
        lst_files = self.listView.get_selection()
        sel = Selection(self.__model)
        sel.load_filenames(lst_files)

        # Start parallel container
        w = Worker_Export(self.__model, sel, lst_rec)
        self.start_worker(w, self.worker_finished_generic)

    def action_import_basicAndOtherMeasurements(self):
        pn = self.__printer.normal
        pn("import.basic_and_other_measurements")

    def action_import_ultraVioletMeasurements(self):
        pn = self.__printer.normal
        pn("import.ultra_violet_measurements")

    def action_import_synop(self):
        pn = self.__printer.normal
        pn("import.synop")

    def action_import_radiosondeMeasurements(self):
        pn = self.__printer.normal
        pn("import.radiosonde_measurements")

    def action_import_ozoneMeasurements(self):
        pn = self.__printer.normal
        pn("import.ozone_measurements")

    def action_import_expandedMeasurementsInHoursIntervalsPart1(self):
        pn = self.__printer.normal
        pn("import.expanded_measurements_in_hours_intervals_part_1")

    def action_import_otherMeasurementsAt10m(self):
        pn = self.__printer.normal
        pn("import.other_measurements_at_10m")

    def action_import_otherMeasurementsAt30m(self):
        pn = self.__printer.normal
        pn("import.other_measurements_at_30m")

    def action_import_otherMeasurementsAt300m(self):
        pn = self.__printer.normal
        pn("import.other_measurements_at_300m")

    def action_import_createAllImportFiles(self):
        pn = self.__printer.normal
        pn("import.create_all_import_files")

    def action_import_overwriteDataset(self):
        pn = self.__printer.normal
        pn("import.overwrite_dataset")

    def action_tools_concatenateFiles(self):
        pn = self.__printer.normal
        pn("tools.concatenate_files")
        dia = DialogConcatenate(self.__model)
        ant = dia.exec_()
        if ant == 0:
            return

    def action_tools_convertWindowsEndOfLineToUnix(self):
        pn = self.__printer.normal
        pn("tools.convert_windows_end_of_line_to_unix")

    def action_tools_convertMacOS9endOfLineToUnix(self):
        pn = self.__printer.normal
        pn("tools.convert_mac_os_9_end_of_line_to_unix")

    def action_tools_decompressFiles(self):
        pn = self.__printer.normal
        pn("tools.decompress_files")

    def action_tools_compressFilesWithGzip(self):
        pn = self.__printer.normal
        pn("tools.compress_files_with_gzip")

    def action_tools_convertTabFilesToUnformattedFormat(self):
        pn = self.__printer.normal
        pn("tools.convert_tab_files_to_unformatted_format")
        dia = DialogUnformated(self.__model)
        ant = dia.exec_()
        if ant == 0:
            return

    def action_tools_convertTabFilesToFormattedFormat(self):
        pn = self.__printer.normal
        pn("tools.convert_tab_fFiles_to_formatted_format")
        dia = DialogFormated(self.__model)
        ant = dia.exec_()
        if ant == 0:
            return

    def action_qc_bsrnRecommendedV20(self):
        pn = self.__printer.normal
        pn("qc.bsrn_recommended_v20")

    def action_help_about(self):
        pn = self.__printer.normal
        QMessageBox.about(self, "About this program", self.__model.get_prg_infos()["info"])

    def action_help_manual(self):
        self.__printer.status("Opening manual in browser ...")
        webbrowser.open("https://wiki.pangaea.de/wiki/BSRN_Toolbox")

    def action_help_bsrnHomepage(self):
        self.__printer.status("Opening BSRN homepage in browser ...")
        webbrowser.open("http://bsrn.awi.de")

    def action_help_howToGetTheBsrnAccount(self):
        self.__printer.status("Opening howto-get-an-BSRN-account in browser ...")
        webbrowser.open("https://bsrn.awi.de/data/data-retrieval-via-pangaea/")

    def action_help_bsrnStatus(self):
        self.__printer.status("Opening BSRN-status in browser ...")
        webbrowser.open("https://www.pangaea.de/PHP/BSRN_Status.php")

    def action_help_bsrnSnapshot2015_09(self):
        self.__printer.status("Opening BSRN-snapshot 2015 09 in browser ...")
        webbrowser.open("https://doi.pangaea.de/10.1594/PANGAEA.852720")

    def action_help_stationToArchiveFileFormatDescription(self):
        self.__printer.status("Opening definition of station-to-archive file format in browser ...")
        webbrowser.open("https://bsrn.awi.de/data/station-to-archive-file-format/")

    def action_help_macroEnabledExcelSpreadsheetToCalculateHourlyAveragesFromBsrnDatFiles(self):
        self.__printer.status('Opening "Excel macro for BSRN .dat hourly averages" in browser ...')
        webbrowser.open("https://epic.awi.de/id/eprint/42267/")

    # --- DEV
    def dev_test1(self):
        if self.isWorkerRunning():
            return
        self.action_download_report_avail_station_files()

    def dev_test2(self):
        pn = self.__printer.normal
        lstSetting = ["LR0100", "LR0300", "LR0100plusLR0300", "LR0500"]
        sSetting = random.choice(lstSetting)
        pn(f"dev: dialog.test: parameter_selection")
        pn(f"random setting: {sSetting}")
        dia = DialogSelectParam(self.__model, sSetting)
        ant = dia.exec_()
        if ant == 0:
            return

    def dev_test3(self):
        pn = self.__printer.normal
        pn(f"dev: dialog.test: empty")


    # --- Admin

    # Test whether a process is already running
    def ready(self):
        # Is BSRN Id System working
        b_ok = self.check_if_bsrn_id_system_is_working()
        if not b_ok:
            return False
        # Is
        if self.__model.get_worker() is None:
            return True
        elif self.__model.get_worker().isRunning():
            self.__popup_generic(title="Process running", header="There is already a process runnning", text="Please wait until the process is finished ...", icon="info")
            return False
        else:
            return True

    # Start Worker
    def start_worker(self, task, finish):

        if task is None or finish is None:
            return

        task.connect_finish(finish)  # Closing method

        if not self.ready():
            return
        else:
            self.__model.set_worker(task)
            task.start()

    # Stop Worker
    def stop_worker(self):
        if self.__model.get_worker() is not None and self.__model.get_worker().isRunning():
            self.print_gui("An abort command has been send to the running process. Please wait for the reaction ...")
            # mb = QMessageBox.warning(self, " ", "Do you really want to abort the running process?", QMessageBox.Yes | QMessageBox.No)
            # if mb == QMessageBox.Yes:
            self.__model.get_worker().abort()

    # Worker is finished

    def worker_finished_buffer_export_import_delete(self, result):
        # Recalc Buffer Size
        self.update_buffer_size()
        # Answer
        s_title = result.get_data("title")
        s_header = result.get_data("header")
        s_text = result.get_data("text")
        s_text_extra = result.get_data("text_extra")
        s_icon = result.get_data("icon")
        if result.is_ok():
            # OK
            self.__popup_generic(title=s_title,
                                 header=s_header,
                                 text=s_text,
                                 text_extra=s_text_extra,
                                 icon=s_icon)
        else:
            # Error
            self.__popup_generic(title=s_title,
                                 header=s_header,
                                 text=s_text,
                                 icon="error",
                                 text_extra=result.get_err_warn_info_string())

    def worker_finished_generic(self, result):

        self.update_buffer_size()
        if result is not None:
            s_title = result.get_data("title")
            if result.is_ok():
                # Get settings
                s_files = result.get_data("files")
                s_log = result.get_data("log")
                if s_log is not None:
                    # Dialog
                    dia = QDialog()
                    dia.setWindowTitle(s_title)
                    dia.setWindowModality(Qt.ApplicationModal)
                    dia.setWindowFlags(
                        Qt.Dialog | Qt.CustomizeWindowHint | Qt.WindowTitleHint | Qt.WindowCloseButtonHint)
                    vbox = QVBoxLayout(dia)
                    #vbox.setContentsMargins(5, 5, 5, 5)
                    vbox.setSpacing(6)
                    text = QTextEdit()
                    font = QFont()
                    font.setFamily("Courier New")
                    font.setPointSize(9)
                    text.setFont(font)
                    text.setLineWrapMode(QTextEdit.NoWrap)
                    text.setReadOnly(True)
                    text.setText(s_log)
                    vbox.addWidget(text)
                    button_box = QDialogButtonBox()
                    button_box = QDialogButtonBox()
                    # OK
                    btn_ok = QPushButton()
                    btn_ok.setText("OK")
                    btn_ok.clicked.connect(lambda: QDialog.done(dia, 0))
                    button_box.accepted.connect(dia.accept)
                    button_box.addButton(btn_ok, QDialogButtonBox.NoRole)
                    vbox.addWidget(button_box)
                    # Save log
                    btn_save = QPushButton()
                    btn_save.setText("Save log")
                    btn_save.clicked.connect(lambda: QDialog.done(dia, 1))
                    button_box.accepted.connect(dia.accept)
                    button_box.addButton(btn_save, QDialogButtonBox.NoRole)
                    vbox.addWidget(button_box)
                    # Files
                    if s_files is not None:
                        btn_log = QPushButton()
                        btn_log.setText("Files")
                        btn_log.setToolTip("Open working directory")
                        btn_log.clicked.connect(lambda: QDialog.done(dia, 2))
                        button_box.accepted.connect(dia.accept)
                        button_box.addButton(btn_log, QDialogButtonBox.NoRole)
                        vbox.addWidget(button_box)
                    # Resize
                    dia.resize(800, 600)
                    # Start
                    ant = dia.exec_()
                    if ant == 1:
                        s_filename = QFileDialog.getSaveFileName(self, "Save log in file", "log.txt")[0]
                        if s_filename == "":
                            return
                        try:
                            with open(s_filename, "w", encoding='utf-8') as file:
                                file.write(s_log)
                            self.print_gui("log was saved in file: " + s_filename)
                        except Exception as ex:
                            self.print_exception(ex)
                    elif ant == 2:
                        if s_files is not None:
                            s_files = 'file://' + str(s_files)
                            if sys.platform == "win32":
                                os.startfile(s_files)
                            elif sys.platform == "darwin":  # macOS
                                subprocess.Popen(["open", s_files])
                            else:  # Linux
                                subprocess.Popen(["xdg-open", s_files])
                else:
                    # No table arrived
                    self.__popup_generic(title=s_title,
                                         header=s_title,
                                         icon="error",
                                         text="The server answerd with nothing",
                                         text_extra=result.get_err_warn_info_string())
            else:
                # Error
                self.__popup_generic(title=s_title,
                                     header=s_title,
                                     text="Something unexpected happend ...",
                                     icon="error",
                                     text_extra=result.get_err_warn_info_string())

    # --- Logic

    # Info
    def info_prg(self):
        QMessageBox.about(self, "About", self.__model.get_prg_infos()["info"])

    # Quit
    def quit_prg(self, event=None):
        self.__model.cfg_save() # Save cfg on disk
        self.__model.get_local_working_database().db_buffer_save_to_disk() # Save buffer on disk
        self.close()

    # ---

    # Invert seleciton in from list
    def action_files_inv_sel(self):
        self.listView.inv_list_sel()

    def action_files_all_sel(self):
        self.listView.all_list_sel()


    def action_files_none_sel(self):
        self.listView.none_list_sel()

    # Clear selected files from list
    def action_files_clear_sel(self):
        self.listView.clear_list_sel()

    # Clear all files from file list
    def action_files_clear_all(self):
        self.listView.clear_list_all()

    # ---

    # Switch buffer
    def action_buffer_switch(self):
        self.__model.get_local_working_database().db_buffer_set_active(not self.__model.get_local_working_database().db_buffer_is_active())
        self.update_buffer_switch_status()

    def action_buffer_refresh(self):
        self.__model.get_local_working_database().db_buffer_set_refresh(not self.__model.get_local_working_database().db_buffer_is_refresh())
        self.update_buffer_switch_status()

    # Export buffer
    def action_buffer_export(self):
        if self.isWorkerRunning():
            return
        reply = QMessageBox.question(
            None,  # kein Eltern-Widget
            "Export buffer",  # Fenstertitel
            "Do you really want to export the buffer?",
            QMessageBox.Yes | QMessageBox.No,  # Buttons
            QMessageBox.No  # Standardbutton
        )
        if reply == QMessageBox.Yes:
            w = Worker_Buffer_import_export_del(self.__model, mode="export", path_export=self.__model.get_user_bak_dir())
            self.start_worker(w, self.worker_finished_buffer_export_import_delete)

    # Import buffer
    def action_buffer_import(self):
        if self.isWorkerRunning():
            return
        s_path_import  = QFileDialog.getOpenFileName(self, "Choose file to import", directory=str(self.__model.get_user_bak_dir()), filter="ZIP-Dateien (*.zip)")[0]
        if s_path_import not in ["", None]:
            w = Worker_Buffer_import_export_del(self.__model, mode="import", path_import=s_path_import)
            self.start_worker(w, self.worker_finished_buffer_export_import_delete)

    # Delete buffer
    def action_buffer_del(self):
        if self.isWorkerRunning():
            return
        reply = QMessageBox.question(
            None,  # kein Eltern-Widget
            "Delete buffer",  # Fenstertitel
            "Do you really want to delete the buffer?",
            QMessageBox.Yes | QMessageBox.No,  # Buttons
            QMessageBox.No  # Standardbutton
        )
        if reply == QMessageBox.Yes:
            w = Worker_Buffer_import_export_del(self.__model, mode="delete", path_export=self.__model.get_user_bak_dir())
            self.start_worker(w, self.worker_finished_buffer_export_import_delete)

    # Show buffer
    def action_buffer_show(self):

        def eval_labels(lst_labels):
            dic_content = {}
            for s_label in lst_labels:
                s_station = str(s_label)[:3]
                s_month_year = str(s_label)[3:]
                if s_station not in dic_content:
                    dic_content[s_station] = []
                dic_content[s_station].append(s_month_year)
            return dic_content

        # ---

        if self.isWorkerRunning():
            return

        lwdb = self.__model.get_local_working_database()
        i_size_all = lwdb.db_get_size_all_files()
        s_content = ""
        if i_size_all == 0:
            # Buffer empty
            s_content += f"buffer is empty ...\n\n"
        else:
            s_content = lwdb.info(details=True)

        # Answer
        sTitel = "Show Buffer"
        self.__popup_text(titel=sTitel, text=s_content)

    # Clear Output
    def console_clear(self):
        self.textBox.clear()

    # Save Output
    def console_save(self):
        s_text = self.textBox.toPlainText()
        s_filename = QFileDialog.getSaveFileName(self, "Save console output in file", "console.txt")[0]
        if s_filename == "":
            return
        try:
            with open(s_filename, "w", encoding='utf-8') as file:
                file.write(s_text)
            self.print_gui("Console out was saved in file: " + s_filename)
        except Exception as ex:
            self.print_exception(ex)

    # Change Verbosity
    def change_verbosity(self):
        s_verbosity = self.comboVerbosity.currentText()
        if s_verbosity == "debug":
            self.__model.set_debug(True)
            self.__model.set_verbose(True)
        elif s_verbosity == "verbose":
            self.__model.set_debug(False)
            self.__model.set_verbose(True)
        else:
            self.__model.set_debug(False)
            self.__model.set_verbose(False)
        self.set_statusbar_text("Changed verbosity level to " + s_verbosity)

# Dialog:Download
s_path_dia = os.path.dirname(os.path.realpath(__file__)) + "/data/dia_download.ui"
class DialogDownload(QDialog, uic.loadUiType(s_path_dia)[0]):

    # Constructor
    def __init__(self, model):
        # Super
        QDialog.__init__(self)

        # Instanzvariablen
        self.__model = model

        # Initialization
        self.setupUi(self)
        #self.setFixedHeight(self.sizeHint().height())
        #self.setFixedWidth(self.sizeHint().width())
        self.setWindowModality(Qt.ApplicationModal)
        self.setWindowFlags(Qt.Dialog | Qt.CustomizeWindowHint | Qt.WindowTitleHint | Qt.WindowCloseButtonHint)

        # UI
        # Stations
        self.__odicCheckBoxen_station = OrderedDict()
        i_width = 10
        for i_idx, s_month in enumerate(self.__model.get_stations()):
            checkbox = QCheckBox(f"{s_month}")
            self.__odicCheckBoxen_station[s_month] = checkbox
            i_row = i_idx // i_width  # int division for row
            i_col = i_idx % i_width  # modulo for col
            self.grid_stations.addWidget(checkbox, i_row, i_col)

        # Months
        self.__odicCheckBoxen_month = OrderedDict()
        i_width = 6
        for i_idx, s_month in enumerate(self.__model.get_months()):
            checkbox = QCheckBox(f"{s_month}")
            self.__odicCheckBoxen_month[i_idx+1] = checkbox
            i_row = i_idx // i_width
            i_col = i_idx % i_width
            self.grid_months.addWidget(checkbox, i_row, i_col)

        # Years
        self.__odicCheckBoxen_year = OrderedDict()
        i_width = 10
        for i_idx, i_year in enumerate(self.__model.get_years()):
            checkbox = QCheckBox(f"{i_year}")
            self.__odicCheckBoxen_year[i_year] = checkbox
            i_row = i_idx // i_width
            i_col = i_idx % i_width
            self.grid_years.addWidget(checkbox, i_row, i_col)

        # Buttons
        self.BrowseDownloadDirectory_pushButton.clicked.connect(self.__open_download_folder)
        self.buttonBox.accepted.connect(self.accept)
        self.buttonBox.rejected.connect(self.reject)
        self.SelectAllStations_pushButton.clicked.connect(partial(self.__select_checkboxes, "station", "all"))
        self.SelectInvertStation_pushButton.clicked.connect(partial(self.__select_checkboxes, "station", "inv"))
        self.SelectNoneStation_pushButton.clicked.connect(partial(self.__select_checkboxes, "station", "none"))
        self.SelectAllMonth_pushButton.clicked.connect(partial(self.__select_checkboxes, "month", "all"))
        self.SelectInvertMonth_pushButton.clicked.connect(partial(self.__select_checkboxes, "month", "inv"))
        self.SelectNoneMonth_pushButton.clicked.connect(partial(self.__select_checkboxes, "month", "none"))
        self.SelectAllYear_pushButton.clicked.connect(partial(self.__select_checkboxes, "year", "all"))
        self.SelectInvertYear_pushButton.clicked.connect(partial(self.__select_checkboxes, "year", "inv"))
        self.SelectNoneYear_pushButton.clicked.connect(partial(self.__select_checkboxes, "year", "none"))
        self.opt_report.stateChanged.connect(self.__check_valid_settings)
        self.opt_avail.stateChanged.connect(self.__check_valid_settings)

        # Set Config if given
        self.set_config()

        # Check valid settings
        self.__check_valid_settings()

        # Size
        # self.resize(self.sizeHint().width(),self.sizeHint().height()) # resize
        self.setFixedHeight(self.sizeHint().height()) # fixed
        self.setFixedWidth(self.sizeHint().width())
        self.update()

    def __check_valid_settings(self):

        # Enable all
        self.opt_zip.setEnabled(True)
        self.opt_report.setEnabled(True)
        self.SelectMonth_groupBox.setEnabled(True)
        self.SelectYear_groupBox.setEnabled(True)

        # Avail: Zip/Check off and Month/Years deactivated
        if self.opt_avail.isChecked():
            self.opt_zip.setChecked(False)
            self.opt_zip.setEnabled(False)
            self.opt_report.setChecked(False)
            self.opt_report.setEnabled(False)
            self.SelectMonth_groupBox.setEnabled(False)
            self.SelectYear_groupBox.setEnabled(False)

    def set_config(self):
        # Preselect stations
        s_stations_code = self.__model.get_stations_code()
        if s_stations_code is not None:
            i_idx = 0
            for s_name, checkbox in self.__odicCheckBoxen_station.items():
                checkbox.setChecked(False)
                try:
                    if s_stations_code[i_idx] == "1":
                        checkbox.setChecked(True)
                except:
                    pass
                i_idx += 1
        # Preselect month
        s_months_code = self.__model.get_months_code()
        if s_months_code is not None:
            i_idx = 0
            for i_month, checkbox in self.__odicCheckBoxen_month.items():
                checkbox.setChecked(False)
                try:
                    if s_months_code[i_idx] == "1":
                        checkbox.setChecked(True)
                except:
                    pass
                i_idx += 1
        # Preselect years
        s_years_code = self.__model.get_years_code()
        if s_years_code is not None:
            i_idx = 0
            for i_year, checkbox in self.__odicCheckBoxen_year.items():
                checkbox.setChecked(False)
                try:
                    if s_years_code[i_idx] == "1":
                        checkbox.setChecked(True)
                except:
                    pass
                i_idx += 1
        # Download dir
        self.download_dir.setText(self.__model.get_working_dir())
        # Unzip
        val = self.__model.get_opt_unzip()
        self.opt_zip.setChecked(val if val is not None else False)
        # Check
        val = self.__model.get_opt_report()
        self.opt_report.setChecked(val if val is not None else False)
        # Availability
        val = self.__model.get_opt_avail()
        self.opt_avail.setChecked(val if val is not None else False)
        # FTP Server
        self.ftp_server.setText(self.__model.get_ftp_server())
        # FTP User
        self.ftp_user.setText(self.__model.get_ftp_user())
        # FTP Pw
        self.ftp_pw.setText(self.__model.get_ftp_pw())

    def get_config(self):
        # Stations
        lst_stations = []
        s_stations_code = ""
        for s_name, combo in self.__odicCheckBoxen_station.items():
            if combo.isChecked():
                s_stations_code += "1"
                lst_stations.append(s_name)
            else:
                s_stations_code += "0"
        # Years
        lst_years = []
        s_years_code = ""
        for i_year, combo in self.__odicCheckBoxen_year.items():
            if combo.isChecked():
                s_years_code += "1"
                lst_years.append(i_year)
            else:
                s_years_code += "0"
        # Months
        lst_months = []
        s_months_code = ""
        for i_month, combo in self.__odicCheckBoxen_month.items():
            if combo.isChecked():
                s_months_code += "1"
                lst_months.append(i_month)
            else:
                s_months_code += "0"
        # Result
        return {
            "stations": lst_stations,
            "years": lst_years,
            "months": lst_months,
            "stations_code": s_stations_code,
            "years_code": s_years_code,
            "months_code": s_months_code,
            "dl_dir": self.download_dir.text(),
            "opt_zip": self.opt_zip.isChecked(),
            "opt_check": self.opt_report.isChecked(),
            "opt_avail": self.opt_avail.isChecked(),
            "ftp_server": self.ftp_server.text(),
            "ftp_user": self.ftp_user.text(),
            "ftp_pw": self.ftp_pw.text()
        }

    def __open_download_folder(self):
        sDir = QFileDialog.getExistingDirectory(self, "Select download directory", directory=self.download_dir.text())
        if sDir not in ["", None]:
            self.download_dir.setText(sDir)
            self.download_dir.setCursorPosition(0)

    def __select_checkboxes(self, s_boxes, s_mode):

        if s_boxes == "station":
            lst_checkboxen = self.__odicCheckBoxen_station.values()
        elif s_boxes == "month":
            lst_checkboxen = self.__odicCheckBoxen_month.values()
        elif s_boxes == "year":
            lst_checkboxen = self.__odicCheckBoxen_year.values()
        else:
            return
        for checkBox in lst_checkboxen:
            if s_mode == "all":
                checkBox.setChecked(True)
            elif s_mode == "inv":
                checkBox.setChecked(not checkBox.isChecked())
            else:
                checkBox.setChecked(False)

# --- Not used yet ---

# Dialog:Concatenate
s_path_dia = os.path.dirname(os.path.realpath(__file__)) + "/data/dia_concatenate.ui"
class DialogConcatenate(QDialog, uic.loadUiType(s_path_dia)[0]):

    # Constructor
    def __init__(self, model):
        # Super
        QDialog.__init__(self)

        # Instanzvariablen
        self.__model = model

        # Initialization
        self.setupUi(self)
        self.setFixedHeight(self.sizeHint().height())
        self.setFixedWidth(self.sizeHint().width())
        self.setWindowModality(Qt.ApplicationModal)
        self.setWindowFlags(Qt.Dialog | Qt.CustomizeWindowHint | Qt.WindowTitleHint | Qt.WindowCloseButtonHint)

        # Buttons connect
        self.buttonBox.accepted.connect(self.accept)
        self.buttonBox.rejected.connect(self.reject)

# Dialog:Formated
s_path_dia = os.path.dirname(os.path.realpath(__file__)) + "/data/dia_formated.ui"
class DialogFormated(QDialog, uic.loadUiType(s_path_dia)[0]):

    # Constructor
    def __init__(self, model):
        # Super
        QDialog.__init__(self)

        # Instanzvariablen
        self.__model = model

        # Initialization
        self.setupUi(self)
        self.setFixedHeight(self.sizeHint().height())
        self.setFixedWidth(self.sizeHint().width())
        self.setWindowModality(Qt.ApplicationModal)
        self.setWindowFlags(Qt.Dialog | Qt.CustomizeWindowHint | Qt.WindowTitleHint | Qt.WindowCloseButtonHint)

        # Buttons
        self.buttonBox.accepted.connect(self.accept)
        self.buttonBox.rejected.connect(self.reject)

# Dialog:Unformated
s_path_dia = os.path.dirname(os.path.realpath(__file__)) + "/data/dia_unformated.ui"
class DialogUnformated(QDialog, uic.loadUiType(s_path_dia)[0]):

    # Constructor
    def __init__(self, model):
        # Super
        QDialog.__init__(self)

        # Instanzvariablen
        self.__model = model

        # Initialization
        self.setupUi(self)
        self.setFixedHeight(self.sizeHint().height())
        self.setFixedWidth(self.sizeHint().width())
        self.setWindowModality(Qt.ApplicationModal)
        self.setWindowFlags(Qt.Dialog | Qt.CustomizeWindowHint | Qt.WindowTitleHint | Qt.WindowCloseButtonHint)

        # Buttons
        self.buttonBox.accepted.connect(self.accept)
        self.buttonBox.rejected.connect(self.reject)

# Dialog: Select Params
s_path_dia = os.path.dirname(os.path.realpath(__file__)) + "/data/dia_selectparam.ui"
class DialogSelectParam(QDialog, uic.loadUiType(s_path_dia)[0]):

        # Constructor
    def __init__(self, model, sSetting):
        # Super
        QDialog.__init__(self)

        # Instanzvariablen
        self.__model = model

        # Data
        self.max_num_ofi_tems = 32
        self.lst_parameter = None
        self.num_of_metadata_columns = 5

        # Initialization
        self.setupUi(self)
        self.setFixedHeight(self.sizeHint().height())
        self.setFixedWidth(self.sizeHint().width())
        self.setWindowModality(Qt.ApplicationModal)
        self.setWindowFlags(Qt.Dialog | Qt.CustomizeWindowHint | Qt.WindowTitleHint | Qt.WindowCloseButtonHint)

        # Buttons
        self.buttonBox.accepted.connect(self.accept)
        self.buttonBox.rejected.connect(self.reject)
        self.SelectAll_pushButton.clicked.connect(partial(self.__select_list, "all"))
        self.DeselectAll_pushButton.clicked.connect(partial(self.__select_list, "none"))
        self.Left2Right_pushButton.clicked.connect(partial(self.__move_list, "l2r"))
        self.Right2Left_pushButton.clicked.connect(partial(self.__move_list, "r2l"))

        # Init
        if sSetting in ["LR0100", "LR0300", "LR0100plusLR0300", "LR0500"]:
            self.init_parameters(sSetting)
            sSetting_show = sSetting.replace("plus", " & ")
            self.setWindowTitle(f"Select Parameters [{sSetting_show}]")

    def init_parameters(self, s_mode):

        lst_parameter_return = [0] * 100  # Beispielparameterliste, groß genug für alle Fälle

        self.lst_parameter = []
        lst_item_list_left = []
        lst_item_list_right = []
        self.num_of_metadata_columns = 5

        if s_mode in ["LR0100", "LR0300", "LR0100plusLR0300", "LR0500"]:
            self.lst_parameter.append("Station")
            self.lst_parameter.append("Date/Time")
            self.lst_parameter.append("Latitude")
            self.lst_parameter.append("Longitude")
            self.lst_parameter.append("Height above ground [m]")

        if s_mode == "LR0100":
            self.lst_parameter.append("Short-wave downward (GLOBAL) radiation [W/m**2]")
            self.lst_parameter.append("Short-wave downward (GLOBAL) radiation, standard deviation [W/m**2]")
            self.lst_parameter.append("Short-wave downward (GLOBAL) radiation, minimum [W/m**2]")
            self.lst_parameter.append("Short-wave downward (GLOBAL) radiation, maximum [W/m**2]")
            self.lst_parameter.append("Direct radiation [W/m**2]")
            self.lst_parameter.append("Direct radiation, standard deviation [W/m**2]")
            self.lst_parameter.append("Direct radiation, minimum [W/m**2]")
            self.lst_parameter.append("Direct radiation, maximum [W/m**2]")
            self.lst_parameter.append("Diffuse radiation [W/m**2]")
            self.lst_parameter.append("Diffuse radiation, standard deviation [W/m**2]")
            self.lst_parameter.append("Diffuse radiation, minimum [W/m**2]")
            self.lst_parameter.append("Diffuse radiation, maximum [W/m**2]")
            self.lst_parameter.append("Long-wave downward radiation [W/m**2]")
            self.lst_parameter.append("Long-wave downward radiation, standard deviation [W/m**2]")
            self.lst_parameter.append("Long-wave downward radiation, minimum [W/m**2]")
            self.lst_parameter.append("Long-wave downward radiation, maximum [W/m**2]")
            self.lst_parameter.append("Air temperature [deg C]")
            self.lst_parameter.append("Humidity, relative [%]")
            self.lst_parameter.append("Station pressure [hPa]")
            lr_label = "Parameters of logical record LR0100"

        elif s_mode in ["LR0300", "LR0100plusLR0300"]:
            self.lst_parameter.append("Short-wave upward (REFLEX) radiation [W/m**2]")
            self.lst_parameter.append("Short-wave upward (REFLEX) radiation, standard deviation [W/m**2]")
            self.lst_parameter.append("Short-wave upward (REFLEX) radiation, minimum [W/m**2]")
            self.lst_parameter.append("Short-wave upward (REFLEX) radiation, maximum [W/m**2]")
            self.lst_parameter.append("Long-wave upward radiation [W/m**2]")
            self.lst_parameter.append("Long-wave upward radiation, standard deviation [W/m**2]")
            self.lst_parameter.append("Long-wave upward radiation, minimum [W/m**2]")
            self.lst_parameter.append("Long-wave upward radiation, maximum [W/m**2]")
            self.lst_parameter.append("Net radiation [W/m**2]")
            self.lst_parameter.append("Net radiation, standard deviation [W/m**2]")
            self.lst_parameter.append("Net radiation, minimum [W/m**2]")
            self.lst_parameter.append("Net radiation, maximum [W/m**2]")
            lr_label = "Parameters of logical record LR0300"

        elif s_mode == "LR0500":
            self.lst_parameter.append("UV-a global [W/m**2]")
            self.lst_parameter.append("UV-a global, standard deviation [W/m**2]")
            self.lst_parameter.append("UV-a global, minimum [W/m**2]")
            self.lst_parameter.append("UV-a global, maximum [W/m**2]")
            self.lst_parameter.append("UV-b direct [W/m**2]")
            self.lst_parameter.append("UV-b direct, standard deviation [W/m**2]")
            self.lst_parameter.append("UV-b direct, minimum [W/m**2]")
            self.lst_parameter.append("UV-b direct, maximum [W/m**2]")
            self.lst_parameter.append("UV-b global [W/m**2]")
            self.lst_parameter.append("UV-b global, standard deviation [W/m**2]")
            self.lst_parameter.append("UV-b global, minimum [W/m**2]")
            self.lst_parameter.append("UV-b global, maximum [W/m**2]")
            self.lst_parameter.append("UV-b diffuse [W/m**2]")
            self.lst_parameter.append("UV-b diffuse, standard deviation [W/m**2]")
            self.lst_parameter.append("UV-b diffuse, minimum [W/m**2]")
            self.lst_parameter.append("UV-b diffuse, maximum [W/m**2]")
            self.lst_parameter.append("UV upward reflected [W/m**2]")
            self.lst_parameter.append("UV upward reflected, standard deviation [W/m**2]")
            self.lst_parameter.append("UV upward reflected, minimum [W/m**2]")
            self.lst_parameter.append("UV upward reflected, maximum [W/m**2]")
            lr_label = "Parameters of logical record LR0500"

        if s_mode == "LR0100":
            for i in range(1, 4):
                lst_item_list_right.append(self.lst_parameter[i])
        elif s_mode in ["LR0300", "LR0100plusLR0300", "LR0500"]:
            lst_item_list_right.append(self.lst_parameter[1])  # Date/Time
        else:
            for i in range(self.num_of_metadata_columns):
                lst_item_list_right.append(self.lst_parameter[i])

        for i in range(self.num_of_metadata_columns, len(self.lst_parameter)):
            if lst_parameter_return[i - self.num_of_metadata_columns + 1] > 0 and \
                    "standard deviation" not in self.lst_parameter[i] and \
                    "minimum" not in self.lst_parameter[i] and \
                    "maximum" not in self.lst_parameter[i]:
                lst_item_list_right.append(self.lst_parameter[i])

        for i in range(len(self.lst_parameter)):
            if self.lst_parameter[i] not in lst_item_list_right:
                lst_item_list_left.append(self.lst_parameter[i])

        for item in lst_item_list_left:
            self.lb1.addItem(item)
        for item in lst_item_list_right:
            self.lb2.addItem(item)

    def __select_list(self, s_mode):
        if s_mode == "all":
            for i in range(self.lb1.count()):
                self.lb1.item(i).setSelected(True)
            self.__move_list("l2r")
        elif s_mode == "none":
            self.__sort_list()

    def __sort_list(self):
        self.lb1.clear()
        self.lb2.clear()
        for item in self.lst_parameter:
            self.lb1.addItem(item)
        self.__enableOkButton()

    def __move_list(self, s_mode):
        if s_mode == "l2r":
            selected_items = self.lb1.selectedItems()
            if self.lb2.count() + len(selected_items) <= self.max_num_ofi_tems:
                for item in selected_items:
                    self.lb2.addItem(item.text())
                    self.lb1.takeItem(self.lb1.row(item))
            else:
                QMessageBox.information(self, "BSRN Toolbox",
                                        f"The application you have chosen supports a maximum of {self.max_num_ofi_tems} parameters.")
                self.lb1.clearSelection()
            self.__enableOkButton()
        elif s_mode == "r2l":
            selected_items = self.lb2.selectedItems()
            for item in selected_items:
                self.lb1.addItem(item.text())
                self.lb2.takeItem(self.lb2.row(item))
            self.__enableOkButton()

    def __enableOkButton(self):
        if self.lb2.count() <= self.max_num_ofi_tems:
            button_ok = self.buttonBox.button(QDialogButtonBox.Ok)
            button_ok.setEnabled(True)
            button_ok.setEnabled(True)
            button_ok.setDefault(True)
            button_ok.setFocus()
            self.SelectAll_pushButton.setDefault(False)
        else:
            button_ok = self.buttonBox.button(QDialogButtonBox.Ok)
            button_ok.setEnabled(False)
            button_ok.setDefault(False)
            self.SelectAll_pushButton.setDefault(True)
            self.SelectAll_pushButton.setFocus()
            if self.lb2.count() > self.max_num_ofi_tems:
                QMessageBox.information(self, "BSRN Toolbox",
                                        f"The application you have chosen supports a maximum of {self.max_num_ofi_tems} parameters.\nPlease remove {self.lb2.count() - self.max_num_ofi_tems} parameter(s) from the\nlist or click on cancel.")