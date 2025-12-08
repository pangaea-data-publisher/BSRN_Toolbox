# coding=utf-8

"""
Converter
"""
import copy
import gzip, os, re
from collections import OrderedDict
from pathlib import Path

from logic.helper import PrettyText, FileTools
from logic.helper import Result
from logic.selection import Selection


class Converter:

    def __init__(self, model):

        self.__model = model
        self.__print = model.get_smart_printer()
        self.__lup = model.get_bsrn_id_system()

    # Convert
    def convert(self, odic_data, s_file, lst_recs=None, b_multi=False, s_path_multi_export=None):

        # --- Helper

        # Add error
        def add_err(s_rec, s_err):
            if s_rec not in odic_err:
                odic_err[s_rec_nr] = []
            odic_err[s_rec_nr].append(s_err)
            nonlocal  b_station_ok
            b_station_ok = False

        # Check if an record number must be exported
        def export_rec(rec_check):
            # All to list
            if isinstance(rec_check, str):
                rec_check = [rec_check]
            if b_station_ok:
                if (s_rec_nr in rec_check) and (s_rec_nr in lst_recs or len(lst_recs) == 0):
                    return True
            return False

        # Check input file name metadata from record 1 with lookup information
        def check_file_name(s_file, s_station_id, s_station_name, s_event, s_year, s_month):
            b_ok_check = True
            try:
                if None in [s_month, s_year, s_station_id, s_event, s_station_name]:
                    add_err("0001", f"Missing essential data: station-id[{s_station_id}] station_name[{s_station_name}] event_label[{s_event}] year[{s_year}] month[{s_month}]")
                    b_ok_check = False
                    return b_ok_check
                b_valid, s_file_event, i_file_month, i_file_year_long = Selection(self.__model).tool_check_filename(s_file)
                if str(s_event).lower() != str(s_file_event).lower():
                    add_err("0001", f"Diff (station_event): file[{s_file_event}] != data[{s_event}]")
                    b_ok_check = False
                if str(s_year) != str(i_file_year_long):
                    add_err("0001", f"Diff (year): file[{str(i_file_year_long)}] != data[{s_year}]")
                    b_ok_check = False
                if int(s_month) != i_file_month:
                    add_err("0001", f"Diff (month): file[{str(i_file_month)}] != data[{s_month}]")
                    b_ok_check = False
            except Exception as ex:
                add_err("0001", f"unexpected problem: {ex}")
                b_ok_check = False
            return b_ok_check

        # Write Export single for file for data
        def write_export(s_file_path, s_record, s_event, s_year, s_month, s_content, **kwargs):
            b_multi = True if (v := kwargs.get("multi")) is True else False
            s_multi_path_export = kwargs.get("multi_path_export")
            try:
                if not b_multi:
                    # Single
                    path_export_file = Path(s_file_path, f"{s_event.upper()}_{s_year}-{s_month}_{s_record}.txt")
                    FileTools.prepare_save_file(path_export_file)
                    with open(path_export_file, "w") as f:
                        f.write(str(s_content))
                else:
                    # Multi: Append
                    path_export_file = Path(s_multi_path_export, f"BSRN_LR{s_record}.txt")
                    if path_export_file.is_file(): #
                        s_content = s_content.partition("\n")[2] # remove entfernen
                    with open(path_export_file, "a") as f:
                        f.write(str(s_content))
                # Store number of files and lines of export
                nonlocal i_exported_files
                i_exported_files += 1
                nonlocal i_exported_lines
                i_exported_lines += len(s_content)
            except Exception as ex:
                add_err(s_record, f"problem saving single export: {ex}")

        # Create Date Time String
        def create_date_time(s_year, s_month, s_day, s_min):
            if (-1 in [s_year, s_month, s_day, s_min]) or ("-1" in [s_year, s_month, s_day, s_min]):
                return ""
            try:
                i_day = int(s_day)
                s_day = f"{i_day:02d}"
            except:
                pass
            try:
                i_hour = int(s_min) // 60
                i_min = int(s_min) % 60
                s_min_hour = f"{i_hour:02d}:{i_min:02d}"
            except:
                s_min_hour = f"00:{s_min}"
            s_date_time = f"{s_year}-{s_month}-{s_day}T{s_min_hour}"
            return s_date_time

        # Create date-time stamp
        def create_date_time_2(s_year, s_month, s_day, s_hour, s_min):
            if (-1 in [s_day, s_hour, s_min]) or ("-1" in [s_day, s_hour, s_min]):
                return ""
            try:
                i_day = int(s_day)
                s_day = f"{i_day:02d}"
            except:
                pass
            try:
                i_hour = int(s_hour)
                i_min = int(s_min)
                s_min_hour = f"{i_hour:02d}:{i_min:02d}"
            except:
                s_min_hour = f"{s_hour}:{s_min}"
            s_date_time = f"{s_year}-{s_month}-{s_day}T{s_min_hour}"
            return s_date_time

        # Create content: meta data
        def create_content_meta(odic_data_tmp, **kwargs):

            # --- Helper
            # Use this column in record?
            def use_col(s_col):
                if s_rec_nr == "0001":
                    if s_col in ["station_id", "month", "year", "version"]:
                        return False
                    else:
                        return True
                elif s_rec_nr == "0003":
                    if s_col in ["Surface type", "Typography type"]:
                        return True
                    else:
                        return False
                elif s_rec_nr == "0005":
                    if s_col in ["Date when change occurred (day)", "Date when change occurred (hour)", "Date when change occurred (min)", "Is radiosonde operating", "Identification of radiosonde", "Remarks about radiosonde"]:
                        return False
                    else:
                        return True
                elif s_rec_nr == "0006":
                    if s_col in [ "Date when change occurred (day)",
                            "Date when change occurred (hour)",
                            "Date when change occurred (min)",
                            "Are ozone measurements operated"]:
                        return False
                    else:
                        return True
                elif s_rec_nr == "0007":
                    if s_col in [ "f1", "f2", "f3", "f4", "f5", "f6"]:
                        return False
                    else:
                        return True
                else:
                    return True

            # --- Head (Base)
            lst_head = ["File name","Station ID","Event label","Station","YYYY-MM"]

            # --- Content (Base)
            lst_row = [s_file_name,s_station_id,s_station_event,s_station_name, f"{s_station_year}-{s_station_month}"]

            # --- Rest (base)
            odic_meta = odic_data_rec_data.get("base")
            if odic_meta is not None:
                for s_col, s_val in odic_meta.items():
                    if use_col(s_col):
                        lst_head.append(s_col)
                        lst_row.append(s_val)

            # --- Special
            # rec: 0001 (Sequence Parameter)
            if s_rec_nr == "0001":
                b_first = True
                odic_seq_data = (odic_data_rec_data.get("data"))
                if odic_seq_data is not None:
                    for i_idx, odic_seq_data in odic_seq_data.items():
                        for s_key, s_val in odic_seq_data.items():
                            if b_first:
                                lst_head.append("Parameter")
                            else:
                                lst_head.append("")
                            lst_row.append(s_val)
                            b_first = False
            # rec: 0006 (Add Pangaea ID)
            if s_rec_nr == "0006":
                try:
                    s_id_ozone_instrument = odic_data_rec_data["base"].get("Identification number of ozone instrument", "")
                    s_pangaea_method_id = self.__lup.get_data("ozonesonde", s_id_ozone_instrument, "pangaea_id")
                    lst_head.append("PANGAEA Method ID")
                    lst_row.append(s_pangaea_method_id)
                except:
                    pass
            # rec: 0007 (Add SYNOP Flags)
            if s_rec_nr == "0007":
                try:
                    s_synop_flag1 = odic_data_rec_data["base"].get("f1", "")
                    s_synop_flag2 = odic_data_rec_data["base"].get("f2", "")
                    s_synop_flag3 = odic_data_rec_data["base"].get("f3", "")
                    s_synop_flag4 = odic_data_rec_data["base"].get("f4", "")
                    s_synop_flag5 = odic_data_rec_data["base"].get("f5", "")
                    s_synop_flag6 = odic_data_rec_data["base"].get("f6", "")
                    s_synop_flags = f"{s_synop_flag1} {s_synop_flag2} {s_synop_flag3} {s_synop_flag4} {s_synop_flag5} {s_synop_flag6} "
                    lst_head.append("SYNOP flags")
                    lst_row.append(s_synop_flags)
                except:
                    pass

            # --- Create Content
            s_content = ""
            s_content += PrettyText.lst2str(lst_head, "\t") + "\n"
            s_content += PrettyText.lst2str(lst_row, "\t") + "\n"

            # --- Special
            # rec 0003: Messages
            if s_rec_nr == "0003":
                odic_seq_data = (odic_data_rec_data.get("data"))
                if odic_seq_data is not None:
                    for i_idx, odic_seq_data in odic_seq_data.items():
                        for s_key, s_val in odic_seq_data.items():
                            s_content += f"{s_val}\n"
            return s_content

        # Create content: data
        def create_content_data(odic_data_tmp, **kwargs):

            if odic_data_tmp is None:
                return ""

            # Kwargs
            b_shrink = v if (v := kwargs.get("shrink", False)) is True else False # Remove empty colums

            # Data
            lst_content = []
            lst_idx_col_full = {}
            b_head = True

            # Loop rows
            for i_row, odic_row in odic_data_tmp.items():

                # Get day and min -> create date-time
                if s_rec_nr not in  ["1000"]: # not in rec 1000
                    s_date_time = ""
                    try:
                        s_day = odic_row.get("date_day", "")
                        s_min = odic_row.get("time_min", "")
                        s_date_time = create_date_time(s_station_year, s_station_month, s_day, s_min)
                    except:
                        pass
                    # Remove day and min
                    try:
                        odic_row.pop("date_day")
                        odic_row.pop("time_min")
                    except:
                        pass
                    # Add Date Time in front
                    odic_row["Date/Time"] = s_date_time
                    odic_row.move_to_end('Date/Time', last=False)

                # Content
                # Header
                if s_rec_nr in ["1000"]:  # no header in rec 1000
                    b_head = False
                    lst_idx_col_full = [False]
                if b_head:
                    lst_head = odic_row.keys()
                    lst_idx_col_full = [False] * len(lst_head)
                    lst_content.append(lst_head)
                    b_head = False
                # Values
                lst_val = odic_row.values()
                lst_content.append(lst_val)
                # Check for headers with content
                for i_idx, (k, v) in enumerate(odic_row.items()):
                    if str(v).strip() not in ["", None, "None"]:
                        lst_idx_col_full[i_idx] = True

            # Create Content
            s_content = ""
            for lst_row in lst_content:
                if b_shrink:
                    lst_row = [val for val, flag in zip(lst_row, lst_idx_col_full) if flag]
                s_content += PrettyText.lst2str(lst_row, "\t")
                s_content += "\n"

            return s_content

        # Check merge recs 0100 and 300
        def check_merge_100_300():
            if ("0100" in lst_recs and "0300" in lst_recs) and (s_rec_nr in ["0100", "0300"]):
                nonlocal odic_merge_100_and_300_part_100
                nonlocal odic_merge_100_and_300_part_300
                try:
                    if s_rec_nr == "0100":
                        # Catch: 0100
                        odic_merge_100_and_300_part_100 = copy.deepcopy(odic_data_rec_data.get("data"))  # Deep copy
                    elif s_rec_nr == "0300":
                        # Catch: 0300
                        odic_merge_100_and_300_part_300 = copy.deepcopy(odic_data_rec_data.get("data"))  # Deep copy
                    if odic_merge_100_and_300_part_100 and odic_merge_100_and_300_part_300:
                        # All data gatherd -> Merge 0100 + 0300
                        odic_merge_100_and_300 = OrderedDict()
                        # Merge per date time
                        lst_ocics_to_merge = [odic_merge_100_and_300_part_100, odic_merge_100_and_300_part_300]
                        for odic_to_merge in lst_ocics_to_merge:
                            for i, row in odic_to_merge.items():
                                s_day = row.get("date_day", "")
                                s_min = row.get("time_min", "")
                                s_date_time = create_date_time(s_station_year, s_station_month, s_day, s_min)
                                if s_min in ["", None]:
                                    add_err("0001_0003",
                                            f"Problem merging records 0100 and 0300: timestamp: {s_date_time}]")
                                if s_date_time not in odic_merge_100_and_300:
                                    odic_merge_100_and_300[s_date_time] = OrderedDict()
                                for k, v in row.items():
                                    odic_merge_100_and_300[s_date_time][k] = v
                                odic_merge_100_and_300[s_date_time] = row
                                if s_date_time not in odic_merge_100_and_300:
                                    odic_merge_100_and_300[s_date_time] = OrderedDict()
                        # Remove only values an enumerate
                        odic_merge_100_and_300_final = OrderedDict()
                        for i_idx, (s_date, odic_row) in enumerate(odic_merge_100_and_300.items()):
                            odic_merge_100_and_300_final[i_idx] = odic_row
                        # Create Content
                        s_content = create_content_data(odic_merge_100_and_300_final, shrink=b_shrink)
                        # Write Export file (always store in a separate file)
                        write_export(s_file_path, "0100_0300", s_station_event, s_station_year, s_station_month, s_content,
                                     multi=False, multi_path_export=s_path_multi_export)
                except Exception as ex:
                    add_err("0001_0003", f"Problem merging records: {ex}]")

        # ---

        pn = self.__print.normal
        pv = self.__print.verbose
        pd = self.__print.debug

        # Parameter
        b_shrink = True # Remove empty columns

        # Overview
        odic_err = OrderedDict()
        i_exported_lines = 0
        i_exported_files = 0

        # Evaluate file in
        s_file_path = str(Path(s_file).parent)
        s_file_name = str(Path(s_file).name)
        s_file_base = s_file_name[0:7]

        # General Metadata for this file
        b_station_ok = True
        s_station_name = None
        s_station_id = None
        s_station_event = None
        s_station_month = None
        s_station_year = None
        s_lon = None
        s_lat = None

        # Special: Merge 0100 and 0300
        odic_merge_100_and_300_part_100 = OrderedDict()
        odic_merge_100_and_300_part_300 = OrderedDict()

        # ---

        # Export data
        for s_data_rec_name, odic_data_rec_data in odic_data.items():
            s_rec_nr = s_data_rec_name[1:5]

            # --- Extract general data for the whole file

            if s_rec_nr == "0001":
                # --- 0001: month,year,station id, station name, event label
                # from record
                s_station_month = odic_data_rec_data["base"].get("month","")
                try:
                    s_station_month = f"{int(s_station_month):02}"
                except:
                    pass
                s_station_year = odic_data_rec_data["base"].get("year","")
                s_station_id = odic_data_rec_data["base"].get("station_id","")
                # from BSRN Id system
                s_station_name = self.__lup.get_data("station", s_station_id, "name")
                s_station_event = self.__lup.get_data("station", s_station_id, "event")
                # check extracted data against filename
                b_station_ok = check_file_name(s_file_name, s_station_id, s_station_name, s_station_event,s_station_year, s_station_month)

            if s_rec_nr == "0004":
                # --- 0004: long/lat
                # from record
                s_lat = odic_data_rec_data["base"].get("lat", "")
                s_lon = odic_data_rec_data["base"].get("lon", "")
                if s_lat in ["", None]:
                    add_err(s_rec_nr, "lat: not given")
                if s_lon in ["", None]:
                    add_err(s_rec_nr, "lon: not given")

            # --- Meta Data
            if export_rec("0001"):
                # --- rec 0001
                # Create Content
                s_content = create_content_meta(odic_data_rec_data)
                # Export file
                write_export(s_file_path, s_rec_nr, s_station_event, s_station_year, s_station_month, s_content, multi=b_multi, multi_path_export=s_path_multi_export)
            elif export_rec("0002"):
                # --- rec 0002
                # ... get data
                s_scientist_name = odic_data_rec_data["base"].get("station scientist name", "")
                s_scientist_phone = odic_data_rec_data["base"].get("station scientist phone", "")
                s_scientist_fax = odic_data_rec_data["base"].get("station scientist fax", "")
                s_scientist_tcpip = odic_data_rec_data["base"].get("station scientist tcp/ip", "")
                s_scientist_email = odic_data_rec_data["base"].get("station scientist email", "")
                s_scientist_address = odic_data_rec_data["base"].get("station scientist address", "")
                s_scientist_pangaea_id = self.__lup.get_data("staff", s_scientist_name, "pangaea_id")
                s_deputy_name = odic_data_rec_data["base"].get("deputy name", "")
                s_deputy_phone = odic_data_rec_data["base"].get("deputy phone", "")
                s_deputy_fax = odic_data_rec_data["base"].get("deputy fax", "")
                s_deputy_tcpip = odic_data_rec_data["base"].get("deputy tcp/ip", "")
                s_deputy_email = odic_data_rec_data["base"].get("deputy email", "")
                s_deputy_address = odic_data_rec_data["base"].get("deputy address", "")
                s_deputy_pangaea_id = self.__lup.get_data("staff", s_deputy_name, "pangaea_id")
                # ... Head
                s_content = ""
                lst_head = ["File name", "Station ID", "Event label", "Station", "YYYY-MM", "Position", "Scientist", "Telephon", "Fax", "TCP/IP", "e-mail", "Address", "PANGAEA staff ID"]
                s_content += PrettyText.lst2str(lst_head, "\t")
                s_content += "\n"
                # ... Scientist
                lst_row = [s_file_name, s_station_id, s_station_event, s_station_name, f"{s_station_year}-{s_station_month}", 1, s_scientist_name, s_scientist_phone, s_scientist_fax, s_scientist_tcpip, s_scientist_email, s_scientist_address, s_scientist_pangaea_id]
                s_content += PrettyText.lst2str(lst_row, "\t")
                s_content += "\n"
                # ... Deputy
                lst_row = [s_file_name, s_station_id, s_station_event, s_station_name,f"{s_station_year}-{s_station_month}", 2, s_deputy_name, s_deputy_phone, s_deputy_fax, s_deputy_tcpip, s_deputy_email, s_deputy_address, s_deputy_pangaea_id]
                s_content += PrettyText.lst2str(lst_row, "\t")
                s_content += "\n"
                # Export file
                write_export(s_file_path, s_rec_nr, s_station_event, s_station_year, s_station_month, s_content, multi=b_multi, multi_path_export=s_path_multi_export)
            elif export_rec("0003"):
                # --- rec 0003
                s_content = create_content_meta(odic_data_rec_data)
                # Export file
                write_export(s_file_path, s_rec_nr, s_station_event, s_station_year, s_station_month, s_content, multi=b_multi, multi_path_export=s_path_multi_export)
            if export_rec("0004"):
                # --- rec 0004
                # Create Content
                s_content = create_content_meta(odic_data_rec_data)
                # Export file
                write_export(s_file_path, s_rec_nr, s_station_event, s_station_year, s_station_month, s_content, multi=b_multi, multi_path_export=s_path_multi_export)
            if export_rec("0005"):
                # --- rec 0005
                # Create Content
                s_content = create_content_meta(odic_data_rec_data)
                # Export file
                write_export(s_file_path, s_rec_nr, s_station_event, s_station_year, s_station_month, s_content, multi=b_multi, multi_path_export=s_path_multi_export)
            if export_rec("0006"):
                # --- rec 0006
                # Create Content
                s_content = create_content_meta(odic_data_rec_data)
                # Export file
                write_export(s_file_path, s_rec_nr, s_station_event, s_station_year, s_station_month, s_content, multi=b_multi, multi_path_export=s_path_multi_export)
            if export_rec("0007"):
                # --- rec 0007
                # Create Content
                s_content = create_content_meta(odic_data_rec_data)
                # Export file
                write_export(s_file_path, s_rec_nr, s_station_event, s_station_year, s_station_month, s_content, multi=b_multi, multi_path_export=s_path_multi_export)
            if export_rec("0008"):
                # --- rec 0008
                # Get data
                # ... Head
                s_content = ""
                lst_head = ["File name", "Station ID", "Event label", "Station", "YYYY-MM", "PANGAEA method", "PANGAEA method ID"]
                s_content += PrettyText.lst2str(lst_head, "\t")
                s_content += "\n"
                # ... Row
                odic_seq_data = odic_data_rec_data.get("data")
                for i_idx, odic_seq_data in odic_seq_data.items():
                    s_method_manufacturer = odic_seq_data.get("Manufacturer", "")
                    s_method_model = odic_seq_data.get("Model", "")
                    s_method_serial = odic_seq_data.get("Serial number", "")
                    s_method_wrmc_id = odic_seq_data.get("Identification number assigned by the WRMC", "")
                    s_method_pangaea_name = f"{s_method_manufacturer}, {s_method_model}, SN {s_method_serial}, WRMC No. {s_method_wrmc_id}"
                    s_method_pangaea_id = self.__lup.get_data("methods_wrmc", s_method_wrmc_id, "pangaea_id")
                    lst_row = [s_file_name, s_station_id, s_station_event, s_station_name,f"{s_station_year}-{s_station_month}", s_method_pangaea_name, s_method_pangaea_id]
                    s_content += PrettyText.lst2str(lst_row, "\t")
                    s_content += "\n"
                # Export file
                write_export(s_file_path, s_rec_nr, s_station_event, s_station_year, s_station_month, s_content,multi=b_multi, multi_path_export=s_path_multi_export)
            if export_rec("0009"):
                # --- rec 0009
                # ... Head
                s_content = ""
                lst_head = ["File name", "Station ID", "Event label", "Station", "YYYY-MM", "Date/Time", "Parameter", "WRMC ID of instrument", "PANGAEA method ID"]
                s_content += PrettyText.lst2str(lst_head, "\t")
                s_content += "\n"
                # ... Row
                odic_seq_data = odic_data_rec_data.get("data")
                for i_idx, odic_seq_data in odic_seq_data.items():
                    s_day = odic_seq_data.get("Date when change occurred (day)", "")
                    s_hour = odic_seq_data.get("Date when change occurred (hour)", "")
                    s_min = odic_seq_data.get("Date when change occurred (min)", "")
                    s_date_time = create_date_time_2(s_station_year, s_station_month, s_day, s_hour, s_min)
                    s_instrument_id = odic_seq_data.get("Id. no. of instrument which measured quantity", "")
                    s_instrument_pangaea_id = self.__lup.get_data("methods_wrmc", s_instrument_id, "pangaea_id")
                    s_parameter = odic_seq_data.get("Id. no. of radiation quantity measured", "")
                    lst_row = [s_file_name, s_station_id, s_station_event, s_station_name, f"{s_station_year}-{s_station_month}", s_date_time, s_parameter, s_instrument_id, s_instrument_pangaea_id]
                    s_content += PrettyText.lst2str(lst_row, "\t")
                    s_content += "\n"
                # Export file
                write_export(s_file_path, s_rec_nr, s_station_event, s_station_year, s_station_month, s_content, multi=b_multi, multi_path_export=s_path_multi_export)

            # --- Data
            elif export_rec(self.__model.get_data_recs_str()):

                # --- Special: Additional merge of 0100 and 0300 (if available)
                check_merge_100_300()

                # --- All data records
                # Get Data
                odic_data_tmp = odic_data_rec_data.get("data")
                # Create Content
                s_content = create_content_data(odic_data_tmp, shrink=b_shrink)
                # Write Export file (always store in a separate file)
                write_export(s_file_path, s_rec_nr, s_station_event, s_station_year, s_station_month,
                             s_content, multi=False, multi_path_export=s_path_multi_export)
        if odic_err:
            b_station_ok = False
        return b_station_ok, i_exported_files, i_exported_lines, odic_err
