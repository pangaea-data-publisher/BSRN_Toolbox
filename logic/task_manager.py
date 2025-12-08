# coding=utf-8

"""
Download Manager
"""

import ftplib, os
import glob
from collections import OrderedDict
from pathlib import Path
from logic.helper import SmartPrinter, Result, PrettyText, FileTools
from logic.ingestor import Ingestor
from logic.parallel import Stoppable
from logic.selection import Selection

class TaskManager(Stoppable):

    # Constructor
    def __init__(self, model):
        Stoppable.__init__(self)
        self.__model = model
        self.__local_working_database = self.__model.get_local_working_database()
        self.__converter = self.__model.get_converter()
        self.add_abort_child(self.__local_working_database)
        self.__smartprinter = model.get_smart_printer() if self.__model is not None else SmartPrinter()

    # ---

    """
    Prepare working dir (add: now and old now -> bak)
    Args:
        s_working_dir (str): Working directory
    """
    def prepare_working_dir(self, s_working_dir_path):
        pv = self.__smartprinter.verbose
        s_timestamp = PrettyText.create_timestamp()
        s_dir_working_dir_now = Path(s_working_dir_path, "now")  # dir now
        pv(f"* Preparing folders")
        if os.path.isdir(s_dir_working_dir_now):  # now existing -> move dir now to bak with timestamp
            pv(f"Found old workind folder: {s_dir_working_dir_now}")
            # create bak (only! cause of windows)
            s_dir_download_bak = Path(s_working_dir_path, "bak")  # bak
            if not os.path.exists(s_dir_download_bak):
                pv(f"Create bak folder: {s_dir_download_bak}")
                os.makedirs(s_dir_download_bak, exist_ok=True)
            else:
                pv(f"Found bak folder: {s_dir_download_bak}")
            try:
                # backup -> new -> old plus timestamp
                s_dir_download_bak_timestamp = Path(s_working_dir_path, "bak", s_timestamp)
                pv(f"Backup old working folder to bak: {s_dir_download_bak_timestamp}")
                os.rename(s_dir_working_dir_now, s_dir_download_bak_timestamp)
            except:
                # should only happen if you backup twice at the same second
                s_dir_download_bak_timestamp_uups = str(s_dir_download_bak_timestamp) + "_extra"
                pv(f"Backup old working folder to bak: {s_dir_download_bak_timestamp_uups}")
                os.rename(str(s_dir_working_dir_now), s_dir_download_bak_timestamp_uups)
        os.makedirs(s_dir_working_dir_now, exist_ok=True)  # create new now
        pv(f"Create new working folder: {s_dir_working_dir_now}")
        pv("")
        return s_dir_working_dir_now

    """
    Download station data and check (report)
    Args:
        sel (Selector): Selector
        s_working_path (str): Working directory
    Kwargs:
        unzip (unzip): Unzip downloaded files
        report (bool): always create a report (and import and check respectively)
    """
    def download_station_data_and_check(self, sel, s_working_path, **kwargs):

        res = Result(name="Download station data")

        self.abort(False)

        # Kwargs
        b_unzip = True if kwargs.get("unzip") is True else False
        b_report = True if kwargs.get("report") is True else False

        # Working dir now
        s_working_path_now = self.prepare_working_dir(s_working_path)

        # Download station data
        res_dl = self.get_station_data_to_working_dir(sel, s_working_path_now, unzip=b_unzip)
        if res_dl.is_err():
            res.add_result(res_dl)
        self.abort(self.is_abort() or res_dl.get_data("abort"))
        s_log_dl = res_dl.get_data("log")
        sel_delivered = res_dl.get_data("selection_delivered")

        # Create reports (only if explicitely wanted -> otherwise import and check is done seperately)
        s_log_rep = ""
        res_rep = self.process_station_data(sel_delivered, report=b_report)
        if res_rep.is_err():
            res.add_result(res_rep)
        s_log_rep = res_rep.get_data("log")
        self.abort(self.is_abort() or res_dl.get_data("abort"))

        # Log
        if b_report:
            s_log = "*** Download and check station data\n\n"
        else:
            s_log = "*** Download station data\n\n"
        if self.is_abort():
            s_log += "CAUTION: Process aborted by user …\n\n"
        s_log += s_log_dl
        s_log += "\n"
        if s_log_rep is not None:
            s_log += s_log_rep
        s_log = PrettyText.clean_wizard(s_log, max_one_empty_row=True)
        # Result
        res.add_data("log", s_log)

        res.add_data("title", "Download station data")
        res.add_data("files", s_working_path_now)
        return res

    """
    Get station data from local working database to working direcory
    Args:
        sel (Selector): Selector
        s_working_dir (str): Working directory
    Kwargs:
        unzip (unzip): Unzip downloaded files
    """
    def get_station_data_to_working_dir(self, sel, s_working_dir, **kwargs):

        res = Result(name="Get station data to working dir")

        # SmartPrinter
        pn = self.__smartprinter.normal
        pv = self.__smartprinter.verbose
        pd = self.__smartprinter.debug
        progress = self.__smartprinter.progress
        workflow = self.__smartprinter.workflow
        status = self.__smartprinter.status
        buffer_update = self.__smartprinter.buffer

        # Reset abort
        self.abort(False)

        # Kwargs
        b_unzip = True if kwargs.get("unzip") is True else False

        # Get files from local working database to working dir
        res_db = self.__local_working_database.dat_deliver_files_to_working_dir(sel, s_working_dir, unzip=b_unzip)
        if res_db.is_err():
            res.add_result(res_db)
        s_log = res_db.get_data("log")
        sel_delivered = res_db.get_data("delivered")

        # Result
        res.add_data("log", s_log)
        res.add_data("title", "Download station data")
        res.add_data("selection_delivered", sel_delivered)
        return res

    """
    Process Station data from Selection (Import, Check, create Report)
    Args:
        sel (Selector): Selector
    Kwargs:
        export_data (bool): export data 
        report (bool): always create a report
    """
    def process_station_data(self, sel, **kwargs):

        res = Result(name="Process Station data from Selection (Import, Check, create Report)")

        # SmartPrinter
        pn = self.__smartprinter.normal
        pv = self.__smartprinter.verbose
        pd = self.__smartprinter.debug
        progress = self.__smartprinter.progress
        workflow = self.__smartprinter.workflow
        status = self.__smartprinter.status
        buffer_update = self.__smartprinter.buffer

        # Kwargs
        b_report = True if kwargs.get("report") is True else False
        b_export = True if kwargs.get("export_data") is True else False
        lst_export_rec = kwargs.get("export_data_recs")

        # Clean input
        if isinstance(lst_export_rec, str):
            lst_export_rec = [lst_export_rec]
        elif isinstance(lst_export_rec, list):
            pass
        else:
            lst_export_rec = []

        # Reset abort
        self.abort(False)

        # Set dir for exporting reports and exports
        # Info: Check single/multi-file | multi-file &  multi-path: -> define one dir for log
        s_dir_deliver_reports_and_exports = None
        s_file_deliver_reports_and_exports = None
        b_input_single_file = False
        b_input_multi_file = False
        b_input_multi_file_choose_log_path = False
        # Init Selection
        sel.init(valid="all", type="file_with_path")
        set_used_paths = sel.get_used_paths()
        if sel.get_length() > 1:
            # Multi file
            b_input_multi_file = True
            if len(set_used_paths) > 1:
                # Multi file -> path -> take one path
                b_input_multi_file_choose_log_path = True
            s_dir_deliver_reports_and_exports = sorted(set_used_paths)[0]
        elif sel.get_length() == 1:
            # Single file
            b_input_single_file = True
            s_dir_deliver_reports_and_exports = sel.get_used_paths().pop()
        else:
            return res
        s_file_deliver_reports_and_exports = Path(s_dir_deliver_reports_and_exports, "BSRN_fcheck_report.txt")

        # Console
        s_tmp = f'Processing mode: {"import & check and export" if b_export else "import & check"}\n'
        s_tmp += f'Selection: {sel.get_base_num_valid()} files\n'
        if sel.has_not_valid():
            s_tmp += f"FYI: Found not valid selections ({sel.get_base_num_not_valid()}x):\n"
            s_tmp += sel.info_not_valid()
            s_tmp += "\n"
        s_tmp += f'Reporting: {"always create a report" if b_report else "just create a report if errors occurr"}\n'
        if b_export:
            lst_export_rec_meta = []
            lst_export_rec_data = []
            for s_tmp_rec in lst_export_rec:
                if self.__model.is_rec_meta(s_tmp_rec):
                    lst_export_rec_meta.append(s_tmp_rec)
                else:
                    lst_export_rec_data.append(s_tmp_rec)
            if len(lst_export_rec)==0:
                s_tmp += f'Exporting records: all\n'
            else:
                if len(lst_export_rec_meta) > 0:
                    s_tmp += f'Exporting records (meta data): {PrettyText.lst2strC(lst_export_rec_meta)}\n'
                if len(lst_export_rec_data) > 0:
                    s_tmp += f'Exporting records (data): {PrettyText.lst2strC(lst_export_rec_data)}\n'
        s_tmp += f'Reporting {"and exporting " if b_export else ""}dir: {s_dir_deliver_reports_and_exports}'
        if b_input_multi_file_choose_log_path:
            s_tmp += f' (multiple files with multiple paths selected and this dir was selected for saving reports{" and exports" if b_export else ""})'
        s_tmp += f'\n'
        pn("* Processing station data files")
        pn(s_tmp)

        # Log
        lst_overview_report_msg = [["Dataset", "Err", "Wrn", "Inf", "Err.overview", "Unknown records", "Size"]]
        lst_overview_report_no_msg = [["Dataset", "Size", "Report file"]]
        lst_overview_export = [["Dataset", "Files", "Lines"]]
        lst_overview_export_err = [["Dataset", "Err", "Err.recs", "Info"]]

        s_log = "--- Processing station data files\n\n"
        s_log += s_tmp
        s_log += "\n"

        # Result Imports/Reports
        lst_reports = []
        lst_reports_w_err = []
        set_unknown_recs_total = set()
        lst_reports_w_unknown_recs = []

        # Result Exports
        lst_exports = []
        lst_exports_w_err = []

        # Result importer data
        odic_data = OrderedDict()

        # Prepare report and export directory (and Backup old log files -> cause: converter must concatenate and cannot know when to backup or concatenate
        if b_input_multi_file:
            # Backup old export files
            ls_old_exports = glob.glob(os.path.join(s_dir_deliver_reports_and_exports, "BSRN_LR*"))
            ls_old_exports  = [f for f in ls_old_exports  if 'bak' not in os.path.basename(f)]
            for s_file_export_old in ls_old_exports:
                FileTools.prepare_save_file(s_file_export_old)  # if existing -> backup old report
        # Backup old report file
        FileTools.prepare_save_file(s_file_deliver_reports_and_exports)

        # Loop
        progress("on", abort=True)
        workflow("Process data")
        i_files_max = len(sel)
        for i_file_now, (s_base, s_file, s_path) in enumerate(sel):
            if self.is_abort():
                break

            # used files
            path_path_file = Path(s_path, s_file)
            s_path_file = str(path_path_file)
            b_valid, s_station, i_month, i_year = self.__local_working_database.admin_filename_eval(s_file)
            s_file_rep = f"{self.__local_working_database.admin_filename_get_base(s_file)}.rep.txt"
            s_file_rep_gz = f"{self.__local_working_database.admin_filename_get_base(s_file)}.rep.txt.gz"
            s_path_file_rep_gz = str(Path(s_path, s_file_rep_gz))

            # UI
            progress(i_file_now+1, i_files_max, 100, proz=True, abs=True,
                     text=f"{s_station.upper()} [{s_file}]",
                     status="", cli=True, problem="", info="", abort=False)
            pv(f"{s_station.upper()} ({i_file_now+1}/{i_files_max}): {s_file}: ", "")

            if not path_path_file.exists():
                # File not exisitng
                pv(f"not found here: {s_path}")
                res.add_err(f"file not found: {s_path_file}")
            else:
                # Import: check file & report
                pv("import data … ", "")

                b_use_buffer = self.__local_working_database.db_buffer_is_active() and self.__local_working_database.imp_is_available(s_base)

                # --- Only when Export: Check if import data really exists: -> otherwise force redo import/report
                if b_export:
                    b_ok, s_err, odic_data = self.__local_working_database.imp_get_data(s_base)
                    if not b_ok:
                        pv("inconsistent import found in buffer … force re-import … ", "")
                        b_use_buffer = False # Force actually redo import

                if b_use_buffer:
                    # --- Buffer
                    b_err_tec, s_err_tec, i_err_rep, i_wrn_rep, i_inf_rep, set_unknown_recs, s_report, s_err_rep_overview = self.__local_working_database.imp_get_report(s_base)
                else:
                    # Do it for real
                    # --- Import/Check/Report
                    ing = Ingestor(self.__model)
                    b_err_tec, s_err_tec, i_err_rep, i_wrn_rep, i_inf_rep, set_unknown_recs, s_report, s_err_rep_overview, odic_data = ing.ingest(s_path_file)

                # Info import/report done (no. of lines, unknown recs, errors)
                lst_tmp_info = [f"report size: {len(s_report)} lines"]
                if len(set_unknown_recs) > 0:
                    set_unknown_recs_total = set_unknown_recs_total.union(set_unknown_recs)
                    lst_reports_w_unknown_recs.append(f"{s_base} ({PrettyText.lst2strC(set_unknown_recs)})")
                if i_err_rep > 0 or len(set_unknown_recs) > 0:
                    lst_tmp_info = []
                    if i_err_rep > 0:
                        lst_tmp_info.append(f"errors [{i_err_rep}x]")
                    if len(set_unknown_recs) > 0:
                        lst_tmp_info.append(
                            f"unknown records [{len(set_unknown_recs)}x -> {PrettyText.lst2strC(set_unknown_recs)}]")
                if not b_err_tec:
                    s_tmp = "found in buffer" if b_use_buffer else "made"
                    pv(f"import {s_tmp} ({PrettyText.lst2strC(lst_tmp_info)}) … " , "")
                else:
                    pv(f"import not made: {s_err_tec} … ", "")

                # Import and Report actually made -> save to buffer
                if not b_use_buffer and not b_err_tec:
                    pv("write import to buffer … ", "")
                    # Write to buffer: Import and Report
                    tp_rep_meta = (i_err_rep, i_wrn_rep, i_inf_rep, set_unknown_recs, s_err_rep_overview)
                    b_err, s_err = self.__local_working_database.imp_add(s_base, odic_data, tp_rep_meta, s_report)
                    if b_err:
                        b_err_tec = True
                        res.add_err(f"error writing import to buffer: file: {s_file} … {s_err}")
                    self.__local_working_database.db_buffer_save_to_disk()  # DEV Always save to disk

                # Add to log Overview: Report must be made explicetely or an error occurred
                if b_report or i_err_rep >0:
                    # Separate if a message was thrown: Msg-Table
                    if (i_err_rep > 0 or i_wrn_rep > 0 or i_inf_rep > 0):
                        lst_overview_report_msg.append([s_base.upper(),
                                                 i_err_rep if i_err_rep > 0 else "",
                                                 i_wrn_rep if i_wrn_rep > 0 else "",
                                                 i_inf_rep if i_inf_rep > 0 else "",
                                                 s_err_rep_overview if s_err_rep_overview != "" else "",
                                                 PrettyText.lst2str(set_unknown_recs, ","),
                                                 len(s_report)])
                    else:
                        # ... or no message was thrown -> normal-Table
                        lst_overview_report_no_msg.append([s_base.upper(),len(s_report), s_path_file_rep_gz ])

                # Append report
                if (b_report or i_err_rep > 0) and not b_err_tec: # Report always or only when report error plus no tec errors occured
                    # Write report to working dir
                    FileTools.prepare_save_file(s_dir_deliver_reports_and_exports) # if existing -> backup old report
                    try:
                        if i_err_rep > 0 or b_report:
                            pv(f"expand overall report … ", "")
                            with open(s_file_deliver_reports_and_exports, 'a') as stream_report:
                                stream_report.write(s_report)
                    except Exception as ex:
                        b_err_tec = True
                        s_err_tec = str(ex)
                        res.add_err(f"error appending report ({s_file_deliver_reports_and_exports}): {str(ex)} … ")
                else:
                    pv("report not delivered … ", "")

                # Add station data with errors in report to list
                if i_err_rep > 0:
                    lst_reports_w_err.append(s_base)

                # Check for ok reports
                if b_report:
                    lst_reports.append(s_file_rep)

                # --- Export

                if b_export:
                    pv(f"export data … ", "")
                    b_ok, i_exported_files, i_exported_lines, odic_err,  = self.__converter.convert(odic_data, s_path_file, lst_export_rec, b_input_multi_file, s_dir_deliver_reports_and_exports)
                    if not b_ok:
                        # Err
                        lst_recs_err = odic_err.keys()
                        pv(f"errors happend ({len(odic_err)}x: in records: {PrettyText.lst2strC(lst_recs_err)}) … ", "")
                        # Add to list
                        lst_exports_w_err.append(s_base)
                        # Add to Overview
                        lst_overview_export_err.append([s_base.upper(), len(odic_err), PrettyText.lst2strC(odic_err.keys()), PrettyText.lst2strC(odic_err)])
                    else:
                        # OK
                        pv(f"export made (files: {i_exported_files}, lines: {i_exported_lines}) … ", "")
                        if i_exported_files > 0:
                            # Add to overview
                            lst_exports.append(s_base)
                            # Add to Overview
                            lst_overview_export.append([s_base.upper(), i_exported_files, i_exported_lines])

                # Finish the line
                pv("done")

        # Final console info
        pv()
        if len(lst_reports) > 0:
            pn(f"Reports delivered: {len(lst_reports)}x")
        else:
            pn(f"No reports have been delivered to working dir")
        if b_export:
            if len(lst_exports) > 0:
                pn(f"Number of stations exported: {len(lst_exports)}x")
            else:
                pn(f"No exports have been made")
            pv()
        pv()

        # --- Log

        if i_files_max > 0:
            b_show_details = False
            lst_table = [["Type", "#", "%", "Info"]]

            # Overview
            if (i_val := i_files_max) > 0:
                lst_table.append(["Station data", i_val, "100%", ""])
            if (i_val := len(lst_reports)) > 0:
                lst_table.append(["Reports made", i_val, PrettyText.percent2str(i_val, i_files_max), ""])
            if (i_val := len(lst_reports_w_err)) > 0:
                b_show_details = True
                lst_table.append(
                    ["Reports with errors", i_val, PrettyText.percent2str(i_val, len(lst_reports)), ""])
            if (i_val := len(set_unknown_recs_total)) > 0:
                b_show_details = True
                lst_table.append(["Unknown cases", i_val, "-", PrettyText.lst2strC(set_unknown_recs_total)])
            if b_export:
                if (i_val := len(lst_exports)) > 0:
                    lst_table.append(["Exported stations", i_val, PrettyText.percent2str(i_val, i_files_max), ""])
            s_log += "* Overview\n\n"
            s_log += PrettyText.table(lst_table)
            s_log += "\n\n"

            # Overview: Reports (messages)
            lst_header, *lst_body = lst_overview_report_msg  # remove Header
            def safe_int(val):
                try:
                    return int(val)
                except (ValueError, TypeError):
                    return float("-inf")
            lst_body.sort(key=lambda x: safe_int(x[1]), reverse=True) # sort
            lst_overview_report_msg[:] = [lst_header] + lst_body # assemlbe
            if len(lst_overview_report_msg) > 1:
                s_log += "\n"
                s_log += "Reports with messages:\n"
                s_log += PrettyText.table(lst_overview_report_msg)
                s_log += "\n\n"

            # Overview: Reports (no messages)
            if len(lst_overview_report_no_msg) > 1:
                s_log += "\n"
                s_log += "Reports without messages:\n"
                s_log += PrettyText.table(lst_overview_report_no_msg)
                s_log += "\n\n"

            # Overview: Export
            if len(lst_overview_export_err) > 1:
                s_log += "\n"
                s_log += "Exports with errors:\n"
                s_log += PrettyText.table(lst_overview_export_err)
                s_log += "\n\n"

            # Overview: Export
            if len(lst_overview_export) > 1:
                s_log += "\n"
                s_log += "Exports made:\n"
                s_log += PrettyText.table(lst_overview_export)
                s_log += "\n\n"

            # Details: Reports
            if b_show_details:
                s_log += f"* Details: Reports\n\n"
                if len(lst_reports_w_err) > 0:
                    s_log += f"Datasets with errors in reports ({len(lst_reports_w_err)}x)\n"
                    s_log += PrettyText.lst2strC(lst_reports_w_err, 5)
                    s_log += "\n\n"
                if len(lst_reports_w_unknown_recs) > 0:
                    s_log += f"Unknown logical records found ({len(set_unknown_recs_total)}x)\n"
                    s_log += PrettyText.lst2strC(lst_reports_w_unknown_recs, 5)
                    s_log += "\n\n"

        else:
            s_log += "<empty>"

        # UI
        workflow()
        progress("off")

        # --- Create Log

        # Clean
        s_log = PrettyText.clean_wizard(s_log, max_one_empty_row=True)

        # Result
        if self.is_abort():
            res.add_err("Process aborted by user")
        res.add_data("log", s_log)
        res.add_data("files", s_dir_deliver_reports_and_exports)
        return res

    """
    Check availability of station data on server
    Args:
        lst_station (lst): Stations to check
        s_downalod_path (str): Working directory
    """
    def check_availability_on_server(self, lst_station, s_download_path):

        res = Result(name="Check availability of station data")

        # SmartPrinter
        pn = self.__smartprinter.normal
        pv = self.__smartprinter.verbose
        pd = self.__smartprinter.debug
        progress = self.__smartprinter.progress
        workflow = self.__smartprinter.workflow
        status = self.__smartprinter.status
        buffer_update = self.__smartprinter.buffer

        # Reset abort
        self.abort(False)

        # Data
        s_download_path_now = self.prepare_working_dir(s_download_path)
        s_ftp_server = self.__model.get_ftp_server()
        s_ftp_user = self.__model.get_ftp_user()
        s_ftp_password = self.__model.get_ftp_pw()

        if len(lst_station) == 0:
            lst_station = self.__model.get_stations()

        # Data
        b_ftp_root_status_err = False
        s_ftp_root_status_err = None
        lst_ftp_root_stations_ok = []
        lst_ftp_root_stations_unknown = []
        lst_ftp_root_else = []
        i_stations = len(lst_station)
        lst_stations_avail = []
        lst_station_avail_but_empty = []
        lst_station_ftp_err = []
        i_station_files_total = 0

        # Console
        s_txt = ""
        s_txt += f"* Check availability of station data files\n"
        s_txt_tmp = "all" if len(lst_station) == self.__model.get_stations_num() else PrettyText.lst2strC(lst_station)
        s_txt += f"Selected stations ({i_stations}x): {s_txt_tmp}\n"
        s_txt += f"Working dir: {s_download_path_now}\n"
        s_txt += f"FTP-server: {s_ftp_user}@{s_ftp_server}\n"
        pn(s_txt)

        # UI
        workflow(f"Check availability")
        progress("pulse", abort=True)

        # --- FTP root status: all stations (dirs) on server
        try:
            # Get files
            ftp = ftplib.FTP(s_ftp_server)
            ftp.login(user=s_ftp_user, passwd=s_ftp_password)
            ftp.cwd("/")
            lst_ftp_dirs = ftp.nlst()
            for s_dir in lst_ftp_dirs:
                if len(s_dir) != 3:
                    lst_ftp_root_else.append(s_dir)
                else:
                    if s_dir in self.__model.get_stations_lower():
                        lst_ftp_root_stations_ok.append(s_dir)
                    else:
                        lst_ftp_root_stations_unknown.append(s_dir)
        except Exception as ex:
            b_ftp_root_status_err = True
            s_ftp_root_status_err = f"error: {ex}"

        # --- Loop Stations
        for i_station_now, s_station_name in enumerate(lst_station):
            s_station = s_station_name.lower()
            if self.is_abort():
                break
            # Dirs and files
            s_avail_ftp_dir = f"/{s_station}/"
            s_avail_local_filepath = Path(s_download_path_now, f"{s_station.upper()}_filelist.txt")

            # Info
            i_not_avail = len(lst_station_ftp_err) + len(lst_station_avail_but_empty)
            s_text = f"{s_station_name} "
            s_text_msg = ""
            if i_not_avail > 0:
                s_text_msg = f"Unavail.: {i_not_avail}/{i_station_now} {PrettyText.percent2str(i_not_avail, i_station_now, justify=False)}"
            progress(i_station_now, i_stations, 100, proz=True, abs=True, cli=True, text=s_text, info=s_text_msg)
            pv(f"{s_station_name}: check availability on ftp://{s_ftp_user}@{s_ftp_server}{s_avail_ftp_dir}  … ","")

            # FTP
            try:
                # Get files
                ftp = ftplib.FTP(s_ftp_server)
                ftp.login(user=s_ftp_user, passwd=s_ftp_password)
                ftp.cwd(s_avail_ftp_dir)
                lst_ftp_files = ftp.nlst()
                # Filter dat.gz only
                lst_ftp_files_gz = [f for f in lst_ftp_files if f.endswith("dat.gz")]
                i_station_files_total += len(lst_ftp_files_gz)
                lst_ftp_files_gz_unknown = []
                for s_file in lst_ftp_files_gz:
                    b_valid, s_station, i_month, i_year = self.__local_working_database.admin_filename_eval(s_file)
                    if not b_valid:
                        s_base = self.__local_working_database.admin_filename_get_base(s_file)
                        lst_ftp_files_gz_unknown.append(s_base)
                i_ftp_files_gz = len(lst_ftp_files_gz)
                # Log it
                if i_ftp_files_gz > 0:
                    pv(f"found {i_ftp_files_gz} files", "")
                    lst_stations_avail.append((s_station_name, i_ftp_files_gz, lst_ftp_files_gz_unknown))
                    # Sort files
                    lst_base_sorted = Selection.tool_sort_files(lst_ftp_files_gz)
                    # Write file log
                    with open(s_avail_local_filepath, "w") as f:
                        for s_base in lst_base_sorted:
                            s_file = f"{s_base}.dat.gz"
                            b_valid, s_station, i_month, i_year = self.__local_working_database.admin_filename_eval(s_file)
                            s_valid = " (invalid/unknown)" if not b_valid else ""
                            f.write(f"{s_file} {s_valid}\n")
                else:
                    pv(f"Station found but without data", "")
                    lst_station_avail_but_empty.append(s_station_name)
                ftp.quit()
            except Exception as ex:
                lst_station_ftp_err.append(s_station_name)
                pv(f"Station not available ({ex})", "")
            pv()

        # --- Log
        s_log = f"*** Check availability of station data files on ftp-server\n\n"
        if self.is_abort():
            s_log += "CAUTION: Process aborted by user …\n\n"
        s_txt_tmp = "all" if len(lst_station) == self.__model.get_stations_num() else PrettyText.lst2strC(lst_station)
        s_log += f"FTP-server: {s_ftp_user}@{s_ftp_server}\n\n"
        if i_stations > 0:
            # Slected stations
            s_log += f"Selected stations ({len(lst_station)}x):\n"
            s_log += PrettyText.lst2strC(lst_station, 10)
            s_log += "\n\n"

            # FTP server root status
            if b_ftp_root_status_err:
                s_log += s_ftp_root_status_err + "\n\n"
            else:
                lst_stations_avail_lower = [s_station.lower() for (s_station, tmp1, tmp2) in lst_stations_avail]
                lst_ftp_root_stations_other_than_selected = [x.upper() for x in lst_ftp_root_stations_ok if
                                                             x not in lst_stations_avail_lower]
                if len(lst_ftp_root_stations_other_than_selected) > 0:
                    s_log += f"The server has more available stations than selected ({len(lst_ftp_root_stations_other_than_selected)}x):\n"
                    s_log += PrettyText.lst2strC(lst_ftp_root_stations_other_than_selected, 10)
                    s_log += "\n\n"
                if len(lst_ftp_root_stations_unknown) > 0:
                    lst_tmp = [item.upper() for item in lst_ftp_root_stations_unknown]
                    s_log += f"FYI: Unavailable/invalid station names found on server ({len(lst_ftp_root_stations_unknown)}x):\n"
                    s_log += PrettyText.lst2strC(lst_tmp, 10)
                    s_log += "\n\n"
                if len(lst_ftp_root_else) > 0:
                    s_log += f"FYI: Others files/dirs found on server ({len(lst_ftp_root_else)}x):\n"
                    s_log += PrettyText.lst2strC(lst_ftp_root_else, 30)
                    s_log += "\n\n"

            # Overview
            b_show_details = False
            s_log += f"--- Overview\n\n"
            lst_table = [["Type", "#", "%"]]
            if (i_val := i_stations) > 0:
                lst_table.append(["Station names checked", i_val, ""])
            if (i_val := len(lst_station_ftp_err)) > 0:
                b_show_details = True
                lst_table.append(["Stations not available", i_val, PrettyText.percent2str(i_val, i_stations)])
            if (i_val := len(lst_station_avail_but_empty)) > 0:
                b_show_details = True
                lst_table.append(["Stations available but without any data", i_val, PrettyText.percent2str(i_val, i_stations)])
            if (i_val := len(lst_stations_avail)) > 0:
                b_show_details = True
                lst_table.append(["Stations available", i_val, PrettyText.percent2str(i_val, i_stations)])
            if (i_val := i_station_files_total) > 0:
                b_show_details = True
                lst_table.append(["Station files total", i_val, ""])
            s_log += PrettyText.table(lst_table)
            s_log += "\n\n"

            # Details
            if b_show_details:
                s_log += f"--- Details\n\n"
                if len(lst_station_ftp_err) > 0:
                    s_log += f"Stations not available ({len(lst_station_ftp_err)}x):\n"
                    s_log += PrettyText.lst2strC(lst_station_ftp_err, 5, 30)
                    s_log += "\n\n"
                if len(lst_station_avail_but_empty) > 0:
                    s_log += f"Station available but without any data ({len(lst_station_avail_but_empty)}x):\n"
                    s_log += PrettyText.lst2strC(lst_station_avail_but_empty, 5, 30)
                    s_log += "\n\n"
                if len(lst_stations_avail) > 0:
                    s_log += f"Available stations ({len(lst_stations_avail)}x):\n"
                    lst_table = [("Station", "files.total", "files.unavailable/invalid")]
                    for s_station, i_base_total, lst_base_unknown in lst_stations_avail:
                        i_base_unknown = len(lst_base_unknown)
                        if len(lst_base_unknown) > 0:
                            lst_base_unknown_sorted = Selection.tool_sort_files(lst_base_unknown)
                            s_txt = f"{i_base_unknown}x ({PrettyText.percent2str(i_base_unknown, i_base_total, justify=False)}): {PrettyText.lst2strC(lst_base_unknown_sorted)}"
                        else:
                            s_txt = ""
                        lst_table.append([s_station, i_base_total, s_txt])
                    s_log += PrettyText.table(lst_table)
                    s_log += "\n\n"

            # Result
            if self.is_abort():
                res.add_err("Process aborted by user")
            res.add_data("log", s_log)
            res.add_data("files_avail", lst_stations_avail)
            res.add_data("files_not_avail", lst_station_ftp_err)
            res.add_data("files_avail_but_empty", lst_station_avail_but_empty)
            res.add_data("title", "Check availability")
            res.add_data("files", s_download_path_now)

        # UI

        progress("off")

        # Result
        return res




