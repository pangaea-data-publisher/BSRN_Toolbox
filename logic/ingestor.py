# coding=utf-8

"""
Ingestor
"""

import gzip, os, re
from collections import OrderedDict
from logic.helper import PrettyText
from logic.helper import Result

class Ingestor:

    def __init__(self, model):

        self.__model = model
        self.__print = model.get_smart_printer()
        self.__odic_data = OrderedDict

    def ingest(self, s_filepath):

        pn = self.__print.normal
        pv = self.__print.verbose
        pd = self.__print.debug

        res = Result(name="ingest")

        # Data

        b_err_tec = False # Technical problems occured while reading data
        s_err_tec = None

        self.__odic_data = OrderedDict()  # data structure (is needed for making qc-reports and if no import is desired it will be deleted later)

        set_check_uniq_radiation_quantitiy_instrument = set()  # Uniq test U009: date-instrument
        set_check_block_already_reported = set()  # Test: Block was already reported - the report will not bo anoying long

        tp_report_block_entry_open = None  # Open block: last entry
        tp_report_seq_entry_open = None  # Open sequence: last entry
        lst_report_content_imported = [["Record", "Name", "Content"]]  # report
        lst_report_error_general = [["Rec", "Rec.info", "Name", "Value", "Info"]]  # errors generic
        lst_report_error_eval_line = [["Rec", "Rec.info", "Name", "Line", "Value", "Pattern", "info"]]  # error while evaling a line
        lst_report_warn_general = [["Rec", "Rec.info", "Name", "Value", "info"]]  # warnings generic
        lst_report_info_eval_line_val_empty = [["Rec", "Rec.info", "Name"]]  # info that a value is empty
        set_report_imported_rec_meta = set()
        set_report_imported_rec_data = set()

        i_import_lines = 0 # imported lines
        i_import_vals = 0 # imported values

        i_row_len_max = 80 # Max line length

        dic_overview_errs_msg_long = {}
        dic_overview_errs_msg_short = {}
        dic_overview_errs_recs = {}

        dic_report_unknown_recs = {}  # unknown records

        b_report_add_info_illegal_chars = False
        s_report_add_info_illegal_chars = \
