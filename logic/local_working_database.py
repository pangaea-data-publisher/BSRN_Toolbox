# coding=utf-8

"""
Local Working Database
"""

import ftplib, gzip, os, pickle, shutil, traceback
import time
from collections import OrderedDict
from datetime import datetime
from pathlib import Path
from logic.helper import Result, PrettyText, SmartPrinter
from logic.parallel import Stoppable
from logic.selection import Selection

class LocalWorkingDatabase(Stoppable):

    def __init__(self, model):
        Stoppable.__init__(self)

        # Instance variables
        self.__model = model
        self.__smartprinter = model.get_smart_printer() if self.__model is not None else SmartPrinter()

        # DB data
        self.__s_db_buffer_dir = self.__model.get_user_buffer_dir()
        self.__set_dat_notfound = set()
        self.__dic_import_meta = {}
        self.__b_db_has_unsaved_data = False

        # Buffer
        self.__b_buffer_refresh_files = False
        self.__b_buffer_active = False

    def __str__(self):
        return self.info()

    # --- Info

    def info(self, **kwargs):
        b_details = True if (v := kwargs.get("details")) is True else False
        i_size_dat = self.db_get_size_dat()
        s_info = ""
        s_info += "*** Status of local working database ***\n\n"
        s_info += f'Buffer: {"on" if self.db_buffer_is_active() else "off"}\n'
        if self.db_buffer_is_active():
            s_info += f'Buffer (refresh): {"on" if self.db_buffer_is_refresh() else "off"}\n'
        s_info += f'\n'
        s_info += f'FTP: {self.__model.get_ftp_user()}@{self.__model.get_ftp_server()}\n'
        s_info += f'Dir: {self.__s_db_buffer_dir}\n'
        s_info += f'\n'
        s_info += f'Locally available datasets: {i_size_dat}\n'
        s_info += f'Locally available imports (with reports): {self.db_get_size_imports()}\n'
        s_info += f'Logged unavailable requests: {self.db_get_size_unknown()}\n'
        s_info += f'Total size of local database: {self.db_get_size_all_as_string()}\n'
        if b_details:
            s_info += "\n"
            if i_size_dat > 0:
                s_info += f"--- Station data ({i_size_dat}x)\n\n"
                lst_dat = self.__db_get_dat_all()
                lst_dat = sorted([self.admin_filename_get_base(str(s_name.name)).upper() for s_name in lst_dat])
                s_info += f"{PrettyText.lst2strC(lst_dat, i_block_size=5, i_block_items_max=50)}\n"
            if self.db_get_size_imports() > 0:
                s_info += f"--- Imports (Reports) ({self.db_get_size_imports()}X)\n\n"
                lst_dat = self.__dic_import_meta.keys()
                lst_dat = sorted([str(s_name).upper() for s_name in lst_dat])
                s_info += f"{PrettyText.lst2strC(lst_dat, i_block_size=5, i_block_items_max=50)}\n"
            if self.db_get_size_unknown() > 0:
                s_info += f"-- Unavailable/invalid station names ({self.db_get_size_unknown()}x)\n\n"
                lst_dat = sorted(list(self.__set_dat_notfound))
                lst_dat = sorted([str(s_name).upper() for s_name in lst_dat])
                s_info += f"{PrettyText.lst2strC(lst_dat, i_block_size=5, i_block_items_max=50)}\n"

        return s_info

    # --- Admin

    # Eval filename
    def admin_filename_eval(self, s_file):
        b_valid = False
        s_base = None
        s_station = None
        i_month = None
        i_year = None
        if s_file.endswith(".dat.gz") or s_file.endswith(".dat") or s_file.endswith(".rep.txt.gz") or s_file.endswith(".rep.txt"):
            try:
                s_station = s_file[0:3]
                i_month = int(s_file[3:5])
                i_year = int(s_file[5:7])
                if ((i_month in self.__model.get_months_int()) and
                        (i_year in self.__model.get_years_short()) and
                        (s_station in self.__model.get_stations_lower())):
                    b_valid = True
                else:
                    b_valid = False
            except:
                pass
        return b_valid, s_station, i_month, i_year

    # Get base name
    def admin_filename_get_base(self, s_file):
        return s_file[0:7]

    def admin_part_valid(self, s_station, i_month, i_year):
        if s_station in self.__model.get_stations() and i_month in self.__model.get_months() and i_year in self.__model.get_year_short():
            return True
        else:
            return False

    # Create base from  parts
    def admin_part_create_base(self, s_station, i_month, i_year):
        s_year = f"{i_year}"[-2:]
        s_month = f"{i_month:02d}"
        s_base = f"{s_station.lower()}{s_month}{s_year}"
        return s_base

    # --- Buffer

    # Is buffer active?
    def db_buffer_is_active(self):
        return self.__b_buffer_active

    # Set buffer active
    def db_buffer_set_active(self, b_bool):
        self.__b_buffer_active = b_bool

    # Is buffer refresh?
    def db_buffer_is_refresh(self):
        return self.__b_buffer_refresh_files

    # Set buffer refresh
    def db_buffer_set_refresh(self, b_bool):
        self.__b_buffer_refresh_files = b_bool

    # Clean buffer
    def db_buffer_clean(self):
        res = Result(name = "Delete Buffer")

        try:
            # Del buffer dir dat
            s_buffer_path = self.__model.get_user_buffer_dir()
            shutil.rmtree(s_buffer_path) # deö
            os.makedirs(s_buffer_path, exist_ok=True) # new

            # Wipe buffer rest
            self.__dic_import_meta = {}
            self.__set_dat_notfound = set()
            self.__b_db_has_unsaved_data = True
            self.db_buffer_save_to_disk()
            self.__b_db_has_unsaved_data = False

            self.__smartprinter.verbose(f"Buffer cleaned")

        except Exception as ex:
            self.__smartprinter.verbose(f"Problems while cleaning buffer: {ex}")

        # Answer
        s_text = "Delete buffer"
        s_text_extra = (f'Buffer has been deleted.')
        s_icon = "info"
        res.add_data("header", s_text)
        res.add_data("text", s_text_extra)
        res.add_data("icon", s_icon)
        return res

    # Import buffer from file
    def db_buffer_import_from_zip(self, s_path_import):
        res = Result(name="Import buffer")

        # UI
        self.__smartprinter.workflow("Importing buffer")
        self.__smartprinter.progress("pulse", problem="", info="", abort=False)

        # Wipe buffer files
        try:
            s_buffer_path = self.__model.get_user_buffer_dir()
            shutil.rmtree(s_buffer_path)  # deö
            os.makedirs(s_buffer_path, exist_ok=True)  # new
            self.__smartprinter.normal(f"Old buffer cleaned")
        except Exception as ex:
            self.__smartprinter.normal(f"Problems while cleaning old buffer: {ex}")

        # Import
        try:
            self.__smartprinter.normal(f"Importing buffer from file: {s_path_import}")
            self.__smartprinter.normal(f"Please be patient. If buffer is large it might take some time.")
            self.__smartprinter.normal(f"Unzipping ... ", "")
            shutil.unpack_archive(s_path_import, s_buffer_path, format="zip")
            self.__smartprinter.normal(f"done")
        except Exception as ex:
            self.__smartprinter.normal(f"Problems while importing buffer: {ex}")

        # Reload buffer
        self.__model.get_local_working_database().db_buffer_read_from_disk()

        # UI
        self.__smartprinter.progress("off")

        # Answer
        s_text = "Import buffer"
        s_text_extra = f'Buffer has been sucessfully imported.'
        s_icon = "info"

        res.add_data("header", s_text)
        res.add_data("text", s_text_extra)
        res.add_data("icon", s_icon)
        return res

    # Export buffer to file
    def db_buffer_export_to_zip(self):
        res = Result(name="Export buffer to file")
        workflow = self.__smartprinter.workflow
        progress = self.__smartprinter.progress
        pn = self.__smartprinter.normal
        pv = self.__smartprinter.verbose

        # Files
        s_buffer_path = self.__model.get_user_buffer_dir()
        s_buffer_bak_zip_path = Path(self.__model.get_user_bak_dir(), "buffer_" + PrettyText.create_timestamp())

        # UI
        workflow("Exporting buffer")
        progress("pulse", problem="", info="", abort=False)

        # Export
        try:
            pn(f"Exporting buffer ({self.db_get_size_all_as_string()}) to file: {s_buffer_bak_zip_path}.zip")
            pn(f"Please be patient. If buffer is large it might take some time." )
            pn(f"Zipping ... ", "")
            shutil.make_archive(s_buffer_bak_zip_path, "zip", s_buffer_path)
            pn(f"done")
        except Exception as ex:
            pv(f"problems while exporting buffer: {ex}")
        self.__smartprinter.progress("off")

        # Result
        s_link = Path(self.__model.get_user_bak_dir()).as_uri()
        s_text = "Export buffer"
        s_text_extra = f'The buffer was exported successfully <a href="{s_link}">here</a>'
        s_icon = "info"
        res.add_data("header", s_text)
        res.add_data("text", s_text_extra)
        res.add_data("icon", s_icon)
        return res

    # Save buffer from memory to disk
    def db_buffer_save_to_disk(self):
        if self.__b_db_has_unsaved_data:
            s_path_buffer = Path(self.__model.get_user_buffer_dir(),"buffer.dat")
            try:
                data = (self.__set_dat_notfound, self.__dic_import_meta)
                with open(s_path_buffer, 'wb') as handle:
                    pickle.dump(data, handle, protocol=pickle.HIGHEST_PROTOCOL)
                self.__b_db_has_unsaved_data = False
            except Exception as ex:
                self.__smartprinter.normal(f"Problems saving buffer reports/notfound: {ex}")

    # Read buffer from disk in memory
    def db_buffer_read_from_disk(self):
        s_path_buffer = Path(self.__model.get_user_buffer_dir(), "buffer.dat")
        try:
            # Report meta
            with open(s_path_buffer, 'rb') as handle:
                self.__set_dat_notfound, self.__dic_import_meta = pickle.load(handle)
            self.__b_db_has_unsaved_data = False
        except Exception as ex:
            self.__smartprinter.normal(f"Problems reading buffer notfound/reports from disk: {ex}")
            self.__smartprinter.normal(f"Perhaps first time starting the app?")
            self.__smartprinter.normal(f"Rewrite buffer …")
            self.__b_db_has_unsaved_data = True
            self.db_buffer_save_to_disk()
            self.__smartprinter.normal(f"… done")

    # --- Server

    # Differs file in buffer from file on ftp server?
    def db_local_file_differs_from_server(self, s_file, ftp):
        s_station = s_file[:3]
        if self.dat_is_file_in_db(s_file):
            # Files
            s_file_buffer = str(Path(self.__s_db_buffer_dir, s_file))
            s_file_server = str(Path("/", s_station, s_file))

            # Local Metadaten
            local_size = os.path.getsize(s_file_buffer)
            local_mtime = os.path.getmtime(s_file_buffer)

            # Server-Metadaten
            ftp_size = ftp.size(s_file_server)  # size
            resp = ftp.sendcmd(f"MDTM {s_file_server}")
            ftp_mtime = datetime.strptime(resp[4:], "%Y%m%d%H%M%S").timestamp()  # date

            # Compare
            if ftp_size != local_size or ftp_mtime > local_mtime:
                return True # different
            else:
                return False # same
        else:
            return None # PRoblem

    # Load frile from server (and biuffer it)
    def db_load_file_from_server(self, s_file, ftp):
        # Files
        s_station = s_file[:3]
        s_file_buffer = str(Path(self.__s_db_buffer_dir, s_file))
        s_file_server = f"/{s_station}/{s_file}"
        b_ok = False
        with open(s_file_buffer, 'wb') as f:
            try:
                #ftp.voidcmd('TYPE I') # no unzipping under windows
                ftp.retrbinary(f"RETR {s_file_server}", f.write)
                b_ok = True
            except Exception as ex:
                b_ok = False
                os.remove(s_file_buffer)  # del empty file
        return b_ok

    # --- Buffer Sizes

    # Get all buffered files (.dat.gz)
    def __db_get_dat_all(self):
        return list(self.__s_db_buffer_dir.glob("*.dat.gz"))

    # Get number of all buffered files
    def db_get_size_dat(self):
        return len(list(self.__db_get_dat_all()))

    # Get number of all buffered imports/reports
    def db_get_size_imports(self):
        return len(self.__dic_import_meta)

    # Get number of all buffered unknonw/invalid (not available) statipn data
    def db_get_size_unknown(self):
        return len(self.__set_dat_notfound)

    # Get number of all buffered data (data,reports and unknown)
    def db_get_size_all(self):
        return self.db_get_size_all_files() + self.db_get_size_unknown()

    # Get number of all buffered files (data and reports)
    def db_get_size_all_files(self):
        return self.db_get_size_dat() + self.db_get_size_imports()

    # Get size of whole buffer in mb
    def db_get_size_all_mb(self):
        i_size_bytes = sum(f.stat().st_size for f in self.__s_db_buffer_dir.glob("*") if f.is_file())
        i_size_mb = int(i_size_bytes / (1024 ** 2))
        return i_size_mb

    # Get size of whole buffer in mb/gb as string
    def db_get_size_all_as_string(self):
        i_size_mb = self.db_get_size_all_mb()
        s_return = ""
        if i_size_mb > 10000:
            s_return = f"{(i_size_mb/1000):.2f} GB"
        else:
            s_return = f"{i_size_mb} MB"
        return s_return

    # --- Not Found

    # Get names of all not found (unavailable) station data
    def notfound_get_names(self):
        return sorted(list(self.__set_dat_notfound.copy()))

    # Is base name in unknown buffer?
    def notfound_is_in(self, s_base):
        return s_base in self.__set_dat_notfound

    # Remove base name from unknown buffer
    def notfound_remove(self, s_base):
        if s_base in self.__set_dat_notfound:
            self.__set_dat_notfound.discard(s_base)
            self.__b_db_has_unsaved_data = True

    # Add base name to unknown buffer
    def notfound_add(self, s_base):
        if s_base not in self.__set_dat_notfound:
            self.__set_dat_notfound.add(s_base)
            self.__b_db_has_unsaved_data = True

    # --- Data (Station data files)

    # Is file of data (base name) in not-found-buffer?
    def dat_is_in_notfound(self, s_file):
        return s_file in self.__set_dat_notfound

    # Is file buffer?
    def dat_is_file_in_db(self, s_file):
        s_file_buffer = str(Path(self.__s_db_buffer_dir, s_file))
        if os.path.isfile(s_file_buffer):
            if os.path.getsize(s_file_buffer) != 0:
                return True
            else:
                os.remove(s_file_buffer)  # if 0-file is found remove by the way
                return False

    """
    Deliver files to working dir (either from buffer or ftp server)
    Args:
        sel (Selector): Selector of files/parts
        s_working_dir (str): Working dir to deliver files
    Kwargs:
        unzip (bool): Unzip data 
    """
    def dat_deliver_files_to_working_dir(self, sel, s_working_dir, **kwargs):

        res = Result(name="Get files from local working database")

        pn = self.__smartprinter.normal
        pv = self.__smartprinter.verbose
        workflow = self.__smartprinter.workflow
        progress = self.__smartprinter.progress
        buffer_update = self.__smartprinter.buffer

        self.abort(False) # Reset

        # Kwargs
        b_unzip = True if kwargs.get("unzip") is True else False

        # Data
        odic_download = OrderedDict()
        lst_stations = []
        i_files_max = 0
        lst_err_copy_zip = [] # problems while copying files to working dir
        sel_files_delivered = Selection(self.__model) # actually delivered files
        lst_files_delivered = []
        i_files_delivered = 0

        # Log
        s_log = ""

        # Prepare internal data structure
        sel.init(valid="valid", type="all")
        for s_base, s_file_name, s_path in sel:
            s_station = s_base[:3]
            if s_station not in odic_download:
                if s_station not in lst_stations:
                    lst_stations.append(s_station)
                odic_download[s_station] = {
                    "files": [],
                    "new": [],
                    "old": [],
                    "old_upd": [],
                    "not_valid": []
                }
            odic_download[s_station]["files"].append(s_base)
            i_files_max += 1
        lst_stations = sorted(lst_stations)
        i_station_max = len(lst_stations)

        # --- Info
        pn("* Getting station data files from local working database")
        if len(lst_stations) == self.__model.get_stations_num():
            s_tmp_stations = f"all"
        else:
            s_tmp_stations = f"{PrettyText.lst2strC(lst_stations)}"
        pn(f'Stations ({len(lst_stations)}): {s_tmp_stations}')
        pn(f'Files: {i_files_max}')
        pn(f'Buffer: {("on" if self.db_buffer_is_active() else "off")}')
        if self.db_buffer_is_active():
            pn(f'Refresh buffer: {"on" if self.db_buffer_is_refresh() else "off"}')
        pn(f'FTP: {self.__model.get_ftp_user()}@{self.__model.get_ftp_server()}')
        pn(f'Dir: {self.__s_db_buffer_dir}')
        pn(f'Unzip data: {"yes" if b_unzip else "no"}')
        if sel.has_not_valid():
            pn(f"FYI: Found not valid selections ({sel.get_base_num_not_valid()}x):")
            pn(sel.info_not_valid())
        pv()
        workflow("Getting files from local working database")
        progress("pulse")

        # --- Get files
        try:
            with ftplib.FTP(self.__model.get_ftp_server()) as ftp:

                # Login
                ftp.login(self.__model.get_ftp_user(), self.__model.get_ftp_pw())

                # --- Loop: Stations
                i_file_now = 0
                for i_station, s_station in enumerate(odic_download.keys()):
                    lst_files = odic_download[s_station]["files"]
                    if self.is_abort():
                        break

                    # --- Loop: dat in station
                    i_file_station_max = len(lst_files)
                    for i_file_station, s_base in enumerate(lst_files):
                        if self.is_abort():
                            break
                        i_file_now += 1
                        s_file = f"{s_base}.dat.gz"
                        progress(i_file_now, i_files_max, 100, proz=True, abs=True,
                                 text=f"{s_station.upper()} [{s_file}] ({i_file_station+1}/{i_file_station_max})",
                                 status="", cli=True, problem="", info="", abort=False)
                        pv(f"{s_station.upper()} ({i_file_station+1}/{i_file_station_max}): {s_file}: ", "")

                        # --- Check lwdb
                        b_in_db = False
                        b_in_db_diff = False
                        b_in_notfound = False
                        b_download = False
                        b_final_available = False
                        b_err_copy = False
                        b_err_unzip = False
                        if self.db_buffer_is_active():
                            # --- Buffer active
                            b_in_notfound = self.dat_is_in_notfound(s_base)
                            if b_in_notfound:
                                # --- File in buffer: not-found
                                odic_download[s_station]["not_valid"].append(s_base)
                                pv(f"not found (buffered)", "")
                            else:
                                # --- File is not in buffer: not-found
                                b_in_db = self.dat_is_file_in_db(s_file)
                                b_in_db_diff = False
                                if b_in_db:
                                    # --- File found in buffer: dat
                                    if not self.db_buffer_is_refresh():
                                        # --- No refesh
                                        pv("found in local working database … ","")
                                        odic_download[s_station]["old"].append(s_base)
                                        b_final_available = True
                                    else:
                                        # --- Refresh
                                        # --- Check if differing
                                        pv(f"found in local working database … check if local version differs from server version: ", "")
                                        b_in_db_diff = self.db_local_file_differs_from_server(s_file, ftp)
                                        if not b_in_db_diff:
                                            # --- Buffered version equal to server version
                                            pv("same … ", "")
                                            odic_download[s_station]["old"].append(s_base)
                                            b_final_available = True
                                        else:
                                            # --- Buffered version differs from server version
                                            pv(f"differing (size/date) … ", "")
                                            odic_download[s_station]["old_upd"].append(s_base)

                        # --- Eval if dat should be downloaded
                        b_download = True
                        if b_in_db and not b_in_db_diff:
                            b_download = False
                        if b_in_notfound:
                            b_download = False
                        if not self.db_buffer_is_active():
                            b_download = True

                        if b_download:
                            # --- Download dat
                            pv(f"downloading … ", "")
                            b_ok = self.db_load_file_from_server(s_file, ftp)
                            if b_ok:
                                pv(f"finished … ", "")
                                odic_download[s_station]["new"].append(s_base)
                                self.notfound_remove(s_base)
                                b_final_available = True
                            else:
                                pv(f"not found", "")
                                self.notfound_add(s_base)
                                odic_download[s_station]["not_valid"].append(s_base)
                            buffer_update()

                        # --- Copy and unzip file to working dir
                        if b_final_available:

                            s_file_buffer = str(Path(self.__s_db_buffer_dir, s_file))
                            s_file_gz_working_dir = str(Path(s_working_dir, s_file))
                            s_file_dat_working_dir = str(Path(s_working_dir, f"{self.admin_filename_get_base(s_file)}.dat"))

                            # --- Copy file to working dir
                            pv(f'copy to download dir{" and unzip" if b_unzip else ""} … ', "")
                            try:
                                shutil.copy(s_file_buffer, s_working_dir)
                            except:
                                pv(f"error while copy … ", "")
                                b_err_copy = True
                                lst_err_copy_zip.append(s_file)
                                res.add_err(f"Problems copying file from lwdb: {s_file}")

                            # --- Unzip file to working dir
                            if b_unzip and not b_err_copy:
                                try:
                                    # unzip
                                    with gzip.open(s_file_gz_working_dir, "rb") as f_in:
                                        with open(s_file_dat_working_dir, "wb") as f_out:
                                            shutil.copyfileobj(f_in, f_out)
                                    # remove zip
                                    os.remove(s_file_gz_working_dir)
                                    lst_files_delivered.append(s_file_gz_working_dir)
                                except:
                                    b_err_unzip = True
                                    lst_err_copy_zip.append(s_file)
                                    res.add_err(f"Problems unzipping file: {s_file}")
                                    pv(f"error while unzipping … ", "")

                            # Add to delivered files
                            if not b_err_copy and not b_err_unzip:
                                if b_unzip:
                                    lst_files_delivered.append(s_file_dat_working_dir)
                                else:
                                    lst_files_delivered.append(s_file_gz_working_dir)
                                pv(f"done", "")
                        # Return
                        pv("")

                    # --- Console info per station
                    s_tmp = ""
                    i_tmp_len_buffered_already = len(odic_download[s_station]["old"])
                    i_tmp_len_buffered_already_upd = len(odic_download[s_station]["old_upd"])
                    i_tmp_len_buffered_new = len(odic_download[s_station]["new"])
                    i_tmp_len_not_valid = len(odic_download[s_station]["not_valid"])
                    i_tmp_total = i_tmp_len_buffered_already + i_tmp_len_buffered_new
                    if i_tmp_total > 0 or i_tmp_len_not_valid > 0:
                        lst_tmp = []
                        if i_tmp_len_buffered_new > 0:
                            lst_tmp.append(f"added to lwdb: {i_tmp_len_buffered_new}")
                        if i_tmp_len_buffered_already_upd > 0:
                            lst_tmp.append(f"updated in lwdb: {i_tmp_len_buffered_already_upd}")
                        if i_tmp_len_buffered_already > 0:
                            lst_tmp.append(f"already in lwdb: {i_tmp_len_buffered_already}")
                        if i_tmp_len_not_valid > 0:
                            lst_tmp.append(f"unavailable: {i_tmp_len_not_valid}")
                        s_tmp += f"{i_tmp_total} ({PrettyText.lst2strC(lst_tmp)})"
                    else:
                        s_tmp += f"empty"
                    pn(f"{s_station.upper()}: {s_tmp}")

        except Exception as ex:
            res.add_err(f"Get files from local working database: FTP connection error: {ex}")

        # --- Final console info
        i_files_delivered = len(lst_files_delivered)
        if i_files_delivered > 0:
            s_txt = f'Files have been downloaded {"and unzipped " if b_unzip else ""}to working directory '
            if res.is_ok():
                s_txt += f"without any problems: {i_files_delivered}x"
            else:
                s_txt += f"but problems occurred: {i_files_delivered}x"
        else:
            s_txt = f'No files have been downloaded to working directory'
        pv()
        pn(s_txt)
        pv()

        # --- Create final selection
        sel_files_delivered.load_filenames(lst_files_delivered)

        # --- Total Statistics
        lst_table_overview = [["Station", "New", "Avail", "Avail.upd", "Total"]]
        lst_table_unrecognized = [["Station", "#", "Unavailable"]]

        # Eval
        i_new_total = 0
        i_old_total = 0
        i_old_upd_total = 0
        i_not_valid__total = 0
        i_buffer_total = 0
        for s_station, odic_station_stat in odic_download.items():
            lst_new = odic_station_stat.get("new")
            i_new_total += len(lst_new)
            lst_old = odic_station_stat.get("old")
            i_old_total += len(lst_old)
            lst_old_upd = odic_station_stat.get("old_upd")
            i_old_upd_total += len(lst_old_upd)
            lst_unrec = odic_station_stat.get("not_valid")
            i_not_valid__total += len(lst_unrec)
            lst_table_overview.append([s_station.upper(), len(lst_new), len(lst_old), len(lst_old_upd),
                                       len(lst_new) + len(lst_old) + len(lst_old_upd)])
            if len(lst_unrec) > 0:
                lst_table_unrecognized.append([s_station.upper(), len(lst_unrec), PrettyText.lst2strC(lst_unrec)])
        i_buffer_total = i_old_total + i_new_total

        # Create Log
        b_log_empty = False
        b_log_details = False
        if (i_new_total > 0) or (i_old_total > 0) or (i_not_valid__total > 0):
            b_log_details = True

        # Overview
        s_log = "--- Get station data files from local working database\n\n"
        if len(lst_stations) == self.__model.get_stations_num():
            s_tmp_stations = f"all"
        else:
            s_tmp_stations = f"{PrettyText.lst2strC(lst_stations)}"
        s_log += f"Requested stations ({len(lst_stations)}): {s_tmp_stations}\n"
        if i_buffer_total > 0:
            s_log += f"Local station data total: {i_buffer_total}\n"
        else:
            s_log += f"Nothing found in Local working database\n"
        if i_new_total > 0:
            s_log += f"New added/downloaded data: {i_new_total}\n"
        if i_old_total > 0:
            s_log += f"Station data already locally available: {i_old_total}\n"
        if i_old_upd_total > 0:
            s_log += f"Station data already locally available (updated): {i_old_upd_total}\n"
        if i_not_valid__total > 0:
            s_log += f"Server: unavailable/invalid file-names: {i_not_valid__total}\n"
        if i_new_total > 0:
            s_log += f'FTP: {self.__model.get_ftp_user()}@{self.__model.get_ftp_server()}\n'
        s_log += f'Working directory: {s_working_dir}\n'
        s_log += f'Unzip data: {"yes" if b_unzip else "no"}\n'
        s_log += "\n"
        if i_files_delivered > 0:
            s_log += f'Delivered station data: {i_files_delivered}x\n'
        else:
            s_log += "No files have been delivered to working directory\n"
        s_log += "\n"

        # FYI: Not valid selection
        if sel.has_not_valid():
            s_log += f"FYI: Found not valid selections ({sel.get_base_num_not_valid()}x):\n"
            s_log += sel.info_not_valid()
            s_log += "\n"

        # Details
        if b_log_details:
            if len(lst_err_copy_zip) > 0:
                s_log += f"Error: Failed to copying/unzip these files to working dir ({len(lst_err_copy_zip)}x):\n"
                s_log += PrettyText.lst2strR(lst_err_copy_zip)
                s_log += "\n\n"
            if i_buffer_total > 0:
                s_log += PrettyText.table(lst_table_overview)
                s_log += "\n\n"
            if len(lst_unrec) > 0:
                s_log += PrettyText.table(lst_table_unrecognized, 5)
                s_log += "\n\n"

        # UI
        workflow()
        progress("off")

        # Save db (buffer part)
        self.db_buffer_save_to_disk()

        # Result
        if self.is_abort():
            res.add_err("Process aborted by user")
        res.add_data("log", s_log)
        res.add_data("delivered", sel_files_delivered)
        return res

    # --- Import (import data plus report)

    # Is import & report available in buffer?
    def imp_is_available(self, s_base):
        return s_base in self.__dic_import_meta

    # Get report meta
    def imp_get_report_meta(self, s_base):
        return self.__dic_import_meta.get(s_base)

    # Get import data for base name
    def imp_get_data(self, s_base):
        path_file_buffer = Path(self.__s_db_buffer_dir, f"{s_base}.imp")
        odic_import = None
        b_ok = False
        s_err = None
        if path_file_buffer.exists():
            try:
                with gzip.open(str(path_file_buffer), "rb") as f:
                    odic_import = pickle.load(f)
                b_ok = True
            except Exception as ex:
                print(ex)
                s_err = f"{ex}"
        return b_ok, s_err, odic_import

    # Get report for base name
    def imp_get_report(self, s_base):
        b_err_tec = False
        s_err_tec = None
        i_err_rep = None
        i_wrn_rep = None
        i_inf_rep = None
        set_unknown_recs = None
        s_report = None
        s_err_rep_overview = None
        if self.imp_is_available(s_base):
            # Meta
            i_err_rep, i_wrn_rep, i_inf_rep, set_unknown_recs, s_err_rep_overview = self.__dic_import_meta.get(s_base)
            # Report
            path_buffer_path_file_rep_gz = Path(self.__s_db_buffer_dir, f"{s_base}.rep.txt.gz")
            if path_buffer_path_file_rep_gz.exists():
                try:
                    with gzip.open(path_buffer_path_file_rep_gz, "rt") as f:
                         s_report = f.read()
                except Exception as ex:
                    b_err_tec = True
                    s_err_tec = str(ex)
            else:
                b_err = True
                s_err_tec = "UUUPS: This should never happen. Meta data is available but no report."
        return b_err_tec, s_err_tec, i_err_rep, i_wrn_rep, i_inf_rep, set_unknown_recs, s_report, s_err_rep_overview

    # Get names of buffered imports & reports
    def imp_get_names(self):
        return sorted(list(self.__dic_import_meta.keys()))

    # Add import and report to buffer
    def imp_add(self, s_base, odic_import, tp_data, s_report):
        try:
            # import
            s_buffer_path_file_import = str(Path(self.__s_db_buffer_dir, f"{s_base}.imp"))
            with gzip.open(s_buffer_path_file_import, "wb") as f:
                pickle.dump(odic_import, f, protocol=pickle.HIGHEST_PROTOCOL)
            # report
            s_buffer_path_file_rep_gz = str(Path(self.__s_db_buffer_dir, f"{s_base}.rep.txt.gz"))
            with gzip.open(s_buffer_path_file_rep_gz, "wb") as f:
                f.write(s_report.encode("utf-8"))
            # meta data
            self.__dic_import_meta[s_base] = tp_data
            self.__b_db_has_unsaved_data = True  # DEV allways save
        except Exception as ex:
            return (True, str(ex))
        return False, None


