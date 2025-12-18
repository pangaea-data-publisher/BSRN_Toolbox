# coding=utf-8

"""
BSRN Id System
"""
import re

import requests
from collections import OrderedDict
from pathlib import Path

from logic.helper import Result, PrettyText


class BsrnIdSystem:

    def __init__(self, model):

        self.__model = model
        self.__print = model.get_smart_printer()

        # Result
        self.__res = Result(name="BSRN-Id system")

        # Files
        self.__s_path_lookup_data = self.__model.get_user_lookup_dir()
        self.__s_file_bsrn_ids = "BSRN_IDs.txt"
        self.__s_path_lookup_data_bsrn_ids = str(Path(self.__model.get_user_lookup_dir(), self.__s_file_bsrn_ids))
        self.__s_download_bsrn_ids = "https://store.pangaea.de/config/bsrn/BSRN_IDs.txt"

        # Data
        self.__lst_section_names = ["station", "staff", "ozonesonde", "expanded", "radiosonde", "methods", "version", "end"]
        self.__lst_section_names_special = ["methods_wrmc"]
        self.__dic_data_sections = {}

        # Init
        self.initialize()

    def __str__(self):
        return self.info()

    def info(self):
        s_out = ""
        if self.is_working():
            s_out += "BSRN-Id system is working …\n"
            lst_table = [["Section", "Entries"]]
            for s_section in self.__lst_section_names:
                if s_section not in ["version", "end"]:
                    lst_table.append([s_section.capitalize(), len(self.__dic_data_sections[s_section])])
            s_out += PrettyText.table(lst_table)
            s_out += "\n"
        else:
            s_out += "BSRN-Id system is not working … \n"
            s_out += self.get_init_error()
        return s_out

    # Helper Parse BSRN ID file

    def eval_bsrn_file(self, file_path):

        # --- helper

        def tab_split(line: str):
            return [x.strip() for x in line.split('\t')]

        # ---

        try:
            with open(file_path, 'r', encoding='utf-8') as f:

                s_current_section = None
                lst_headers = None

                # Loop lines ---
                for s_line in f.readlines():
                    s_line = s_line.strip()
                    if not s_line:
                        continue

                    # Detect new section
                    if s_line.startswith('[') and s_line.endswith(']'):
                        s_current_section = s_line[1:-1].lower()
                        if s_current_section == "end":
                            # End
                            return
                        else:
                            # Prepare Section data
                            self.__dic_data_sections[s_current_section] = {}
                            lst_headers = []
                        continue

                    # Just in case: No section -> next line
                    if s_current_section is None:
                        continue

                    # Special: Version line (has no headers)
                    if s_current_section == "version":
                        lst_val = tab_split(s_line)
                        self.__dic_data_sections[s_current_section] = lst_val
                        continue

                    # Header
                    if not lst_headers:
                        lst_headers = tab_split(s_line)
                        continue

                    # Data line
                    lst_line_values = tab_split(s_line)

                    # --- Station
                    if s_current_section == "station":
                        i_idx_key = 0 # BSRN Station ID
                        s_key = lst_line_values[i_idx_key]
                        lst_val = [v for i, v in enumerate(lst_line_values) if i != i_idx_key] # PANGAEA Event label, Full name, PANGAEA Institute ID
                        self.__dic_data_sections[s_current_section][s_key] = lst_val
                        continue

                    # --- Staff
                    if s_current_section == "staff":
                        s_key = lst_line_values[0] # Station Scientist
                        s_val = lst_line_values[1] # PANGAEA ID
                        self.__dic_data_sections[s_current_section][s_key] = [s_val]
                        continue

                    # --- Ozonesonde
                    if s_current_section == "ozonesonde":
                        i_idx_key = 0 # Manufacturer, Identification
                        s_key = lst_line_values[i_idx_key]
                        s_val = lst_line_values[1] # PANGAEA ID
                        self.__dic_data_sections[s_current_section][s_key] = [s_val]
                        continue

                    # --- Expanded
                    if s_current_section == "expanded":
                        i_idx_key = 0 # Manufacturer, Identification
                        s_key = lst_line_values[i_idx_key]
                        s_val = lst_line_values[1] # PANGAEA ID
                        self.__dic_data_sections[s_current_section][s_key] = [s_val]
                        continue

                    # --- Radiosonde
                    if s_current_section == "radiosonde":
                        i_idx_key = 0 # Manufacturer, Identification
                        s_key = lst_line_values[i_idx_key]
                        s_val = lst_line_values[1] # PANGAEA ID
                        self.__dic_data_sections[s_current_section][s_key] = [s_val]
                        continue

                    # --- Methods
                    if s_current_section == "methods":
                        i_idx_key = 0 # Station ID, Serial No., WRMC No.
                        s_key = lst_line_values[i_idx_key]
                        s_val = lst_line_values[1] # PANGAEA ID
                        self.__dic_data_sections[s_current_section][s_key] = [s_val]
                        # Special: WRMC to PANGAEA ID
                        try:
                            s_special_section = "methods_wrmc"
                            if s_special_section not in self.__dic_data_sections:
                                self.__dic_data_sections[s_special_section] = {}
                            lst_line_values_special = [x.strip() for x in re.split(r'[\t,]+', s_line)]
                            s_wrmc_id = lst_line_values_special[2]
                            s_wrmc_id = s_wrmc_id.replace("WRMC No. ","")
                            s_pg_id = lst_line_values_special[3]
                            self.__dic_data_sections[s_special_section][s_wrmc_id] = [s_pg_id]
                        except:
                            pass
                        continue
        except Exception as ex:
            self.__res.add_err(f"{ex}")


    # Init
    def initialize(self):

        pn = self.__print.normal
        pv = self.__print.verbose
        pv = self.__print.normal

        pv("* Init BSRN-Ids")

        # Data
        b_available_locally = False
        self.__res = Result(name="BSRN-IDs")

        # Check if ID file is locally available
        if Path(self.get_bsrn_id_file()).is_file():
            pv("File found")
            b_available_locally = True
        else:
            pv("File not found")

        # Download if not available
        if not b_available_locally:
            pv(f"Download ids from server: {self.__s_download_bsrn_ids}")
            try:
                # Download
                response = requests.get(self.__s_download_bsrn_ids, timeout=30)
                response.raise_for_status()  # exception if something went wrong
                with open(self.__s_path_lookup_data_bsrn_ids, "wb") as f:
                    f.write(response.content)
                pv(f"Downloaded to: {self.__s_path_lookup_data}")
                b_download_ok = True
            except requests.exceptions.RequestException as e:
                pv(f"Error while downloading: {e}")
                self.__res.add_err(f"error while downloading file from server: {e}")
            except OSError as e:
                pv(f"Error while writing to disk: {e}")
                self.__res.add_err(f"error while downloading file from server: {e}")
            except Exception as e:
                pv(f"Unexpected error: {e}")
                self.__res.add_err(f"unexpected error: {e}")

        if self.__res.is_err():
            return

        if not b_available_locally:
            # It did not work
            return

        # File locally available -> Evaluate
        pv(f"Processing file")
        self.eval_bsrn_file(self.__s_path_lookup_data_bsrn_ids)
        pv(self.info())
    # ---

    def is_working(self):
        return self.__res.is_ok()

    def is_error(self):
        return self.__res.is_err()

    def get_init_error(self):
        return self.__res.get_err_warn_info_string()

    # ---

    def get_bsrn_id_dir(self):
        return self.__s_path_lookup_data

    def get_bsrn_id_file_name(self):
        return self.__s_file_bsrn_ids

    def get_bsrn_id_file(self):
        return self.__s_path_lookup_data_bsrn_ids

    # ---

    # Get data
    def get_data(self, s_section, s_key, s_col="pangaea_id"):
        s_key = str(s_key)
        dic_meta= {
            "version": {"name": 0, "url": 1},
            "station": {"event": 0, "name": 1, "pangaea_id": 2},
            "staff": {"pangaea_id": 0},
            "expanded": {"pangaea_id": 0},
            "ozonesonde": {"pangaea_id": 0},
            "radiosonde": {"pangaea_id": 0},
            "methods": {"pangaea_id": 0},
            "methods_wrmc": {"pangaea_id": 0}
        }
        try:
            return self.__dic_data_sections[s_section][s_key][dic_meta[s_section][s_col]]
        except:
            return ""