"""
Allowed ASCII characters:
- in logical records 1000 and less than 100
all printable characters from 'space' to '~',
and in addition, for logical record 3 also
'tabulator' (09 hex) is allowed.
- in all other logical records: 
'space','+','-','.' and digits from '0' to '9'.
"""

        # --- Helper

        # Add Err to Overview dict
        def add_err_2_overview(s_name, s_name_short, s_rec):
            if s_name not in dic_overview_errs_msg_long:
                dic_overview_errs_msg_long[s_name] = 1
            else:
                dic_overview_errs_msg_long[s_name] += 1
            if s_name_short not in dic_overview_errs_msg_short:
                dic_overview_errs_msg_short[s_name_short] = 1
            else:
                dic_overview_errs_msg_short[s_name_short] += 1
            if s_rec not in dic_overview_errs_recs:
                dic_overview_errs_recs[s_rec] = 1
            else:
                dic_overview_errs_recs[s_rec] += 1

        # Check allowed content
        def check_allowed_content(content_work, lst_contstraint_missing_codes, lst_contstraint_allowed_range, lst_contstraint_allowed_vals):
            # Check for illegal chars
            for i, c_char in enumerate(str(content_work)):
                if i_rec_num > 99 and i_rec_num != 1000:
                    b_found = not (c_char in ' +-.' or c_char.isdigit() or c_char == '\n')
                else:
                    b_found = not ((' ' <= c_char <= '~') or (
                            i_rec_num == 3 and c_char == '\t') or c_char == '\n')
                if b_found:
                    # illegal char found
                    nonlocal b_report_add_info_illegal_chars
                    b_report_add_info_illegal_chars = True
                    raise Exception(f"Illegal char found on position {i}: [{c_char}]")
                    add_err_2_overview("Illegal char", "char", s_rec_name)
            # str -> lower
            if isinstance(content_work, str):
                content_work = content_work.lower()
            # Check allowed vals
            if content_work not in lst_contstraint_missing_codes:
                # Not in missing code
                if lst_contstraint_allowed_range:
                    if isinstance(content_work, int) or isinstance(content_work, float):
                    # Allowed range is given -> test
                        i_min = int(lst_contstraint_allowed_range[0])
                        i_max = (lst_contstraint_allowed_range[1])
                        if not (i_min <= content_work <= i_max):
                            add_err_2_overview("Value out of range", "range", s_rec_name)
                            raise Exception(f"value out of range [allowed: {i_min}-{i_max}]: {content_work}")
                elif lst_contstraint_allowed_vals:
                    # Allowed values given -> test
                    if content_work not in lst_contstraint_allowed_vals:
                        add_err_2_overview("Value not allowed", "val", s_rec_name)
                        raise Exception(f"value not valid [allowed: {PrettyText.lst2strC(lst_contstraint_allowed_vals)}]: {content_work}")

        """
        Extract data from row using a given pattern
        
        Pattern Nomenclature
            Based on:
                Baseline Surface Radiation Network (BSRN) Update of the Technical Plan for BSRN Data Management
            Syntax:
                Datatypes: I,F,A (int, float, string)
                Datatypes with length: I5 (int with 5 digits)
                Multiplikator: 3[...]: multiplikator of content between the [] brackets; no nesting allowed
                Allowed ranges: (1-100) or specific vakues (y;n)
                Allowed empty data entries: (!999)
                Separator: X  
            Example:
                3[X,I2(0-59!-1)],F5(0-255,!-99.9),X,A1(Y;N)
        """
        def extract_data_from_row(s_row, lst_col_names, s_pattern):

            lst_return = []
            if s_row.endswith("\n"):
                s_row = s_row[:-1] # remove return at the end - just for better debugging
            try:
                # Some cleaning
                s_pattern = s_pattern.lower()
                s_pattern = s_pattern.replace(" ","")

                # Expand n(abc) -> abc,abc,abc
                re_pattern = re.compile(r'(\d+)\[([^\[\]]+)\]')
                s_pattern_expanded = s_pattern
                while True:
                    match = re_pattern.search(s_pattern_expanded)
                    if not match:
                        break
                    count = int(match.group(1))
                    content = match.group(2)
                    content_parts = content.split(",")
                    expanded = ",".join(content_parts * count)
                    s_pattern_expanded = s_pattern_expanded[:match.start()] + expanded + s_pattern_expanded[match.end():]
                if s_pattern_expanded != s_pattern:
                    s_pattern = s_pattern_expanded
                # Split pattern
                lst_pattern_parts = s_pattern.split(",")
                i_digit_nr_now = 0
                # Internal Check: is pattern matching contentent parts
                lst_pattern_without_x = [item for item in lst_pattern_parts if item[0] != "x"]
                if len(lst_pattern_without_x) != len(lst_col_names):
                    add_err_2_overview("Pattern length != content length", "pattern", s_rec_name)
                    raise Exception(
                        f"Internal error: pattern length [{PrettyText.lst2strC(lst_pattern_without_x)}] != content length [{PrettyText.lst2strC(lst_col_names)}]")
                # Iterator for col names
                it_col_names = iter(lst_col_names)

                # Loop pattern parts
                for i_idx, s_pattern_part in enumerate(lst_pattern_parts):
                    s_pattern_part_start = s_pattern_part[0]
                    if s_pattern_part_start == "x":
                        # --- X (Separator: Tab)
                        i_digit_nr_now += 1
                    elif s_pattern_part_start in ["a", "i", "f"]:
                        # --- Pattern part with definition
                        # Get corresponding col name
                        s_col_name = next(it_col_names, None) # found a col and get next col name (cause it starts with none)
                        # Extract Contraints from pattern part
                        s_constraints = lst[0] if len(lst := re.findall(r"\((.*?)\)", s_pattern_part)) > 0 else None
                        lst_contstraint_allowed_vals = []
                        lst_contstraint_allowed_range = []
                        lst_contstraint_missing_codes = []
                        s_pattern_part = re.sub(r"\(.*?\)", "", s_pattern_part) # clean pattern part
                        if s_constraints is not None:
                            s_constraint_allowed_vals , _ , s_constraint_missing = s_constraints.partition("!")
                            # Allowed values
                            if s_constraint_allowed_vals != "":
                                if "-" in s_constraint_allowed_vals:
                                    # Range
                                    lst_contstraint_allowed_range = s_constraint_allowed_vals.split("-")
                                if ";" in s_constraint_allowed_vals:
                                    # Single values
                                        lst_contstraint_allowed_vals = s_constraint_allowed_vals.split(";")
                                # Some checks
                                if lst_contstraint_allowed_vals and lst_contstraint_allowed_range:
                                    add_err_2_overview("Range and single values simultaneously set", "def_constr", s_rec_name)
                                    raise Exception(f"Internal error: pattern: constraint: range and single values simultaneously set: {s_constraint_allowed_vals}")
                                if len(lst_contstraint_allowed_range) > 2:
                                    add_err_2_overview("Range has more than two parts", "def_constr", s_rec_name)
                                    raise Exception(f"Internal error: pattern: constraint: range has more than two parts: {s_constraint_allowed_vals}")
                            # Missing codes
                            if s_constraint_missing != "":
                                lst_contstraint_missing_codes = s_constraint_missing.split(";")
                            # Convert the list elements
                            if s_pattern_part_start == "i":
                                # int
                                lst_contstraint_missing_codes = [int(val) for val in lst_contstraint_missing_codes]
                                lst_contstraint_allowed_range = [int(val) for val in lst_contstraint_allowed_range]
                                lst_contstraint_allowed_vals = [int(val) for val in lst_contstraint_allowed_vals]
                            elif s_pattern_part_start == "f":
                                # float
                                lst_contstraint_missing_codes = [float(val) for val in lst_contstraint_missing_codes]
                                lst_contstraint_allowed_range = [float(val) for val in lst_contstraint_allowed_range]
                                lst_contstraint_allowed_vals = [float(val) for val in lst_contstraint_allowed_vals]
                            else:
                                # str: all to lower
                                lst_contstraint_missing_codes = [str(val).lower() for val in lst_contstraint_missing_codes]  # -> lower
                                lst_contstraint_allowed_vals = [str(val).lower() for val in lst_contstraint_allowed_vals]  # -> lower

                        # Get number of digits number from Pattern
                        i_digits = int(s_pattern_part[1:])
                        # Calc Pointer to content (digit start/end)
                        i_digit_nr_start = i_digit_nr_now
                        i_digit_nr_end = i_digit_nr_start + i_digits
                        i_digit_nr_now += i_digits
                        # Get content part
                        s_conversion_type = ""
                        try:
                            content = s_row[i_digit_nr_start:i_digit_nr_end].strip()
                        except Exception as ex:
                            raise Exception(
                                f"Internal error: could not extract content part from row: start={i_digit_nr_start} to end={i_digit_nr_end}) from row=[{s_row}]")
                        # Start conversion
                        try:
                            if s_pattern_part_start == "a":
                                s_conversion_type = "str"
                                content = str(content)
                                if lst_contstraint_allowed_range:
                                    add_err_2_overview("Range not allowed with string", "def_constr", s_rec_name)
                                    raise Exception(f"Internal error: pattern: constraint: range not allowed with strings: {lst_contstraint_allowed_range}")
                                check_allowed_content(content, lst_contstraint_missing_codes, lst_contstraint_allowed_range, lst_contstraint_allowed_vals)
                                if content.lower() in lst_contstraint_missing_codes: # remove missing codes
                                    content = ""
                            elif s_pattern_part_start == "i":
                                s_conversion_type = "int"
                                content = int(content)
                                check_allowed_content(content, lst_contstraint_missing_codes, lst_contstraint_allowed_range, lst_contstraint_allowed_vals)
                                if content in lst_contstraint_missing_codes:
                                    content = ""
                            elif s_pattern_part_start == "f":
                                s_conversion_type = "float"
                                content = float(content)
                                check_allowed_content(content, lst_contstraint_missing_codes, lst_contstraint_allowed_range, lst_contstraint_allowed_vals)
                                if content in lst_contstraint_missing_codes:
                                    content = ""
                            # Save content
                            lst_return.append(content)
                        except Exception as ex:
                            # Conversion problem
                            add_err_2_overview(f"Conversion to {s_conversion_type}", f"conv_{s_conversion_type}", s_rec_name)
                            s_err_msg = f"Problem while conversion to {s_conversion_type}: {ex}"
                            lst_report_error_eval_line.append([s_rec_name, s_rec_name_add_info, s_col_name, f"{i_file_line+1}", content, s_pattern_part, s_err_msg])
            except Exception as ex:
                # Other problem while evaluation
                add_err_2_overview(f"Other conversion problem", "conv", s_rec_name)
                lst_report_error_eval_line.append([s_rec_name, s_rec_name_add_info, "-", "-", "-", "-", str(ex)])
            return lst_return

        # Helper: ingest line
        def ingest_row(lst_col_names, s_pattern, **kwargs):

            # Parameter
            s_seq_name = kwargs.get("seq_name")
            i_seq_number = kwargs.get("seq_nr")
            b_seq = True if s_seq_name is not None else False
            b_seq_start = False
            i_seq_start_line = 0

            # Extract line
            lst_return = extract_data_from_row(s_row, lst_col_names, s_pattern)  # Report line
            odic_data_line = OrderedDict()  # Data line

            # Increase import counter
            nonlocal i_import_lines, i_import_vals
            i_import_lines += 1
            i_import_vals += len(lst_return)

            # Prepare where to store the data
            s_key_top = f"{s_rec_name})"
            if not b_seq:
                # base data
                s_name_base_data = "base"
                if s_key_top not in self.__odic_data:
                    self.__odic_data[s_key_top] = OrderedDict()
                if "base" not in self.__odic_data[s_key_top]:
                    self.__odic_data[s_key_top][s_name_base_data] = OrderedDict()
                s_odic_store = self.__odic_data[s_key_top][s_name_base_data]  # Store here
            else:
                # Sequence data/meta data
                s_key_seq_name = s_seq_name
                s_key_seq_name_data_id = i_seq_number
                if s_key_top not in self.__odic_data:
                    self.__odic_data[s_key_top] = OrderedDict()
                if s_key_seq_name not in self.__odic_data[s_key_top]:
                    self.__odic_data[s_key_top][s_key_seq_name] = OrderedDict()
                if s_key_seq_name_data_id not in self.__odic_data[s_key_top][s_key_seq_name]:
                    self.__odic_data[s_key_top][s_key_seq_name][s_key_seq_name_data_id] = OrderedDict()
                s_odic_store = self.__odic_data[s_key_top][s_key_seq_name][s_key_seq_name_data_id]  # Store here
            # Store to data structure
            for i, s_name in enumerate(lst_col_names):
                # Error: Name exists already in odic store data
                if s_name in s_odic_store:
                    s_name = f"{s_name} (double!)"
                    lst_report_error_eval_line.append([f"{s_rec_name}.{i_rec_line}", s_rec_name_add_info, s_name, i_file_line+1, "-" , "found double name"])
                    add_err_2_overview(f"Store to data structure: double names found", "store_dbl", s_rec_name)
                # Get value and store
                try:
                    value = lst_return[i]
                except:
                    value = None
                s_odic_store[s_name] = value
                # Warning: empty value (deactivated)
                if value == "":
                    pass
                    # lst_report_info_eval_line_val_empty.append([f"{s_rec_name}.{i_rec_line}", s_rec_name_add_info, s_name])

            # Check for open actual block/seq -> report it and close
            if not self.__model.is_debug():
                nonlocal tp_report_block_entry_open
                # Check if record changed and a block is open
                if tp_report_block_entry_open is not None:
                    if tp_report_block_entry_open[0] != s_rec_name: # Next record
                        lst_report_content_imported.append(tp_report_block_entry_open[1])
                        tp_report_block_entry_open = None # Reset
                # Check if sequence changed and a old sequence is open
                nonlocal tp_report_seq_entry_open
                if tp_report_seq_entry_open is not None:
                    if tp_report_seq_entry_open[0] != s_rec_name or tp_report_seq_entry_open[1] != s_rec_name_add_info: # Next Record or next Sequence in same record
                        lst_report_content_imported.append(tp_report_seq_entry_open[2])
                        tp_report_seq_entry_open = None # Reset

            # Build report
            lst_report_content_imported.append([f"{s_rec_name}.{i_rec_line}", s_rec_name_add_info, PrettyText.dic2str(odic_data_line)])
            if i_rec_num in self.__model.get_metadata_recs_int():
                set_report_imported_rec_meta.add(i_rec_num)
            elif i_rec_num in self.__model.get_data_recs_int():
                set_report_imported_rec_data.add(i_rec_num)

        # Helper: Check max line length
        def check_line_length_max(s_row):
            s_line_no_white_spaces = s_row.rstrip('\n\r')
            i_line_len = len(s_line_no_white_spaces)
            s_line_short_info = f"{s_line_no_white_spaces[0:10]}...{s_line_no_white_spaces[-10:]}"
            s_err_msg = f"Line longer than {i_row_len_max} chars or missing line separator: {i_line_len} chars"
            if i_line_len > 80:
                add_err_2_overview(f"Line longer than {i_row_len_max} chars", "line>80", s_rec_name)
                lst_report_error_eval_line.append(
                    [f"{s_rec_name}", s_rec_name_add_info, f"whole line", f"{i_file_line + 1}", s_line_short_info, "-", s_err_msg])

        # ---

        try:

            # Check file is zipped or unzipped
            if s_filepath.endswith(".gz"):
                # file is zip
                file_in =  gzip.open(str(s_filepath), "rt")
            else:
                # file is unzipped
                file_in =  open(s_filepath, 'r')

            # Initialize variables
            s_rec_name = ""
            i_rec_num = 0 # record number
            i_rec_line = 0  # line in record
            i_file_line = 0  # line in file

            # Reset the file pointer to the beginning
            file_in.seek(0)

            # Read the file line by line
            for i_file_line, line in enumerate(file_in):

                # Actual row in file
                s_row = line

                # Check for new Record -> get name and number
                if s_row[0] == '*':
                    # Get line data
                    s_rec_name = s_row[1:6]
                    i_rec_num = int(s_rec_name[1:])
                    i_rec_line = 0 # reset line in rec to 0
                    continue  # Skip to the next line in the record

                # Line numbers
                i_rec_line += 1

                # --- Check Line Legth
                check_line_length_max(s_row)

                # --- Import/Check specific record numbers

                # rec 1: station and date
                if i_rec_num == 1:
                    s_rec_name_add_info = "Station and date"
                    if i_rec_line == 1:
                        # 1.1: station_id, month, year, version_data
                        ingest_row(["station_id", "month", "year", "version"], "X,I2(1-99),X,I2(1-12),X,I4(1992-9999),X,I2(1-99)")
                    else:
                        # 1.2+ seq: id numbers
                        s_seq_name = "data"
                        i_seq_nr = i_rec_line-2
                        ingest_row(["id1","id2","id3","id4","id5","id6","id7","id8"], "8[X,I9]", seq_name=s_seq_name, seq_nr=i_seq_nr)

                # rec 2: station scientist
                elif i_rec_num == 2:
                    s_rec_name_add_info = "Scientist"
                    if i_rec_line == 1:
                        # 2.1: date, hour, min when scientist changed
                        ingest_row(["date when scientist changed (day)","date when scientist changed (hour)","date when scientist changed (min)"], "3[X,I2(0-59!-1)]")
                    elif i_rec_line == 2:
                        # 2.2: station scientist: name, phone, fax
                        ingest_row(["station scientist name","station scientist phone","station scientist fax"], "A38,X,A20,X,A20")
                    elif i_rec_line == 3:
                        # 2.3: tcp/ip, email
                        ingest_row(["station scientist tcp/ip","station scientist email"], "A15(!xxx),X,A50(!xxx)")
                    elif i_rec_line == 4:
                        # 2.4: station scentist: address
                        ingest_row(["station scientist address"], "A80")
                    elif i_rec_line == 5:
                        # 2.5: deputy: date when deputy changed (day, hour, min.)
                        ingest_row(["deputy changed day","deputy changed hour","deputy changed min"], "3[X,I2(0-59!-1)]")
                    elif i_rec_line == 6:
                        # 2.6: deputy: name, phone, fax
                        ingest_row(["deputy name","deputy phone","deputy fax"], "A38,X,A20,X,A20")
                    elif i_rec_line == 7:
                        # 2.7: tcp/ip, email
                        ingest_row(["deputy tcp/ip","deputy email"], "A15(!xxx),X,A50(!xxx)")
                    elif i_rec_line == 8:
                        # 2.4: station scentist: address
                        ingest_row(["deputy address"], "A80")

                # rec 3: messages not to be inserted in the BSRN database
                elif i_rec_num == 3:
                    # 3.1+ seq: messages
                    s_rec_name_add_info = "messages not to be inserted in the BSRN database"
                    s_seq_name = "data"
                    i_seq_nr = i_rec_line - 1
                    ingest_row(["msg"], "A80(!xxx)", seq_name=s_seq_name, seq_nr=i_seq_nr)

                # rec 4: station descr. horizon
                elif i_rec_num == 4:
                    s_rec_name_add_info = "Station descriptor horizon"
                    if i_rec_line == 1:
                        # 4.1: station decr. changed (day, hour, min.)
                        ingest_row(["date when station description changed (day)","date when station description changed (hour)", "date when station description changed (min)"], "3[X,I2(0-59!-1)]")
                    elif i_rec_line == 2:
                        # 4.2: types (surface type, typography type)
                        ingest_row(["Surface type", "Typography type"], "2[X,I2]")
                    elif i_rec_line == 3:
                        # 4.3: address
                        ingest_row(["station address"], "A80")
                    elif i_rec_line == 4:
                        # 4.4: phone, fax
                        ingest_row(["station phone","station fax"], "A20(!xxx),X,A20(!xxx)")
                    elif i_rec_line == 5:
                        # 4.5: tcp/ip, email
                        ingest_row(["station tcp/ip","station email"], "A15(!xxx),X,A50(!xxx)")
                    elif i_rec_line == 6:
                        # 4.6: lat, lon, altitude, identification
                        ingest_row(["lat","lon","altitude (m above sea level)","id (identification of SYNOP station)"], "2[X,F7],X,I4,X,A5(!xxxxx)")
                    elif i_rec_line == 7:
                        # 4.7: date when horizon changed
                        ingest_row(["date when horizon changed (day)","date when horizon description changed (hour)", "date when horizon description changed (min)"], "3[X,I2(0-59!-1)]")
                    elif i_rec_line >= 8:
                        # 4.8+ seq: azimuth (degrees from north clockwise) and elevation (degrees) 11 x per line
                        s_seq_nr = i_rec_line-8
                        s_seq_name = "azimuth and elevation"
                        ingest_row(["azimuth1","elevation1","azimuth2","elevation2","azimuth3","elevation3","azimuth4","elevation4","azimuth5","elevation5","azimuth6","elevation6","azimuth7","elevation7","azimuth8","elevation8","azimuth9","elevation9","azimuth10","elevation10","azimuth11","elevation11"], "11[X,I3(0-360!-1),X,I2(0-89!-1)]", seq_name=s_seq_name, seq_nr=s_seq_nr)

                # rec 5: radiosonde equipment
                elif i_rec_num == 5:
                    s_rec_name_add_info = "Radiosonde equipment"
                    if i_rec_line == 1:
                        # 5.1: date when change occurred (day, hour, min.), is radiosonde operating?
                        ingest_row([
                            "Date when change occurred (day)",
                            "Date when change occurred (hour)",
                            "Date when change occurred (min)",
                            "Is radiosonde operating"
                        ],
                            "3[X,I2(0-59!-1)],X,A1(Y;N)")
                    elif i_rec_line == 2:
                        # 5.2: manufacturer, location, distance, time,
                        ingest_row(
                            [
                                "Manufacturer",
                                "Location",
                                "Distance from radiation site [km]",
                                "Time of 1st launch [h UTC]",
                                "Time of 2nd launch [h UTC]",
                                "Time of 3rd launch [h UTC]",
                                "Time of 4th launch [h UTC]",
                                "Identification of radiosonde"
                             ], "A30,X,A25,X,I3,X,4[I2(0-23!-1),X],A5")
                    elif i_rec_line == 3:
                        # 5.3: remarks
                        ingest_row(["Remarks about radiosonde"], "A80(!xxx)")

                # rec 6: Ozone Equipment
                elif i_rec_num == 6:
                    s_rec_name_add_info = "Ozone m. equipment"
                    if i_rec_line == 1:
                        # 6.1: date when change occurred (day, hour, min.), are ozone measurements operated?
                        ingest_row([
                            "Date when change occurred (day)",
                            "Date when change occurred (hour)",
                            "Date when change occurred (min)",
                            "Are ozone measurements operated"
                        ],
                            "3[X,I2(0-59!-1)],X,A1(y;n)")
                    elif i_rec_line == 2:
                        # 6.2: manufacturer, location, distance, id
                        ingest_row(
                            [
                                "Manufacturer",
                                "Location",
                                "Distance from radiation site [km]",
                                "Identification number of ozone instrument"
                            ], "A30,X,A25,X,I3,X,I5")
                    elif i_rec_line == 3:
                        # 6.3: remarks
                        ingest_row(["Remarks"], "A80(!xxx)")

                # rec 7: Station history
                elif i_rec_num == 7:
                    s_rec_name_add_info = "Station history"
                    if i_rec_line == 1:
                        # 7.1: date when change occurred (day, hour, min.)
                        ingest_row([
                            "Date when change occurred (day)",
                            "Date when change occurred (hour)",
                            "Date when change occurred (min)"
                        ],
                            "3[X,I2(0-59!-1)]")
                    elif i_rec_line == 2:
                        # 7.2: method est. cloud amount (digital proc.)
                        ingest_row(
                            [
                                "Method est. cloud amount (digital proc.)"
                            ], "A80(!xxx)")
                    elif i_rec_line == 3:
                        # 7.3: method est. cloud base height (with instrument)
                        ingest_row([
                            "Method est. cloud base height (with instrument)"
                            ],
                            "A80(!xxx)")
                    elif i_rec_line == 4:
                        # 7.4: method est. cloud liquid water content
                        ingest_row([
                            "Method est. cloud liquid water content"
                            ],
                            "A80(!xxx)")
                    elif i_rec_line == 5:
                        # 7.5: method est. cloud aerosol vertical distribution
                        ingest_row([
                            "Method est. cloud aerosol vertical distribution"
                            ],
                            "A80(!xxx)")
                    elif i_rec_line == 6:
                        # 7.6: method est. water vapour press. v.d.
                        ingest_row([
                            "Method est. water vapour press. v.d."
                            ],
                            "A80(!xxx)")
                    elif i_rec_line == 7:
                        # 7.7: flags indicating if the SYNOP and/or the corresponding quantities of the expanded programme, are measured
                        ingest_row([
                            "f1", "f2", "f3", "f4", "f5", "f6"
                            ],
                            "5[A1(y;n),X],A1(y;n)")

                # rec 8: Radiation instruments (block: multi-line)
                elif i_rec_num == 8:
                    # Special case: Radiation instrumients is "divided" in blockes. every 10 lines a new instrument
                    i_block_size = 10
                    i_block_nr = int((i_rec_line - 1) / i_block_size) + 1  # every 10 entries are one block
                    i_block_line = i_rec_line % i_block_size
                    s_rec_name_add_info = f"Radiation instrument"
                    s_seq_name = "data"
                    i_seq_nr = i_block_nr
                    if i_block_line == 1:
                        # 8.1: date when change occurred (day, hour, min.), is instrument measuring
                        ingest_row([
                            "Date when change occurred (day)",
                            "Date when change occurred (hour)",
                            "Date when change occurred (min)",
                            "Is instrument measuring"
                        ],"3[X,I2(0-59!-1)],X,A1(y;n)", seq_name=s_seq_name, seq_nr=i_seq_nr)
                    elif i_block_line == 2:
                        # 8.2: manufacturer, model, serial number, date of purchase, identification number assigned by the WRMC
                        ingest_row(
                            [
                                "Manufacturer",
                                "Model",
                                "Serial number",
                                "Date of purchase [MM/DD/YY]",
                                "Identification number assigned by the WRMC"
                            ], "A30,X,A15,X,A18,X,A8(!xxx),X,I5", seq_name=s_seq_name, seq_nr=i_seq_nr)
                    elif i_block_line == 3:
                        # 8.3: remarks about the radiation istrument
                        ingest_row([
                            "Remarks"
                        ],
                            "A80(!xxx)", seq_name=s_seq_name, seq_nr=i_seq_nr)
                    elif i_block_line == 4:
                        # 8.4: pyrgeometer body compensation code, pyrgeometer dome compensation code, ...
                        ingest_row([
                            "Pyrgeometer body compensation code",
                            "Pyrgeometer dome compensation code",
                            "Wavelength of band 1of spectral i. [micron]",
                            "Bandwidth of band 1of spectral i. [micron]",
                            "Wavelength of band 2",
                            "Bandwidth of band 2",
                            "Wavelength of band 3",
                            "Bandwidth of band 3",
                            "Max. zenith angle [degree] of direct",
                            "Min. (spectral) instrument"
                        ],
                            "2[X,I2(!-1)],6[X,F7(!-1.000)],2[X,I2(0-90!-1)]", seq_name=s_seq_name, seq_nr=i_seq_nr)
                    elif i_block_line == 5:
                        # 8.5: location of calibration, person doing calibration
                        ingest_row([
                            "Location of calibration",
                            "Person doing calibration"
                        ],
                            "A30,X,A40", seq_name=s_seq_name, seq_nr=i_seq_nr)
                    elif i_block_line == 6:
                        # 8.6: start of calibration period (band 1 of spectr. instr.), ...
                        ingest_row([
                            "Start of calibration period (band 1 of spectr. instr.) [MM/DD/YY]",
                            "End of calibration period (band 1 of spectr. instr.) [MM/DD/YY]",
                            "Number of comparisons (band 1 of spectr. instr.)",
                            "Mean calibration coefficient (band 1 of spectr. instr.)",
                            "Standard error of cal. coeff. (band 1 of spectr. instr.)"
                        ],
                            "2[A8,X],I2(!-1),2[X,F12(!-1.0000)]", seq_name=s_seq_name, seq_nr=i_seq_nr)
                    elif i_block_line == 7:
                        # 8.7: start of calibration period band 2 of spectr. instr., ...
                        ingest_row([
                            "Start of calibration period band 2 of spectr. instr. [MM/DD/YY]",
                            "End of calibration period band 2 of spectr. instr. [MM/DD/YY]",
                            "Number of comparisons band 2 of spectr. instr.",
                            "Mean calibration coefficient band 2 of spectr. instr.",
                            "Standard error of cal. coeff. band 2 of spectr. instr."
                        ],
                            "2[A8(!XXX),X],I2(!-1),2[X,F12(!-1.0000)]", seq_name=s_seq_name, seq_nr=i_seq_nr)
                    elif i_block_line == 8:
                        # 8.8: start of calibration period band 3 of spectr. instr., ...
                        ingest_row([
                            "Start of calibration period band 3 of spectr. instr. [MM/DD/YY]",
                            "End of calibration period band 3 of spectr. instr. [MM/DD/YY]",
                            "Number of comparisons band 3 of spectr. instr.",
                            "Mean calibration coefficient band 3 of spectr. instr.",
                            "Standard error of cal. coeff. band 3 of spectr. instr."
                        ],
                            "2[A8(!XXX),X],I2(!-1),2[X,F12(!-1.0000)]", seq_name=s_seq_name, seq_nr=i_seq_nr)
                    elif i_block_line == 9:
                        # 8.9: start of calibration period band 3 of spectr. instr., ...
                        ingest_row([
                            "Remarks on calibration, e.g. units of cal. coeff."
                        ],
                            "A80(!xxx)", seq_name=s_seq_name, seq_nr=i_seq_nr)
                    elif i_block_line == 0:
                        # 8.10: remarks on calibration (continued)
                        ingest_row([
                            "Remarks on calibration (continued)"
                        ],
                            "A80(!xxx)", seq_name=s_seq_name, seq_nr=i_seq_nr)

                # rec 9: assignment of radiation quantities to instruments
                elif i_rec_num == 9:
                    s_rec_name_add_info = "Assignment radiation quantities to instruments"
                    s_seq_name = "data"
                    i_seq_nr = i_rec_line
                    if i_rec_line >= 1:
                        # 9.1+ seq: date when change occurred (day, hour, min.), ...
                        ingest_row([
                            "Date when change occurred (day)",
                            "Date when change occurred (hour)",
                            "Date when change occurred (min)",
                            "Id. no. of radiation quantity measured",
                            "Id. no. of instrument which measured quantity",
                            "No. of band (for spectral instruments)"],
                            "3[X,I2(0-59!-1)],X,I9,X,I5,X,I2(!-1)",
                            seq_name=s_seq_name, seq_nr=i_seq_nr
                        )
                        # Check Uniq (hard on row): Radiation - Instrument
                        s_check_day = s_row[1:3]
                        s_check_hour = s_row[4:6]
                        s_check_min = s_row[7:9]
                        s_check_intrument = s_row[20:25]
                        s_check_uniq = f"{s_check_day}|{s_check_hour}|{s_check_min}|{s_check_intrument}|"
                        if s_check_uniq in set_check_uniq_radiation_quantitiy_instrument:
                            lst_report_error_eval_line.append([s_rec_name, s_rec_name_add_info, "-", "-", "-", f"Assignment radiation quantities to instruments is not uniq: day[{s_check_day}], hour[{s_check_hour}] min[{s_check_min}] instrument_id[{s_check_intrument}]"])
                        else:
                            set_check_uniq_radiation_quantitiy_instrument.add(s_check_uniq)

                # rec 100: Basic measurement
                elif i_rec_num == 100:
                    # Special case: Basic measurement is "divided" in blocks. every 2 lines a new measurement
                    i_block_size = 2
                    i_block_nr = int((i_rec_line - 1) / i_block_size) + 1  # every 2 entries are one block
                    i_block_line = i_rec_line % i_block_size
                    s_rec_name_add_info = f"Basic measurement"
                    s_seq_name = "data"
                    i_seq_nr = i_block_nr
                    if i_block_line == 1:
                        # 100.1: date, time ...
                        ingest_row([
                            "date_day",
                            "time_min",
                            "Short-wave downward (GLOBAL) radiation [W/m**2]",
                            "Short-wave downward (GLOBAL) radiation, standard deviation [W/m**2]",
                            "Short-wave downward (GLOBAL) radiation, minimum [W/m**2]",
                            "Short-wave downward (GLOBAL) radiation, maximum [W/m**2]",
                            "Direct radiation [W/m**2]",
                            "Direct radiation, standard deviation [W/m**2]",
                            "Direct radiation, minimum [W/m**2]",
                            "Direct radiation, maximum [W/m**2]",
                        ], "X,I2(1-31),X,I4(0-1439),2[X,X,X,I4(!-999),X,F5(!-99.9),X,I4(!-999),X,I4(!-999)]", seq_name=s_seq_name, seq_nr=i_seq_nr)
                    elif i_block_line == 0:
                        # 100.2: diffuse mean ...
                        ingest_row([
                            "Diffuse radiation [W/m**2]",
                            "Diffuse radiation, standard deviation [W/m**2]",
                            "Diffuse radiation, minimum [W/m**2]",
                            "Diffuse radiation, maximum [W/m**2]",
                            "Long-wave downward radiation [W/m**2]",
                            "Long-wave downward radiation, standard deviation [W/m**2]",
                            "Long-wave downward radiation, minimum [W/m**2]",
                            "Long-wave downward radiation, maximum [W/m**2]",
                            "Air temperature [°C]",
                            "Relative Humidity [%]",
                            "Station pressure [hPa]"
                        ], "8[X],2[X,X,X,I4(!-999),X,F5(!-99.9),X,I4(!-999),X,I4(!-999)],4[X],F5(!-99.9),X,F5(!-99.9),X,I4(!-999)", seq_name=s_seq_name, seq_nr=i_seq_nr)

                # rec 200: Expanded measurement
                elif i_rec_num == 200:
                    s_rec_name_add_info = f"Expanded measurement"
                    s_seq_name = "data"
                    if i_rec_line >= 1:
                        i_seq_nr = i_rec_line
                        # 200.1+ seq: date, time ...
                        ingest_row([
                            "date_day",
                            "time_min",
                            "Downward short-wave spectral at wavellength 1, mean",
                            "Downward short-wave spectral at wavellength 1, standard deviation ",
                            "Downward short-wave spectral at wavellength 1, min.",
                            "Downward short-wave spectral at wavellength 1, max.",
                            "Downward short-wave spectral at wavellength 2, mean",
                            "Downward short-wave spectral at wavellength 2, standard deviation ",
                            "Downward short-wave spectral at wavellength 2, min.",
                            "Downward short-wave spectral at wavellength 2, max.",
                            "Downward short-wave spectral at wavellength 3, mean",
                            "Downward short-wave spectral at wavellength 3, standard deviation ",
                            "Downward short-wave spectral at wavellength 3, min.",
                            "Downward short-wave spectral at wavellength 3, max."
                        ], "X,I2(1-31),X,I4(0-1439),3[X,X,X,I4(!-999),X,F5(!-99.9),X,I4(!-999),X,I4(!-999)]",
                            seq_name=s_seq_name, seq_nr=i_seq_nr)

                # rec 300: Other measurement in minutes intervals
                elif i_rec_num == 300:
                    s_rec_name_add_info = f"Other measurement in minutes intervals"
                    s_seq_name = "data"
                    if i_rec_line >= 1:
                        i_seq_nr = i_rec_line
                        # 300.1+ seq: date, time ...
                        ingest_row([
                            "date_day",
                            "time_min",
                            "Short-wave upward (REFLEX) radiation [W/m**2]",
                            "Short-wave upward (REFLEX) radiation, standard deviation [W/m**2]",
                            "Short-wave upward (REFLEX) radiation, minimum [W/m**2]",
                            "Short-wave upward (REFLEX) radiation, maximum [W/m**2]",
                            "Long-wave upward radiation [W/m**2]",
                            "Long-wave upward radiation, standard deviation [W/m**2]",
                            "Long-wave upward radiation, minimum [W/m**2]",
                            "Long-wave upward radiation, maximum [W/m**2]",
                            "Net radiation [W/m**2]",
                            "Net radiation, standard deviation [W/m**2]",
                            "Net radiation, minimum [W/m**2]",
                            "Net radiation, maximum [W/m**2]"
                        ], "X,I2(1-31),X,I4(0-1439),3[X,X,X,I4(!-999),X,F5(!-99.9),X,I4(!-999),X,I4(!-999)]",
                            seq_name=s_seq_name, seq_nr=i_seq_nr)

                # rec 400: Special spectral measurement
                elif i_rec_num == 400:
                    # Special case: Special spectral measurement
                    i_block_size = 3
                    i_block_nr = int((i_rec_line - 1) / i_block_size) + 1  # every 2 entries are one block
                    i_block_line = i_rec_line % i_block_size
                    s_rec_name_add_info = f"Special spectral measurement"
                    s_seq_name = "data"
                    i_seq_nr = i_block_nr
                    if i_block_line == 1:
                        # 400.1: date, time ...
                        ingest_row([
                            "date_day",
                            "time_min",
                            "Downward short-wave spectral at wavellength 4, mean",
                            "Downward short-wave spectral at wavellength 4, standard deviation ",
                            "Downward short-wave spectral at wavellength 4, min.",
                            "Downward short-wave spectral at wavellength 4, max.",
                            "Downward short-wave spectral at wavellength 5, mean",
                            "Downward short-wave spectral at wavellength 5, standard deviation ",
                            "Downward short-wave spectral at wavellength 5, min.",
                            "Downward short-wave spectral at wavellength 5, max.",
                            "Downward short-wave spectral at wavellength 6, mean",
                            "Downward short-wave spectral at wavellength 6, standard deviation ",
                            "Downward short-wave spectral at wavellength 6, min.",
                            "Downward short-wave spectral at wavellength 6, max."
                        ], "X,I2(1-31),X,I4(0-1439),3[X,X,X,I4(!-999),X,F5(!-99.9),X,I4(!-999),X,I4(!-999)]",
                            seq_name=s_seq_name, seq_nr=i_seq_nr)
                    elif i_block_line == 2:
                        # 400.2: downward short-wave spectr 7-9
                        ingest_row([
                            "Downward short-wave spectral at wavellength 7, mean",
                            "Downward short-wave spectral at wavellength 7, standard deviation ",
                            "Downward short-wave spectral at wavellength 7, min.",
                            "Downward short-wave spectral at wavellength 7, max.",
                            "Downward short-wave spectral at wavellength 8, mean",
                            "Downward short-wave spectral at wavellength 8, standard deviation ",
                            "Downward short-wave spectral at wavellength 8, min.",
                            "Downward short-wave spectral at wavellength 8, max.",
                            "Downward short-wave spectral at wavellength 9, mean",
                            "Downward short-wave spectral at wavellength 9, standard deviation ",
                            "Downward short-wave spectral at wavellength 9, min.",
                            "Downward short-wave spectral at wavellength 9, max."
                        ], "8[X],3[X,X,X,X,I4(!-999),X,F5(!-99.9),X,I4(!-999),X,I4(!-999)]",
                            seq_name=s_seq_name, seq_nr=i_seq_nr)
                    elif i_block_line == 0:
                        # 400.3: downward short-wave spectr 10-12 ...
                        ingest_row([
                            "Downward short-wave spectral at wavellength 10, mean",
                            "Downward short-wave spectral at wavellength 10, standard deviation ",
                            "Downward short-wave spectral at wavellength 10, min.",
                            "Downward short-wave spectral at wavellength 10, max.",
                            "Downward short-wave spectral at wavellength 11, mean",
                            "Downward short-wave spectral at wavellength 11, standard deviation ",
                            "Downward short-wave spectral at wavellength 11, min.",
                            "Downward short-wave spectral at wavellength 11, max.",
                            "Downward short-wave spectral at wavellength 12, mean",
                            "Downward short-wave spectral at wavellength 12, standard deviation ",
                            "Downward short-wave spectral at wavellength 12, min.",
                            "Downward short-wave spectral at wavellength 12, max."
                        ], "8[X],3[X,X,X,I4(!-999),X,F5(!-99.9),X,I4(!-999),X,I4(!-999)]",
                            seq_name=s_seq_name, seq_nr=i_seq_nr)

                # rec 500: Ultra-violet measurement
                elif i_rec_num == 500:
                    # Special case: ultra-violet measurement
                    i_block_size = 2
                    i_block_nr = int((i_rec_line - 1) / i_block_size) + 1  # every 2 entries are one block
                    i_block_line = i_rec_line % i_block_size
                    s_rec_name_add_info = f"Ultra-violet measurement"
                    s_seq_name = "data"
                    i_seq_nr = i_block_nr
                    if i_block_line == 1:
                        # 500.1: date, time ...
                        ingest_row([
                            "date_day",
                            "time_min",
                            "UV-a global [W/m**2]",
                            "UV-a global, standard deviation [W/m**2]",
                            "UV-a global, minimum [W/m**2]",
                            "UV-a global, maximum [W/m**2]",
                            "UV-b direct [W/m**2]",
                            "UV-b direct, standard deviation [W/m**2]",
                            "UV-b direct, minimum [W/m**2]",
                            "UV-b direct, maximum [W/m**2]"
                        ], "X,I2(1-31),X,I4(0-1439),8[X,F5(!-99.9)]",
                        seq_name=s_seq_name, seq_nr=i_seq_nr)
                    elif i_block_line == 0:
                        # 400.1+:
                        ingest_row([
                            "UV-b global [W/m**2]",
                            "UV-b global, standard deviation [W/m**2]",
                            "UV-b global, minimum [W/m**2]",
                            "UV-b global, maximum [W/m**2]",
                            "UV-b diffuse [W/m**2]",
                            "UV-b diffuse, standard deviation [W/m**2]",
                            "UV-b diffuse, minimum [W/m**2]",
                            "UV-b diffuse, maximum [W/m**2]",
                            "UV upward reflected [W/m**2]",
                            "UV upward reflected, standard deviation [W/m**2]",
                            "UV upward reflected, minimum [W/m**2]",
                            "UV upward reflected, maximum [W/m**2]",
                        ], "8[X],12[X,F5(!-99.9)]",
                        seq_name=s_seq_name, seq_nr=i_seq_nr)

                # rec 1000: surface SYNOP
                # TODO: the definition is not right i guess
                elif i_rec_num == 1000:
                    s_rec_name_add_info = f"Surface SYNOP"
                    s_seq_name = "data"
                    i_seq_nr = i_rec_line
                    if i_rec_line >= 1:
                        # 1000.1+ seq: surface SYNOP
                        ingest_row([
                            "FM 12–XII Ext. SYNOP code"
                        ],
                            "A80",
                            seq_name=s_seq_name, seq_nr=i_seq_nr)

                # rec 1100: radiosonde measurem. in launch intervals
                elif i_rec_num == 1100:
                    s_rec_name_add_info = f"Radiosonde measurement in launch intervals"
                    s_seq_name = "data"
                    i_seq_nr = i_rec_line
                    if i_rec_line >= 1:
                        # 1100.1+ seq: date, time ...
                        ingest_row([
                            "date_day",
                            "time_min",
                            "Level number (first level = 1)",
                            "Pressure, at given altitude [hPa]",
                            "Altitude [m]",
                            "Temperature, air [deg C]",
                            "Dew/frost point [deg C]",
                            "Wind direction [deg]",
                            "Wind speed [m/sec]",
                            "Ozone [mPa]"
                        ], "X,I2(1-31),X,I4(0-1439),3[X],I4(0-9999),X,I4(!-999),X,I5,X,F5(!-99.9),X,F6(!-999.9),X,I3(0-360!-99),X,I3(!-99),X,F4(!-9.9)",
                            seq_name=s_seq_name, seq_nr=i_seq_nr)

                # rec 1200: Ozone measurem. in hours intervals
                elif i_rec_num == 1200:
                    s_rec_name_add_info = f"Ozone measurement in hours intervals"
                    s_seq_name = "data"
                    i_seq_nr = i_rec_line
                    if i_rec_line >= 1:
                        # 1300.1+ seq: date, time ...
                        ingest_row([
                            "date_day",
                            "time_min",
                            "Ozone total [DU]"
                        ],
                            "X,I2(1-31),X,I4(0-1439),3[X],I4(!-999)",
                            seq_name=s_seq_name, seq_nr=i_seq_nr)

                # rec 1300: Expanded measurem. in hours intervals
                elif i_rec_num == 1300:
                    s_rec_name_add_info = f"Expanded measurement in hours intervals"
                    s_seq_name = "data"
                    i_seq_nr = i_rec_line
                    if i_rec_line >= 1:
                        # 1300.1+ seq: date, time ...
                        ingest_row([
                            "date_day",
                            "time_min",
                            "Total cloud amount with instrument [%]",
                            "Cloud base height (no clouds 99999) [m]",
                            "Cloud liquid water [mm]"
                        ],
                            "X,I2(1-31),X,I4(0-1439),3[X],I2(!-9),X,I5(!-9999),X,F5(!-9.99)",
                            seq_name=s_seq_name, seq_nr=i_seq_nr)

                # rec 1500: Other measurement in hours intervals
                elif i_rec_num == 1500:
                    s_rec_name_add_info = f"Other measurement in hours intervals"
                    s_seq_name = "data"
                    i_seq_nr = i_rec_line
                    if i_rec_line >= 1:
                        # 1500.1+ seq date, time ...
                        ingest_row([
                            "date_day",
                            "time_min",
                            "Thermal spectral at wavelength 1",
                            "Thermal spectral at wavelength 2",
                            "Thermal spectral at wavelength 3",
                            "Hemispheric solar spectral at wavelength 1",
                            "Hemispheric solar spectral at wavelength 2",
                            "Hemispheric solar spectral at wavelength 3"
                        ],
                            "X,I2(1-31),X,I4(0-1439),2[X,X,X,I4(!-9),X,I4(!-9),X,I4(!-9)]",
                            seq_name=s_seq_name, seq_nr=i_seq_nr)

                # rec 3010: Other measurement at 10m
                elif i_rec_num == 3010:
                    # Special case: Other measurement at 10m
                    i_block_size = 2
                    i_block_nr = int((i_rec_line - 1) / i_block_size) + 1  # every 2 entries are one block
                    i_block_line = i_rec_line % i_block_size
                    s_rec_name_add_info = f"Other measurement at 10m"
                    s_seq_name = "data"
                    i_seq_nr = i_block_nr
                    if i_block_line == 1:
                        # 3010.1: date, time ...
                        ingest_row([
                            "date_day",
                            "time_min",
                            "Short-wave downward (GLOBAL) radiation [W/m**2]",
                            "Short-wave downward (GLOBAL) radiation, standard deviation [W/m**2]",
                            "Short-wave downward (GLOBAL) radiation, minimum [W/m**2]",
                            "Short-wave downward (GLOBAL) radiation, maximum [W/m**2]",
                            "Short-wave upward (REFLEX) radiation [W/m**2]",
                            "Short-wave upward (REFLEX) radiation, standard deviation [W/m**2]",
                            "Short-wave upward (REFLEX) radiation, minimum [W/m**2]",
                            "Short-wave upward (REFLEX) radiation, maximum [W/m**2]"
                        ], "X,I2(1-31),X,I4(0-1439),2[X,X,X,I4(!-999),X,F5(!-99.9),X,I4(!-999),X,I4(!-999)]",
                        seq_name=s_seq_name, seq_nr=i_seq_nr)
                    elif i_block_line == 0:
                        # 3010.2+:
                        ingest_row([
                            "Long-wave downward radiation [W/m**2]",
                            "Long-wave downward radiation, standard deviation [W/m**2]",
                            "Long-wave downward radiation, minimum [W/m**2]",
                            "Long-wave downward radiation, maximum [W/m**2]",
                            "Long-wave upward radiation [W/m**2]",
                            "Long-wave upward radiation, standard deviation [W/m**2]",
                            "Long-wave upward radiation, minimum [W/m**2]",
                            "Long-wave upward radiation, maximum [W/m**2]",
                            "Air temperature [°C]",
                            "Relative Humidity [%]"
                        ], "8[X],2[X,X,X,I4(!-999),X,F5(!-99.9),X,I4(!-999),X,I4(!-999)],4[X],F5(!-99.9),X,F5(!-99.9)",
                        seq_name=s_seq_name, seq_nr=i_seq_nr)

                # rec 3030: Other measurement at 30m
                elif i_rec_num == 3030:
                    # Special case: Other measurement at 30m
                    i_block_size = 2
                    i_block_nr = int((i_rec_line - 1) / i_block_size) + 1  # every 2 entries are one block
                    i_block_line = i_rec_line % i_block_size
                    s_rec_name_add_info = f"Other measurement at 30m"
                    s_seq_name = "data"
                    i_seq_nr = i_block_nr
                    if i_block_line == 1:
                        # 3030.1: date, time ...
                        ingest_row([
                            "date_day",
                            "time_min",
                            "Short-wave downward (GLOBAL) radiation [W/m**2]",
                            "Short-wave downward (GLOBAL) radiation, standard deviation [W/m**2]",
                            "Short-wave downward (GLOBAL) radiation, minimum [W/m**2]",
                            "Short-wave downward (GLOBAL) radiation, maximum [W/m**2]",
                            "Short-wave upward (REFLEX) radiation [W/m**2]",
                            "Short-wave upward (REFLEX) radiation, standard deviation [W/m**2]",
                            "Short-wave upward (REFLEX) radiation, minimum [W/m**2]",
                            "Short-wave upward (REFLEX) radiation, maximum [W/m**2]"
                        ], "X,I2(1-31),X,I4(0-1439),2[X,X,X,I4(!-999),X,F5(!-99.9),X,I4(!-999),X,I4(!-999)]",
                        seq_name=s_seq_name, seq_nr=i_seq_nr)
                    elif i_block_line == 0:
                        # 3030.2+:
                        ingest_row([
                            "Long-wave downward radiation [W/m**2]",
                            "Long-wave downward radiation, standard deviation [W/m**2]",
                            "Long-wave downward radiation, minimum [W/m**2]",
                            "Long-wave downward radiation, maximum [W/m**2]",
                            "Long-wave upward radiation [W/m**2]",
                            "Long-wave upward radiation, standard deviation [W/m**2]",
                            "Long-wave upward radiation, minimum [W/m**2]",
                            "Long-wave upward radiation, maximum [W/m**2]",
                            "Air temperature [°C]",
                            "Relative Humidity [%]"
                        ],
                            "8[X],2[X,X,X,I4(!-999),X,F5(!-99.9),X,I4(!-999),X,I4(!-999)],4[X],F5(!-99.9),X,F5(!-99.9)",
                        seq_name=s_seq_name, seq_nr=i_seq_nr)

                # rec 4000/10/30+ seq: radiosonde measurem. in launch intervals
                elif i_rec_num in (4000, 4010, 4030):
                    s_rec_name_add_info = "Pyrgeometer temperature"
                    s_seq_name = "data"
                    i_seq_nr = i_rec_line
                    if i_rec_line >= 1:
                        # 4000/10/30.1 date, time ...
                        ingest_row([
                            "date_day",
                            "time_min",
                            "Dome temperature 1 downward long-wave instrument [°C]",
                            "Dome temperature 2 downward long-wave instrument [°C]",
                            "Dome temperature 3 downward long-wave instrument [°C]",
                            "Body temperature downward long-wave instrument [°C]",
                            "Thermopile output downward long-wave instrument [W/m2]",
                            "Dome temperature 1 upward long-wave instrument [°C]",
                            "Dome temperature 2 upward long-wave instrument [°C]",
                            "Dome temperature 3 upward long-wave instrument [°C]",
                            "Body temperature upward long-wave instrument [°C]",
                            "Thermopile output upward long-wave instrument [W/m2]"
                        ],
                            "X,I2(1-31),X,I4(0-1439),X,4[F6(!-99.99),X],F6(!-999.9),X,X,4[F6(!-99.99),X],F6(!-999.9)",
                        seq_name=s_seq_name, seq_nr=i_seq_nr)

                # all other recs:
                else:
                    # save unknown_recs

                    if i_rec_num not in dic_report_unknown_recs:
                        dic_report_unknown_recs[i_rec_num] = [0, s_rec_name]
                    dic_report_unknown_recs[i_rec_num][0] += 1 # count up

            # Check if file ends with a"new line"
            if not s_row.endswith('\n'):
                add_err_2_overview("Missing new line at the end of file", "end_no_nl", "other")
                lst_report_error_general.append([["-", "-", "End of file", "-", f'Missing "new line" at the end of file']])

            # Final check: close open blocks or seqs and write to report
            if not self.__model.is_debug():
                # Check if record changed and a block is open
                if tp_report_block_entry_open is not None:
                    lst_report_content_imported.append(tp_report_block_entry_open[1])
                    tp_report_block_entry_open = None  # Reset
                # Check if sequence changed and a old sequence is open
                if tp_report_seq_entry_open is not None:
                    lst_report_content_imported.append(tp_report_seq_entry_open[2])
                    tp_report_seq_entry_open = None  # Reset

        except Exception as ex:
            b_err_tec = True
            s_err_tec = str(ex)
            add_err_2_overview("Error while opening file", "file_open", "other")
            lst_report_error_general.append([["-", "-", "-", "-", f"Error while opening file: {s_filepath}: {ex}"]])

        # --- Report

        # * Report: Header

        s_report_header = ""
        s_file_name = os.path.basename(s_filepath)
        s_report_title = f"Report: {s_file_name}"
        s_report_bars = "*" * len(s_report_title)
        s_report_header += f"{s_report_bars}\n"
        s_report_header += f"{s_report_title}\n"
        s_report_header += f"{s_report_bars}\n\n"
        s_report_header += f"*** Results\n\n"
        s_report_header += f"\n"

        # * Report Part: Imported content (only in debug mode)
        s_report_content = ""
        if self.__model.is_debug():
            s_report_content += f"--- Imported content\n\n"
            s_report_content += PrettyText.table(lst_report_content_imported)
            s_report_content += "\n\n"

        # * Report Part: Errors, Warnings and Infos (Overview and Details)

        s_report_errors_warn_info_overview = ""
        s_report_errors_warn_info_overview_errors = ""
        s_report_errors_warn_info_details = ""
        i_err = len(lst_report_error_general) + len(lst_report_error_eval_line) - 2
        i_wrn = len(lst_report_warn_general) - 1
        i_inf = len(lst_report_info_eval_line_val_empty) - 1
        i_unknown_rec = len(dic_report_unknown_recs)
        i_ewi = i_err + i_wrn + i_inf
        b_err = True if i_err > 0 else False
        b_wrn = True if i_wrn > 0 else False
        b_inf = True if i_inf > 0 else False
        b_unknown_rec = True if i_unknown_rec > 0 else False

        # Overview 1
        lst_table_overview = [["Type", "#", "%", "Info"]]
        lst_table_overview.append(["Imported metadata records", len(set_report_imported_rec_meta), "", PrettyText.lst2strC([f"{i:04d}" for i in sorted(list(set_report_imported_rec_meta))])])
        lst_table_overview.append(["Imported data records", len(set_report_imported_rec_data), "", PrettyText.lst2strC([f"{i:04d}" for i in sorted(list(set_report_imported_rec_data))])])
        lst_table_overview.append(["Imported lines", i_import_lines, "", ""])
        lst_table_overview.append(["Imported values", i_import_vals, "100%", ""])

        if b_err:
            lst_table_overview.append(["Errors", i_err, PrettyText.percent2str(i_err, i_import_vals), ""])
        if b_wrn:
            lst_table_overview.append(["Warnings", i_wrn, PrettyText.percent2str(i_wrn, i_import_vals), ""])
        if b_inf:
            lst_table_overview.append(["Information", i_inf, PrettyText.percent2str(i_inf, i_import_vals), ""])
        if b_unknown_rec:
            lst_tmp_unknown_rec_names = [ s_rec_name for i_rec_num, (i_rec_count, s_rec_name) in dic_report_unknown_recs.items()]
            lst_table_overview.append(["Unknown records", i_unknown_rec, "", PrettyText.lst2strC(lst_tmp_unknown_rec_names)])
        s_report_errors_warn_info_overview += f"--- Overview\n\n"
        s_report_errors_warn_info_overview += PrettyText.table(lst_table_overview)
        s_report_errors_warn_info_overview += "\n\n"

        # Overview 2 (Error summary)
        if len(dic_overview_errs_msg_long) > 0:
            lst_table_overview_err = [["Error category", "#"]]
            for s_tmp_name, i_tmp in dic_overview_errs_msg_long.items():
                lst_table_overview_err.append([s_tmp_name, i_tmp])
            s_report_errors_warn_info_overview += PrettyText.table(lst_table_overview_err)
            s_report_errors_warn_info_overview += "\n\n"

        # Details
        if i_ewi > 0:
            if b_err:
                if len(lst_report_error_general) > 1:
                    s_table = f"--- Errors (general) ({len(lst_report_error_general)-1}x)\n\n"
                    s_table += PrettyText.table(lst_report_error_general)
                    s_table += "\n\n"
                    s_report_errors_warn_info_details += s_table
                if len(lst_report_error_eval_line) > 1:
                    s_table = f"--- Errors (evaluating row) ({len(lst_report_error_eval_line) - 1}x)\n\n"
                    s_table += PrettyText.table(lst_report_error_eval_line)
                    s_table += "\n\n"
                    s_report_errors_warn_info_details += s_table
            if b_wrn:
                if len(lst_report_warn_general) > 1:
                    s_table = f"--- Warning (general) ({len(lst_report_warn_general) - 1}x)\n\n"
                    s_table += PrettyText.table(lst_report_error_general)
                    s_table += "\n\n"
                    s_report_errors_warn_info_details += s_table
            if b_inf:
                if len(lst_report_info_eval_line_val_empty) > 1:
                    s_table = f"--- Info (no content) ({len(lst_report_info_eval_line_val_empty) - 1}x)\n\n"
                    s_table += PrettyText.table(lst_report_info_eval_line_val_empty)
                    s_table += "\n\n"
                    s_report_errors_warn_info_details += s_table
        else:
            s_report_errors_warn_info_details = "--- Messages\n\n"
            s_report_errors_warn_info_details += "No errors, warnings or informations occurred\n"

        # * Report Part: Unknown Recs

        s_report_unknown_recs = ""
        i_unknown_recs = len(dic_report_unknown_recs)
        if i_unknown_recs > 0:
            lst_table = [["Name", "size/lines"]]
            for i_rec_num, (i_rec_count, s_rec_name) in dic_report_unknown_recs.items():
                lst_table.append(([f"{s_rec_name}", i_rec_count]))
            s_report_unknown_recs += f"--- Unknown logical records ({len(lst_table)}x)\n\n"
            s_report_unknown_recs += f"{PrettyText.table(lst_table)}\n\n"

        # * Report Part: Additional Information

        s_report_add_info = ""
        if b_report_add_info_illegal_chars:
            s_report_add_info += "--- Additional information\n\n"
            s_report_add_info += s_report_add_info_illegal_chars

        # * Report: Full
        s_report = ""
        s_report += s_report_header
        s_report += s_report_errors_warn_info_overview
        s_report += s_report_content
        s_report += s_report_unknown_recs
        s_report += s_report_errors_warn_info_details
        s_report += s_report_add_info
        s_report += "\n"
        s_report = PrettyText.clean_wizard(s_report, max_one_empty_row=True)

        # * Short error summary
        s_err_overview_short = ""
        lst_err_overview_short_part_recs = []
        lst_err_overview_short_part_msg = []
        if len(dic_overview_errs_recs) > 0:
            for s_rec, i_nr in dic_overview_errs_recs.items():
                lst_err_overview_short_part_recs.append(f"{s_rec}({i_nr}x)")
        if len(dic_overview_errs_msg_short) > 0:
            for s_err, i_nr in dic_overview_errs_msg_short.items():
                lst_err_overview_short_part_msg.append(f"{s_err}({i_nr}x)")
        if len(lst_err_overview_short_part_recs) > 0:
            s_err_overview_short += PrettyText.lst2str(lst_err_overview_short_part_recs,",") + ":"
        if len(lst_err_overview_short_part_msg) > 0:
            s_err_overview_short += PrettyText.lst2str(lst_err_overview_short_part_msg,",")

        # return
        return b_err_tec, s_err_tec, i_err, i_wrn, i_inf, set(dic_report_unknown_recs.keys()), s_report, s_err_overview_short, self.__odic_data
