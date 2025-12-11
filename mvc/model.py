# coding=utf-8

"""
Model
"""

import base64, calendar, configparser, os
from pathlib import Path

from logic.converter import Converter
from logic.helper import SmartPrinter
from logic.local_working_database import LocalWorkingDatabase
from logic.bsrn_id_system import BsrnIdSystem
from logic.task_manager import TaskManager

class Model:

    # Constructor
    def __init__(self):

        # --- App-wide data

        # Info about the program
        self.__s_prg_name = "BSRN Toolbox"
        self.__s_prg_version = "3.0.0-0"
        self.__s_prg_contact = '<a href="https://www.awi.de" target="_blank">www.awi.de</a>'
        self.__s_prg_warranty = "BSD-3 license (<a href=\"https://opensource.org/licenses/BSD-3-Clause\">LICENSE</a>)"
        self.__s_prg_year = "2025"
        self.__s_prg_info = f'''
<h3> {self.__s_prg_name} v. {self.__s_prg_version}</h3>
The original BSRN quality check was developed by Holger Schmidthüsen (<a href="mailto:holger.schmithuesen@awi.de">email</a>).
The app was ported to Python by Paul Kloss (<a href="mailto:paul@kloss.info">email</a>).
<p>Warranty: {self.__s_prg_warranty}</p>
<p>Contact: {self.__s_prg_contact}</p>
Copyright: {self.__s_prg_year}
'''
        # Verbosity
        self.__b_verbose = False
        self.__b_debug = False
        self.__observer = None # Oberver der View

        # Print
        self.__smartprinter = SmartPrinter(model=self) # Smartprinter

        # Parallel
        self.__worker = None

        # Files
        self.__s_path_user_home = os.path.expanduser("~")
        self.__s_path_user_data = Path(self.__s_path_user_home, "bsrn_user_data")
        self.__s_path_user_data_tmp = Path(self.__s_path_user_data, "tmp")
        self.__s_path_user_data_ids = Path(self.__s_path_user_data, "ids")
        self.__s_path_user_data_bak = Path(self.__s_path_user_data, "bak")
        self.__s_path_user_data_cfg = Path(self.__s_path_user_data, "cfg.txt")
        self.__s_path_user_data_buffer = Path(self.__s_path_user_data, "buffer")

        # Parts
        self.__lst_months = list(calendar.month_name)[1:]
        self.__lst_months_int = [1,2,3,4,5,6,7,8,9,10,11,12]
        self.__lst_years = list(range(1992, 2027)) # 1992 - 2026
        self.__lst_stations = [
            "ALE",
            "ASP",
            "BAR",
            "BER",
            "BIL",
            "BON",
            "BOS",
            "BOU",
            "BRB",
            "BUD",
            "CAB",
            "CAM",
            "CAP",
            "CAR",
            "CLH",
            "CNR",
            "COC",
            "DAA",
            "DAR",
            "DOM",
            "DON",
            "DRA",
            "DWN",
            "E13",
            "ENA",
            "EUR",
            "FLO",
            "FPE",
            "FUA",
            "GAN",
            "GCR",
            "GOB",
            "GUR",
            "GVN",
            "HOW",
            "ILO",
            "ISH",
            "IZA",
            "KWA",
            "LAU",
            "LER",
            "LIN",
            "LLN",
            "LRC",
            "MAN",
            "MNM",
            "NAU",
            "NEW",
            "NYA",
            "PAL",
            "PAY",
            "PSA",
            "PSU",
            "PTR",
            "REG",
            "RLM",
            "SAP",
            "SBO",
            "SMS",
            "SON",
            "SOV",
            "SPO",
            "SXF",
            "SYO",
            "TAM",
            "TAT",
            "TIK",
            "TIR",
            "TOR",
            "XIA",
            #"ZVE",
            "ABS",
            "CYL",
            "NPT",
            "INO",
            "TNB",
            "EFS"
        ]
        self.__lst_stations.sort()

        # Seq Names (Record numbers with sequences)
        self.__lst_seq = [
            8,
            100,
            400,
            500,
            3010,
            3030
        ]

        # Meta data records
        self.__lst_metadata_recs = [
            1,
            2,
            3,
            4,
            5,
            6,
            7,
            8,
            9
        ]

        self.__lst_data_recs = [
            100,
            300,
            400,
            500,
            1000,
            1100,
            1200,
            1300,
            1500,
            3010,
            3030,
            4000
        ]

        # Last User selected Download parameters
        self.__s_stations_code = None
        self.__s_years_code = None
        self.__s_months_code = None
        self.__s_working_dir = None
        self.__b_opt_zip = None
        self.__b_opt_report = None
        self.__b_opt_avail = None
        self.__s_ftp_server = None
        self.__s_ftp_user = None
        self.__s_ftp_pw = None

        # Prepare dirs
        if not os.path.exists(self.__s_path_user_data):
            try:
                print("No user data folder found: " + str(self.__s_path_user_data))
                os.mkdir(self.__s_path_user_data)
                print("... now its made.")
            except:
                print("Error: could not generate user data folder: " + str(self.__s_path_user_data))
        if not os.path.exists(self.__s_path_user_data_tmp):
            try:
                print("No user data tmp folder found: " + str(self.__s_path_user_data_tmp))
                os.mkdir(self.__s_path_user_data_tmp)
                print("... now its made.")
            except:
                print("Error: could not generate user data tmp folder: " + str(self.__s_path_user_data_tmp))
        if not os.path.exists(self.__s_path_user_data_buffer):
            try:
                print("No user data buffer folder found: " + str(self.__s_path_user_data_buffer))
                os.mkdir(self.__s_path_user_data_buffer)
                print("... now its made.")
            except:
                print("Error: could not generate user buffer folder: " + str(self.__s_path_user_data_buffer))
        if not os.path.exists(self.__s_path_user_data_ids):
            try:
                print("No user data lookup folder found: " + str(self.__s_path_user_data_ids))
                os.mkdir(self.__s_path_user_data_ids)
                print("... now its made.")
            except:
                print("Error: could not generate user bsrn id folder: " + str(self.__s_path_user_data_ids))


        # Systems
        self.__local_working_database = LocalWorkingDatabase(self)
        self.__bsrn_id_sys = BsrnIdSystem(self)
        self.__converter = Converter(self)
        self.__taskmanager = TaskManager(self)

        # Read conf from file
        self.cfg_read()

        # Read buffer from disk
        self.get_local_working_database().db_buffer_read_from_disk()

    # --- Info

    def get_prg_infos(self):
        return {"name": self.__s_prg_name, "version": self.__s_prg_version , "info" : self.__s_prg_info, "contact": self.__s_prg_contact, "warranty": self.__s_prg_warranty, "year": self.__s_prg_year}

    # --- Verbosity

    def is_normal(self):
        return not(self.is_verbose() or self.is_debug())

    def is_verbose(self):
        return self.__b_verbose

    def set_verbose(self, bBool):
        self.__b_verbose = bBool

    def is_debug(self):
        return self.__b_debug

    def set_debug(self, bBool):
        self.__b_debug = bBool

    # --- Directories

    def get_working_dir(self):
        return self.__s_working_dir

    def set_working_dir(self, s_txt):
        self.__s_working_dir = s_txt

    def get_user_buffer_dir(self):
        return self.__s_path_user_data_buffer

    def get_user_lookup_dir(self):
        return self.__s_path_user_data_ids

    def get_user_home_data_dir(self):
        return self.__s_path_user_data

    def get_user_bak_dir(self):
        return self.__s_path_user_data_bak

    # --- Records

    def get_seq_numbers(self):
        return self.__lst_seq

    def get_metadata_recs(self):
        return self.__lst_metadata_recs

    def get_metadata_recs_str(self):
        return [f"{i:04d}" for i in self.__lst_metadata_recs]

    # Is record name/number a meta data record
    def is_rec_meta(self, s_rec):
        try:
            return int(s_rec) in self.get_metadata_recs_int()
        except:
            return False
    def get_metadata_recs_int(self):
        return self.__lst_metadata_recs

    # Is record name/number a data record
    def is_rec_data(self, s_rec):
        try:
            return int(s_rec) in self.get_data_recs_int()
        except:
            return False

    def get_data_recs_int(self):
        return self.__lst_data_recs

    def get_data_recs_str(self):
        return [f"{i:04d}" for i in self.__lst_data_recs]


    # Has record name a block
    def is_block(self, s_rec_name):
        try:
            i_number_in = int(s_rec_name[1:])
        except:
            i_number_in = None
        if i_number_in in self.get_seq_numbers():
            return True
        else:
            return False

    # --- Observer

    def get_observer(self):
        return self.__observer

    def set_observer(self, obs):
        self.__observer = obs

    # -- Task manager

    def get_task_manager(self):
        return self.__taskmanager

    # --- Subsystems

    def get_bsrn_id_system(self):
        return self.__bsrn_id_sys

    def get_local_working_database(self):
        return self.__local_working_database

    def get_converter(self):
        return self.__converter

    # --- Worker (parallel containers)
    def get_worker(self):
        return self.__worker

    def set_worker(self, w):
        self.__worker = w

    # --- Smartprinter

    def get_smart_printer(self):
        return self.__smartprinter

    # --- Parts (station/Yyear/month)

    def get_stations(self):
        return self.__lst_stations

    def get_stations_lower(self):
        return [s_station.lower() for s_station in self.__lst_stations]

    def get_stations_num(self):
        return len(self.__lst_stations)

    def is_station(self, sName):
        return sName in self.__lst_stations

    def get_years(self):
        return self.__lst_years

    def get_year_lowest(self):
        return min(self.__lst_years)

    def get_year_lowest_short(self):
        return int(str(min(self.__lst_years))[2:4])

    def get_years_short(self): # workaroud, because of missdesign of using only the last two digits of the year
        return [int(str(i_year)[2:4]) for i_year in self.__lst_years]

    def get_years_num(self):
        return len(self.__lst_years)

    def is_year(self, sYear):
        return sYear in self.__lst_years

    def get_months(self):
        return self.__lst_months

    def get_months_int(self):
        return self.__lst_months_int
    def get_stations_code(self):
        return self.__s_stations_code

    def set_stations_code(self, s_txt):
        self.__s_stations_code = s_txt

    def get_years_code(self):
        return self.__s_years_code

    def set_years_code(self, s_txt):
        self.__s_years_code = s_txt

    def get_months_code(self):
        return self.__s_months_code

    def set_months_code(self, s_txt):
        self.__s_months_code = s_txt

    # --- App options (set by user)

    def get_opt_unzip(self):
        return self.__b_opt_zip

    def set_opt_unzip(self, b_bool):
        self.__b_opt_zip = b_bool

    def get_opt_report(self):
        return self.__b_opt_report

    def set_opt_report(self, b_bool):
        self.__b_opt_report = b_bool

    def get_opt_avail(self):
        return self.__b_opt_avail

    def set_opt_avail(self, b_bool):
        self.__b_opt_avail = b_bool

    def get_ftp_server(self):
        return self.__s_ftp_server

    def set_ftp_server(self, s_txt):
        self.__s_ftp_server = s_txt

    def get_ftp_user(self):
        return self.__s_ftp_user

    def set_ftp_user(self, s_txt):
        self.__s_ftp_user = s_txt

    def get_ftp_pw(self):
        return self.__s_ftp_pw

    def set_ftp_pw(self, s_txt):
        self.__s_ftp_pw = s_txt

    # --- Save/read app config to disk

    def cfg_read(self):
        try:
            cfg = configparser.ConfigParser()
            cfg.read(self.__s_path_user_data_cfg)
            # Default
            if cfg.has_option("DL", "stations_code"): self.set_stations_code(cfg["DL"]["stations_code"])
            if cfg.has_option("DL", "months_code"): self.set_months_code(cfg["DL"]["months_code"])
            if cfg.has_option("DL", "years_code"): self.set_years_code(cfg["DL"]["years_code"])
            if cfg.has_option("DL", "dl_dir"): self.set_working_dir(cfg["DL"]["dl_dir"])
            if cfg.has_option("DL", "ftp_server"): self.set_ftp_server(cfg["DL"]["ftp_server"])
            if cfg.has_option("DL", "ftp_user"): self.set_ftp_user(cfg["DL"]["ftp_user"])
            if cfg.has_option("DL", "ftp_pw"):
                s_enc = cfg["DL"]["ftp_pw"]
                bytes_enc = base64.b64decode(s_enc)
                s_dec = bytes_enc.decode('utf-8')
                self.set_ftp_pw(str(s_dec))
            if cfg.has_option("DL", "opt_zip"): self.__b_opt_zip = (True if str(cfg["DL"]["opt_zip"]) == "True" else False)
            if cfg.has_option("DL", "opt_check"): self.__b_opt_report = (True if str(cfg["DL"]["opt_check"]) == "True" else False)
            if cfg.has_option("DL", "opt_avail"): self.__b_opt_avail = (True if str(cfg["DL"]["opt_avail"]) == "True" else False)
            if cfg.has_option("ETC", "verbose"): self.__b_verbose = (True if str(cfg["ETC"]["verbose"]) == "True" else False)
            if cfg.has_option("ETC", "debug"): self.__b_debug = (True if str(cfg["ETC"]["debug"]) == "True" else False)
            if cfg.has_option("ETC", "buffer"): self.get_local_working_database().db_buffer_set_active(True if str(cfg["ETC"]["buffer"]) == "True" else False)
            if cfg.has_option("ETC", "buffer_refresh"): self.get_local_working_database().db_buffer_set_refresh(True if str(cfg["ETC"]["buffer_refresh"]) == "True" else False)
        except (configparser.Error, ValueError) as ex:
            self.__smartprinter.normal(f"Error while reading the config file ({self.__s_path_user_data_cfg}): {ex}")
            return False
        return True

    def cfg_save(self):
        try:
            cfg = configparser.ConfigParser()
            cfg.read(self.__s_path_user_data_cfg)
            # DL
            cfg["DL"] = {}
            cfg["DL"]["stations_code"] = self.__s_stations_code
            cfg["DL"]["months_code"] = self.__s_months_code
            cfg["DL"]["years_code"] = self.__s_years_code
            cfg["DL"]["dl_dir"] = self.__s_working_dir
            cfg["DL"]["opt_zip"] = "True" if self.__b_opt_zip else "False"
            cfg["DL"]["opt_check"] = "True" if self.__b_opt_report else "False"
            cfg["DL"]["opt_avail"] = "True" if self.__b_opt_avail else "False"
            cfg["DL"]["ftp_server"] = self.__s_ftp_server
            cfg["DL"]["ftp_user"] = self.__s_ftp_user
            bytes_dec = self.get_ftp_pw().encode('utf-8')
            bytes_enc = base64.b64encode(bytes_dec)
            str_enc = bytes_enc.decode('utf-8')
            cfg["DL"]["ftp_pw"] = str_enc
            # Etc
            cfg["ETC"] = {}
            cfg["ETC"]["verbose"] = "True" if self.__b_verbose else "False"
            cfg["ETC"]["debug"] = "True" if self.__b_debug else "False"
            cfg["ETC"]["buffer"] = "True" if self.get_local_working_database().db_buffer_is_active() else "False"
            cfg["ETC"]["buffer_refresh"] = "True" if self.get_local_working_database().db_buffer_is_refresh() else "False"
            with open(self.__s_path_user_data_cfg, 'w') as cfgdat:
                cfg.write(cfgdat)
            self.__smartprinter.verbose(f"Configuration is saved in file: {self.__s_path_user_data_cfg}")
        except (configparser.Error, ValueError) as ex:
            self.__smartprinter.normal(f"Error while saving the configuration file ({self.__s_path_user_data_cfg}): {ex})")
            return False
        return True

